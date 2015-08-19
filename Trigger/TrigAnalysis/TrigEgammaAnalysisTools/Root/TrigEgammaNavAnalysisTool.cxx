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
  m_eventCounter=0;
}

StatusCode TrigEgammaNavAnalysisTool::childInitialize(){
    ATH_MSG_INFO("TrigEgammaNavAnalysisTool: child Initialize");

    m_eventCounter=0;
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childBook(){

    ATH_MSG_DEBUG("Now configuring chains for analysis");
    std::vector<std::string> selectElectronChains  = m_trigdec->getListOfTriggers("HLT_e.*, L1_EM.*");
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
        ATH_MSG_DEBUG("Electron trigger " << selectElectronChains[j]);
    }
    std::vector<std::string> selectPhotonChains  = m_trigdec->getListOfTriggers("HLT_g.*");

    for (int i = 0; i < (int) m_trigInputList.size(); i++) {
        std::string trigname = "";
        if (!boost::starts_with(m_trigInputList[i], "L1" )) {
            trigname = "HLT_"+m_trigInputList[i];
        } else {
            trigname = m_trigInputList[i];
        }
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
    addDirectory(m_dir+"/Counters");
    const int nTrigger = (int) m_trigList.size();
    
    addHistogram(new TH1F("electrons", "Offline Electrons; ; N_{electrons}", 6, 1., 6));   
    std::vector<std::string> el_labels;
    el_labels.push_back("loose");
    el_labels.push_back("medium");
    el_labels.push_back("tight");
    el_labels.push_back("lhloose");
    el_labels.push_back("lhmedium");
    el_labels.push_back("lhtight");
    setLabels(hist1("electrons"),el_labels);
    
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
    unsigned int runNumber               = m_eventInfo->runNumber();
    unsigned int eventNumber             = m_eventInfo->eventNumber();
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
    cd(m_dir+"/Counters");
    if( eventWiseSelection().isFailure() ) {
        ATH_MSG_DEBUG("Unable to retrieve offline containers");
        return StatusCode::FAILURE;
    }

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string trigger = m_trigList.at(ilist);

        // Trigger counts
        cd(m_dir+"/Counters");
        if(m_trigdec->isPassed("HLT_"+trigger)) hist1("trigger_counts")->AddBinContent(ilist+1);
        
        // Skip event if prescaled out 
        // Prescale cut has ill effects
        // if(m_trigdec->isPassedBits("HLT_"+trigger) & TrigDefs::EF_prescaled) continue;

        std::string basePath = m_dir+"/"+trigger+"/Distributions/";
        
        std::string type="";
        bool isL1=false;
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
	parseTriggerName(trigger,m_defaultProbePid,isL1,type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger

        if(isL1) etthr=l1thr;
        if ( executeNavigation(trigger).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger);
        for(unsigned int i=0;i<m_objTEList.size();i++){
            efficiency(m_dir+"/"+trigger+"/Efficiency/",etthr,m_objTEList[i]); // Requires offline match
            inefficiency(m_dir+"/"+trigger+"/Efficiency/HLT",runNumber,eventNumber,etthr,m_objTEList[i]); // Requires offline match
            resolution(m_dir+"/"+trigger,m_objTEList[i]); // Requires offline match
            fillShowerShapes(m_dir+"/"+trigger+"/Distributions/Offline",m_objTEList[i].first); // Fill Offline shower shapes
            if(xAOD::EgammaHelpers::isElectron(m_objTEList[i].first)) { // Fill offline tracking
                const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (m_objTEList[i].first);
                fillTracking(m_dir+"/"+trigger+"/Distributions/Offline",elOff);
            }
        }
        // Fill distributions / trigger
        distribution(basePath,trigger,type);
        
        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavAnalysisTool::childFinalize(){
    ATH_MSG_DEBUG("Processed N events " << m_eventCounter);
    return StatusCode::SUCCESS;
}

void TrigEgammaNavAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_DEBUG("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    bookAnalysisHistos(basePath);
}

void TrigEgammaNavAnalysisTool::bookPerCategory(const std::string category){
    ATH_MSG_DEBUG("Now booking category histograms " << category);
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
    if(m_lumiTool)
        avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();
    ATH_MSG_DEBUG("Fill probe histograms");

    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedL2=false;
    bool passedEFCalo=false;
    bool passedEF=false;
    if ( feat ) {
        ATH_MSG_DEBUG("Retrieve Ancestor passed");
        passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
        passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
        if(pairObj.first->type()==xAOD::Type::Electron)
            passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);

        else 
            passedL2 = ancestorPassed<xAOD::TrigPhotonContainer>(feat);
        
        passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
        
        if(pairObj.first->type()==xAOD::Type::Electron)
            passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
        else 
            passedEF = ancestorPassed<xAOD::PhotonContainer>(feat);

    } // Features
    fillEfficiency(basePath+"L1Calo",passedL1Calo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"L2Calo",passedL2Calo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"L2",passedL2,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"EFCalo",passedEFCalo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"HLT",passedEF,etthr,et,eta,phi,avgmu);
    ATH_MSG_DEBUG("Complete efficiency"); 
}

