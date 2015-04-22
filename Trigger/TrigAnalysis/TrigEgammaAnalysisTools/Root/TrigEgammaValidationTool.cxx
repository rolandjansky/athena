/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaValidationTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *     Generic validation tool for simple trigger matched efficiencies 
 *     Comparison of online/offline distributions 
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaValidationTool.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************


TrigEgammaValidationTool::
TrigEgammaValidationTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="Validation");
  declareProperty("PhotonPid",m_photonPid = "Tight");
  declareProperty("TriggerList",m_probeTrigList);
  declareProperty("doUnconverted", m_doUnconverted=false);
}

//**********************************************************************
StatusCode TrigEgammaValidationTool::childInitialize(){

    m_eventCounter=0;
    ATH_MSG_INFO("TrigEgammaValidationTool: child Initialize");

   
    addDirectory(m_dir+"/HLTEfficiency");
    std::vector <std::string> dirnames;
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        ATH_MSG_INFO("Trigger " << probeTrigger);
        dirnames.push_back(m_dir+"/" + probeTrigger + "/HLTEfficiencies");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_INFO(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("match_pt", "HLT p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("match_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("match_highet", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
        addHistogram(new TH1F("match_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("match_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        
        addHistogram(new TH1F("matchEF_pt", "HLT p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("matchEF_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("matchEF_highet", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 2000.));
        addHistogram(new TH1F("matchEF_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("matchEF_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
        
        addHistogram(new TH1F("eff_pt", "#epsilon(p_T); p_{T} ; #epsilon", 50, 0., 100.));
        addHistogram(new TH1F("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eff_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("eff_phi", "phi; phi ; Count", 50, -3.14, 3.14));
        
        addHistogram(new TH1F("effEF_pt", "#epsilon(p_T); p_{T} ; #epsilon", 50, 0., 100.));
        addHistogram(new TH1F("effEF_et", "#epsilon(E_T); E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("effEF_highet", "#epsilon(E_T); E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("effEF_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("effEF_phi", "phi; phi ; Count", 50, -3.14, 3.14));
    }

    dirnames.clear();
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        ATH_MSG_INFO("Trigger " << probeTrigger);
        dirnames.push_back(m_dir+"/" + probeTrigger + "/Distributions");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_INFO(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.));
        addHistogram(new TH1F("eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("phi", "phi; phi ; Count", 50, -3.14, 3.14));
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

        
        addHistogram(new TH1F("hlt_pt", "HLT p_{T}; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("hlt_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
        addHistogram(new TH1F("hlt_highet", "HLT E_{T}; E_{T} [GeV]; Count", 100, 0., 1000.));
        addHistogram(new TH1F("hlt_eta", "eta; eta ; Count", 50, -2.47, 2.47));
        addHistogram(new TH1F("hlt_phi", "phi; phi ; Count", 50, -3.14, 3.14));
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

    dirnames.clear();
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        ATH_MSG_INFO("Trigger " << probeTrigger);
        dirnames.push_back(m_dir+"/" + probeTrigger + "/Resolutions");
    }
    
    for (int i = 0; i < (int) dirnames.size(); i++) {
        ATH_MSG_INFO(dirnames[i]);
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("res_pt", "HLT p_{T} resolution; (p_{T}(on)-p_{T}(off))/p_{T}(off) ; Count", 200, -1.5, 1.5));
        addHistogram(new TH1F("res_et", "HLT E_{T} resolution; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
        addHistogram(new TH1F("res_eta", "#eta resolution; (#eta(on)-#eta(off))/#eta(off) ; Count", 200, -0.02, 0.02));
        addHistogram(new TH1F("res_phi", "#phi resolution; (#phi(on)-#phi(off))/#phi(off) ; Count", 200, -0.02, 0.02));

	addHistogram(new TH2F("res_etVsEta", "HLT E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
			      50, -2.47, 2.47,
			      200, -0.1, 0.1));
	addHistogram(new TH2F("res_etVsEt", "HLT E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
			      50, 0., 100.,
			      200, -0.1, 0.1));

	addHistogram(new TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0,1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37,1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55,1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));
	addHistogram(new TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8,2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1));

        addHistogram(new TH1F("res_e011", "e011 resolution; (e011(on)-res_e011(off))/res_e011(off) ; Count", 200, -0.002, 0.002));
        addHistogram(new TH1F("res_e132", "e132 resolution; (e132(on)-res_e132(off))/res_e132(off) ; Count", 200, -0.002, 0.002));
        addHistogram(new TH1F("res_e237", "e237 resolution; (e237(on)-res_e237(off))/res_e237(off) ; Count", 200, -0.002, 0.002));
        addHistogram(new TH1F("res_e277", "e277 resolution; (e277(on)-res_e277(off))/res_e277(off) ; Count", 200, -0.005, 0.005));
        addHistogram(new TH1F("res_ethad", "ethad resolution; (ethad(on)-res_ethad(off))/res_ethad(off) ; Count", 200, -0.2, 0.2));
        addHistogram(new TH1F("res_ethad1", "ethad1 resolution; (ethad1(on)-res_ethad1(off))/res_ethad1(off) ; Count", 200, -0.2, 0.2));
        addHistogram(new TH1F("res_Rhad", "Rhad resolution; (Rhad(on)-res_Rhad(off))/res_Rhad(off) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("res_Reta", "Reta resolution; (Reta(on)-res_Reta(off))/res_Reta(off) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("res_Rphi", "Rphi resolution; (Rphi(on)-res_Rphi(off))/res_Rphi(off) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("res_weta1", "weta1 resolution; (weta1(on)-res_weta1(off))/res_weta1(off) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("res_weta2", "weta2 resolution; (weta2(on)-res_weta2(off))/res_weta2(off) ; Count", 200, -0.001, 0.001));
        addHistogram(new TH1F("res_f1", "f1 resolution; (f1(on)-res_f1(off))/res_f1(off) ; Count", 200, -0.01, 0.01));
        addHistogram(new TH1F("res_f3", "f3 resolution; (f3(on)-res_f3(off))/res_f3(off) ; Count", 200, -0.01, 0.01));
        addHistogram(new TH1F("res_eratio", "eratio resolution; (eratio(on)-res_eratio(off))/res_eratio(off) ; Count", 200, -0.001, 0.001));
    }
    addDirectory(m_dir+"/TrigElectrons");
    addDirectory(m_dir+"/EmTauRoI");

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaValidationTool::childExecute()
{

    ATH_MSG_DEBUG("Executing TrigEgammaValidationTool");
   
   
    m_offElectrons=0;
    m_onlElectrons=0;
    m_trigElectrons=0;
    m_caloClusters=0;
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    ATH_MSG_INFO("Offline Electron container size " << m_offElectrons->size());
    if ( (m_storeGate->retrieve(m_offPhotons,"Photons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
    }
    ATH_MSG_INFO("Offline Photon container size " << m_offPhotons->size());

    if ( (m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("Online Electron container size " << m_onlElectrons->size());
    if ( (m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
    }
    else ATH_MSG_INFO("Online Photon container size " << m_onlPhotons->size());
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

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string trigger = m_probeTrigList.at(ilist);
        SimpleEfficiency(trigger);
    }

    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaValidationTool::childFinalize()
{

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
        
        hist1("matchEF_pt")->Sumw2();
        hist1("effEF_pt")->Divide(hist1("matchEF_pt"),hist1("pt"),1,1,"b");
        hist1("matchEF_et")->Sumw2();
        hist1("effEF_et")->Divide(hist1("matchEF_et"),hist1("et"),1,1,"b");
        hist1("matchEF_highet")->Sumw2();
        hist1("effEF_highet")->Divide(hist1("matchEF_highet"),hist1("highet"),1,1,"b");
        hist1("matchEF_eta")->Sumw2();
        hist1("effEF_eta")->Divide(hist1("matchEF_eta"),hist1("eta"),1,1,"b");
        hist1("matchEF_phi")->Sumw2();
        hist1("effEF_phi")->Divide(hist1("matchEF_phi"),hist1("phi"),1,1,"b");
    }
    return StatusCode::SUCCESS;

}

void TrigEgammaValidationTool::fillShowerShapes(const xAOD::Egamma *onl, const xAOD::Egamma *off){

    if(!onl) ATH_MSG_DEBUG("Online pointer fails"); 
    if(!off) ATH_MSG_DEBUG("Offline pointer fails");
    ATH_MSG_DEBUG("Shower Shapes");
    hist1("et")->Fill(getCluster_et(off)/1e3);
    hist1("highet")->Fill(getCluster_et(off)/1e3);
    hist1("eta")->Fill(off->eta());
    hist1("phi")->Fill(off->phi());
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
    if(onl->type()==xAOD::Type::Electron){
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (onl);
        hist1("hlt_et")->Fill(getEt(el)/1e3);
        hist1("hlt_highet")->Fill(getEt(el)/1e3);
    }
    else if(onl->type()==xAOD::Type::Photon){
        hist1("hlt_et")->Fill(getCluster_et(onl)/1e3);
        hist1("hlt_highet")->Fill(getCluster_et(onl)/1e3);
    }
    hist1("hlt_eta")->Fill(onl->eta());
    hist1("hlt_phi")->Fill(onl->phi());

}

void TrigEgammaValidationTool::fillTracking(const xAOD::Electron *onl, const xAOD::Electron *off){
    if(!onl) ATH_MSG_DEBUG("Online pointer fails"); 
    if(!off) ATH_MSG_DEBUG("Offline pointer fails");
    ATH_MSG_DEBUG("Tracking plots");
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
    hist1("pt")->Fill(getTrack_pt(off)/1e3);

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

}

void TrigEgammaValidationTool::fillRes(const xAOD::Egamma *onl, const xAOD::Egamma *off){
    ATH_MSG_DEBUG("Resolution");
    if(onl->type()==xAOD::Type::Electron){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        hist1("res_pt")->Fill((getTrack_pt(elonl)-getTrack_pt(eloff))/getTrack_pt(eloff));
        hist1("res_et")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
        hist1("res_eta")->Fill((elonl->trackParticle()->eta()-eloff->trackParticle()->eta())/eloff->trackParticle()->eta());
        hist1("res_phi")->Fill((elonl->trackParticle()->phi()-eloff->trackParticle()->phi())/eloff->trackParticle()->phi());

	hist2("res_etVsEta")->Fill(elonl->trackParticle()->eta(),
				   (getEt(elonl)-getEt(eloff))/getEt(eloff)
				   );
	hist2("res_etVsEt")->Fill( getEt(elonl)/1e3,
				   (getEt(elonl)-getEt(eloff))/getEt(eloff)
				   );
	float feta = fabs(elonl->trackParticle()->eta());
	if( feta < 1.37 )
	  hist1("res_etInEta0")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >=1.37 && feta <= 1.52 )
	  hist1("res_etInEta1")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >= 1.55 && feta < 1.8 )
	  hist1("res_etInEta2")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
	else if( feta >= 1.8 && feta < 2.45 )
	  hist1("res_etInEta3")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
    }
    else{ 
      hist1("res_et")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      hist1("res_eta")->Fill((onl->eta()-off->eta())/off->eta());
      hist1("res_phi")->Fill((onl->phi()-off->phi())/off->phi());

      hist2("res_etVsEta")->Fill(onl->eta(),
				 (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
				 );
      hist2("res_etVsEt")->Fill( getCluster_et(onl)/1e3,
				 (getCluster_et(onl)-getCluster_et(off))/getCluster_et(off)
				 );
      float feta = fabs(onl->eta());
      if( feta < 1.37 )
	hist1("res_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >=1.37 && feta <= 1.52 )
	hist1("res_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >= 1.55 && feta < 1.8 )
	hist1("res_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
      else if( feta >= 1.8 && feta < 2.45 )
	hist1("res_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off))/getCluster_et(off));
    }
    
    hist1("res_e011")->Fill((getShowerShape_e011(onl)-getShowerShape_e011(off))/getShowerShape_e011(off));
    hist1("res_e132")->Fill((getShowerShape_e132(onl)-getShowerShape_e132(off))/getShowerShape_e132(off));
    hist1("res_e237")->Fill((getShowerShape_e237(onl)-getShowerShape_e237(off))/getShowerShape_e237(off));
    hist1("res_e277")->Fill((getShowerShape_e277(onl)-getShowerShape_e277(off))/getShowerShape_e277(off));
    hist1("res_ethad")->Fill((getShowerShape_ethad(onl)-getShowerShape_ethad(off))/getShowerShape_ethad(off));
    hist1("res_ethad1")->Fill((getShowerShape_ethad1(onl)-getShowerShape_ethad1(off))/getShowerShape_ethad1(off));
    hist1("res_Rhad")->Fill((getShowerShape_Rhad(onl)-getShowerShape_Rhad(off))/getShowerShape_Rhad(off));
    hist1("res_Reta")->Fill((getShowerShape_Reta(onl)-getShowerShape_Reta(off))/getShowerShape_Reta(off));
    hist1("res_Rphi")->Fill((getShowerShape_Rphi(onl)-getShowerShape_Rphi(off))/getShowerShape_Rphi(off));
    hist1("res_weta1")->Fill((getShowerShape_weta1(onl)-getShowerShape_weta1(off))/getShowerShape_weta1(off));
    hist1("res_weta2")->Fill((getShowerShape_weta2(onl)-getShowerShape_weta2(off))/getShowerShape_weta2(off));
    hist1("res_f1")->Fill((getShowerShape_f1(onl)-getShowerShape_f1(off))/getShowerShape_f1(off));
    hist1("res_f3")->Fill((getShowerShape_f3(onl)-getShowerShape_f3(off))/getShowerShape_f3(off));
    hist1("res_eratio")->Fill((getShowerShape_Eratio(onl)-getShowerShape_Eratio(off))/getShowerShape_Eratio(off));
}
void TrigEgammaValidationTool::SimpleEfficiency(std::string trigger){

    ATH_MSG_DEBUG("Calling SimpleEfficiency");

    std::string type="";
    float etthr=0;
    float l1thr=0;
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    parseTriggerName(trigger,"Loose",type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
    if(type == "electron") {
        SimpleElectronEfficiency(trigger,etthr,pidname);
    }
    else if(type == "photon"){
        SimplePhotonEfficiency(trigger,etthr);
    }
    
}
void TrigEgammaValidationTool::SimplePhotonEfficiency(const std::string trigger, const float etthr){
    ATH_MSG_DEBUG("Execute SimplePhotonEfficiency");
    
    for(const auto& eg : *m_offPhotons){
        cd(m_dir+"/"+trigger+"/HLTEfficiencies");
	if( !(getCluster_et(eg) > (etthr+2.0)*1.e3)) continue; //Take 2GeV above threshold
        if(!eg->passSelection(m_photonPid)) continue;
	if(m_doUnconverted)
	  if (eg->vertex()){
	    ATH_MSG_DEBUG("Removing converted photons, continuing...");
	    continue;
	  }
        hist1("et")->Fill(getCluster_et(eg)/1e3);
        hist1("highet")->Fill(getCluster_et(eg)/1e3);
        hist1("eta")->Fill(eg->eta());
        hist1("phi")->Fill(eg->phi());

        if ( m_trigdec->isPassed("HLT_"+trigger) ){
            const xAOD::Egamma * HLTobject = m_matchTool->closestHLTObject(eg,trigger);
            if(HLTobject){
                const xAOD::Photon* phEF =static_cast<const xAOD::Photon*> (HLTobject);
                hist1("match_et")->Fill(getCluster_et(eg)/1e3);
                hist1("match_highet")->Fill(getCluster_et(eg)/1e3);
                hist1("match_eta")->Fill(eg->eta());
                hist1("match_phi")->Fill(eg->phi());
                cd(m_dir+"/"+trigger+"/Distributions");
                fillShowerShapes(phEF,eg);
                cd(m_dir+"/"+trigger+"/Resolutions");
                fillRes(phEF,eg);
            }
        }
    }
}
void TrigEgammaValidationTool::SimpleElectronEfficiency(const std::string trigger,const float etthr,const std::string pidname){

    ATH_MSG_DEBUG("Execute SimpleElectronEfficiency");
    
    for(const auto& eg : *m_offElectrons){
        cd(m_dir+"/"+trigger+"/HLTEfficiencies");
        if( !(eg->e()/cosh(eg->trackParticle()->eta())  > (etthr-5.)*1.e3) ) continue;
        if ( (fabs(eg->eta())>1.37 && fabs(eg->eta())<1.52) || fabs(eg->eta())>2.47 )
            continue; 
        if(!eg->passSelection(pidname)) continue;
        hist1("pt")->Fill(getTrack_pt(eg)/1e3);
        hist1("et")->Fill(getEt(eg)/1e3);
        hist1("highet")->Fill(getEt(eg)/1e3);
        if( (eg->e()/cosh(eg->trackParticle()->eta())  > (etthr+1.0)*1.e3) ) {
            hist1("eta")->Fill(eg->eta());
            hist1("phi")->Fill(eg->phi());
        }

        if ( m_trigdec->isPassed("HLT_"+trigger) ){
            const xAOD::Egamma * HLTobject = m_matchTool->closestHLTObject(eg,trigger);
            if(HLTobject){
                const xAOD::Electron* elEF =static_cast<const xAOD::Electron*> (HLTobject);
                hist1("match_pt")->Fill(getTrack_pt(eg)/1e3);
                hist1("match_et")->Fill(getEt(eg)/1e3);
                hist1("match_highet")->Fill(getEt(eg)/1e3);
                if( (eg->e()/cosh(eg->trackParticle()->eta())  > (etthr+1.0)*1.e3) ) {
                    hist1("match_eta")->Fill(eg->eta());
                    hist1("match_phi")->Fill(eg->phi());
                }
                cd(m_dir+"/"+trigger+"/Distributions");
                fillShowerShapes(elEF,eg);
                fillTracking(elEF,eg);
                cd(m_dir+"/"+trigger+"/Resolutions");
                fillRes(elEF,eg);
            }
        }
    }
}


