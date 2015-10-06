/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"

using namespace std;

TrigEgammaNavAnalysisTool::TrigEgammaNavAnalysisTool( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  declareProperty("DirectoryPath",m_dir="Analysis");
  declareProperty("IsEMLabels",m_labels);
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

    cd(m_dir+"/Counters");
    if( !TrigEgammaNavBaseTool::EventWiseSelection() ) {
        ATH_MSG_DEBUG("Unable to retrieve offline containers");
        return StatusCode::FAILURE;
    }
    unsigned int runNumber               = m_eventInfo->runNumber();
    unsigned int eventNumber             = m_eventInfo->eventNumber();

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string trigger = m_trigList.at(ilist);
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger);
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
        std::string pidword="ElectronPass"+pidname;
        if(isL1) etthr=l1thr;
        if ( TrigEgammaNavBaseTool::executeNavigation(trigger).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }
        // Fill distributions / trigger
        distribution(basePath,trigger,type); 
        for(unsigned int i=0;i<m_objTEList.size();i++){
            efficiency(m_dir+"/"+trigger+"/Efficiency/",etthr,pidword,m_objTEList[i]); // Requires offline match
            // Disabling due to error in retrieving track in distribution method 
            // Get an error in filling track quantities when trying to retrieve eg->trackParticle()
            inefficiency(m_dir+"/"+trigger+"/Efficiency/HLT",runNumber,eventNumber,etthr,m_objTEList[i]);
            resolution(m_dir+"/"+trigger,m_objTEList[i]); // Requires offline match
            fillShowerShapes(m_dir+"/"+trigger+"/Distributions/Offline",m_objTEList[i].first); // Fill Offline shower shapes
            if(xAOD::EgammaHelpers::isElectron(m_objTEList[i].first)) { // Fill offline tracking
              const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (m_objTEList[i].first);
              fillTracking(m_dir+"/"+trigger+"/Distributions/Offline",elOff);
              }
        }
        
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

