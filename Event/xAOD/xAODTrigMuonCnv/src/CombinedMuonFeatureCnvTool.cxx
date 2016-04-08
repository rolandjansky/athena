/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"

// Local include(s):
#include "CombinedMuonFeatureCnvTool.h"


namespace xAODMaker {
    
    CombinedMuonFeatureCnvTool::CombinedMuonFeatureCnvTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
    : AthAlgTool( type, name, parent ) {
        
        // Declare the interface(s) provided by the tool:
        declareInterface< ICombinedMuonFeatureCnvTool >( this );
    }
    
    StatusCode CombinedMuonFeatureCnvTool::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
    StatusCode CombinedMuonFeatureCnvTool::convert( const CombinedMuonFeature* aod,
                                           xAOD::L2CombinedMuon* xaod ) {
        
        xaod->setPt          (aod->pt());
        xaod->setEta         (aod->eta());
        xaod->setPhi         (aod->phi());
        xaod->setCharge      (aod->charge());
        xaod->setStrategy    (aod->comb_strategy());
        xaod->setErrorFlag   (aod->comb_errorFlag());
        xaod->setMatchFlag   (aod->comb_matchFlag());
        xaod->setSigmaPt     (aod->sigma_pt());
         
	ATH_MSG_VERBOSE("Orig vs xAOD: Pt:       " << aod->pt() << " " << xaod->pt());
	ATH_MSG_VERBOSE("Orig vs xAOD: Eta:      " << aod->eta() << " " << xaod->eta());
	ATH_MSG_VERBOSE("Orig vs xAOD: phi:      " << aod->phi() << " " << xaod->phi());
	ATH_MSG_VERBOSE("Orig vs xAOD: charge:   " << aod->charge() << " " << xaod->charge());
	ATH_MSG_VERBOSE("Orig vs xAOD: strategy: " << aod->comb_strategy() << " " << xaod->strategy());
	ATH_MSG_VERBOSE("Orig vs xAOD: flag:     " << aod->comb_errorFlag() << " " << xaod->errorFlag());
	ATH_MSG_VERBOSE("Orig vs xAOD: match:    " << aod->comb_matchFlag() << " " << xaod->matchFlag());
	ATH_MSG_VERBOSE("Orig vs xAOD: sigmaPt:  " << aod->sigma_pt() << " " << xaod->sigmaPt());

        ATH_MSG_DEBUG("Note: elinks to muon and ID track not implemented yet");
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
} // xAODMaker

