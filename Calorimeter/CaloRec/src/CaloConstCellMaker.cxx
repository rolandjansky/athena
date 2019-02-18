/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloRec/src/CaloConstCellMaker.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Create a @c CaloConstCellContainer.
 */


#include "CaloConstCellMaker.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

#include "AthenaKernel/Chrono.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <memory>

using CLHEP::microsecond;
using CLHEP::second;


/**
 * @brief Constructor.
 * @param name Algorithm name.
 * @param pSvcLocator Gaudi service locator.
 */
CaloConstCellMaker::CaloConstCellMaker (const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_caloCellsOutputKey(""),
    m_chrono("ChronoStatSvc", name)
{
  declareProperty("OwnPolicy", m_ownPolicy = static_cast<int>(SG::VIEW_ELEMENTS),
                  "Will the new container own its cells?  Default is no.");
  declareProperty("CaloCellMakerTools", m_caloCellMakerTools,
                  "List of tools to run.");
  declareProperty("CaloCellsOutputName", m_caloCellsOutputKey,
                  "Output container name.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloConstCellMaker::initialize()
{
  ATH_CHECK( m_chrono.retrieve() );

  // access tools and store them
  ATH_CHECK( m_caloCellMakerTools.retrieve() );
  ATH_CHECK( m_caloCellsOutputKey.initialize());
  
  ATH_MSG_DEBUG( "Successfully retrieved CaloConstCellMakerTools: "
                 << m_caloCellMakerTools );

  ATH_MSG_INFO( " Output CaloConstCellContainer Name "
                << m_caloCellsOutputKey.key() );
  if (m_ownPolicy == SG::OWN_ELEMENTS) {
    ATH_MSG_INFO( "...will OWN its cells." );
  } else {
    ATH_MSG_INFO( "...will VIEW its cells." );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode CaloConstCellMaker::execute (const EventContext& ctx) const
{
  // Create empty container.

  SG::WriteHandle<CaloConstCellContainer> theContainer( m_caloCellsOutputKey, ctx);
  ATH_CHECK( theContainer.record (std::make_unique<CaloConstCellContainer>(static_cast<SG::OwnershipPolicy>(m_ownPolicy))) );

  // Loop on tools.
  // Note that finalization and checks are also done with tools.
  for (auto& tool : m_caloCellMakerTools) {
    // For performance reasons want to remove the cell-checker tool
    // from the list of tools after the fifth event.
    if (ctx.evt() > 5) {
      if (tool.typeAndName() == "CaloCellContainerCheckerTool/CaloCellContainerCheckerTool")
        continue;
    }

    ATH_MSG_DEBUG( "Calling tool " << tool.name() );

    std::string chronoName = this->name() + "_" + tool.name();

    StatusCode sc;
    sc.ignore();
    {
      Athena::Chrono (chronoName, &*m_chrono);
      sc = tool->process(theContainer.ptr(), ctx);
    }

    ATH_MSG_DEBUG( "Chrono stop : delta "
                   << m_chrono->chronoDelta(chronoName, IChronoStatSvc::USER) * (microsecond / second)
        << " second " );

    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error executing tool " << tool.name() );
    }
  }


  return StatusCode::SUCCESS;
}

