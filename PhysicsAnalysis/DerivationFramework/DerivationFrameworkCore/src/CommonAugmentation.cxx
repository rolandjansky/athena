/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CommonAugmentation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This code loops over common tools from the CP groups, which write
// into SG so that other derivations can make use of them without
// re-running the tools. 

#include "DerivationFrameworkCore/CommonAugmentation.h"

#include <sstream>                                      // C++ utilities
#include <string>
#include <algorithm>
#include <fstream>

#include "GaudiKernel/ISvcLocator.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "GaudiKernel/Chrono.h"

#include "StoreGate/StoreGateSvc.h"             // Storegate stuff
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/DefaultKey.h"
#include "SGTools/StlVectorClids.h"
//#include "AthenaRootComps/TransferTree.h"

///////////////////////////////////////////////////////////////////////////////

DerivationFramework::CommonAugmentation::CommonAugmentation(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_chronoSvc("ChronoStatSvc", name) 
//m_ometaStore("StoreGateSvc/MetaDataStore", name )
{
    // ------- Python changeable properties -------
    declareProperty("AugmentationTools",     m_augmentationTools);
    //declareProperty("OutputMetaStore",       m_ometaStore);
    //---------------------------------------------
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::CommonAugmentation::initialize() {
    
    ATH_MSG_INFO("Initializing the common selections in " << name());
    
    // get the tool service
    IToolSvc* toolSvc;
    StatusCode sc = service("ToolSvc",toolSvc);
    if (StatusCode::SUCCESS != sc) {
        ATH_MSG_FATAL("Unable to retrieve ToolSvc");
        return StatusCode::FAILURE;
    }
    
    // get the augmentation tools
    if( m_augmentationTools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve augmentation tools");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("The following augmentation tools will be applied....");
        ATH_MSG_INFO(m_augmentationTools);
    }
 
    // get the chrono auditor
    if ( m_chronoSvc.retrieve().isFailure() ) {
	    ATH_MSG_FATAL("Failed to retrieve service " << m_chronoSvc);
	    return StatusCode::FAILURE;
    } 

    // Set accumulation variables to zero
    //m_eventCount = 0;
    //m_eventWeights = 0.0;

    return StatusCode::SUCCESS;
    
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::CommonAugmentation::execute() {
    
    // On your marks.... get set....
    Chrono chrono( &(*m_chronoSvc), name() ); 
    // GO!!

    //=============================================================================
    // AUGMENTATION ===============================================================
    //=============================================================================
    
    ToolHandleArray<IAugmentationTool>::iterator augmentationTool(m_augmentationTools.begin());
    ToolHandleArray<IAugmentationTool>::iterator endOfAugmentationTools(m_augmentationTools.end());
    while (augmentationTool != endOfAugmentationTools) {
    	if ( (**augmentationTool).addBranches().isFailure() ) {
		ATH_MSG_ERROR("Augmentation failed!");
		return StatusCode::FAILURE;
	}
        ++augmentationTool;
    }
    
    //++m_eventCount;
    //float* weight;
    //if (evtStore()->contains<float>("mc_event_weight")) { 
    //	CHECK(evtStore()->retrieve((const float*&)weight,"mc_event_weight"));
   //	m_eventWeights += *weight;
    //}
    return StatusCode::SUCCESS;
        
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
//StatusCode DerivationFramework::CommonAugmentation::stop() {
//
//    TTree* tree = new TTree("DerivationTree","Extra metadata from derivation framework");
//    tree->Branch("totalEventsProcessed",&m_eventCount,"totalEventsProcessed/I");
//    tree->Branch("summedEventWeights",&m_eventWeights,"summedEventWeights/F");
//    tree->Fill();
//    TransferTree* temp = new TransferTree(tree);
//    if (m_ometaStore->record(temp,"ExtraMetadata").isFailure()) ATH_MSG_ERROR("Unable to record metadata tree " << tree->GetName());
//    std::cout << "Just filled " <<  tree->GetName() << " with " << m_eventCount << " and " << m_eventWeights << std::endl;
//    tree->Print();
//    return StatusCode::SUCCESS; 
//
//}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::CommonAugmentation::finalize() {
    
    ATH_MSG_INFO( "============================================================================");
    ATH_MSG_INFO( " The following CP tools were called by " << name() << " for the whole train:");
    ToolHandleArray<IAugmentationTool>::iterator augmentationTool(m_augmentationTools.begin());
    ToolHandleArray<IAugmentationTool>::iterator endOfAugmentationTools(m_augmentationTools.end());
    while (augmentationTool != endOfAugmentationTools) { 
        ATH_MSG_INFO ( (**augmentationTool).name() );
        ++augmentationTool;
    }
    ATH_MSG_INFO( "============================================================================");
    
    return StatusCode::SUCCESS;
    
}
