/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! Gaudi includes
#include "AthenaKernel/errorcheck.h"

//! PanTau includes
#include "PanTauAlgs/Tool_InputConverter.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauEvent/TauConstituent.h"

//! input object headers
#include "eflowEvent/eflowObject.h"
#include "tauEvent/TauPi0Cluster.h"
#include "tauEvent/TauShot.h"
#include "tauEvent/TauJet.h"

#include "Particle/TrackParticle.h"

//! helpers
#include "FourMom/P4EEtaPhiM.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFODefs.h"


PanTau::Tool_InputConverter::Tool_InputConverter(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_HelperFunctions("PanTau::Tool_InformationStore/Tool_HelperFunctions")
{
    declareInterface<ITool_InputConverter>(this);

    declareProperty("Tool_InformationStore",     m_Tool_InformationStore, "Link to tool with all information");
    declareProperty("Tool_HelperFunctions",      m_Tool_HelperFunctions, "Link to tool with helper functions");
}

PanTau::Tool_InputConverter::~Tool_InputConverter() {
}

StatusCode PanTau::Tool_InputConverter::initialize() {

    StatusCode sc = AlgTool::initialize();
    ATH_MSG_INFO(" initialize()");
    
    CHECK( m_Tool_InformationStore.retrieve() );
    CHECK( m_Tool_HelperFunctions.retrieve() );
    
    CHECK( m_Tool_InformationStore->getInfo_Int("TauConstituents_UsePionMass", m_Config_UsePionMass) );
    CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_eflowRec_BDTThreshold_Pi0Neut", m_Config_eflowRec_MinBDTValue_Pi0Neut) );
    CHECK( m_Tool_InformationStore->getInfo_Int("TauConstituents_eflowRec_UseMomentumAsEnergy", m_Config_eflowRec_UseMomentumAsEnergy) );
    CHECK( m_Tool_InformationStore->getInfo_Int("TauConstituents_UseShrinkingCone", m_Config_TauConstituents_UseShrinkingCone) );
    
    
    CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_Types_DeltaRCore", m_Config_TauConstituents_Types_DeltaRCore) );
    CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_PreselectionMinEnergy", m_Config_TauConstituents_PreselectionMinEnergy) );
    
    
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_BinEdges_Eta", m_Config_CellBased_BinEdges_Eta) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_1prong", m_Config_CellBased_EtaBinned_Pi0MVACut_1prong) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("CellBased_EtaBinned_Pi0MVACut_3prong", m_Config_CellBased_EtaBinned_Pi0MVACut_3prong) );
    
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("eflowRec_BinEdges_Eta", m_Config_eflowRec_BinEdges_Eta) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("eflowRec_EtaBinned_Pi0MVACut_1prong", m_Config_eflowRec_EtaBinned_Pi0MVACut_1prong) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("eflowRec_EtaBinned_Pi0MVACut_3prong", m_Config_eflowRec_EtaBinned_Pi0MVACut_3prong) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong", m_Config_eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong", m_Config_eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong) );
    
    return sc;
}


// StatusCode PanTau::Tool_InformationStore::finalize() {
//     StatusCode sc = AlgTool::finalize();
//     return sc;
// }


bool    PanTau::Tool_InputConverter::passesPreselectionEnergy(double itsEnergy) const {
    if(itsEnergy < m_Config_TauConstituents_PreselectionMinEnergy) return false;
    return true;
}



//! ============================================
//! PFO converter
//! ============================================


