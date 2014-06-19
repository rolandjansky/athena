/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

#include "TrigParticle/TrigL2Bphys.h"

// Local include(s):
#include "TrigL2BphysCnvTool.h"


namespace xAODMaker {
    
    TrigL2BphysCnvTool::TrigL2BphysCnvTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
    : AthAlgTool( type, name, parent ) {
        
        // Declare the interface(s) provided by the tool:
        declareInterface< ITrigL2BphysCnvTool >( this );
    }
    
    StatusCode TrigL2BphysCnvTool::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
    StatusCode TrigL2BphysCnvTool::convert( const TrigL2Bphys* aod,
                                           xAOD::TrigBphys* xaod ) {
        
        xaod->setEta         (aod->eta());
        xaod->setPhi         (aod->phi());
        xaod->setMass        (aod->mass());
        xaod->setRoiId       (aod->roiId());
        xaod->setParticleType((xAOD::TrigBphys::pType)aod->particleType());
        xaod->setLevel       (xAOD::TrigBphys::levelType::L2);
        xaod->setFitmass     (aod->fitmass());
        xaod->setFitchi2     (aod->fitchi2());
        xaod->setFitndof     (aod->fitndof());
        xaod->setFitx        (aod->fitx());
        xaod->setFity        (aod->fity());
        xaod->setFitz        (aod->fitz());
        
        //xaod->addTrack       (aod->());
        //xaod->pSecondDecay   (aod->());
        
	ATH_MSG_VERBOSE("Orig vs xAOD: Eta:   " << aod->eta() << " " << xaod->eta());
	ATH_MSG_VERBOSE("Orig vs xAOD: phi:   " << aod->phi() << " " << xaod->phi());
        ATH_MSG_VERBOSE("Orig vs xAOD: mass:  " << aod->mass() << " " << xaod->mass());
	ATH_MSG_VERBOSE("Orig vs xAOD: roiId: " << aod->roiId() << " " << xaod->roiId());

	ATH_MSG_VERBOSE("Orig vs xAOD: : particleType: " << aod->particleType() << " " << xaod->particleType());
	ATH_MSG_VERBOSE("Orig vs xAOD: : level: " << "L2" << " " << xaod->level());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fitmass: " << aod->fitmass() << " " << xaod->fitmass());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fitchi2: " << aod->fitchi2() << " " << xaod->fitchi2());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fitndf:  " << aod->fitndof() << " " << xaod->fitndof());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fitx     " << aod->fitx() << " " << xaod->fitx());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fity     " << aod->fity() << " " << xaod->fity());
	ATH_MSG_VERBOSE("Orig vs xAOD: : fitz     " << aod->fitz() << " " << xaod->fitz());

        
        ATH_MSG_DEBUG("No track or secondary decay implemented");
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    
} // xAODMaker

