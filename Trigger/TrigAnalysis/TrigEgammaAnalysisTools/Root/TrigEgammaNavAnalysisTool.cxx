/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"

using namespace std;

TrigEgammaNavAnalysisTool::TrigEgammaNavAnalysisTool( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  
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
    //Set the base directory from the plot()
    m_dir=plot()->getBasePath();
    std::vector<std::string> chains  = tdt()->getListOfTriggers("HLT_e.*, L1_EM.*, HLT_g.*");

    for(const auto trigName:m_trigInputList){ 
        if (std::find(chains.begin(), chains.end(), trigName) != chains.end()) {
            if(plot()->getTrigInfoMap().count(trigName) != 0)
                ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
            else 
                m_trigList.push_back(trigName);
        }
    }

    // Container level kinematic histograms
    addDirectory(m_dir+"/Expert/Event");
    const int nTrigger = (int) m_trigList.size();
   
    std::string histname=m_anatype+"_electrons";
    addHistogram(new TH1F(histname.c_str(), "Offline Electrons; ; N_{electrons}", 6, 1., 6));   
    std::vector<std::string> el_labels;
    el_labels.push_back("loose");
    el_labels.push_back("medium");
    el_labels.push_back("tight");
    el_labels.push_back("lhloose");
    el_labels.push_back("lhmedium");
    el_labels.push_back("lhtight");
    setLabels(plot()->hist1(histname),el_labels);
    
    histname=m_anatype+"_trigger_counts";
    if(nTrigger>0) {
        addHistogram(new TH1F(histname.c_str(), "Trigger Counts; Trigger ; Count", nTrigger, 1, nTrigger));
        setLabels(plot()->hist1(m_anatype+"_trigger_counts"),m_trigList);
    }
   
    for (const auto trigger: m_trigList)
            setTrigInfo(trigger);

    if(plot()->book(getTrigInfoMap()).isFailure()) {
        ATH_MSG_ERROR("Unable to book histos for " << m_dir); 
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childExecute(){

    ATH_MSG_DEBUG("Executing TrigEgammaValidationTool");
    m_eventCounter++;

    cd(m_dir+"/Expert/Event");
    if( !TrigEgammaNavBaseTool::EventWiseSelection() ) {
        ATH_MSG_DEBUG("Unable to retrieve offline containers");
        return StatusCode::FAILURE;
    }

    TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex();

    int ilist=0;
    for(const auto trigger : m_trigList){
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger 
                << " " << getTrigInfo(trigger).trigName); 

        if(isPrescaled(trigger)) continue; //Account for L1 and HLT prescale discard event
        // Trigger counts
        cd(m_dir+"/Expert/Event");
        if(tdt()->isPassed(trigger)) hist1(m_anatype+"_trigger_counts")->AddBinContent(ilist+1);
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
            tool->setTP(getTP()); 
            if(tool->toolExecute(m_dir+"/Expert",info,m_objTEList).isFailure())
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
        }
        ilist++;
        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavAnalysisTool::childFinalize(){
    ATH_MSG_DEBUG("Processed N events " << m_eventCounter);
    plot()->setTP(getTP());
    if(plot()->finalizeShifterHistos(getTrigInfoMap()).isFailure()) {
        ATH_MSG_ERROR("Unable to book histos for " << m_dir); 
        return StatusCode::FAILURE;
    }
    
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

