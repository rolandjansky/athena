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
    std::vector<std::string> selectElectronChains  = tdt()->getListOfTriggers("HLT_e.*, L1_EM.*");
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
        ATH_MSG_DEBUG("Electron trigger " << selectElectronChains[j]);
    }
    std::vector<std::string> selectPhotonChains  = tdt()->getListOfTriggers("HLT_g.*");

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
    setLabels(plot()->hist1("electrons"),el_labels);
    
    addHistogram(new TH1F("trigger_counts", "Trigger Counts; Trigger ; Count", nTrigger, 1, nTrigger));
    setLabels(plot()->hist1("trigger_counts"),m_trigList);
   
    for (int i = 0; i < (int) m_trigList.size(); i++) {
        bookPerSignature(m_trigList[i]);
        setTrigInfo(m_trigList[i]);
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

    TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex();

    int ilist=0;
    for(const auto trigger : m_trigList){
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger 
                << " " << getTrigInfo(trigger).trigName); 

        // Trigger counts
        cd(m_dir+"/Counters");
        if(tdt()->isPassed("HLT_"+trigger)) hist1("trigger_counts")->AddBinContent(ilist+1);
        
        // Skip event if prescaled out 
        // Prescale cut has ill effects
        // if(tdt()->isPassedBits("HLT_"+trigger) & TrigDefs::EF_prescaled) continue;
        std::string basePath = m_dir+"/"+trigger+"/Distributions/";
        const TrigInfo info = getTrigInfo(trigger);
        if ( TrigEgammaNavBaseTool::executeNavigation(info).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }
        // Fill distributions / trigger
        for( const auto& tool : m_tools) {
            // Set detail level from analysis tool each time
            tool->setDetail(getDetail()); 
            if(tool->toolExecute(m_dir,info,m_objTEList).isFailure())
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
        }
        ilist++;
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

