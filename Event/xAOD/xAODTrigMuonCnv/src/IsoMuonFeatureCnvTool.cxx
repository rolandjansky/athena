/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuon.h"

#include "TrigMuonEvent/IsoMuonFeature.h"

// Local include(s):
#include "IsoMuonFeatureCnvTool.h"


namespace xAODMaker {
    
    IsoMuonFeatureCnvTool::IsoMuonFeatureCnvTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
    : AthAlgTool( type, name, parent ) {
        
        // Declare the interface(s) provided by the tool:
        declareInterface< IIsoMuonFeatureCnvTool >( this );
    }
    
    StatusCode IsoMuonFeatureCnvTool::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
    StatusCode IsoMuonFeatureCnvTool::convert( const IsoMuonFeature* aod,
                                           xAOD::L2IsoMuon* xaod ) {
        
        xaod->setPt          (aod->pt());
        xaod->setEta         (aod->eta());
        xaod->setPhi         (aod->phi());
        xaod->setRoiWord     (aod->getRoiIdMu());
        xaod->setCharge      (aod->charge());
        xaod->setErrorFlag   (aod->getFlag());
        xaod->setSumPt01     (aod->getsumpt01());
        xaod->setSumPt02     (aod->getsumpt02());
        xaod->setSumPt03     (aod->getsumpt03());
        xaod->setSumPt04     (aod->getsumpt04());
        xaod->setSumEt01     (aod->getsumet01());
        xaod->setSumEt02     (aod->getsumet02());
        xaod->setSumEt03     (aod->getsumet03());
        xaod->setSumEt04     (aod->getsumet04());
        
	ATH_MSG_VERBOSE("Orig vs xAOD: Pt:      " << aod->pt() << " " << xaod->pt());
	ATH_MSG_VERBOSE("Orig vs xAOD: Eta:     " << aod->eta() << " " << xaod->eta());
	ATH_MSG_VERBOSE("Orig vs xAOD: phi:     " << aod->phi() << " " << xaod->phi());
	ATH_MSG_VERBOSE("Orig vs xAOD: roiWord: " << aod->getRoiIdMu() << " " << xaod->roiWord());
	ATH_MSG_VERBOSE("Orig vs xAOD: charge:  " << aod->charge() << " " << xaod->charge());
	ATH_MSG_VERBOSE("Orig vs xAOD: flag:    " << aod->getFlag() << " " << xaod->errorFlag());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumpt01: " << aod->getsumpt01() << " " << xaod->sumPt01());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumpt02: " << aod->getsumpt02() << " " << xaod->sumPt02());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumpt03: " << aod->getsumpt03() << " " << xaod->sumPt03());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumpt04: " << aod->getsumpt04() << " " << xaod->sumPt04());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumet01: " << aod->getsumet01() << " " << xaod->sumEt01());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumet02: " << aod->getsumet02() << " " << xaod->sumEt02());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumet03: " << aod->getsumet03() << " " << xaod->sumEt03());
	ATH_MSG_VERBOSE("Orig vs xAOD: sumet04: " << aod->getsumet04() << " " << xaod->sumEt04());

        ATH_MSG_DEBUG("Note: some of the original infos considered obsolete not implemented");
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
} // xAODMaker

