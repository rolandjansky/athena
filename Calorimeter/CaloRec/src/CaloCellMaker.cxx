/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************


 NAME:     CaloCellMaker.h
 PACKAGE:  offline/Calorimeter/CaloRec

 AUTHORS:  David Rousseau
 CREATED:  May 11, 2004

 PURPOSE:  Create a CaloCellContainer by calling a set of tools
 sharing interface CaloInterface/ICaloCellMakerTool.h
 FIXME : should see how Chronostat and MemStat report could still be obtained
 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/IChronoStatSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "StoreGate/WriteHandle.h"

// Calo includes
#include "CaloCellMaker.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::microsecond;
using CLHEP::second;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellMaker::CaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
  , m_chrono("ChronoStatSvc", name)
  , m_ownPolicy(static_cast<int>(SG::VIEW_ELEMENTS))
  , m_caloCellsOutputKey("")
      //, m_caloCellHack(false)
  , m_caloCellMakerTools(this)
      //, m_evCounter(0)
{
  declareProperty("OwnPolicy", m_ownPolicy);
  declareProperty("CaloCellMakerToolNames", m_caloCellMakerTools);
  declareProperty("CaloCellsOutputName", m_caloCellsOutputKey);
  //declareProperty("CaloCellHack", m_caloCellHack);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellMaker::~CaloCellMaker() {
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellMaker::initialize() {

  CHECK( m_chrono.retrieve() );

  // // Add ":PUBLIC" to the tool names to force ToolSvc into creating
  // // public tools.
  // std::vector< std::string > typesAndNames;
  // for( const std::string& typeName : m_caloCellMakerTools.typesAndNames() ) {
  //    typesAndNames.push_back( typeName + ":PUBLIC" );
  // }
  // m_caloCellMakerTools.setTypesAndNames( typesAndNames );

  // access tools and store them
  CHECK( m_caloCellMakerTools.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerTools );

  ATH_CHECK(m_caloCellsOutputKey.initialize());
  ATH_MSG_INFO( " Output CaloCellContainer Name " << m_caloCellsOutputKey.key() );
  if (m_ownPolicy == SG::OWN_ELEMENTS) {
    ATH_MSG_INFO( "...will OWN its cells." );
  } else {
    ATH_MSG_INFO( "...will VIEW its cells." );
  }

  // if (m_caloCellHack) {
  //   ATH_MSG_WARNING( " CaloCellContainer: " << m_caloCellsOutputName
  //                   << "will be read in and modified !. To be used with care. " );
  // }

  return StatusCode::SUCCESS;

}

StatusCode CaloCellMaker::execute (const EventContext& ctx) const {

  SG::WriteHandle<CaloCellContainer> caloCellsOutput(m_caloCellsOutputKey, ctx);

  ATH_CHECK( caloCellsOutput.record(std::make_unique<CaloCellContainer>(static_cast<SG::OwnershipPolicy>(m_ownPolicy))) );

  // loop on tools
  // note that finalization and checks are also done with tools
  for (const ToolHandle<ICaloCellMakerTool>& tool : m_caloCellMakerTools) {
    ATH_MSG_DEBUG( "Calling tool " << tool.name() );

    std::string chronoName = this->name() + "_" + tool.name();

    m_chrono->chronoStart(chronoName);
    StatusCode sc = tool->process(caloCellsOutput.ptr(), ctx);
    m_chrono->chronoStop(chronoName);

    ATH_MSG_DEBUG( "Chrono stop : delta "
                   << m_chrono->chronoDelta(chronoName, IChronoStatSvc::USER) * (microsecond / second)
        << " second " );

    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error executing tool " << tool.name() );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloCellMaker::finalize() {

  return StatusCode::SUCCESS;

}

