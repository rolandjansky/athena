/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"


#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE


namespace Trig {
    TrigEgammaMatchingTool::TrigEgammaMatchingTool( const std::string& name )
        : asg::AsgMetadataTool( name ),
        m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
    {
        declareProperty( "TriggerTool", m_trigDecTool);
        declareProperty( "DeltaR", m_dR=0.07);
        declareProperty( "L1DeltaR", m_dRL1=0.15);
    }

    TrigEgammaMatchingTool::~TrigEgammaMatchingTool(){
    }

    StatusCode TrigEgammaMatchingTool::finalize() {
        return StatusCode::SUCCESS;
    }

    StatusCode TrigEgammaMatchingTool::execute() {
        return StatusCode::SUCCESS;
    }

    StatusCode TrigEgammaMatchingTool::initialize() {
        // Greet the user:
        ATH_MSG_INFO( "Initialising... " );

        ATH_CHECK(m_trigDecTool.retrieve());

        return StatusCode::SUCCESS;
    }

    double TrigEgammaMatchingTool::dR(const double eta1, const double phi1, const double eta2, const double phi2){
        double deta = fabs(eta1 - eta2);
        double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
                      Pi() - fabs(phi1 - phi2);
        return sqrt(deta*deta + dphi*dphi);
    }

    /*! Calls match with a FeatureContainer for a given triggerfor HLT only 
     * Note does not use DeactiviateTEs
     * Returns closest EF object to offline object */ 
    const xAOD::Egamma* TrigEgammaMatchingTool::closestHLTObject(const xAOD::Egamma *eg,const std::string trigger){

        if(eg==NULL) {
            ATH_MSG_DEBUG("NULL Offline object");
            return false;
        }
        Trig::FeatureContainer fc = m_trigDecTool->features("HLT_"+trigger);
        double deltaR=0.;
        double dRMax = 100;
        const xAOD::Electron *elEF = 0;
        const xAOD::Photon *phEF = 0;
        if(eg->type()==xAOD::Type::Electron){
            const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (eg);
            const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec = fc.get<xAOD::ElectronContainer>();
            
            for(auto feat : vec){
                const xAOD::ElectronContainer *cont = feat.cptr();
                if(cont == NULL) {
                    ATH_MSG_WARNING("Electron container from TE NULL");
                    continue;
                }
                for(const auto& el : *cont){
                    if(el == NULL) {
                        ATH_MSG_WARNING("Electron from TE NULL");
                        continue;
                    }
                    deltaR = dR(elOff->trackParticle()->eta(),elOff->trackParticle()->phi(), el->trackParticle()->eta(),el->trackParticle()->phi()); 
                    if (deltaR < dRMax) {
                        dRMax = deltaR;
                        elEF =el;
                    }
                }
            }
            if(dRMax < m_dR) return elEF;
        }
        else if(eg->type()==xAOD::Type::Photon){
            const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (eg);
            const std::vector< Trig::Feature<xAOD::PhotonContainer> > vec = fc.get<xAOD::PhotonContainer>();
            for(auto feat : vec){
                const xAOD::PhotonContainer *cont = feat.cptr();
                if(cont == NULL) {
                    ATH_MSG_WARNING("Photon Container from TE NULL");
                    continue;
                }
                for(const auto& ph : *cont){
                    if(ph == NULL) {
                        ATH_MSG_WARNING("Photon from TE NULL");
                        continue;
                    }
                    deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->caloCluster()->eta(),ph->caloCluster()->phi()); 
                    if (deltaR < dRMax) {
                        dRMax = deltaR;
                        phEF =ph;
                    }
                }
            }
            if(dRMax < m_dR) return phEF;
        }
        return 0; // No match
    }

    /*! Calls match with a FeatureContainer for a given triggerfor HLT only 
     * Note does not use DeactiviateTEs */ 
    bool TrigEgammaMatchingTool::matchHLT(const xAOD::Egamma *eg,const std::string trigger){

        Trig::FeatureContainer fc = m_trigDecTool->features("HLT_"+trigger);
        double deltaR=0.; 
        if(eg->type()==xAOD::Type::Electron){
            const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (eg);
#ifdef XAOD_ANALYSIS
            const auto vec = fc.containerFeature<xAOD::ElectronContainer>();
#else
            const auto vec = fc.get<xAOD::ElectronContainer>();
#endif // XAOD_ANALYSIS
            for(auto feat : vec){
                const xAOD::ElectronContainer *cont = feat.cptr();
                if(cont == NULL) {
                    ATH_MSG_WARNING("Electron container from TE NULL");
                    continue;
                }
                for(const auto& el : *cont){
                    if(el == NULL) {
                        ATH_MSG_WARNING("Electron from TE NULL");
                        continue;
                    }
                    if(el->trackParticle() && elOff->trackParticle())
                        deltaR = dR(elOff->trackParticle()->eta(),elOff->trackParticle()->phi(), el->trackParticle()->eta(),el->trackParticle()->phi()); 
                    else
                        deltaR = dR(elOff->eta(),elOff->phi(), el->eta(),el->phi());
                    if(deltaR < m_dR){
                        return true;
                    }
                }
            }
        }
        else if(eg->type()==xAOD::Type::Photon){
            const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (eg);
#ifdef XAOD_ANALYSIS
            const auto vec = fc.containerFeature<xAOD::PhotonContainer>();
#else
            const auto vec = fc.get<xAOD::PhotonContainer>();
#endif // XAOD_ANALYSIS
            for(auto feat : vec){
                const xAOD::PhotonContainer *cont = feat.cptr();
                if(cont == NULL) {
                    ATH_MSG_DEBUG("Photon Container from TE NULL");
                    continue;
                }
                for(const auto& ph : *cont){
                    if(ph == NULL) {
                        ATH_MSG_WARNING("Photon from TE NULL");
                        continue;
                    }
                    if(ph->caloCluster() && phOff->caloCluster())
                        deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->caloCluster()->eta(),ph->caloCluster()->phi()); 
                    else
                        deltaR = dR(phOff->eta(),phOff->phi(), ph->eta(),ph->phi());
                    if(deltaR < m_dR){
                        return true;
                    }
                }
            }
        }
        return false; // No match
    }

    bool TrigEgammaMatchingTool::matchHLTPhoton(const xAOD::Photon *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        finalFC=NULL;
        ATH_MSG_DEBUG("Match HLT Photon");
        // Get the container of online electrons associated to passed items
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));

        auto vec = fc.get<xAOD::PhotonContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EF FC Size " << vec.size());
        double deltaR=0.; 
        for(auto feat : vec){
            const xAOD::PhotonContainer *cont = feat.cptr();
            if(cont == NULL) {
                ATH_MSG_WARNING("Photon Container from TE NULL");
                continue;
            }
            ATH_MSG_DEBUG("EF Size " << cont->size());
            for(const auto& ph : *cont){
                if(ph == NULL) {
                    ATH_MSG_WARNING("Photon from TE NULL");
                    continue;
                }
                deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), ph->caloCluster()->eta(),ph->caloCluster()->phi()); 
                if(deltaR < m_dR){
                    finalFC = (feat.te());
                    return true;
                }
            }
        }
        return false;
    }

    bool TrigEgammaMatchingTool::matchHLTElectron(const xAOD::Electron *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        ATH_MSG_DEBUG("Match HLT electron");
        finalFC=NULL;
        // Get the container of online electrons associated to passed items
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));

        auto vec = fc.get<xAOD::ElectronContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EF FC Size " << vec.size());
        double deltaR=0.; 
        for(auto feat : vec){
            const xAOD::ElectronContainer *cont = feat.cptr();
            if(cont == NULL) {
                ATH_MSG_WARNING("Electron Container from TE NULL");
                continue;
            }
            ATH_MSG_DEBUG("EF Size " << cont->size());
            for(const auto& el : *cont){
                if(el == NULL) {
                    ATH_MSG_WARNING("Electron from TE NULL");
                    continue;
                }
                deltaR = dR(eg->trackParticle()->eta(),eg->trackParticle()->phi(), el->trackParticle()->eta(),el->trackParticle()->phi()); 
                if(deltaR < m_dR){
                    finalFC = (feat.te());
                    return true;
                }
            }
        }
        return false;
    }

    bool TrigEgammaMatchingTool::matchHLTCalo(const xAOD::Egamma *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        ATH_MSG_DEBUG("Match HLT CaloCluster");
        finalFC=NULL;
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto vec = fc.get<xAOD::CaloClusterContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EFCal  FC Size " << vec.size());
        double deltaR=0.;
        for(auto feat : vec){
            const xAOD::CaloClusterContainer *cont = feat.cptr();
            ATH_MSG_DEBUG("EF Calo Size " << cont->size());
            if(cont == NULL) {
                ATH_MSG_WARNING("CaloCluster Container from TE NULL");
                continue;
            }
            for(const auto& clus : *cont){
                if(clus == NULL) {
                    ATH_MSG_WARNING("CaloCluster from TE NULL");
                    continue;
                }
                if(eg->type()==xAOD::Type::Electron){
                    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
                    deltaR = dR(el->trackParticle()->eta(),el->trackParticle()->phi(), clus->eta(),clus->phi());
                }
                else if (eg->type()==xAOD::Type::Photon)
                    deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), clus->eta(),clus->phi());
                if(deltaR < m_dR){
                    finalFC = (feat.te());
                    return true;
                }
            }
        }
        return false;

    }

    bool TrigEgammaMatchingTool::matchL2Photon(const xAOD::Photon *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        finalFC=NULL;
        ATH_MSG_DEBUG("Match L2 Photon");
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto vec = fc.get<xAOD::TrigPhotonContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("L2 FC Size " << vec.size());
        double deltaR=0.;
        for(auto feat : vec){
            const xAOD::TrigPhotonContainer *cont = feat.cptr();
            if(cont == NULL) {
                ATH_MSG_WARNING("TrigPhoton Container from TE NULL");
                continue;
            }
            ATH_MSG_DEBUG("L2 Size " << cont->size());
            for(const auto& l2 : *cont){
                if(l2 == NULL) {
                    ATH_MSG_WARNING("TrigElectron from TE NULL");
                    continue;
                }
                deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), l2->eta(),l2->phi()); 
                if(deltaR < m_dR){
                    finalFC = (feat.te());
                    return true;
                }
            }
        }
        return false;
    }

    bool TrigEgammaMatchingTool::matchL2Electron(const xAOD::Electron *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        finalFC=NULL;
        ATH_MSG_DEBUG("Match L2 Electron");
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto vec = fc.get<xAOD::TrigElectronContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("L2 FC Size " << vec.size());
        double deltaR=0.;
        for(auto feat : vec){
            const xAOD::TrigElectronContainer *cont = feat.cptr();
            if(cont == NULL) {
                ATH_MSG_WARNING("TrigElectron Container from TE NULL");
                continue;
            }
            ATH_MSG_DEBUG("L2 Size " << cont->size());
            for(const auto& l2 : *cont){
                if(l2 == NULL) {
                    ATH_MSG_WARNING("TrigElectron from TE NULL");
                    continue;
                }
                deltaR = dR(eg->trackParticle()->eta(),eg->trackParticle()->phi(), l2->eta(),l2->phi()); 
                if(deltaR < m_dR){
                    finalFC = (feat.te());
                    return true;
                }
            }
        }
        return false;

    }

    bool TrigEgammaMatchingTool::matchL2Calo(const xAOD::Egamma *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        finalFC=NULL;
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto vec = fc.get<xAOD::TrigEMCluster>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("L2 FC Size " << vec.size());
        for(auto feat : vec){
            const xAOD::TrigEMCluster *em = feat.cptr();
            if(em == NULL) {
                ATH_MSG_WARNING("TrigEMCluster from TE NULL");
                continue;
            }
            ATH_MSG_DEBUG("TrigEMCluster << " << em->et() );
            double deltaR=0.;
            if(eg->type()==xAOD::Type::Electron){
                const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
                deltaR = dR(el->trackParticle()->eta(),el->trackParticle()->phi(), em->eta(),em->phi());
            }
            else if (eg->type()==xAOD::Type::Photon)
                deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), em->eta(),em->phi());
            if(deltaR < m_dR){
                finalFC = (feat.te());
                return true;
            }
        }

        return false;

    }

    bool TrigEgammaMatchingTool::matchL1( const xAOD::Egamma*
#ifndef XAOD_ANALYSIS
                                          eg
#endif // XAOD_ANALYSIS
                                          , const std::string
#ifndef XAOD_ANALYSIS
                                          trigger
#endif // XAOD_ANALYSIS
                                          , const HLT::TriggerElement*& finalFC ){

       finalFC=NULL;

#if defined(ASGTOOL_STANDALONE) || defined(XAOD_ANALYSIS)
        if(m_trigDecTool->isPassed("L1_EM.*")) return false;
        /*const xAOD::EMTauRoIContainer *emTauRoIs=0;
        StatusCode sc = evtStore()->retrieve(emTauRoIs,"LVL1EmTauRoIs");
        if(!sc) { 
            ATH_MSG_ERROR("No L1 EM Objects retrieved");
            return false;
        }
        for(const auto &l1 : emTauRoIs){
            if(eg->type()==xAOD::Type::Electron){
                const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
                deltaR = dR(el->trackParticle()->eta(),el->trackParticle()->phi(), l1->eta(),l1->phi());
            }
            else if (eg->type()==xAOD::Type::Photon)
                deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), l1->eta(),l1->phi());
            if(deltaR < m_dRL1){
                finalFC = (itEmTau.te());
                return true;
            }
        }*/

#else
        double deltaR=0.;
        auto fc = (m_trigDecTool->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto initRois = fc.get<TrigRoiDescriptor>();
        if ( initRois.size() < 1 ) return false;
        auto itEmTau = m_trigDecTool->ancestor<xAOD::EmTauRoI>(initRois[0]);
        const xAOD::EmTauRoI *l1 = itEmTau.cptr();
        if(l1 == NULL) {
            ATH_MSG_DEBUG("EMTauRoI from TE NULL");
            return false;
        }
     
        if(eg->type()==xAOD::Type::Electron){
            const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
            deltaR = dR(el->trackParticle()->eta(),el->trackParticle()->phi(), l1->eta(),l1->phi());
        }
        else if (eg->type()==xAOD::Type::Photon)
            deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), l1->eta(),l1->phi());

        if(deltaR < m_dRL1){
            finalFC = (itEmTau.te());
            return true;
        }
