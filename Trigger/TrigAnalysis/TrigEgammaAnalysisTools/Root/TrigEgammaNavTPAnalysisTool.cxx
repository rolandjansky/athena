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
  declareProperty("Analysis",m_anatype="TPAnalysis");
  declareProperty("IsEMLabels",m_labels);
  declareProperty("CutLabels",m_cutlabels);
  declareProperty("ProbeLabels",m_probelabels);
  declareProperty("TagLabels",m_taglabels);
 
  // Coverity
  m_eventInfo=nullptr;
  m_eventCounter=0;
}

//**********************************************************************
StatusCode TrigEgammaNavTPAnalysisTool::childInitialize(){

    m_eventCounter=0;
    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPAnalysisTool::childBook(){
    ATH_MSG_DEBUG("Now configuring chains for analysis");
    //Set the base directory from the plot()
    m_dir=plot()->getBasePath();
    std::vector<std::string> chains  = tdt()->getListOfTriggers("HLT_e.*, L1_EM.*, HLT_g.*");

    for(const auto trigName:m_trigInputList){ 
        if (std::find(chains.begin(), chains.end(), trigName) != chains.end()){ 
            if(plot()->getTrigInfoMap().count(trigName) != 0)
                ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
            else 
                m_trigList.push_back(trigName);
        }
    }
    // Book histograms for average efficiencies and counters
    const int nTrigger = (int) m_trigList.size();
    addDirectory(m_dir+"/Expert/Event");
    std::string histname=m_anatype+"_ProbeCutCounter";
    addHistogram(new TH1F(histname.c_str(), "Number of Probes; Cut ; Count", 12, 0., 12));
    histname=m_anatype+"_TagCutCounter";
    addHistogram(new TH1F(histname.c_str(), "Number of Tags; Cut ; Count", 10, 0., 10));
    histname=m_anatype+"_Mee";
    addHistogram(new TH1F(histname.c_str(), "Offline M(ee); m_ee [GeV] ; Count", 50, m_ZeeMassMin, m_ZeeMassMax));
    histname=m_anatype+"_CutCounter";
    addHistogram(new TH1I(histname.c_str(), "Event Selection; Cut ; Count", 6, 0., 6));
    if(nTrigger>0){
        histname=m_anatype+"_trigger_counts";
        addHistogram(new TH1F(histname.c_str(), "Trigger Counts; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbes";
        addHistogram(new TH1F(histname.c_str(), "Number of Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbesL1";
        addHistogram(new TH1F(histname.c_str(), "Number of L1 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbesL2";
        addHistogram(new TH1F(histname.c_str(), "Number of L2 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbesL2Calo";
        addHistogram(new TH1F(histname.c_str(), "Number of L2Calo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbesEFCalo";
        addHistogram(new TH1F(histname.c_str(), "Number of EFCalo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_nProbesHLT";
        addHistogram(new TH1F(histname.c_str(), "Number of HLT Probes; Trigger ; Count", nTrigger, 0., nTrigger));
        histname=m_anatype+"_EffL1";
        addHistogram(new TProfile(histname.c_str(), "Average L1 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
        histname=m_anatype+"_EffL2";
        addHistogram(new TProfile(histname.c_str(), "Average L2 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
        histname=m_anatype+"_EffL2Calo";
        addHistogram(new TProfile(histname.c_str(), "Average L2Calo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
        histname=m_anatype+"_EffEFCalo";
        addHistogram(new TProfile(histname.c_str(), "Average EFCalo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
        histname=m_anatype+"_EffHLT";
        addHistogram(new TProfile(histname.c_str(), "Average HLT Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
        
        setLabels(plot()->hist1(m_anatype+"_trigger_counts"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbes"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbesL1"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbesL2"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbesL2Calo"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbesEFCalo"),m_trigList);
        setLabels(hist1(m_anatype+"_nProbesHLT"),m_trigList);

        setLabels(hist1(m_anatype+"_EffL1"),m_trigList);
        setLabels(hist1(m_anatype+"_EffL2"),m_trigList);
        setLabels(hist1(m_anatype+"_EffL2Calo"),m_trigList);
        setLabels(hist1(m_anatype+"_EffEFCalo"),m_trigList);
        setLabels(hist1(m_anatype+"_EffHLT"),m_trigList);
        setLabels(hist1(m_anatype+"_CutCounter"),m_cutlabels);
    }
    // Book the histograms per signature
    for (int i = 0; i < (int) m_trigList.size(); i++)
        setTrigInfo(m_trigList[i]);
 
    plot()->setEmulation(getEmulation()); 
    if(plot()->book(getTrigInfoMap()).isFailure()){
        ATH_MSG_ERROR("Unable to book histos for " << m_dir); 
        return StatusCode::FAILURE;
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
    cd(m_dir+"/Expert/Event");
    hist1(m_anatype+"_CutCounter")->Fill("Events",1);

    if ( !TrigEgammaNavTPBaseTool::EventWiseSelection() ){
        ATH_MSG_DEBUG("Fails EventWise selection");
        return StatusCode::SUCCESS;
    }
    // Check for Rnn container in SG
    if(m_storeGate->contains<xAOD::TrigRNNOutputContainer>("HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFex")){
        setSGContainsRnn(true);
    }
    if(m_storeGate->contains<xAOD::TrigPhotonContainer>("HLT_xAOD__TrigPhotonContainer_L2PhotonFex")){
        setSGContainsTrigPhoton(true);
    }
    ATH_MSG_DEBUG("Rnn container in SG " << getSGContainsRnn());
    ATH_MSG_DEBUG("TrigPhotonContainer in SG " << getSGContainsTrigPhoton());
    for( const auto& tool : m_tools) {
        tool->setSGContainsRnn(getSGContainsRnn());
        tool->setSGContainsTrigPhoton(getSGContainsTrigPhoton());
    }
    // Event Wise Selection (independent of the required signatures)
    hist1(m_anatype+"_CutCounter")->Fill("EventWise",1);
    // Select TP Pairs
    ATH_MSG_DEBUG("Execute TP selection");
    TrigEgammaNavTPBaseTool::executeTandP();

    // Check HLTResult
    if(tdt()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS;
    }

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string probeTrigger = m_trigList.at(ilist);
        const char * cprobeTrigger = m_trigList.at(ilist).c_str();
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << probeTrigger);
        cd(m_dir+"/Expert/Event");
        if(tdt()->isPassed(probeTrigger))  hist1(m_anatype+"_trigger_counts")->Fill(cprobeTrigger,1);
        
        const TrigInfo info = getTrigInfo(probeTrigger);
        std::string trigName=probeTrigger;
        
        ATH_MSG_DEBUG("Trigger " << probeTrigger << " pidword " << info.trigPidDecorator << " threshold " << info.trigThrHLT);
        TrigEgammaNavTPBaseTool::matchObjects(trigName);

        // Just for counting
        ATH_MSG_DEBUG("Probes " << m_probeElectrons.size() << " Pairs " << m_pairObj.size() );
        for( const auto& tool : m_tools) {
	    ATH_MSG_DEBUG("Running tool " << tool);
            tool->setDetail(getDetail()); 
            tool->setTP(getTP()); 
            tool->setEmulation(getEmulation());
            tool->setPVertex(getNPVtx(), getNGoodVertex());
            tool->setAvgMu(getAvgOnlineMu(),getAvgOfflineMu());
            if(tool->toolExecute(m_dir+"/Expert",info,m_pairObj).isFailure())
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
        }
        if(isPrescaled(probeTrigger)){
            ATH_MSG_DEBUG(probeTrigger << " prescaled, skipping efficiency");    
            continue; //Account for L1 and HLT prescale, discard event
        }
        for(unsigned int i=0;i<m_pairObj.size();i++){
            ATH_MSG_DEBUG("Savin efficiencies...");    

            const xAOD::Electron* offEl = static_cast<const xAOD::Electron *> (m_pairObj[i].first);
            float et = getEt(offEl)/1e3;
            if(et < info.trigThrHLT-5.0) continue;
	    ATH_MSG_DEBUG(" probe et = " << et << "  >=  info.trigThrHLT-5.0 = " << info.trigThrHLT-5.0 );
            if(!offEl->auxdecor<bool>(info.trigPidDecorator)) continue; 
	    ATH_MSG_DEBUG("offEl->auxdecor<bool>(" << info.trigPidDecorator <<  ") True" );
            const HLT::TriggerElement* feat = m_pairObj[i].second;
            setAccept(feat,info); //Sets the trigger accepts
            cd(m_dir+"/Expert/Event");
            if(et > info.trigThrHLT + 1.0)
                hist1(m_anatype+"_nProbes")->Fill(cprobeTrigger,1);
            if ( feat ) {
		ATH_MSG_DEBUG(" Saving efficiencies for " << cprobeTrigger  << " for " << m_anatype);
                if(et > info.trigThrHLT + 1.0){
                    hist1(m_anatype+"_EffL1")->Fill(cprobeTrigger,getAccept().getCutResult("L1Calo"));
                    hist1(m_anatype+"_EffL2Calo")->Fill(cprobeTrigger,getAccept().getCutResult("L2Calo"));
                    hist1(m_anatype+"_EffL2")->Fill(cprobeTrigger,getAccept().getCutResult("L2"));
                    hist1(m_anatype+"_EffEFCalo")->Fill(cprobeTrigger,getAccept().getCutResult("EFCalo"));
                    hist1(m_anatype+"_EffHLT")->Fill(cprobeTrigger,getAccept().getCutResult("HLT"));
                    if( getAccept().getCutResult("L1Calo")){
                        hist1(m_anatype+"_nProbesL1")->Fill(cprobeTrigger,1);
                    }
                    if( getAccept().getCutResult("L2Calo") ){
                        hist1(m_anatype+"_nProbesL2Calo")->Fill(cprobeTrigger,1);
                    }
                    if( getAccept().getCutResult("L2") ){
                        hist1(m_anatype+"_nProbesL2")->Fill(cprobeTrigger,1);
                    }
                    if( getAccept().getCutResult("EFCalo") ){
                        hist1(m_anatype+"_nProbesEFCalo")->Fill(cprobeTrigger,1);
                    }
                    if( getAccept().getCutResult("HLT") ){
                        hist1(m_anatype+"_nProbesHLT")->Fill(cprobeTrigger,1);
                    }
                }
            } // Features

            // Fill TProfile for no feature found (means no match)
            else {
		ATH_MSG_DEBUG(" No Feature!!!  Filling " << cprobeTrigger << " with 0");
                hist1(m_anatype+"_EffL1")->Fill(cprobeTrigger,0);
                hist1(m_anatype+"_EffL2Calo")->Fill(cprobeTrigger,0);
                hist1(m_anatype+"_EffL2")->Fill(cprobeTrigger,0);
                hist1(m_anatype+"_EffEFCalo")->Fill(cprobeTrigger,0);
                hist1(m_anatype+"_EffHLT")->Fill(cprobeTrigger,0);
            }
        } // End loop over electrons
    } // End loop over trigger list
    cd(m_dir+"/Expert/Event");
    hist1(m_anatype+"_CutCounter")->Fill("Success",1);
    ATH_MSG_DEBUG("Clear decorations");
    //clearDecorations();
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPAnalysisTool::childFinalize()
{
    ATH_MSG_DEBUG("Processed N events " << m_eventCounter);
    plot()->setTP(getTP());
    if(plot()->finalizeShifterHistos(getTrigInfoMap()).isFailure()) {
        ATH_MSG_ERROR("Unable to book histos for " << m_dir); 
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}
