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

//**********************************************************************

TrigEgammaNavTPAnalysisTool::
TrigEgammaNavTPAnalysisTool( const std::string& myname )
: TrigEgammaNavTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavTPAnalysisTool");
  declareProperty("IsEMLabels",m_labels);
}

//**********************************************************************
StatusCode TrigEgammaNavTPAnalysisTool::childInitialize(){

    m_eventCounter=0;
    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPAnalysisTool::childBook(){
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
    // Book histograms for average efficiencies and counters
    const int nTrigger = (int) m_trigList.size();
    addDirectory(m_dir);
    addHistogram(new TH1F("nProbes", "Number of Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL1", "Number of L1 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL2", "Number of L2 Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesL2Calo", "Number of L2Calo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesEFCalo", "Number of EFCalo Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("nProbesHLT", "Number of HLT Probes; Trigger ; Count", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("EffL1", "Average L1 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("EffL2", "Average L2 Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("EffL2Calo", "Average L2Calo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("EffEFCalo", "Average EFCalo Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    addHistogram(new TH1F("EffHLT", "Average HLT Efficiency; Trigger ; #epsilon", nTrigger, 0., nTrigger));
    
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
    // Book the histograms per signature
    for (int i = 0; i < (int) m_trigList.size(); i++) {
        bookPerSignature(m_trigList[i]);
    }



    return StatusCode::SUCCESS;
}

void TrigEgammaNavTPAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_DEBUG("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    bookAnalysisHistos(basePath);

}


StatusCode TrigEgammaNavTPAnalysisTool::childExecute()
{

    m_eventCounter++;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string probeTrigger = m_trigList.at(ilist);

        std::string type="";
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
        parseTriggerName(probeTrigger,"Loose",type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
        std::string basePath = m_dir+"/"+probeTrigger+"/Efficiency/";
        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;

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

            resolution(m_dir+"/"+probeTrigger,m_probeElectrons[i]); // Requires offline match
            
            float avgmu=0.;
            if(m_lumiTool)
                avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();

            cd(m_dir);
            if(et > etthr + 1.0)
                hist1("nProbes")->AddBinContent(ilist+1);
            
            fillHistos(basePath+"L1Calo",etthr,et,eta,phi,avgmu,mass);
            fillHistos(basePath+"L2Calo",etthr,et,eta,phi,avgmu,mass);
            fillHistos(basePath+"L2",etthr,et,eta,phi,avgmu,mass);
            fillHistos(basePath+"EFCalo",etthr,et,eta,phi,avgmu,mass);
            fillHistos(basePath+"HLT",etthr,et,eta,phi,avgmu,mass);

            if ( feat ) {
                passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
                passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                if( passedL1Calo){
                    fillMatchHistos(basePath+"L1Calo",etthr,et,eta,phi,avgmu,mass);
                    cd(m_dir);
                    if(et > etthr + 1.0)
                        hist1("nProbesL1")->AddBinContent(ilist+1);
                }
                if( passedL2Calo ){
                    fillMatchHistos(basePath+"L2Calo",etthr,et,eta,phi,avgmu,mass);
                    cd(m_dir);
                    if(et > etthr + 1.0)
                        hist1("nProbesL2Calo")->AddBinContent(ilist+1);
                }
                if( passedL2 ){
                    fillMatchHistos(basePath+"L2",etthr,et,eta,phi,avgmu,mass);
                    cd(m_dir);
                    if(et > etthr + 1.0)
                        hist1("nProbesL2")->AddBinContent(ilist+1);
                }
                if( passedEFCalo ){
                    fillMatchHistos(basePath+"EFCalo",etthr,et,eta,phi,avgmu,mass);
                    cd(m_dir);
                    if(et > etthr + 1.0)
                        hist1("nProbesEFCalo")->AddBinContent(ilist+1);
                }
                if( passedEF ){
                    fillMatchHistos(basePath+"HLT",etthr,et,eta,phi,avgmu,mass);
                    cd(m_dir);
                    if(et > etthr + 1.0)
                        hist1("nProbesHLT")->AddBinContent(ilist+1);
                }
                else {
                    ATH_MSG_DEBUG("Fails EF, find nearby candidates");
                    // Inefficiency analysis
                    const xAOD::Electron* selEF = NULL;
                    const xAOD::Photon* selPh = NULL;
                    const xAOD::CaloCluster* selClus = NULL;
                    const xAOD::TrackParticle* selTrk = NULL;
                    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
                    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
                    const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat);
                    const auto* EFTrk = getFeature<xAOD::TrackParticleContainer>(feat);
                    float dRmax=0.15;
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
                    if ( EFPh != NULL ){
                        ATH_MSG_DEBUG("Retrieved PhotonnContainer for inefficiency " << EFPh->size());
                        for(const auto& ph : *EFPh){
                            float dr=dR(eta,phi,ph->eta(),ph->phi());
                            if ( dr<dRmax){
                                dRmax=dr;
                                selPh = ph;
                            } // dR
                        } // loop over EFEl
                        ATH_MSG_DEBUG("Closest electron dR " << dRmax);
                    } //FC exists
                    else ATH_MSG_DEBUG("Photon Container NULL");
                    if ( EFClus != NULL ){
                        ATH_MSG_DEBUG("Retrieved PhotonContainer for inefficiency");
                        for(const auto& clus : *EFClus){
                            float dr=dR(eta,phi,clus->eta(),clus->phi());
                            if(dr<dRmax){
                                dRmax=dr;
                                selClus = clus;
                            } // dR
                        } // loop over EFPh
                    } //FC exists
                    dRmax=0.15;
                    if ( EFTrk != NULL ){
                        ATH_MSG_DEBUG("Retrieved PhotonContainer for inefficiency");
                        for(const auto& trk : *EFTrk){
                            float dr=dR(eta,phi,trk->eta(),trk->phi());
                            if(dr<dRmax){
                                dRmax=dr;
                                selTrk = trk;
                            } // dR
                        } // loop over EFPh
                    } //FC exists
                    // Call inefficiency method in AnalysisBaseTool
                    fillInefficiency(basePath+"HLT",selEF,selPh,selClus,selTrk);
                }
            } // Features
        } // End loop over electrons
    } // End loop over trigger list

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPAnalysisTool::childFinalize()
{
    cd(m_dir);

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
    hist1("EffHLT")->Divide(hist1("nProbesHLT"),hist1("nProbes"),1,1,"b");

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