#endif
        return false; // otherwise, someone matched!*/

    }

    bool TrigEgammaMatchingTool::match(const xAOD::Egamma *eg,const std::string trigger,const HLT::TriggerElement*& finalFC){
        // Set TE to NULL
        // If no match easy check
        finalFC=NULL;

        ATH_MSG_DEBUG("Match objec with trigger " << trigger);
        if(eg->type()==xAOD::Type::Electron){
            const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
            if( matchHLTElectron(el,trigger,finalFC) ) return true;
        }
        else if(eg->type()==xAOD::Type::Photon){
            const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
            if( matchHLTPhoton(ph,trigger,finalFC) ) return true;
        }
        if( matchHLTCalo(eg,trigger,finalFC) ) return true;
        if(eg->type()==xAOD::Type::Electron){
            const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
            if( matchL2Electron(el,trigger,finalFC) ) return true;
        }
        else if(eg->type()==xAOD::Type::Photon){
            const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
            //if( matchL2Photon(ph,fc,trigger,finalFC) ) return true;
            if( matchL2Photon(ph,trigger,finalFC) ) return true;
        }
        if( matchL2Calo(eg,trigger,finalFC) ) return true;
        if( matchL1(eg,trigger,finalFC) ) return true;
        return false;// otherwise, someone matched!*/
    }

    /*! Calls match with a TE, so recursive matching to L1 
     * Check that the HLT Electron or Photon ancester passed */
    bool TrigEgammaMatchingTool::match(const xAOD::Egamma *eg,const std::string trigger){
        const HLT::TriggerElement* finalFC;
        bool passed = false;
        if( match(eg,trigger,finalFC) ){
            if(eg->type()==xAOD::Type::Electron){
                if ( (m_trigDecTool->ancestor<xAOD::ElectronContainer>(finalFC)).te() != NULL)
                    passed = true;
            }
            else if(eg->type()==xAOD::Type::Photon){
                if ( (m_trigDecTool->ancestor<xAOD::PhotonContainer>(finalFC)).te() != NULL)
                    passed = true;
            }
        }
        if( finalFC == NULL) return false;
        return passed;
    }

} //namespace Trig


