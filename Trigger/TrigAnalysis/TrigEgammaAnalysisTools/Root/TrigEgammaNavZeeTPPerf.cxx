/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPPerf
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Derived class for studying performance triggers (e.g e0_perf)
 *      Can be used to emulate trigger selection with triggered objects.
 *      Or to study performance of new calibrations, selections, etc.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPPerf.h"

//**********************************************************************

TrigEgammaNavZeeTPPerf::
TrigEgammaNavZeeTPPerf( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavZeeTPPerf");
  declareProperty("ElectronOnlPPSelector", m_electronOnlPPCutIDTool);
}

//**********************************************************************
StatusCode TrigEgammaNavZeeTPPerf::childInitialize(){

    m_eventCounter=0;
    std::cout << "child Initialize" << std::endl;

   
    addDirectory(m_dir+"/HLTEfficiency");
    std::vector <std::string> dirnames;
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        ATH_MSG_INFO("Trigger " << probeTrigger);
        dirnames.push_back(m_dir+"/" + probeTrigger + "/HLTEfficiencies");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("hlt_pt", "HLT p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("hlt_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("hlt_highet", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 1000.));
        addHistogram(new TH1F("hlt_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("hlt_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("match_pt", "HLT p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("match_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("match_highet", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 1000.));
        addHistogram(new TH1F("match_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("match_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 1000.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon", 50, 0., 100.));
        addHistogram(new TH1F("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 1000.));
        addHistogram(new TH1F("eff_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("eff_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        
        addHistogram(new TH1F("e011", "e011; e011 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("e132", "e132; e132 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("e237", "e237; e237 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("e277", "e277; e277 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("ethad", "ethad; ethad ; Count", 50, -5., 5.));
        addHistogram(new TH1F("ethad1", "ethad1; ehad1 ; Count", 50, -5., 5.));
        addHistogram(new TH1F("weta1", "weta1; weta1 ; Count", 50, 0., 1.));
        addHistogram(new TH1F("weta2", "weta2; weta2 ; Count", 50, 0., 0.05));
        addHistogram(new TH1F("f1", "f1; f1 ; Count", 50, -0.1, 1.1));
        addHistogram(new TH1F("f3", "f3; f3 ; Count", 50, -0.1, 0.25));
        addHistogram(new TH1F("e2tsts1", "e2tsts1; e2tsts1 ; Count", 50, 0., 100.));
        addHistogram(new TH1F("Reta", "Reta; Reta ; Count", 50, 0., 2.));
        addHistogram(new TH1F("Rphi", "Rphi; Rphi ; Count", 50, 0., 2.));
        addHistogram(new TH1F("Rhad", "Rhad; Rhad ; Count", 50, -0.25, 0.25));
        addHistogram(new TH1F("Rhad1", "Rhad1; Rhad1 ; Count", 50, -1., 1.));
        addHistogram(new TH1F("deta1", "deta1; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("deta2", "deta2; deta2 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("dphi2", "dphi2; dphi2 ; Count", 100, -0.25, 0.25));
        addHistogram(new TH1F("dphiresc", "dphiresc; dphiresc ; Count", 100, -0.1, 0.1));
        addHistogram(new TH1F("d0", "d0; d0 ; Count", 100, -0.5, 0.5));
        addHistogram(new TH1F("d0sig", "d0sig; d0sig ; Count", 50, -10, 10));
        addHistogram(new TH1F("eratio","eratio; eratio; Count",50, 0, 2));
        addHistogram(new TH1F("eprobht","eProbHT; eProbHT; Count",50, 0, 1.1));
        addHistogram(new TH1F("nscthits","nSCTHit; nSCTHits; Count",30, 0, 30));
        addHistogram(new TH1F("npixhits","nPixHit; nPixHits; Count",10, 0, 10));
        addHistogram(new TH1F("charge","charge; charge; Count", 4,-2,2));
        
        addHistogram(new TH1F("hlt_e011", "e011; e011 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("hlt_e132", "e132; e132 ; Count", 165, -15., 150.));
        addHistogram(new TH1F("hlt_e237", "e237; e237 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("hlt_e277", "e277; e277 ; Count", 215, -15., 200.));
        addHistogram(new TH1F("hlt_ethad", "ethad; ethad ; Count", 50, -5., 5.));
        addHistogram(new TH1F("hlt_ethad1", "ethad1; ehad1 ; Count", 50, -5., 5.));
        addHistogram(new TH1F("hlt_weta1", "weta1; weta1 ; Count", 50, 0., 1.));
        addHistogram(new TH1F("hlt_weta2", "weta2; weta2 ; Count", 50, 0., 0.05));
        addHistogram(new TH1F("hlt_f1", "f1; f1 ; Count", 50, -0.1, 1.1));
        addHistogram(new TH1F("hlt_f3", "f3; f3 ; Count", 50, -0.1, 0.25));
        addHistogram(new TH1F("hlt_e2tsts1", "e2tsts1; e2tsts1 ; Count", 50, 0., 100.));
        addHistogram(new TH1F("hlt_Reta", "Reta; Reta ; Count", 50, 0., 2.));
        addHistogram(new TH1F("hlt_Rphi", "Rphi; Rphi ; Count", 50, 0., 2.));
        addHistogram(new TH1F("hlt_Rhad", "Rhad; Rhad ; Count", 50,-0.25, 0.25));
        addHistogram(new TH1F("hlt_Rhad1", "Rhad1; Rhad1 ; Count", 50, -1., 1.));
        addHistogram(new TH1F("hlt_deta1", "deta1; deta1 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("hlt_deta2", "deta2; deta2 ; Count", 90, -0.03, 0.03));
        addHistogram(new TH1F("hlt_dphi2", "dphi2; dphi2 ; Count", 100, -0.25, 0.25));
        addHistogram(new TH1F("hlt_dphiresc", "dphiresc; dphiresc ; Count", 100, -0.1, 0.1));
        addHistogram(new TH1F("hlt_d0", "d0; d0 ; Count", 100, -0.5, 0.5));
        addHistogram(new TH1F("hlt_d0sig", "d0sig; d0sig ; Count", 50, -10, 10));
        addHistogram(new TH1F("hlt_eratio","eratio; eratio; Count",50, 0, 2));
        addHistogram(new TH1F("hlt_eprobht","eProbHT; eProbHT; Count",50, 0, 1.1));
        addHistogram(new TH1F("hlt_nscthits","nSCTHit; nSCTHits; Count",30, 0, 30));
        addHistogram(new TH1F("hlt_npixhits","nPixHit; nPixHits; Count",10, 0, 10));
        addHistogram(new TH1F("hlt_charge","charge; charge; Count", 4,-2,2));
    }

    
    addDirectory(m_dir+"/TrigElectrons");
    addDirectory(m_dir+"/EmTauRoI");

      return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPPerf::childExecute()
{

    ATH_MSG_DEBUG("Executing NavZeePerf");
    m_eventCounter++;
    m_offElectrons=0;
    m_onlElectrons=0;
    m_trigElectrons=0;
    m_caloClusters=0;
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    ATH_MSG_INFO("Offline Electron container size " << m_offElectrons->size());

    if ( (m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("Online Electron container size " << m_onlElectrons->size());
    if ( (m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("Trig Electron container size " << m_trigElectrons->size());
    if ( (m_storeGate->retrieve(m_emTauRoI,"LVL1EmTauRoIs")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("L1 EM container size " << m_emTauRoI->size());
    if( (m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve CaloClusters");
    }
    else ATH_MSG_INFO("CaloCluster size " << m_caloClusters->size());
    if( (m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve TrigEMClusters");
    }
    else ATH_MSG_INFO("TrigEMCluser size " << m_trigEMClusters->size());


    StatusCode sc = TDCounts();

    if(sc != StatusCode::SUCCESS)
        ATH_MSG_DEBUG("TDCounts Fails");
    sc = SimpleEfficiency();
    if(sc != StatusCode::SUCCESS)
        ATH_MSG_DEBUG("SimpleEfficiency Fails");

    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPPerf::childFinalize()
{

    ATH_MSG_INFO ("Finalizing " << name() << "...");
    std::ofstream countsFile ("TriggerCounts.txt");
    if(countsFile.is_open()){
        for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
            ATH_MSG_INFO(iter->first << " == " << iter->second);
            countsFile << std::setw(50) << iter->first << " ---- Passed " << iter->second <<" -----\n";
        }
    }
    countsFile.close();
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
        hist1("pt")->Sumw2();
        hist1("match_pt")->Sumw2();
        hist1("eff_pt")->Divide(hist1("match_pt"),hist1("pt"),1,1,"b");
        hist1("et")->Sumw2();
        hist1("match_et")->Sumw2();
        hist1("eff_et")->Divide(hist1("match_et"),hist1("et"),1,1,"b");
        hist1("highet")->Sumw2();
        hist1("match_highet")->Sumw2();
        hist1("eff_highet")->Divide(hist1("match_highet"),hist1("highet"),1,1,"b");
        hist1("eta")->Sumw2();
        hist1("match_eta")->Sumw2();
        hist1("eff_eta")->Divide(hist1("match_eta"),hist1("eta"),1,1,"b");
        hist1("phi")->Sumw2();
        hist1("match_phi")->Sumw2();
        hist1("eff_phi")->Divide(hist1("match_phi"),hist1("phi"),1,1,"b");
    }
    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPPerf::TDCounts(){

  ATH_MSG_DEBUG( "L1: " << m_trigdec->isPassed( "L1_.*" )
          << ", L2: " << m_trigdec->isPassed( "L2_.*" )
          << ", EF: " << m_trigdec->isPassed( "EF_.*" )
          << ", HLT: " << m_trigdec->isPassed( "HLT_.*" ) );

  auto chainGroups = m_trigdec->getChainGroup("HLT_e.*");
  for(auto &trig : chainGroups->getListOfTriggers()) {
      if(m_trigdec->isPassed(trig))
          ATH_MSG_DEBUG("Passed: " << trig);
      m_counterBits[trig]+=m_trigdec->isPassed(trig);
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPPerf::Distributions(const xAOD::Electron *onl, const xAOD::Electron *off){
    if(!onl) return StatusCode::FAILURE;
    if(!off) return StatusCode::FAILURE;
    hist1("e011")->Fill(getShowerShape_e011(off)/1e3);
    hist1("e132")->Fill(getShowerShape_e132(off)/1e3);
    hist1("e237")->Fill(getShowerShape_e237(off)/1e3);
    hist1("e277")->Fill(getShowerShape_e277(off)/1e3);
    hist1("ethad")->Fill(getShowerShape_ethad(off)/1e3);
    hist1("ethad1")->Fill(getShowerShape_ethad1(off)/1e3);
    hist1("Rhad")->Fill(getShowerShape_Rhad(off));
    hist1("Reta")->Fill(getShowerShape_Reta(off));
    hist1("Rphi")->Fill(getShowerShape_Rphi(off));
    hist1("weta1")->Fill(getShowerShape_weta1(off));
    hist1("weta2")->Fill(getShowerShape_weta2(off));
    hist1("f1")->Fill(getShowerShape_f1(off));
    hist1("f3")->Fill(getShowerShape_f3(off));
    hist1("deta1")->Fill(getCaloTrackMatch_deltaEta1(off));
    hist1("deta2")->Fill(getCaloTrackMatch_deltaEta2(off));
    hist1("dphi2")->Fill(getCaloTrackMatch_deltaPhi2(off));
    hist1("dphiresc")->Fill(getCaloTrackMatch_deltaPhiRescaled2(off));
    hist1("d0")->Fill(getTrack_d0(off));
    hist1("d0sig")->Fill(getD0sig(off));
    hist1("eratio")->Fill(getShowerShape_Eratio(off));
    hist1("eprobht")->Fill(getTrackSummaryFloat_eProbabilityHT(off));
    hist1("npixhits")->Fill(getTrackSummary_numberOfPixelHits(off));
    hist1("nscthits")->Fill(getTrackSummary_numberOfSCTHits(off));
    hist1("charge")->Fill(off->charge());

    hist1("hlt_e011")->Fill(getShowerShape_e011(onl)/1e3);
    hist1("hlt_e132")->Fill(getShowerShape_e132(onl)/1e3);
    hist1("hlt_e237")->Fill(getShowerShape_e237(onl)/1e3);
    hist1("hlt_e277")->Fill(getShowerShape_e277(onl)/1e3);
    hist1("hlt_ethad")->Fill(getShowerShape_ethad(onl)/1e3);
    hist1("hlt_ethad1")->Fill(getShowerShape_ethad1(onl)/1e3);
    hist1("hlt_Rhad")->Fill(getShowerShape_Rhad(onl));
    hist1("hlt_Reta")->Fill(getShowerShape_Reta(onl));
    hist1("hlt_Rphi")->Fill(getShowerShape_Rphi(onl));
    hist1("hlt_weta1")->Fill(getShowerShape_weta1(onl));
    hist1("hlt_weta2")->Fill(getShowerShape_weta2(onl));
    hist1("hlt_f1")->Fill(getShowerShape_f1(onl));
    hist1("hlt_f3")->Fill(getShowerShape_f3(onl));
    hist1("hlt_deta1")->Fill(getCaloTrackMatch_deltaEta1(onl));
    hist1("hlt_deta2")->Fill(getCaloTrackMatch_deltaEta2(onl));
    hist1("hlt_dphi2")->Fill(getCaloTrackMatch_deltaPhi2(onl));
    hist1("hlt_dphiresc")->Fill(getCaloTrackMatch_deltaPhiRescaled2(onl));
    hist1("hlt_d0")->Fill(getTrack_d0(onl));
    hist1("hlt_d0sig")->Fill(getD0sig(onl));
    hist1("hlt_eratio")->Fill(getShowerShape_Eratio(onl));
    hist1("hlt_eprobht")->Fill(getTrackSummaryFloat_eProbabilityHT(onl));
    hist1("hlt_npixhits")->Fill(getTrackSummary_numberOfPixelHits(onl));
    hist1("hlt_nscthits")->Fill(getTrackSummary_numberOfSCTHits(onl));
    hist1("hlt_charge")->Fill(onl->charge());
    hist1("hlt_pt")->Fill(getTrack_pt(onl)/1e3);
    hist1("hlt_et")->Fill(getEt(onl)/1e3);
    hist1("hlt_highet")->Fill(getEt(onl)/1e3);
    hist1("hlt_eta")->Fill(onl->eta());
    hist1("hlt_phi")->Fill(onl->phi());
    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPPerf::SimpleEfficiency(){
  for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
      std::string probeTrigger = m_probeTrigList.at(ilist);
      cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
      std::string pid = getPid(probeTrigger);
      for(const auto& eg : *m_offElectrons){
          if( !(eg->e()/cosh(eg->trackParticle()->eta())  > 20.0*1.e3) ) continue;
          if(!eg->passSelection(pid)) continue;
          hist1("pt")->Fill(getTrack_pt(eg)/1e3);
          hist1("et")->Fill(getEt(eg)/1e3);
          hist1("highet")->Fill(getEt(eg)/1e3);
          hist1("eta")->Fill(eg->eta());
          hist1("phi")->Fill(eg->phi());
          if ( m_trigdec->isPassed("HLT_"+probeTrigger) ){
              if(isMatchHLT(eg)){
                  hist1("match_pt")->Fill(getTrack_pt(eg)/1e3);
                  hist1("match_et")->Fill(getEt(eg)/1e3);
                  hist1("match_highet")->Fill(getEt(eg)/1e3);
                  hist1("match_eta")->Fill(eg->eta());
                  hist1("match_phi")->Fill(eg->phi());
              }
          }
      }
  }
  return StatusCode::SUCCESS;

}

bool TrigEgammaNavZeeTPPerf::isMatchHLT(const xAOD::Electron *eloff){
    // Find the closest EF object with minimum pt > 25
    double dRMax = 100;
    const xAOD::Electron *elEF = 0;
    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(eloff->pt(), eloff->trackParticle()->eta(), eloff->trackParticle()->phi(), eloff->e());

    bool isMatch = false;
    for(const auto& eg : *m_onlElectrons){
        TLorentzVector elLV;
        elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
        double deltaR = elLV.DeltaR(eloffLV);
        //find closest trigger object
        if (deltaR < dRMax) {
            dRMax = deltaR;
            elEF = eg;
        }
    }
    if(dRMax < 0.07){
        isMatch = true;
        StatusCode sc = Distributions(elEF,eloff);
        if(sc != StatusCode::SUCCESS)
            ATH_MSG_DEBUG("Distributions Fails");
    }
    return isMatch;
}