StatusCode PanTau::Tool_InputConverter::ConvertToTauConstituent(xAOD::PFO* pfo,
                                                                PanTau::TauConstituent* &tauConstituent,
                                                                const xAOD::TauJet* tauJet,
                                                                std::string algName) const {
    //
    
    //! ==================================================
    //! check for invalid eta, phi, e values
    //!
    if(pfo->eta() != pfo->eta()) {
        ATH_MSG_WARNING("Will not convert PFO with eta value of " << pfo->eta() << " -> return to Tool_TauConstituentGetter");
        return StatusCode::SUCCESS;
    }
    if(pfo->phi() != pfo->phi()) {
        ATH_MSG_WARNING("Will not convert PFO with phi value of " << pfo->phi() << " -> return to Tool_TauConstituentGetter");
        return StatusCode::SUCCESS;
    }
    if(pfo->e() != pfo->e()) {
        ATH_MSG_WARNING("Will not convert PFO with e value of " << pfo->e() << " -> return to Tool_TauConstituentGetter");
        return StatusCode::SUCCESS;
    }
    
    
    //! ==================================================
    //! Check whether neutral input pfo has pion mass (it may have if xAOD is being reprocessed)
    //! If it does, make it massless again and use that
    if(pfo->charge() == 0 && pfo->m() != 0) {
        P4EEtaPhiM  hlvUpdate = P4EEtaPhiM(pfo->e(), pfo->eta(), pfo->phi(), 0);
        pfo->setP4(hlvUpdate.pt(), pfo->eta(), pfo->phi(), 0);
    }
    
    //! ==================================================
    //! preselection to veto very low energetic PFOs:
    int     pfoCharge       = (int)(pfo->charge());
    double constituentEnergy = pfo->e();
    if (passesPreselectionEnergy(constituentEnergy) == false) {
        ATH_MSG_DEBUG("EFO of charge " << pfoCharge << " and energy " << constituentEnergy << " does not pass presel Energy cut of " << m_Config_TauConstituents_PreselectionMinEnergy);
        return StatusCode::SUCCESS;
    }
    
    
    //! ==================================================
    //! get the mass correct & build 4-vector
    double  constituentMass = pfo->m();
    if(m_Config_UsePionMass == true) {
        
        //clusters: don't touch the measured energy. set mass to pion mass, so momentum will be altered
        if(pfoCharge == 0) {
            constituentMass = 134.98;
        }
        //tracks: change energy and set pion mass, so measured momentum is not changed
        if(pfoCharge != 0) {
            constituentMass = 139.57;
            constituentEnergy = sqrt(constituentEnergy * constituentEnergy + constituentMass*constituentMass);
        }
    }
    
    ATH_MSG_DEBUG("About to create 4 vector of EFO. E / Eta / Phi / m: " << constituentEnergy << " / " << pfo->eta() << " / " << pfo->phi() << " / " << constituentMass);
    P4EEtaPhiM  momentum    = P4EEtaPhiM(constituentEnergy, pfo->eta(), pfo->phi(), constituentMass);
    
    
    //! ==================================================
    //! get type (based on charge and DR to tau)
    std::vector<int> itsTypeFlags = std::vector<int>((unsigned int)PanTau::TauConstituent::t_nTypes, 0);
    itsTypeFlags[(int)PanTau::TauConstituent::t_NoType] = 1;
    
    double mvaValue         = PanTau::TauConstituent::DefaultBDTValue();
    double dEta             = tauJet->etaIntermediateAxis() - pfo->eta();
    double dPhi             = tauJet->phiIntermediateAxis() - pfo->phi();
    double deltaR_toTauJet  = sqrt( dEta*dEta + dPhi*dPhi );
    
    if(deltaR_toTauJet > m_Config_TauConstituents_Types_DeltaRCore) {
        if(pfoCharge != 0) {
            itsTypeFlags[(int)PanTau::TauConstituent::t_OutChrg] = 1;
        }
        if(pfoCharge == 0) {
            itsTypeFlags[(int)PanTau::TauConstituent::t_OutNeut] = 1;
            mvaValue = pfo->bdtPi0Score();
        }
    }//end if pfo is not in core
    
    if(deltaR_toTauJet <= m_Config_TauConstituents_Types_DeltaRCore) {
    
        if(pfoCharge != 0) {
            itsTypeFlags[(int)PanTau::TauConstituent::t_Charged] = 1;
        }
        if(pfoCharge == 0) {
            itsTypeFlags[(int)PanTau::TauConstituent::t_Neutral] = 1;
            itsTypeFlags[(int)PanTau::TauConstituent::t_NeutLowA] = 1;
            itsTypeFlags[(int)PanTau::TauConstituent::t_NeutLowB] = 1;
            
            //neutral PFO arranging --- check for pi0 tag
            mvaValue = pfo->bdtPi0Score();
            
            bool isThreeProng = false;
            if(tauJet->nTracks() == 3) isThreeProng = true;
//             ATH_MSG_DEBUG("numTrack = " << tauJet->nTracks() << ", is Threeprong = " << isThreeProng);
            
            int etaBinIndex = -1;
            if(algName == "CellBased")  etaBinIndex = m_Tool_HelperFunctions->getBinIndex(m_Config_CellBased_BinEdges_Eta, fabs(pfo->eta()) );
            if(algName == "eflowRec")   etaBinIndex = m_Tool_HelperFunctions->getBinIndex(m_Config_eflowRec_BinEdges_Eta, fabs(pfo->eta()) );
            
            if(isThreeProng == false) {
                double  minMVAValue = -5;
                if(algName == "CellBased")  minMVAValue = m_Config_CellBased_EtaBinned_Pi0MVACut_1prong.at(etaBinIndex);
                if(algName == "eflowRec")   minMVAValue = m_Config_eflowRec_EtaBinned_Pi0MVACut_1prong.at(etaBinIndex);
                if(mvaValue > minMVAValue)  itsTypeFlags[(int)PanTau::TauConstituent::t_Pi0Neut] = 1;
            }
            
            if(isThreeProng == true) {
                double  minMVAValue = -5;
                if(algName == "CellBased")  minMVAValue = m_Config_CellBased_EtaBinned_Pi0MVACut_3prong.at(etaBinIndex);
                if(algName == "eflowRec")   minMVAValue = m_Config_eflowRec_EtaBinned_Pi0MVACut_3prong.at(etaBinIndex);
                if(mvaValue > minMVAValue)  itsTypeFlags[(int)PanTau::TauConstituent::t_Pi0Neut] = 1;
            }
            
        }
    }//end if pfo is in core
    
    
    //! ==================================================
    //! create the tau constituent
    tauConstituent = new PanTau::TauConstituent(momentum, pfoCharge, itsTypeFlags, mvaValue, pfo);
    ATH_MSG_DEBUG("Created new TauConstituent at: " << tauConstituent);
    ATH_MSG_DEBUG("\t Eta value of its 4-vector: " << momentum.eta());
    
    
    //! ==================================================
    //! Check if the pfo object has shots:
    std::vector<const xAOD::IParticle*> list_TauShots = std::vector<const xAOD::IParticle*>(0);
    bool shotsOK = pfo->associatedParticles(xAOD::PFODetails::TauShot, list_TauShots);
    if(shotsOK == false) {
        ATH_MSG_DEBUG("WARNING: Could not get shots from current pfo");
    }
    
    ATH_MSG_DEBUG("Number of shots in cluster:" << list_TauShots.size());
    if(list_TauShots.size() > 0) {
        ATH_MSG_DEBUG("Now converting shots of cluster with E, Eta, Phi, M: " << momentum.e() << ", " << momentum.eta() << ", " << momentum.phi() << ", " << momentum.m());
        for(unsigned int iShot=0; iShot<list_TauShots.size(); iShot++) {
        
            if(list_TauShots[iShot] == 0) {
                ATH_MSG_WARNING("Shot number " << iShot << " points to 0! Skip it");
                continue;
            }
        
            xAOD::PFO*              curShot         = const_cast<xAOD::PFO*>( dynamic_cast<const xAOD::PFO*>(list_TauShots[iShot]) );
            ATH_MSG_DEBUG("Checking shot E, Eta, Phi, M: " << curShot->e() << ", " << curShot->eta() << ", " << curShot->phi() << ", " << curShot->m());
            P4EEtaPhiM              shotMomentum    = P4EEtaPhiM(curShot->e(), curShot->eta(), curShot->phi(), curShot->m());
            std::vector<int>        shotTypeFlags   = std::vector<int>((unsigned int)PanTau::TauConstituent::t_nTypes, 0);
            double                  shotMVAValue    = PanTau::TauConstituent::DefaultBDTValue();
            PanTau::TauConstituent* shotConstituent = new PanTau::TauConstituent(shotMomentum, 0, itsTypeFlags, shotMVAValue, curShot);
            ATH_MSG_DEBUG("\tAdded shot with E, Eta, Phi, M: " << shotMomentum.e() << ", " << shotMomentum.eta() << ", " << shotMomentum.phi() << ", " << shotMomentum.m());
            
            int nPhotons = 0;
            if( curShot->attribute(xAOD::PFODetails::tauShots_nPhotons, nPhotons) == false) {
                nPhotons = -1;
                ATH_MSG_DEBUG("WARNING: Could not get nPhotons for this shot! Set to -1");
            }
            shotConstituent->setNPhotonsInShot(nPhotons);
            ATH_MSG_DEBUG("\tThe shot has " << shotConstituent->getNPhotonsInShot() << " photons in it");
            tauConstituent->addShot(shotConstituent);
            
        }//end loop over shots
    }
    
    return StatusCode::SUCCESS;
}









    


