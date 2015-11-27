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
}

//**********************************************************************
StatusCode TrigEgammaNavTPAnalysisTool::childInitialize(){

    m_eventCounter=0;
    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPAnalysisTool::childBook(){
    ATH_MSG_DEBUG("Now configuring chains for analysis");
    std::vector<std::string> selectElectronChains  = m_trigdec->getListOfTriggers("HLT_e.*,L1_EM.*");
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
        ATH_MSG_VERBOSE("Electron trigger " << selectElectronChains[j]);
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
    // Book histograms for average efficiencies and counters
    const int nTrigger = (int) m_trigList.size();
    addDirectory(m_dir+"/Counters");
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
    }



    return StatusCode::SUCCESS;
}

void TrigEgammaNavTPAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_VERBOSE("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    addDirectory(basePath);
    addHistogram(new TH1F("ProbeCutCounter", "Number of Probes; Cut ; Count", 12, 0., 12));
    addHistogram(new TH1F("TagCutCounter", "Number of Tags; Cut ; Count", 10, 0., 10));
    bookAnalysisHistos(basePath);
}


StatusCode TrigEgammaNavTPAnalysisTool::childExecute()
{

    m_eventCounter++;
    m_eventInfo=0;
    cd(m_dir+"/Counters");
    hist1("CutCounter")->Fill("Events",1);

    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve eventInfo ");
        return StatusCode::FAILURE;
    }
    unsigned int runNumber               = m_eventInfo->runNumber();
    unsigned int eventNumber             = m_eventInfo->eventNumber();
    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;
    hist1("CutCounter")->Fill("EventWise",1);
    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string probeTrigger = m_trigList.at(ilist);
        const char * cprobeTrigger = m_trigList.at(ilist).c_str();

        std::string type="";
        bool isL1=false;
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
        parseTriggerName(probeTrigger,m_defaultProbePid,isL1,type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
        std::string basePath = m_dir+"/"+probeTrigger +"/Efficiency/" ;
	
        /*std::string trigname="";
	if (isL1) {
	  trigname = probeTrigger;
	} else {
	  trigname = "HLT_"+probeTrigger;
	}*/
        if(isL1) etthr=l1thr;
        cd(m_dir+"/"+probeTrigger);
        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;
        // Fill online disrtiubtions for Zee reco events
        if(m_probeElectrons.size() > 1)
            distribution(m_dir+"/"+probeTrigger+"/Distributions/",probeTrigger,type);
        // Just for counting
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            const xAOD::Electron* offEl = m_probeElectrons[i].first;
            float et = getEt(offEl)/1e3;//offEl->caloCluster()->et()/1e3;
            float eta = offEl->eta();
            float phi = offEl->phi();
            float mass = m_mee[i]/1e3;
            bool passedL1Calo=false;
            bool passedL2Calo=false;
            bool passedL2=false;
            bool passedEFCalo=false;
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            
            inefficiency(m_dir+"/"+probeTrigger+"/Efficiency/HLT",runNumber,eventNumber,etthr,m_probeElectrons[i]); // Requires offline match
            resolution(m_dir+"/"+probeTrigger,m_probeElectrons[i]); // Requires offline match
            
            // Fill the offline distributions for selected Zee probe electrons
            fillShowerShapes(m_dir+"/"+probeTrigger+"/Distributions/Offline",offEl); // Fill Offline shower shapes
            fillTracking(m_dir+"/"+probeTrigger+"/Distributions/Offline",offEl); // Fill HLT shower shapes
            
            float avgmu=0.;
            if(m_lumiTool)
                avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();

            cd(m_dir+"/Counters");
            if(et > etthr + 1.0)
                hist1("nProbes")->Fill(cprobeTrigger,1);
            if ( feat ) {
                passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
                passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                if(et > etthr + 1.0){
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
            fillEfficiency(basePath+"L1Calo",passedL1Calo,etthr,et,eta,phi,avgmu,mass);
            fillEfficiency(basePath+"L2Calo",passedL2Calo,etthr,et,eta,phi,avgmu,mass);
            fillEfficiency(basePath+"L2",passedL2,etthr,et,eta,phi,avgmu,mass);
            fillEfficiency(basePath+"EFCalo",passedEFCalo,etthr,et,eta,phi,avgmu,mass);
            fillEfficiency(basePath+"HLT",passedEF,etthr,et,eta,phi,avgmu,mass);
        } // End loop over electrons
    } // End loop over trigger list
    cd(m_dir+"/Counters");
    hist1("CutCounter")->Fill("Success",1);
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPAnalysisTool::childFinalize()
{
   /* cd(m_dir+"/Counters");

    hist1("nProbes")->Sumw2();
    
    hist1("nProbesL1")->Sumw2();
    hist1("EffL1")->Divide(hist1("nProbesL1"),hist1("nProbes"),1,1,"b");
    
    hist1("nProbesL2Calo")->Sumw2();
    hist1("EffL2Calo")->Divide(hist1("nProbesL2Calo"),hist1("nProbes"),1,1,"b");
    
    hist1("nProbesL2")->Sumw2();
    hist1("EffL2")->Divide(hist1("nProbesL2"),hist1("nProbes"),1,1,"b");
    
    hist1("nProbesEFCalo")->Sumw2();
    hist1("EffEFCalo")->Divide(hist1("nProbesEFCalo"),hist1("nProbes"),1,1,"b");
    
    hist1("nProbesHLT")->Sumw2();
    hist1("EffHLT")->Divide(hist1("nProbesHLT"),hist1("nProbes"),1,1,"b");*/

    return StatusCode::SUCCESS;
}
