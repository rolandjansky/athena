/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************


 NAME:     CaloCellMaker.h
 PACKAGE:  offline/Calorimeter/CaloRec

 AUTHORS:  David Rousseau
 CREATED:  May 11, 2004

 PURPOSE:  Create a CaloCellContainer by calling a set of tools
 sharing interface CaloUtils/ICaloCellMakerTool.h
 FIXME : should see how Chronostat and MemStat report could still be obtained
 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/IChronoStatSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

// Calo includes
#include "CaloRec/CaloCellMaker.h"
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
    : AthAlgorithm(name, pSvcLocator)
  , m_chrono("ChronoStatSvc", name)
  , m_ownPolicy(static_cast<int>(SG::VIEW_ELEMENTS))
  , m_caloCellsOutputName("")
  , m_caloCellHack(false)
  , m_caloCellMakerTools()
  , m_evCounter(0)
{
  declareProperty("OwnPolicy", m_ownPolicy);
  declareProperty("CaloCellMakerToolNames", m_caloCellMakerTools);
  declareProperty("CaloCellsOutputName", m_caloCellsOutputName);
  declareProperty("CaloCellHack", m_caloCellHack);
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

  // Add ":PUBLIC" to the tool names to force ToolSvc into creating
  // public tools.
  std::vector< std::string > typesAndNames;
  for( const std::string& typeName : m_caloCellMakerTools.typesAndNames() ) {
     typesAndNames.push_back( typeName + ":PUBLIC" );
  }
  m_caloCellMakerTools.setTypesAndNames( typesAndNames );

  // access tools and store them
  CHECK( m_caloCellMakerTools.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerTools );

  ATH_MSG_INFO( " Output CaloCellContainer Name " << m_caloCellsOutputName );
  if (m_ownPolicy == SG::OWN_ELEMENTS) {
    ATH_MSG_INFO( "...will OWN its cells." );
  } else {
    ATH_MSG_INFO( "...will VIEW its cells." );
  }

  if (m_caloCellHack) {
    ATH_MSG_WARNING( " CaloCellContainer: " << m_caloCellsOutputName
                    << "will be read in and modified !. To be used with care. " );
  }

  return StatusCode::SUCCESS;

}

StatusCode CaloCellMaker::execute() {

  // create and record the empty container
  CaloCellContainer * theContainer;

  if (!m_caloCellHack) {

    theContainer = new CaloCellContainer(static_cast<SG::OwnershipPolicy>(m_ownPolicy));

    if (evtStore()->record(theContainer, m_caloCellsOutputName).isFailure()) {
      ATH_MSG_WARNING( "execute() : cannot record CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::SUCCESS;
    }

    // also symLink as INavigable4MomentumCollection!
    INavigable4MomentumCollection* theNav4Coll = 0;

    if (evtStore()->symLink(theContainer, theNav4Coll).isFailure()) {
      ATH_MSG_WARNING( "Error symlinking CaloCellContainer to INavigable4MomentumCollection " );
      return StatusCode::SUCCESS;
    }

  } else {
    // take CaloCellContainer from input and cast away constness
    const CaloCellContainer * theConstContainer;

    if (evtStore()->retrieve(theConstContainer, m_caloCellsOutputName).isFailure()
        || theConstContainer == 0) {

      ATH_MSG_WARNING( "Could not retrieve CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::SUCCESS;
    }
    theContainer = const_cast<CaloCellContainer *>(theConstContainer);

  }

  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerTools.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerTools.end();

  //The following piece of code would be even more efficient but unfortunately the 
  //ToolHandleArray doesn't allow to remove elements.

  //  // For performance reasons want to remove the cell-checker tool from the list
//   // of tools at the fifth event.
//   if ((++m_evCounter)==5) {
//     //Search for cell-checker tool in list of tools
//     for (;itrTool!=endTool && itrTool->typeAndName()!="CaloCellContainerCheckerTool/CaloCellContainerCheckerTool";++itrTool);
//     if (itrTool!=endTool) { //Found cell-checker tool
//       m_caloCellMakerTools.erase(itrTool); //Remove cell-checker tool
//       //Re-initialize the iterators
//       itrTool=m_caloCellMakerTools.begin();
//       endTool=m_caloCellMakerTools.end();
//       ATH_MSG_INFO( "Remove CellChecking tool from list of tools" );
//       ATH_MSG_DEBUG( "Tools left: " << m_caloCellMakerTools );
//     }
//   }

  // loop on tools
  // note that finalization and checks are also done with tools
  ++m_evCounter;
  for (; itrTool != endTool; ++itrTool) {
    if (m_evCounter > 5) {
      if (itrTool->typeAndName() == "CaloCellContainerCheckerTool/CaloCellContainerCheckerTool")
        continue;
    }

    ATH_MSG_DEBUG( "Calling tool " << itrTool->name() );

    std::string chronoName = this->name() + "_" + itrTool->name();

    m_chrono->chronoStart(chronoName);
    StatusCode sc = (*itrTool)->process(theContainer);
    m_chrono->chronoStop(chronoName);

    ATH_MSG_DEBUG( "Chrono stop : delta "
                   << m_chrono->chronoDelta(chronoName, IChronoStatSvc::USER) * (microsecond / second)
        << " second " );

    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Error executing tool " << itrTool->name() );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloCellMaker::finalize() {

  return StatusCode::SUCCESS;

}

