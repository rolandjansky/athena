/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaNavTPAnalysisTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Derived class for measuring the trigger efficiency.
 *      Produces trigger efficiency distributions at each trigger level
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPAnalysisTool.h"
#include "TProfile.h"
//**********************************************************************

TrigEgammaNavTPAnalysisTool::
TrigEgammaNavTPAnalysisTool( const std::string& myname )
: TrigEgammaNavTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavTPAnalysisTool");
  declareProperty("IsEMLabels",m_labels);
  declareProperty("CutLabels",m_cutlabels);
  declareProperty("ProbeLabels",m_probelabels);
  declareProperty("TagLabels",m_taglabels);
  
  m_eventInfo=nullptr;
}

//**********************************************************************
StatusCode TrigEgammaNavTPAnalysisTool::childInitialize(){

    m_eventCounter=0;
    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPAnalysisTool::childBook(){
    ATH_MSG_DEBUG("Now configuring chains for analysis");
    std::vector<std::string> selectElectronChains  = tdt()->getListOfTriggers("HLT_e.*,L1_EM.*");
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
        ATH_MSG_VERBOSE("Electron trigger " << selectElectronChains[j]);
    }
    std::vector<std::string> selectPhotonChains  = tdt()->getListOfTriggers("HLT_g.*");

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
    // Book histograms for average efficiencies and counters
    const int nTrigger = (int) m_trigList.size();
    addDirectory(m_dir+"/Counters");
    addHistogram(new TH1F("ProbeCutCounter", "Number of Probes; Cut ; Count", 12, 0., 12));
    addHistogram(new TH1F("TagCutCounter", "Number of Tags; Cut ; Count", 10, 0., 10));
    addHistogram(new TH1F("Mee", "Offline M(ee); m_ee [GeV] ; Count", 50, m_ZeeMassMin, m_ZeeMassMax));
    addHistogram(new TH1I("CutCounter", "Event Selection; Cut ; Count", 6, 0., 6));
    addHistogram(new TH1F("nProbes", "Number of Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL1", "Number of L1 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL2", "Number of L2 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL2Calo", "Number of L2Calo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesEFCalo", "Number of EFCalo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesHLT", "Number of HLT Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TProfile("EffL1", "Average L1 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TProfile("EffL2", "Average L2 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TProfile("EffL2Calo", "Average L2Calo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TProfile("EffEFCalo", "Average EFCalo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TProfile("EffHLT", "Average HLT Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    
    setLabels(hist1("nProbes"),m_trigList);
    setLabels(hist1("nProbesL1"),m_trigList);
    setLabels(hist1("nProbesL2"),m_trigList);
    setLabels(hist1("nProbesL2Calo"),m_trigList);
    setLabels(hist1("nProbesEFCalo"),m_trigList);
    setLabels(hist1("nProbesHLT"),m_trigList);
    
    setLabels(hist1("EffL1"),m_trigList);
    setLabels(hist1("EffL2"),m_trigList);
    setLabels(hist1("EffL2Calo"),m_trigList);
    setLabels(hist1("EffEFCalo"),m_trigList);
    setLabels(hist1("EffHLT"),m_trigList);
    setLabels(hist1("CutCounter"),m_cutlabels);
    // Book the histograms per signature
    for (int i = 0; i < (int) m_trigList.size(); i++) {
        bookPerSignature(m_trigList[i]);
        setTrigInfo(m_trigList[i]);
    }



    return StatusCode::SUCCESS;
}

void TrigEgammaNavTPAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_VERBOSE("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    bookAnalysisHistos(basePath);
}


StatusCode TrigEgammaNavTPAnalysisTool::childExecute()
{

    m_eventCounter++;
    cd(m_dir+"/Counters");
    hist1("CutCounter")->Fill("Events",1);

    if ( !TrigEgammaNavTPBaseTool::EventWiseSelection() ){
        ATH_MSG_DEBUG("Fails EventWise selection");
        return StatusCode::SUCCESS;
    }
    TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex();
    // Event Wise Selection (independent of the required signatures)
    hist1("CutCounter")->Fill("EventWise",1);
    // Select TP Pairs
    ATH_MSG_DEBUG("Execute TP selection");
    TrigEgammaNavTPBaseTool::executeTandP();

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string probeTrigger = m_trigList.at(ilist);
        const char * cprobeTrigger = m_trigList.at(ilist).c_str();
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << probeTrigger);
       
        const TrigInfo info = getTrigInfo(probeTrigger);
        std::string trigName=probeTrigger;
        if(info.trigL1)      trigName=probeTrigger;
        else trigName="HLT_"+probeTrigger;
        
        cd(m_dir+"/"+probeTrigger);
        ATH_MSG_DEBUG("Trigger " << probeTrigger << " pidword " << info.trigPidDecorator << " threshold " << info.trigThrHLT);
        TrigEgammaNavTPBaseTool::matchObjects(trigName);

        // Just for counting
        ATH_MSG_DEBUG("Probes " << m_probeElectrons.size() << " Pairs " << m_pairObj.size() );
        for( const auto& tool : m_tools) {
            tool->setDetail(getDetail()); 
            if(tool->toolExecute(m_dir,info,m_pairObj).isFailure())
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
        }
        for(unsigned int i=0;i<m_pairObj.size();i++){

            const xAOD::Electron* offEl = static_cast<const xAOD::Electron *> (m_pairObj[i].first);
            float et = getEt(offEl)/1e3;
            if(et < info.trigThrHLT-5.0) continue; 
            if(!offEl->auxdecor<bool>(info.trigPidDecorator)) continue; 
            bool passedL1Calo=false;
            bool passedL2Calo=false;
            bool passedL2=false;
            bool passedEFCalo=false;
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_pairObj[i].second;

            cd(m_dir+"/Counters");
            if(et > info.trigThrHLT + 1.0)
                hist1("nProbes")->Fill(cprobeTrigger,1);
            if ( feat ) {
                passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
                passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                if(et > info.trigThrHLT + 1.0){
                    if( passedL1Calo){
                        hist1("nProbesL1")->Fill(cprobeTrigger,1);
                        hist1("EffL1")->Fill(cprobeTrigger,1);
                    }
                    else  hist1("EffL1")->Fill(cprobeTrigger,0);

                    if( passedL2Calo ){
                        hist1("nProbesL2Calo")->Fill(cprobeTrigger,1);
                        hist1("EffL2Calo")->Fill(cprobeTrigger,1);
                    }
                    else  hist1("EffL2Calo")->Fill(cprobeTrigger,0);
                    
                    if( passedL2 ){
                        hist1("nProbesL2")->Fill(cprobeTrigger,1);
                        hist1("EffL2")->Fill(cprobeTrigger,1);
                    }
                    else  hist1("EffL2")->Fill(cprobeTrigger,0);
                    
                    if( passedEFCalo ){
                        hist1("nProbesEFCalo")->Fill(cprobeTrigger,1);
                        hist1("EffEFCalo")->Fill(cprobeTrigger,1);
                    }
                    else hist1("EffEFCalo")->Fill(cprobeTrigger,0);
                    
                    if( passedEF ){
                        hist1("nProbesHLT")->Fill(cprobeTrigger,1);
                        hist1("EffHLT")->Fill(cprobeTrigger,1);
                    }
                    else  hist1("EffHLT")->Fill(cprobeTrigger,0);
                }
            } // Features
            // Fill TProfile for no feature found (means no match)
            else {
                hist1("EffL1")->Fill(cprobeTrigger,0);
                hist1("EffL2Calo")->Fill(cprobeTrigger,0);
                hist1("EffL2")->Fill(cprobeTrigger,0);
                hist1("EffEFCalo")->Fill(cprobeTrigger,0);
                hist1("EffHLT")->Fill(cprobeTrigger,0);
            }
        } // End loop over electrons
    } // End loop over trigger list
    cd(m_dir+"/Counters");
    hist1("CutCounter")->Fill("Success",1);
    ATH_MSG_DEBUG("Clear decorations");
    //clearDecorations();
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPAnalysisTool::childFinalize()
{
    return StatusCode::SUCCESS;
}
