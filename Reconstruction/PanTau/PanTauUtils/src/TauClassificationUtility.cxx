/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauClassificationUtility.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PanTauEvent/TauSeed.h"
#include "TauTrackEvent/TruthTau.h"
#include "PanTauUtils/TauClassificationUtility.h"
#include "GaudiKernel/MsgStream.h"
#include "tauEvent/TauJet.h"
#include "PanTauEvent/PanTauDetails.h"

PanTau::TauClassificationTypes::TruthTauType PanTau::TauClassificationUtility::getTruthTauType(const TauID::TruthTau& truthTau, MsgStream& log) const {
    TauClassificationTypes::TruthTauType truthTypeIndex = TauClassificationTypes::NumberOfTruthTauTypes;

    // For the moment we put all decays with Kaons in the Kaon class and do not count them as 1- or 3-prong!

    if (truthTau.type(TauID::TruthTau::Kaon)) {
        truthTypeIndex = TauClassificationTypes::TruthKaon;
    } else if (truthTau.type(TauID::TruthTau::OneProng)) {
        truthTypeIndex = (truthTau.type(TauID::TruthTau::Neutrals) ? TauClassificationTypes::TruthOneProngNeutrals : TauClassificationTypes::TruthOneProng);
    } else if (truthTau.type(TauID::TruthTau::ThreeProng)) {
        truthTypeIndex = (truthTau.type(TauID::TruthTau::Neutrals) ? TauClassificationTypes::TruthThreeProngNeutrals : TauClassificationTypes::TruthThreeProng);
    } else if (truthTau.type(TauID::TruthTau::Electron)) {
        truthTypeIndex = TauClassificationTypes::TruthElectron;
    } else if (truthTau.type(TauID::TruthTau::Muon)) {
        truthTypeIndex = TauClassificationTypes::TruthMuon;
    } else if (truthTau.type(TauID::TruthTau::FiveProng)) {
        truthTypeIndex = TauClassificationTypes::TruthOther;
    } else {
        log << MSG::WARNING  << "Something is wrong, could not identify truth tau type of " << truthTau.dumpType() << " BC =" << truthTau.barcode() << endmsg;
        log << MSG::WARNING  << "Resonance: " << truthTau.resonancePdgId() << ", charged daughters: " << endmsg;
        std::vector< const TruthParticle * >::const_iterator chargedProdIter = truthTau.chargedProducts().begin();
        for (; chargedProdIter != truthTau.chargedProducts().end(); chargedProdIter++) {
            log << MSG::WARNING  << "  * BC " << (*chargedProdIter)->barcode() << ", PDG ID " << (*chargedProdIter)->pdgId() << endmsg;
        }
    }
    return truthTypeIndex;
}

PanTau::TauClassificationTypes::PanTauRecoMode PanTau::TauClassificationUtility::getSeedTauType(const PanTau::TauSeed& seed, MsgStream& log) const
{
    TauClassificationTypes::PanTauRecoMode seedTypeIndex = seed.getRecoModeLoose();
    log << MSG::VERBOSE << "tau type is: " << seedTypeIndex << endmsg;
    return seedTypeIndex;
}

PanTau::TauClassificationTypes::PanTauRecoMode PanTau::TauClassificationUtility::getSeedTauType(const Analysis::TauJet& tauJet, MsgStream& log) const {

    TauClassificationTypes::PanTauRecoMode  candTypeIndex = TauClassificationTypes::Reco_Error;

    const PanTauDetails* tauDetails = tauJet.details<PanTauDetails>();
    if (tauDetails) {
        // the PanTau case: Use PanTau::TauSeed to identify the type
        const TauSeed* tauSeed = tauDetails->seed();
        if (!tauSeed) {
            log << MSG::WARNING << "Could not get TauSeed for PanTau TauJet! Cannot identify its reconstructed type" << endmsg;
        } else {
            candTypeIndex = this->getSeedTauType(*tauSeed, log);
        } // end if (!tauSeed)
    } 

    return candTypeIndex;
}
