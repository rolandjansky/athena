/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "Gaudi/Property.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/errorcheck.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloUtils/CaloCollectionHelper.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloTowerAlgorithm.h"

#include <string>
#include <vector>
#include <iomanip>

CaloTowerAlgorithm::CaloTowerAlgorithm(const std::string& name, 
				       ISvcLocator* pSvcLocator) 
  : AthReentrantAlgorithm(name,pSvcLocator)
  , m_nEtaTowers(50)
  , m_nPhiTowers(64)
  , m_minEta(-2.5)
  , m_maxEta(2.5)
  , m_genericLink(true) 
  , m_ptools( this )
  , m_towerContainerKey("")
{
  // tool names
  //declareProperty("TowerBuilderTools",m_toolNames);
  declareProperty("TowerBuilderTools",m_ptools);
  // output data
  declareProperty("TowerContainerName",m_towerContainerKey);
  // tower grid
  declareProperty("NumberOfEtaTowers",m_nEtaTowers);
  declareProperty("NumberOfPhiTowers",m_nPhiTowers);
  declareProperty("EtaMin",m_minEta);
  declareProperty("EtaMax",m_maxEta);
  // linkable
  declareProperty("GenericLinked",m_genericLink);
}

CaloTowerAlgorithm::~CaloTowerAlgorithm()
{ }

////////////////
// Initialize //
////////////////

StatusCode CaloTowerAlgorithm::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // tool service
  IToolSvc* myToolSvc;
  ATH_CHECK(service("ToolSvc",myToolSvc));
  ATH_CHECK(m_towerContainerKey.initialize());
  ////////////////////
  // Allocate Tools //
  ////////////////////

  // check tool names
  if (m_ptools.size() == 0) {
    ATH_MSG_ERROR(" no tools given for this algorithm.");
    return StatusCode::FAILURE;
  }

  // find tools


  CaloTowerSeg theTowerSeg(m_nEtaTowers,m_nPhiTowers,m_minEta,m_maxEta);


  ToolHandleArray<ICaloTowerBuilderToolBase>::iterator firstITool = m_ptools.begin();
  ToolHandleArray<ICaloTowerBuilderToolBase>::iterator lastITool  = m_ptools.end();

  unsigned int toolCtr = 0;
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("List of tools in execution sequence:");
  ATH_MSG_INFO("------------------------------------");

  ATH_CHECK(m_ptools.retrieve());

  for (; firstITool != lastITool; firstITool++) {
    toolCtr++;

    ATH_MSG_INFO(std::setw(2) << toolCtr << ".) " << (*firstITool)->type()
        << "::name() = \042" << (*firstITool)->name() << "\042");

    ATH_MSG_INFO("------------------------------------");
    ATH_MSG_INFO(" ");

    ATH_MSG_DEBUG(" set correct tower seg for this tool "
        << (*firstITool)->name());

    (*firstITool)->setTowerSeg(theTowerSeg);

//    if ((*firstITool)->initializeTool().isFailure()) {
//      ATH_MSG_WARNING(" Tool failed to initialize");
//    }

  } //close iteration over tools
  return StatusCode::SUCCESS;
}

/////////////
// Execute //
/////////////

StatusCode CaloTowerAlgorithm::execute (const EventContext& ctx) const
{

  //////////////////////////
  // Re-allocate Services //
  //////////////////////////

  //timing
  IChronoStatSvc* theTicker = chronoSvc();

  /////////////////////
  // Tool Processing //
  /////////////////////

  CaloTowerSeg theTowerSeg(m_nEtaTowers,m_nPhiTowers,m_minEta,m_maxEta);

  SG::WriteHandle<CaloTowerContainer> theTowers(m_towerContainerKey, ctx);
  ATH_CHECK( theTowers.record(std::make_unique<CaloTowerContainer>(theTowerSeg)) );
  

  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator firstITool  = m_ptools.begin();
  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator lastITool   = m_ptools.end();
  StatusCode processStatus = StatusCode::SUCCESS;
  //
  // loop stops only when Failure indicated by one of the tools
  //

  ATH_MSG_DEBUG("In execute() ");
  
  while (!processStatus.isFailure() && firstITool != lastITool) {

    if (theTicker != 0) {
      theTicker->chronoStart((*firstITool)->name());
    }

    processStatus = (*firstITool)->execute(theTowers.ptr());

    if (theTicker != 0) {
      theTicker->chronoStop((*firstITool)->name());
    }
    if (!processStatus.isFailure()) {
      ATH_MSG_DEBUG((*firstITool)->name()
          << ": CaloTowerContainer::size() = " << theTowers->size());

      firstITool++;
    } else {
      // some problem - but do not skip event loop!
      ATH_MSG_ERROR("problems while or after processing tool \042"
          << (*firstITool)->name()
          << "\042 - cross-check CaloTowerContainer::size() = "
          << theTowers->size());

      firstITool++;
    }
  }

  return StatusCode::SUCCESS;
}

//////////////
// Finalize //
//////////////

StatusCode CaloTowerAlgorithm::finalize()
{
  return StatusCode::SUCCESS;
}
