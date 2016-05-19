/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/errorcheck.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloUtils/CaloCollectionHelper.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloRec/CaloTowerAlgorithm.h"

#include <string>
#include <vector>
#include <iomanip>

CaloTowerAlgorithm::CaloTowerAlgorithm(const std::string& name, 
				       ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name,pSvcLocator)
  , m_nEtaTowers(50)
  , m_nPhiTowers(64)
  , m_minEta(-2.5)
  , m_maxEta(2.5)
  , m_genericLink(true) 
  , m_ptools( this )
{
  // tool names
  //declareProperty("TowerBuilderTools",m_toolNames);
  declareProperty("TowerBuilderTools",m_ptools);
  // output data
  declareProperty("TowerContainerName",m_towerContainerName);
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
  CHECK(service("ToolSvc",myToolSvc));

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


  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator firstITool = m_ptools.begin();
  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator lastITool  = m_ptools.end();

  unsigned int toolCtr = 0;
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("List of tools in execution sequence:");
  ATH_MSG_INFO("------------------------------------");

  CHECK(m_ptools.retrieve());

  for (; firstITool != lastITool; firstITool++) {
    toolCtr++;

    ATH_MSG_INFO(std::setw(2) << toolCtr << ".) " << (*firstITool)->type()
        << "::name() = \042" << (*firstITool)->name() << "\042");

    // reset statistics
    m_toolInvoke[(*firstITool)->name()] = 0;
    m_toolReject[(*firstITool)->name()] = 0;
    m_toolAccept[(*firstITool)->name()] = 0;

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

StatusCode CaloTowerAlgorithm::execute()
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
  CaloTowerContainer* theTowers = new CaloTowerContainer(theTowerSeg);

  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator firstITool  = m_ptools.begin();
  ToolHandleArray<ICaloTowerBuilderToolBase>::const_iterator lastITool   = m_ptools.end();
  StatusCode processStatus = StatusCode::SUCCESS;
  //
  // loop stops only when Failure indicated by one of the tools
  //

  ATH_MSG_DEBUG("In execute() ");
  
  while (!processStatus.isFailure() && firstITool != lastITool) {

    m_toolInvoke[(*firstITool)->name()]++;
    if (theTicker != 0) {
      theTicker->chronoStart((*firstITool)->name());
    }

    processStatus = (*firstITool)->execute(theTowers);

    if (theTicker != 0) {
      theTicker->chronoStop((*firstITool)->name());
    }
    if (!processStatus.isFailure()) {
      ATH_MSG_DEBUG((*firstITool)->name()
          << ": CaloTowerContainer::size() = " << theTowers->size());

      m_toolAccept[(*firstITool)->name()]++;
      firstITool++;
    } else {
      // some problem - but do not skip event loop!
      ATH_MSG_ERROR("problems while or after processing tool \042"
          << (*firstITool)->name()
          << "\042 - cross-check CaloTowerContainer::size() = "
          << theTowers->size());

      m_toolReject[(*firstITool)->name()]++;
      firstITool++;
    }
  }

  const bool allowMods(false);
  if ((evtStore()->record(theTowers, m_towerContainerName, allowMods)).isSuccess()) {
    const INavigable4MomentumCollection* theNav4Coll = 0;
    return evtStore()->symLink(theTowers, theNav4Coll);
  } else {
    return StatusCode::FAILURE;
  }
}

//////////////
// Finalize //
//////////////

StatusCode CaloTowerAlgorithm::finalize()
{

  ////////////////
  // Tool Stats //
  ////////////////
  
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("Summary of Tool invocation: (invoked/success/failure)");
  ATH_MSG_INFO("---------------------------");

  tool_stats_iterator firstName = m_toolInvoke.begin();
  tool_stats_iterator lastName  = m_toolInvoke.end();
  unsigned int toolCtr = 0;
  for (; firstName != lastName; firstName++) {
      toolCtr++;
    ATH_MSG_INFO(std::setw(2) << toolCtr << ".) "
        << std::setw(36) //<< std::setfill(".")
        << (*firstName).first << " (" << (*firstName).second << "/"
        << m_toolAccept[(*firstName).first] << "/"
        << m_toolReject[(*firstName).first] << ")");
  }
  ATH_MSG_INFO("---------------------------");
  ATH_MSG_INFO(" ");
 
  return StatusCode::SUCCESS;
}
