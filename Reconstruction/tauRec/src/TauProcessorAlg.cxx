/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRec/TauProcessorAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"


#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

using Gaudi::Units::GeV;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauProcessorAlg::TauProcessorAlg(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator) {
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauProcessorAlg::~TauProcessorAlg() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::initialize() {
  ATH_CHECK( detStore()->retrieve(m_cellID) );
    
    ATH_CHECK( m_jetInputContainer.initialize() );
    ATH_CHECK( m_tauOutputContainer.initialize() );
    ATH_CHECK( m_tauTrackOutputContainer.initialize() );
    ATH_CHECK( m_tauShotClusOutputContainer.initialize() );
    ATH_CHECK( m_tauShotPFOOutputContainer.initialize() );
    ATH_CHECK( m_tauPi0CellOutputContainer.initialize() );

    ATH_CHECK( m_cellMakerTool.retrieve() );

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    //-------------------------------------------------------------------------
    // Allocate tools
    //-------------------------------------------------------------------------
    ATH_CHECK( m_tools.retrieve() );
    
    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");

    unsigned int tool_count = 0;

    for (ToolHandle<ITauToolBase>& tool : m_tools) {
      ++tool_count;
      ATH_MSG_INFO(tool->type() << " - " << tool->name());
    }
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("------------------------------------");

    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::finalize() {

  StatusCode sc;

  //-----------------------------------------------------------------
  // Loop stops when Failure indicated by one of the tools
  //-----------------------------------------------------------------
  for (ToolHandle<ITauToolBase>& tool : m_tools) {
    ATH_MSG_VERBOSE("Invoking tool " << tool->name());
    sc = tool->finalize();
    if (sc.isFailure()) break;
  }

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("All the invoded tools are finilized successfully.");
  } 

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::execute() {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  /// record output containers
  SG::WriteHandle<xAOD::TauJetContainer> tauHandle( m_tauOutputContainer, ctx );
  ATH_CHECK(tauHandle.record(std::make_unique<xAOD::TauJetContainer>(), std::make_unique<xAOD::TauJetAuxContainer>()));
  xAOD::TauJetContainer* pContainer = tauHandle.ptr();

  SG::WriteHandle<xAOD::TauTrackContainer> tauTrackHandle( m_tauTrackOutputContainer, ctx );
  ATH_CHECK(tauTrackHandle.record(std::make_unique<xAOD::TauTrackContainer>(), std::make_unique<xAOD::TauTrackAuxContainer>()));
  xAOD::TauTrackContainer* pTauTrackCont = tauTrackHandle.ptr();

  SG::WriteHandle<xAOD::CaloClusterContainer> tauShotClusHandle( m_tauShotClusOutputContainer, ctx );
  ATH_CHECK(tauShotClusHandle.record(std::make_unique<xAOD::CaloClusterContainer>(), std::make_unique<xAOD::CaloClusterAuxContainer>()));
  xAOD::CaloClusterContainer* tauShotClusContainer = tauShotClusHandle.ptr();

  SG::WriteHandle<xAOD::PFOContainer> tauShotPFOHandle( m_tauShotPFOOutputContainer, ctx );
  ATH_CHECK(tauShotPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
  xAOD::PFOContainer* tauShotPFOContainer = tauShotPFOHandle.ptr();

  SG::WriteHandle<CaloCellContainer> tauPi0CellHandle( m_tauPi0CellOutputContainer, ctx );
  ATH_CHECK(tauPi0CellHandle.record(std::make_unique<CaloCellContainer>()));
  CaloCellContainer* Pi0CellContainer = tauPi0CellHandle.ptr();

  /// retrieve the input jet seed container
  SG::ReadHandle<xAOD::JetContainer> jetHandle( m_jetInputContainer, ctx );
  if (!jetHandle.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << jetHandle.key());
    return StatusCode::FAILURE;
  }
  const xAOD::JetContainer *pSeedContainer = jetHandle.cptr();
  
  /// Initialize the cell map per event, used to avoid dumplicate cell  in TauPi0CreateROI
  IdentifierHash hashMax = m_cellID->calo_cell_hash_max(); 
  ATH_MSG_DEBUG("CaloCell Hash Max: " << hashMax);
  std::vector<CaloCell*> addedCellsMap;
  addedCellsMap.resize(hashMax,NULL);
  
  //---------------------------------------------------------------------                                                        
  // Loop over seeds
  //---------------------------------------------------------------------                                                 
  ATH_MSG_VERBOSE("Number of seeds in the container: " << pSeedContainer->size());
    
  for (const xAOD::Jet* pSeed : *pSeedContainer) {
    ATH_MSG_VERBOSE("Seeds eta:" << pSeed->eta() << ", pt:" << pSeed->pt());

    if (std::abs(pSeed->eta()) > m_maxEta) {
      ATH_MSG_VERBOSE("--> Seed rejected, eta out of range!");
      continue;
    }

    if (pSeed->pt() < m_minPt) {
      ATH_MSG_VERBOSE("--> Seed rejected, pt out of range!");
      continue;
    }

      //-----------------------------------------------------------------                                                                 
      // Seed passed cuts --> create tau candidate
      //-----------------------------------------------------------------                                                                           
      xAOD::TauJet* pTau = new xAOD::TauJet();
      pContainer->push_back( pTau );
      pTau->setJet(pSeedContainer, pSeed);
      
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      StatusCode sc;
      for (ToolHandle<ITauToolBase>& tool : m_tools) {
	ATH_MSG_DEBUG("ProcessorAlg Invoking tool " << tool->name());

	if (tool->type() == "TauVertexFinder" ) {
	  sc = tool->executeVertexFinder(*pTau);
	}
	else if ( tool->type() == "TauTrackFinder") {
	  sc = tool->executeTrackFinder(*pTau, *pTauTrackCont);
	}
	else if ( tool->type() == "tauRecTools::TauTrackClassifier") {
	  sc = tool->executeTrackClassifier(*pTau, *pTauTrackCont);
	}
	else if ( tool->type() == "tauRecTools::TauTrackRNNClassifier") {
	  sc = tool->executeRNNTrackClassifier(*pTau, *pTauTrackCont);
	}
	else if ( tool->type() == "TauShotFinder"){
	  sc = tool->executeShotFinder(*pTau, *tauShotClusContainer, *tauShotPFOContainer);
	}
	else if ( tool->type() == "TauPi0CreateROI"){
	  sc = tool->executePi0CreateROI(*pTau, *Pi0CellContainer, addedCellsMap);
	}
	else {
	  sc = tool->execute(*pTau);
	}
	if (sc.isFailure())  break;
      }

      if (sc.isSuccess()) {
	ATH_MSG_VERBOSE("The tau candidate has been registered");
      } 
      else {
	//remove orphaned tracks before tau is deleted via pop_back
	xAOD::TauJet* bad_tau = pContainer->back();
	ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	pTauTrackCont->erase(pTauTrackCont->end()-bad_tau->nAllTracks(), pTauTrackCont->end());

	pContainer->pop_back();
      } 
    }// loop through seeds

    // Check this is needed for the cell container?
    // symlink as INavigable4MomentumCollection (as in CaloRec/CaloCellMaker)
    ATH_CHECK(evtStore()->symLink(Pi0CellContainer, static_cast<INavigable4MomentumCollection*> (0)));
  
    // sort the cell container by hash
    ATH_CHECK( m_cellMakerTool->process(static_cast<CaloCellContainer*> (Pi0CellContainer), ctx) );

  ATH_MSG_VERBOSE("The tau candidate container has been modified");
  
  return StatusCode::SUCCESS;
}

