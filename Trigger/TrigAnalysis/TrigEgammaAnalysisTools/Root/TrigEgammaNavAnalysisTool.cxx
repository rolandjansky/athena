/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"

using namespace std;

TrigEgammaNavAnalysisTool::TrigEgammaNavAnalysisTool( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  declareProperty("DirectoryPath",m_dir="Analysis");
  declareProperty("IsEMLabels",m_labels);
  m_onlElectrons=nullptr;
  m_onlPhotons=nullptr;
  m_trigElectrons=nullptr;
  m_caloClusters=nullptr;
  m_trigEMClusters=nullptr;
  m_emTauRoI=nullptr;
  m_eventInfo=nullptr;
}

StatusCode TrigEgammaNavAnalysisTool::childInitialize(){
    ATH_MSG_INFO("TrigEgammaNavAnalysisTool: child Initialize");

    m_eventCounter=0;
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childBook(){

    ATH_MSG_DEBUG("Now configuring chains for analysis");
    std::vector<std::string> selectElectronChains  = m_trigdec->getListOfTriggers("HLT_e.*");
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
        ATH_MSG_DEBUG("Electron trigger " << selectElectronChains[j]);
    }
    std::vector<std::string> selectPhotonChains  = m_trigdec->getListOfTriggers("HLT_g.*");

    for (int i = 0; i < (int) m_trigInputList.size(); i++) {
        std::string trigname = "HLT_"+m_trigInputList[i];
        for (int j = 0; j < (int) selectElectronChains.size(); j++) {
            size_t found = trigname.find(selectElectronChains[j]);
            if(found != std::string::npos) {
                m_trigList.push_back(m_trigInputList[i]);
                break;
            }
        }
        for (int j = 0; j < (int) selectPhotonChains.size(); j++) {
            std::string trigname = "HLT_"+m_trigInputList[i];
            size_t found = trigname.find(selectPhotonChains[j]);
            if(found != std::string::npos) {
                m_trigList.push_back(m_trigInputList[i]);
                break;
            }
        }
    }
    // Container level kinematic histograms
    addDirectory(m_dir);
    const int nTrigger = (int) m_trigList.size();
    
    addHistogram(new TH1F("hlt_el_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH2F("hlt_el_eta_phi", "HLT #eta; #eta ; Count", 50, -2.47, 2.47,50,-3.14,3.14));

    addHistogram(new TH1F("l1_energy", "L1 Energy; E [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH1F("l1_roi_et", "L1 RoI Energy; E [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH2F("l1_eta_phi", "L1 Calo; eta ; phi", 50, -2.47, 2.47,50,-3.14,3.14));

    addHistogram(new TH1F("electrons", "Offline Electrons; ; N_{electrons}", 6, 1., 6));   
    addHistogram(new TH1F("elperevt", "Offline Electrons; ; N_{electrons}/Evt", 6, 1., 6));   
    std::vector<std::string> el_labels;
    el_labels.push_back("loose");
    el_labels.push_back("medium");
    el_labels.push_back("tight");
    el_labels.push_back("lhloose");
    el_labels.push_back("lhmedium");
    el_labels.push_back("lhtight");
    setLabels(hist1("electrons"),el_labels);
    setLabels(hist1("elperevt"),el_labels);
    
    addHistogram(new TH1F("trigger_counts", "Trigger Counts; Trigger ; Count", nTrigger, 1, nTrigger));
    setLabels(hist1("trigger_counts"),m_trigList);
    for (int i = 0; i < (int) m_trigList.size(); i++) {
        bookPerSignature(m_trigList[i]);
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childExecute(){
    ATH_MSG_DEBUG("Executing TrigEgammaValidationTool");

    m_eventCounter++;
    m_eventInfo = 0;

    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve eventInfo ");
        return StatusCode::FAILURE;
    }
    if ((m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Electron container size " << m_onlElectrons->size());
    if ((m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Photon container size " << m_onlPhotons->size());
    if ((m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Trig Electron container size " << m_trigElectrons->size());
    if ((m_storeGate->retrieve(m_emTauRoI,"LVL1EmTauRoIs")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("L1 EM container size " << m_emTauRoI->size());
    if ((m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve CaloClusters");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("CaloCluster size " << m_caloClusters->size());
    if ((m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ){
        ATH_MSG_DEBUG("Failed to retrieve TrigEMClusters");
    }
    else ATH_MSG_DEBUG("TrigEMCluser size " << m_trigEMClusters->size());
   

    // Event wise distributions of persisted containers
    cd(m_dir);
    if( eventWiseSelection().isFailure() ) {
        ATH_MSG_DEBUG("Unable to retrieve offline containers");
        return StatusCode::FAILURE;
    }
    
   for (const auto& l1: *m_emTauRoI) {
        hist2("l1_eta_phi")->Fill(l1->eta(),l1->phi());
        hist1("l1_energy")->Fill(l1->emClus()/1.e3);
        hist1("l1_roi_et")->Fill(l1->eT()/1.e3);
    }
    float et=0.;
    for (const auto& eg : *m_onlElectrons){
        et = getEt(eg)/1e3;
        hist2("hlt_el_eta_phi")->Fill(eg->eta(),eg->phi());
        hist1("hlt_el_et")->Fill(et);
    }
    

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string trigger = m_trigList.at(ilist);

        // Trigger counts
        cd(m_dir);
        // Require event passes trigger
        // Killing events?
        //if(!m_trigdec->isPassed("HLT_"+trigger)) continue;
        
        hist1("trigger_counts")->AddBinContent(ilist+1);
        std::string basePath = m_dir+"/"+trigger+"/Distributions/";
        
        std::string type="";
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
        parseTriggerName(trigger,"Loose",type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
        
        if ( executeNavigation(trigger).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }
        for(unsigned int i=0;i<m_objTEList.size();i++){
            efficiency(m_dir+"/"+trigger+"/Efficiency/",etthr,m_objTEList[i]); // Requires offline match
            inefficiency(m_dir+"/"+trigger+"/Efficiency/HLT",etthr,m_objTEList[i]); // Requires offline match
            resolution(m_dir+"/"+trigger+"/Resolutions/HLT",m_objTEList[i]); // Requires offline match
        }
       
        // Retrieve FeatureContainer for a given trigger
        auto fc = (m_trigdec->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto initRois = fc.get<TrigRoiDescriptor>();
        for(auto feat : initRois){
            if(feat.te()==NULL) continue;
            ATH_MSG_DEBUG("Retrievec L1 FC");
            auto itEmTau = m_trigdec->ancestor<xAOD::EmTauRoI>(feat);
            const xAOD::EmTauRoI *l1 = itEmTau.cptr();
            if(l1==NULL) continue;
            ATH_MSG_DEBUG("Retrievec L1 Object");
            fillL1Calo(basePath+"L1Calo",l1);
        }
        auto vec_clus = fc.get<xAOD::CaloClusterContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EF FC Size " << vec_clus.size());
        for(auto feat : vec_clus){
            if(feat.te()==NULL) continue;
            const xAOD::CaloClusterContainer *cont = feat.cptr();
            if(cont==NULL) continue;
            ATH_MSG_DEBUG("Retrieved EF Calo Container");
            for(const auto& clus : *cont){
                if(clus==NULL) continue;
                ATH_MSG_DEBUG("Retrieved EF Cluster");
                fillEFCalo(basePath+"EFCalo",clus);           
            }
        }
        auto vec = fc.get<xAOD::ElectronContainer>("",TrigDefs::alsoDeactivateTEs);
        for(auto feat : vec){
            if(feat.te()==NULL) continue;
            const xAOD::ElectronContainer *cont = feat.cptr();
            if(cont==NULL) continue;
            ATH_MSG_DEBUG("EF Electron Size " << cont->size());
            for(const auto& el : *cont){
                if(el==NULL) continue;
                fillHLTShowerShapes(basePath+"HLT",el);           
                fillHLTTracking(basePath+"HLT",el);           
            }
        }
        auto vec_ph = fc.get<xAOD::PhotonContainer>("",TrigDefs::alsoDeactivateTEs);
        for(auto feat : vec_ph){
            if(feat.te()==NULL) continue;
            const xAOD::PhotonContainer *cont = feat.cptr();
            if(cont==NULL) continue;
            ATH_MSG_DEBUG("EF Photon Size " << cont->size());
            for(const auto& ph : *cont){
                if(ph==NULL) continue;
                fillHLTShowerShapes(basePath+"HLT",ph);           
            }
        }
    } // End loop over trigger list

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavAnalysisTool::childFinalize(){
    ATH_MSG_DEBUG("Processed N events " << m_eventCounter);
    cd(m_dir);
    hist1("elperevt")->Scale(float(m_eventCounter));
    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string probeTrigger = m_trigList.at(ilist);
        finalizeEfficiency(m_dir+"/"+probeTrigger+"/Efficiency/HLT");
        finalizeEfficiency(m_dir+"/" + probeTrigger + "/Efficiency/L2Calo");
        finalizeEfficiency(m_dir+"/" + probeTrigger + "/Efficiency/L2");
        finalizeEfficiency(m_dir+"/" + probeTrigger + "/Efficiency/EFCalo");
        finalizeEfficiency(m_dir+"/" + probeTrigger + "/Efficiency/L1Calo");
    }

  
    return StatusCode::SUCCESS;
}

void TrigEgammaNavAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_DEBUG("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    bookAnalysisHistos(basePath);
}

void bookPerCategory(const std::string category){

}

void TrigEgammaNavAnalysisTool::resolutionL2Photon(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedL2Ph = ancestorPassed<xAOD::TrigPhotonContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::TrigPhoton *phL2 = NULL;

    const auto* L2Ph = getFeature<xAOD::TrigPhotonContainer>(feat);
    dRMax=100.;
    if(L2Ph != NULL){
        for(const auto& ph : *L2Ph){
            if(ph == NULL) {
                ATH_MSG_WARNING("Photon from TE NULL");
                continue;
            }
            deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->eta(),ph->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                phL2 =ph;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedL2Ph && phL2!=NULL){
                //fillRes(trigger,phEF,phOff);
                //fillShowerShapes(trigger,phEF,phOff);           
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}

void TrigEgammaNavAnalysisTool::resolutionL2Electron(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
}

void TrigEgammaNavAnalysisTool::resolutionEFCalo(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    const xAOD::Egamma *eg = pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::CaloCluster *clusEF = NULL;

    const auto* EFCalo = getFeature<xAOD::CaloClusterContainer>(feat);
    dRMax=100.;
    if(EFCalo!=NULL){
        for(const auto& clus : *EFCalo){
            if(clus==NULL) {
                ATH_MSG_WARNING("Photon from TE NULL");
                continue;
            }
            deltaR = dR(eg->caloCluster()->eta(),eg->caloCluster()->phi(), clus->eta(),clus->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                clusEF=clus;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedEFCalo && clusEF!=NULL){
                //fillRes(trigger,phEF,phOff);
                //fillShowerShapes(trigger,phEF,phOff);           
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}

void TrigEgammaNavAnalysisTool::resolutionPhoton(const std::string dir,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Resolution photon "<< dir);

    const xAOD::Photon* phOff =static_cast<const xAOD::Photon*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFPh = ancestorPassed<xAOD::PhotonContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::Photon *phEF = NULL;

    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
    dRMax=100.;
    if(EFPh != NULL){
        for(const auto& ph : *EFPh){
            if(ph == NULL) {
                ATH_MSG_WARNING("Photon from TE NULL");
                continue;
            }
            deltaR = dR(phOff->caloCluster()->eta(),phOff->caloCluster()->phi(), ph->caloCluster()->eta(),ph->caloCluster()->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                phEF =ph;
            } 
        } //Loop over EF photons
        if(dRMax < 0.05) { 
            if(passedEFPh && phEF!=NULL){
                fillHLTResolution(dir,phEF,phOff);
            } // Is EF Photon
        } // Found closest photon match
    } // Feature Container
    else ATH_MSG_DEBUG("Feature Container NULL");
}
void TrigEgammaNavAnalysisTool::resolutionElectron(const std::string dir,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Resolution Electron " << dir);

    const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (pairObj.first);
    const HLT::TriggerElement *feat = pairObj.second; 
    bool passedEFEl = ancestorPassed<xAOD::ElectronContainer>(feat);
    double deltaR=0.;
    double dRMax = 100;
    const xAOD::Electron *elEF = NULL;

    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    if(EFEl != NULL) {
        dRMax=100.;
        ATH_MSG_DEBUG("Retrieve Electron FC");
        for(const auto& el : *EFEl){
            if(el == NULL) {
                ATH_MSG_WARNING("Electron from TE NULL");
                continue;
            }
            deltaR = dR(elOff->trackParticle()->eta(),elOff->trackParticle()->phi(), el->trackParticle()->eta(),el->trackParticle()->phi());
            if (deltaR < dRMax) {
                dRMax = deltaR;
                elEF =el;
            }
        } // Loop over EF container
        if(dRMax < 0.05) {
            if(passedEFEl && elEF!=NULL){
                fillHLTResolution(dir,elEF,elOff);
            } // Is EF electron
        } // Found closest math
    } // Feature
    else ATH_MSG_WARNING("NULL Feature");
}

void TrigEgammaNavAnalysisTool::resolution(const std::string dir,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
   
    ATH_MSG_DEBUG("Executing resolution for " << dir);
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    
    if ( feat!=NULL ) {
        if(eg->type()==xAOD::Type::Electron){
            resolutionElectron(dir,pairObj);
        } // Offline object Electron
        else if(eg->type()==xAOD::Type::Photon){
            resolutionPhoton(dir,pairObj);
        } // Offline photon
    }
}

void TrigEgammaNavAnalysisTool::inefficiency(const std::string basePath,const float etthr,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    // Inefficiency analysis
    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(pairObj.first->type()==xAOD::Type::Electron){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    const xAOD::Electron* selEF = NULL;
    const xAOD::CaloCluster* selClus = NULL;
    const xAOD::TrackParticle* selTrk = NULL;
    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat);
    const auto* EFTrk = getFeature<xAOD::TrackParticleContainer>(feat);
    float dRmax=0.15;
    bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
    unsigned int runNumber               = m_eventInfo->runNumber();
    unsigned int eventNumber             = m_eventInfo->eventNumber();
    if(!passedEF){
        ATH_MSG_DEBUG("REGEST::Fails EF Electron Hypo Run " << runNumber << " Event " << eventNumber);
        if ( EFEl != NULL ){
            ATH_MSG_DEBUG("Retrieved ElectronContainer for inefficiency");
            for(const auto& el : *EFEl){
                float dr=dR(eta,phi,el->eta(),el->phi());
                if ( dr<dRmax){
                    dRmax=dr;
                    selEF = el;
                } // dR
            } // loop over EFEl
        } //FC exists
        dRmax=0.15;
        if ( EFClus != NULL ){
            ATH_MSG_DEBUG("Retrieved PhotonContainer for inefficiency");
            for(const auto& clus : *EFClus){
                float dr=dR(eta,phi,clus->eta(),clus->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selClus = clus;
                } // dR
            } // loop over EFPh
        }
        dRmax=0.15;
        if ( EFTrk != NULL ){
            ATH_MSG_DEBUG("Retrieved TrackContainer for inefficiency");
            for(const auto& trk : *EFTrk){
                float dr=dR(eta,phi,trk->eta(),trk->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selTrk = trk;
                } // dR
            } // loop over EFPh
        } //FC exists

        fillInefficiency(basePath,selEF,selClus,selTrk);
    }
}

void TrigEgammaNavAnalysisTool::efficiency(const std::string basePath,const float etthr,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){

    ATH_MSG_DEBUG("Calling SimpleEfficiency");

    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(pairObj.first->type()==xAOD::Type::Electron){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    float avgmu=0.;
    if(m_lumiBlockMuTool)
        avgmu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
    ATH_MSG_DEBUG("Fill probe histograms");
    fillHistos(basePath+"L1Calo",etthr,et,eta,phi,avgmu);
    fillHistos(basePath+"L2Calo",etthr,et,eta,phi,avgmu);
    fillHistos(basePath+"L2",etthr,et,eta,phi,avgmu);
    fillHistos(basePath+"EFCalo",etthr,et,eta,phi,avgmu);
    fillHistos(basePath+"HLT",etthr,et,eta,phi,avgmu);

    if ( feat ) {
        ATH_MSG_DEBUG("Fill passed  histograms");
        bool passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
        bool passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
        bool passedL2El = ancestorPassed<xAOD::TrigElectronContainer>(feat);
        bool passedL2Ph = ancestorPassed<xAOD::TrigPhotonContainer>(feat);
        bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
        bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
        if( passedL1Calo) fillMatchHistos(basePath+"L1Calo",etthr,et,eta,phi,avgmu);
        if( passedL2Calo ) fillMatchHistos(basePath+"L2Calo",etthr,et,eta,phi,avgmu);
        if( passedL2El || passedL2Ph ) fillMatchHistos(basePath+"L2",etthr,et,eta,phi,avgmu);
        if( passedEFCalo ) fillMatchHistos(basePath+"EFCalo",etthr,et,eta,phi,avgmu);
        if( passedEF ) fillMatchHistos(basePath+"HLT",etthr,et,eta,phi,avgmu);
    } // Features
    ATH_MSG_DEBUG("Complete efficiency"); 
}

