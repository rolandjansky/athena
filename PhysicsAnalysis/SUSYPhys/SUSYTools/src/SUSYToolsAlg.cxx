/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SUSYToolsAlg.cxx

#include "SUSYToolsAlg.h"
#include "SUSYTools/ISUSYObjDef_xAODTool.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"

#include "TH1F.h"
#include "TFile.h"
#include "TString.h"

using std::string;

const unsigned int nSel=5;
enum Cut {
  all=0,
  baseline,
  passOR,
  signal,
  trigmatch
};
static const TString SCut[] = {"All","Baseline","passOR","Signal","TrigM"};

//**********************************************************************

SUSYToolsAlg::SUSYToolsAlg(const std::string& name,
                           ISvcLocator* pSvcLocator )
  : ::AthAnalysisAlgorithm( name, pSvcLocator ),
    m_Nevts(0)
{

  declareProperty( "SUSYTools",   m_SUSYTools      );
  declareProperty( "DataSource",  m_dataSource = 1 ); //default is fullsim = 1
  declareProperty( "DoSyst",      m_doSyst = false );

  declareProperty( "RateMonitoringPath", m_rateMonitoringPath=".", "will try to add rate to file found in given path");

}

//**********************************************************************
SUSYToolsAlg::~SUSYToolsAlg() { }

//**********************************************************************

StatusCode SUSYToolsAlg::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  if (m_SUSYTools.retrieve().isFailure())
  {
    ATH_MSG_ERROR("Failed to retrieve tool: " << m_SUSYTools->name());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Retrieved tool: " << m_SUSYTools->name() );

  sysInfoList.clear();
  // this is the nominal set
  if (m_doSyst) {
    sysInfoList = m_SUSYTools->getSystInfoList();
  } else {
    ST::SystInfo infodef;
    infodef.affectsKinematics = false;
    infodef.affectsWeights = false;
    infodef.affectsType = ST::Unknown;
    sysInfoList.push_back(infodef);
  }


  //count number of weight systematics per object
  syst_event_weights={"Nominal"};
  syst_el_weights={"Nominal"};
  syst_mu_weights={"Nominal"};
  syst_ph_weights={"Nominal"};
  syst_tau_weights={"Nominal"};
  syst_jet_weights={"Nominal"};
  syst_btag_weights={"Nominal"};

  for (const auto& sysInfo : sysInfoList) { 

    bool syst_affectsElectrons = ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType);
    bool syst_affectsMuons     = ST::testAffectsObject(xAOD::Type::Muon, sysInfo.affectsType);
    bool syst_affectsPhotons   = ST::testAffectsObject(xAOD::Type::Photon, sysInfo.affectsType);
    bool syst_affectsTaus      = ST::testAffectsObject(xAOD::Type::Tau, sysInfo.affectsType);
    bool syst_affectsJets      = ST::testAffectsObject(xAOD::Type::Jet, sysInfo.affectsType);
    bool syst_affectsBTag      = ST::testAffectsObject(xAOD::Type::BTag, sysInfo.affectsType);
    bool syst_affectsEventWeight     = (sysInfo.affectsType == ST::SystObjType::EventWeight);
    
    if(sysInfo.affectsWeights){
      std::string sys_name = sysInfo.systset.name();
      if (syst_affectsElectrons)                syst_el_weights.push_back(sys_name);  
      if (syst_affectsMuons)                    syst_mu_weights.push_back(sys_name);  
      if (syst_affectsPhotons)                  syst_ph_weights.push_back(sys_name);  
      if (syst_affectsTaus)                     syst_tau_weights.push_back(sys_name);  
      if (syst_affectsJets)                     syst_jet_weights.push_back(sys_name);  
      if (syst_affectsBTag)                     syst_btag_weights.push_back(sys_name);
      if (syst_affectsEventWeight)              syst_event_weights.push_back(sys_name);  
    }
  }

  //reset signal lepton counters
  count_el_signal=0;
  count_ph_signal=0;
  count_mu_signal=0;

  /// SETUP TRIGGERS TO BE CHECKED
  const int n_el_trig = 3;
  el_triggers.push_back("HLT_e24_lhmedium_L1EM18VH"); // reference xAOD used for tests doesn't have the reprocessed menu yet (i.e. no *20VH in there) //MT
  el_triggers.push_back("HLT_e60_lhmedium");
  el_triggers.push_back("HLT_e120_lhloose");

  const int n_ph_trig = 2;
  ph_triggers.push_back("HLT_g120_loose");
  ph_triggers.push_back("HLT_g140_loose");


  const int n_mu_trig = 5;
  mu_triggers.push_back("HLT_mu20_iloose_L1MU15");
  mu_triggers.push_back("HLT_mu50");
  mu_triggers.push_back("HLT_mu18");
  mu_triggers.push_back("HLT_mu8noL1");
  mu_triggers.push_back("HLT_mu18_mu8noL1");

  //-- Book histograms
  CHECK( book(TH1D("el_n_flow_nominal", "Electron Cutflow (Nominal)", nSel, 0, nSel) ) );
  CHECK( book(TH1D("el_trigmatch_eff_nominal", "Electron Trigger Matching Efficiency (Nominal)", n_el_trig, 0, n_el_trig) ) );
  CHECK( book(TH1D("el_pt_nominal", "Electron Pt (Nominal)", 100, 0, 100) ) );

  CHECK( book(TH1D("ph_n_flow_nominal", "Photon Cutflow (Nominal)", nSel, 0, nSel) ) );
  CHECK( book(TH1D("ph_trigmatch_eff_nominal", "Photon Trigger Matching Efficiency (Nominal)", n_ph_trig, 0, n_ph_trig) ) );
  CHECK( book(TH1D("ph_pt_nominal", "Photon Pt (Nominal)", 100, 0, 100) ) );

  CHECK( book(TH1D("mu_n_flow_nominal", "Muon Cutflow (Nominal)", nSel, 0, nSel) ) );
  CHECK( book(TH1D("mu_trigmatch_eff_nominal", "Muon Trigger Matching Efficiency (Nominal)", n_mu_trig, 0, n_mu_trig) ) );
  CHECK( book(TH1D("mu_pt_nominal", "Muon Pt (Nominal)", 100, 0, 100) ) );

  CHECK( book(TH1D("jet_n_flow_nominal", "Jet Cutflow (Nominal)", nSel, 0, nSel) ) );
  CHECK( book(TH1D("jet_pt_nominal", "Jet Pt (Nominal)", 100, 0, 500) ) );

  CHECK( book(TH1D("met_et", "MET (Nominal)", 50, 0, 500) ) ); //MET (+Components) 
  CHECK( book(TH1D("met_phi", "MET_phi (Nominal)", 50, -5, 5) ) ); 
  CHECK( book(TH1D("met_sumet", "MET_sumet (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_tst", "MET [PVSoftTrk] (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_el", "MET [RefEle] (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_ph", "MET [RefGamma] (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_mu", "MET [Muons] (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_jet", "MET [RefJet] (Nominal)", 50, 0, 500) ) ); 
  CHECK( book(TH1D("met_et_tau", "MET [RefTau] (Nominal)", 50, 0, 500) ) ); 
  
  const int n_event_weights = (const int)(syst_event_weights.size());
  const int n_el_weights = (const int)(syst_el_weights.size());
  const int n_mu_weights = (const int)(syst_mu_weights.size());
  const int n_ph_weights = (const int)(syst_ph_weights.size());
  const int n_tau_weights = (const int)(syst_tau_weights.size());
  const int n_jet_weights = (const int)(syst_jet_weights.size());
  const int n_btag_weights = (const int)(syst_btag_weights.size());

  CHECK( book(TH1D("weight_event", "Event weights (Nom+Systematics) [MC*PRW]", n_event_weights,0,n_event_weights))); //weights
  CHECK( book(TH1D("weight_electrons", "Electron total weights (Nom+Systematics)",  n_el_weights,0, n_el_weights) ) );  //weights
  CHECK( book(TH1D("weight_muons", "Muon total weights (Nom+Systematics)",  n_mu_weights,0, n_mu_weights) ) );  //weights
  CHECK( book(TH1D("weight_photons", "Photon total weights (Nom+Systematics)",  n_ph_weights,0, n_ph_weights) ) );  //weights
  CHECK( book(TH1D("weight_taus", "Tau total weights (Nom+Systematics)",  n_tau_weights,0, n_tau_weights) ) );  //weights
  CHECK( book(TH1D("weight_jets", "Jet total weights (Nom+Systematics)",  n_jet_weights,0, n_jet_weights) ) );  //weights
  CHECK( book(TH1D("weight_btags", "Btagging total weights (Nom+Systematics)",  n_btag_weights,0, n_btag_weights) ) );  //weights


  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode SUSYToolsAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_MSG_INFO("Time per event:          (CPU / WALL)");
  ATH_MSG_INFO("  Entire loop           - " << m_clock0.CpuTime() * 1e3 / m_Nevts << " / " << m_clock0.RealTime() * 1e3 / m_Nevts << " ms");
  ATH_MSG_INFO("  First event           - " << m_clock1.CpuTime() * 1e3 << " / " << m_clock1.RealTime() * 1e3 << " ms");
  ATH_MSG_INFO("  Excluding first event - " << m_clock2.CpuTime() * 1e3 / (m_Nevts - 1) << " / " << m_clock2.RealTime() * 1e3 / (m_Nevts - 1) << " ms");

  TH1F* rateHist = 0;
  if(!m_rateMonitoringPath.empty()) {
    TFile* f = 0;
    //determine which nightly we are looking for
    int i=6;
    while(i>0) {
      if(m_rateMonitoringPath.find(TString::Format("rel_%d",i))!=std::string::npos) break;
      i--;
    }
    //now go back through up to 7 nightlies
    TString s(m_rateMonitoringPath);
    int attempt=0;
    while(attempt<7) {
       f = TFile::Open(s + TString::Format("/eventRate_%s.root",name().c_str()) );
       if(f) break;
       if(i==0) i=6;
       s.ReplaceAll(TString::Format("rel_%d",(i+1)%7),TString::Format("rel_%d",i));
       attempt++;
    }
    if(f) {
      rateHist = static_cast<TH1F*>(f->Get("eventRate"));
    }
  }
  if(!rateHist) {
    rateHist = new TH1F("eventRate","Event Rate [Hz]",1,0,1);
  }

  rateHist->SetBins(rateHist->GetNbinsX()+1,0,rateHist->GetNbinsX()+1); //add a bin
  rateHist->Rebuild();
  rateHist->SetBinContent(rateHist->GetNbinsX(), float(m_Nevts-1)/m_clock2.RealTime() );

  TFile fOut(TString::Format("eventRate_%s.root",name().c_str()),"RECREATE");
  rateHist->SetDirectory(&fOut);
  rateHist->Write();
  fOut.Close();


  //label cutflow histograms
  TH1* el_n_flow_nominal = hist("el_n_flow_nominal");
  for (unsigned int i=1; i < nSel+1; i++){ el_n_flow_nominal->GetXaxis()->SetBinLabel(i, SCut[i-1].Data()); }
  el_n_flow_nominal->GetXaxis()->SetLabelSize(0.05);

  TH1* ph_n_flow_nominal = hist("ph_n_flow_nominal");
  for (unsigned int i=1; i < nSel+1; i++){ ph_n_flow_nominal->GetXaxis()->SetBinLabel(i, SCut[i-1].Data()); }
  ph_n_flow_nominal->GetXaxis()->SetLabelSize(0.05);

  TH1* mu_n_flow_nominal = hist("mu_n_flow_nominal");
  for (unsigned int i=1; i < nSel+1; i++){ mu_n_flow_nominal->GetXaxis()->SetBinLabel(i, SCut[i-1].Data()); }
  mu_n_flow_nominal->GetXaxis()->SetLabelSize(0.05);

  TH1* jet_n_flow_nominal = hist("jet_n_flow_nominal");
  for (unsigned int i=1; i < nSel+1; i++){ jet_n_flow_nominal->GetXaxis()->SetBinLabel(i, SCut[i-1].Data()); }
  jet_n_flow_nominal->GetXaxis()->SetLabelSize(0.05);

  //normalize and label efficiency histograms
  TH1* el_trigmatch_eff_nominal = hist("el_trigmatch_eff_nominal");
  for (unsigned int i=1; i < el_triggers.size()+1; i++){ el_trigmatch_eff_nominal->GetXaxis()->SetBinLabel(i, el_triggers[i-1].substr(4,string::npos).c_str()); }
  el_trigmatch_eff_nominal->GetXaxis()->SetLabelSize(0.05);
  el_trigmatch_eff_nominal->Scale(1/(float)count_el_signal);

  TH1* ph_trigmatch_eff_nominal = hist("ph_trigmatch_eff_nominal");
  for (unsigned int i=1; i < ph_triggers.size()+1; i++){ ph_trigmatch_eff_nominal->GetXaxis()->SetBinLabel(i, ph_triggers[i-1].substr(4,string::npos).c_str()); }
  ph_trigmatch_eff_nominal->GetXaxis()->SetLabelSize(0.05);
  ph_trigmatch_eff_nominal->Scale(1/(float)count_ph_signal);

  TH1* mu_trigmatch_eff_nominal = hist("mu_trigmatch_eff_nominal");
  for (unsigned int i=1; i < mu_triggers.size()+1; i++){ mu_trigmatch_eff_nominal->GetXaxis()->SetBinLabel(i, mu_triggers[i-1].substr(4,string::npos).c_str()); }
  mu_trigmatch_eff_nominal->GetXaxis()->SetLabelSize(0.05);
  mu_trigmatch_eff_nominal->Scale(1/(float)count_mu_signal);


  //label weights histograms
  TH1* weight_event     = hist("weight_event");
  TH1* weight_electrons = hist("weight_electrons");
  TH1* weight_muons     = hist("weight_muons");
  TH1* weight_photons   = hist("weight_photons");
  TH1* weight_taus      = hist("weight_taus");
  TH1* weight_jets      = hist("weight_jets");
  TH1* weight_btags     = hist("weight_btags");
  for (unsigned int i=1; i < syst_event_weights.size()+1; i++){ weight_event->GetXaxis()->SetBinLabel(i, syst_event_weights.at(i-1).c_str()); }
  weight_event->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_el_weights.size()+1; i++){ weight_electrons->GetXaxis()->SetBinLabel(i, syst_el_weights.at(i-1).c_str()); }
  weight_electrons->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_mu_weights.size()+1; i++){ weight_muons->GetXaxis()->SetBinLabel(i, syst_mu_weights.at(i-1).c_str()); }
  weight_muons->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_ph_weights.size()+1; i++){ weight_photons->GetXaxis()->SetBinLabel(i, syst_ph_weights.at(i-1).c_str()); }
  weight_photons->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_tau_weights.size()+1; i++){ weight_taus->GetXaxis()->SetBinLabel(i, syst_tau_weights.at(i-1).c_str()); }
  weight_taus->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_jet_weights.size()+1; i++){ weight_jets->GetXaxis()->SetBinLabel(i, syst_jet_weights.at(i-1).c_str()); }
  weight_jets->GetXaxis()->SetLabelSize(0.05);
  for (unsigned int i=1; i < syst_btag_weights.size()+1; i++){ weight_btags->GetXaxis()->SetBinLabel(i, syst_btag_weights.at(i-1).c_str()); }
  weight_btags->GetXaxis()->SetLabelSize(0.05);

  //average weight histograms
  weight_event     ->Scale(1/(float)weight_event->GetBinContent(1));
  weight_electrons ->Scale(1/(float)weight_electrons->GetBinContent(1));
  weight_muons     ->Scale(1/(float)weight_muons->GetBinContent(1));
  weight_photons   ->Scale(1/(float)weight_photons->GetBinContent(1));
  weight_taus      ->Scale(1/(float)weight_taus->GetBinContent(1));
  weight_jets      ->Scale(1/(float)weight_jets->GetBinContent(1));
  weight_btags     ->Scale(1/(float)weight_btags->GetBinContent(1));
  
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode SUSYToolsAlg::execute() {
  ATH_MSG_VERBOSE("Executing " << name() << "...");
  m_clock0.Start(false);
  if (m_Nevts == 0) {
    m_clock1.Start(false);
  } else {
    m_clock2.Start(false);
  }

  if (m_SUSYTools->resetSystematics() != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR( "Cannot reset SUSYTools systematics" );
  }

  // METMaker crashes if there's no PV, check that before anything, 
  // and skip to the next event if there isn't one
  if (m_SUSYTools->GetPrimVtx() == nullptr) {
    ATH_MSG_WARNING("No PV found for this event! Skipping...");
    return StatusCode::SUCCESS;
  }

  // call PRW tool to apply all relevant decorations
  CHECK( m_SUSYTools->ApplyPRWTool());


  // First, create the nominal containers
  // Only create them freshly if affected by systematics

  xAOD::ElectronContainer* electrons_nominal(0);
  xAOD::ShallowAuxContainer* electrons_nominal_aux(0);
  CHECK( m_SUSYTools->GetElectrons(electrons_nominal, electrons_nominal_aux) );
  ATH_MSG_DEBUG( "Number of electrons: " << electrons_nominal->size() );

  xAOD::PhotonContainer* photons_nominal(0);
  xAOD::ShallowAuxContainer* photons_nominal_aux(0);
  CHECK( m_SUSYTools->GetPhotons(photons_nominal, photons_nominal_aux) );
  ATH_MSG_DEBUG( "Number of photons: " << photons_nominal->size() );

  xAOD::MuonContainer* muons_nominal(0);
  xAOD::ShallowAuxContainer* muons_nominal_aux(0);
  CHECK( m_SUSYTools->GetMuons(muons_nominal, muons_nominal_aux) );
  ATH_MSG_DEBUG( "Number of muons: " << muons_nominal->size() );

  xAOD::JetContainer* jets_nominal(0);
  xAOD::ShallowAuxContainer* jets_nominal_aux(0);
  CHECK( m_SUSYTools->GetJets(jets_nominal, jets_nominal_aux) );
  ATH_MSG_DEBUG( "Number of jets: " << jets_nominal->size() );

  xAOD::MissingETContainer* metcst_nominal = new xAOD::MissingETContainer;
  xAOD::MissingETAuxContainer* metcst_nominal_aux = new xAOD::MissingETAuxContainer;
  metcst_nominal->setStore(metcst_nominal_aux);
  metcst_nominal->reserve(10);

  CHECK( m_SUSYTools->GetMET(*metcst_nominal,
                             jets_nominal,
                             electrons_nominal,
                             muons_nominal,
                             photons_nominal, 0, false, false) );

  ATH_MSG_DEBUG("RefFinal CST etx=" << (*metcst_nominal)["Final"]->mpx()
                << ", ety=" << (*metcst_nominal)["Final"]->mpy()
                << ", et=" << (*metcst_nominal)["Final"]->met()
                << ", sumet=" << (*metcst_nominal)["Final"]->sumet());

  xAOD::MissingETContainer* mettst_nominal = new xAOD::MissingETContainer;
  xAOD::MissingETAuxContainer* mettst_nominal_aux = new xAOD::MissingETAuxContainer;
  mettst_nominal->setStore(mettst_nominal_aux);
  mettst_nominal->reserve(10);
  
  CHECK( m_SUSYTools->GetMET(*mettst_nominal,
                             jets_nominal,
                             electrons_nominal,
                             muons_nominal,
                             photons_nominal, 0, true, true) );
  
  ATH_MSG_DEBUG("RefFinal TST etx=" << (*mettst_nominal)["Final"]->mpx()
                << ", ety=" << (*mettst_nominal)["Final"]->mpy()
                << ", et=" << (*mettst_nominal)["Final"]->met()
                << ", sumet=" << (*mettst_nominal)["Final"]->sumet());
  

  TH1* met_et    = hist("met_et");
  TH1* met_sumet = hist("met_sumet");
  TH1* met_phi   = hist("met_phi");
  
  TH1* met_et_tst = hist("met_et_tst");
  TH1* met_et_el  = hist("met_et_el");
  //TH1* met_et_ph  = hist("met_et_ph");
  TH1* met_et_mu  = hist("met_et_mu");
  TH1* met_et_jet = hist("met_et_jet");
  //TH1* met_et_tau = hist("met_et_tau");
  
  met_et->Fill( (*mettst_nominal)["Final"]->met()*0.001 );
  met_sumet->Fill( (*mettst_nominal)["Final"]->sumet()*0.001 );
  met_phi->Fill( (*mettst_nominal)["Final"]->phi() );
  
  met_et_tst->Fill( (*mettst_nominal)["PVSoftTrk"]->met() *0.001 );
  met_et_el->Fill(  (*mettst_nominal)["RefEle"]->met()    *0.001 );
  //met_et_ph->Fill(  (*mettst_nominal)["RefGamma"]->met()  *0.001 );
  met_et_mu->Fill(  (*mettst_nominal)["Muons"]->met()     *0.001 );
  met_et_jet->Fill( (*mettst_nominal)["RefJet"]->met()    *0.001 );
  // met_et_tau->Fill( (*mettst_nominal)["RefTau"]->met()    *0.001 );
  
  //--- Overlap Removal
  ATH_CHECK( m_SUSYTools->OverlapRemoval(electrons_nominal, muons_nominal, jets_nominal) );
  
  
  //--- Weights
  TH1* weight_event     = hist("weight_event");
  TH1* weight_electrons = hist("weight_electrons");
  TH1* weight_muons     = hist("weight_muons");
  TH1* weight_photons   = hist("weight_photons");
  //TH1* weight_taus      = hist("weight_taus");
  TH1* weight_jets      = hist("weight_jets");
  TH1* weight_btags      = hist("weight_btags");


  //--- Monitoring
  //----- Electrons 
      
  TH1* el_n_flow_nominal = hist("el_n_flow_nominal");
  TH1* el_trigmatch_eff_nominal = hist("el_trigmatch_eff_nominal");
  TH1* el_pt_nominal     = hist("el_pt_nominal");

  for(auto el : *electrons_nominal) {
    el_n_flow_nominal->Fill(Cut::all);
    if ( el->auxdata<char>("baseline") == 1 ){ 
      el_n_flow_nominal->Fill(Cut::baseline);
      if ( el->auxdata<char>("passOR") == 1 ){ 
	el_n_flow_nominal->Fill(Cut::passOR);
	if ( el->auxdata<char>("signal") == 1 ){ 
	  el_n_flow_nominal->Fill(Cut::signal);

	  count_el_signal++;

	  bool passTM=false;
	  unsigned int idx=1;
	  for(const auto& t : el_triggers){
	    bool passit = m_SUSYTools->IsTrigMatched(el, t);
	    passTM |= passit;
	    if(passit) el_trigmatch_eff_nominal->SetBinContent(idx, el_trigmatch_eff_nominal->GetBinContent(idx)+1);
	    idx++;
	  }	      
	  if(passTM) el_n_flow_nominal->Fill(Cut::trigmatch);
	}	  
      }
    }
    el_pt_nominal->Fill(el->pt() * 1e-3);
  }

  //----- Photons
      
  TH1* ph_n_flow_nominal = hist("ph_n_flow_nominal");
  TH1* ph_trigmatch_eff_nominal = hist("ph_trigmatch_eff_nominal");
  TH1* ph_pt_nominal     = hist("ph_pt_nominal");

  for(auto ph : *photons_nominal) {
    ph_n_flow_nominal->Fill(Cut::all);
    if ( ph->auxdata<char>("baseline") == 1 ){ 
      ph_n_flow_nominal->Fill(Cut::baseline);
      if ( ph->auxdata<char>("passOR") == 1 ){ 
	ph_n_flow_nominal->Fill(Cut::passOR);
	if ( ph->auxdata<char>("signal") == 1 ){ 
	  ph_n_flow_nominal->Fill(Cut::signal);

	  count_ph_signal++;

	  bool passTM=false;
	  unsigned int idx=1;
	  for(const auto& t : ph_triggers){
	    bool passit = m_SUSYTools->IsTrigMatched(ph, t);
	    passTM |= passit;
	    if(passit) ph_trigmatch_eff_nominal->SetBinContent(idx, ph_trigmatch_eff_nominal->GetBinContent(idx)+1);
	    idx++;
	  }	      
	  if(passTM) ph_n_flow_nominal->Fill(Cut::trigmatch);
	}	  
      }
    }
    ph_pt_nominal->Fill(ph->pt() * 1e-3);
  }

  //----- Muons
      
  TH1* mu_n_flow_nominal = hist("mu_n_flow_nominal");
  TH1* mu_trigmatch_eff_nominal = hist("mu_trigmatch_eff_nominal");
  TH1* mu_pt_nominal     = hist("mu_pt_nominal");

  for(auto mu : *muons_nominal) {
    mu_n_flow_nominal->Fill(Cut::all);
    if ( mu->auxdata<char>("baseline") == 1 ){ 
      mu_n_flow_nominal->Fill(Cut::baseline);
      if ( mu->auxdata<char>("passOR") == 1 ){ 
	mu_n_flow_nominal->Fill(Cut::passOR);
	if ( mu->auxdata<char>("signal") == 1 ){ 
	  mu_n_flow_nominal->Fill(Cut::signal);

	  count_mu_signal++;

	  bool passTM=false;
	  unsigned int idx=1;
	  for(const auto& t : mu_triggers){
	    bool passit = m_SUSYTools->IsTrigMatched(mu, t);
	    passTM |= passit;
	    if(passit) mu_trigmatch_eff_nominal->SetBinContent(idx, mu_trigmatch_eff_nominal->GetBinContent(idx)+1);
	    idx++;
	  }
	  if(passTM) mu_n_flow_nominal->Fill(Cut::trigmatch);
	}	  
      }
    }
    mu_pt_nominal->Fill(mu->pt() * 1e-3);
  }

  //----- Jets
      
  TH1* jet_n_flow_nominal = hist("jet_n_flow_nominal");
  TH1* jet_pt_nominal     = hist("jet_pt_nominal");

  for(auto jet : *jets_nominal) {
    jet_n_flow_nominal->Fill(Cut::all);
    if ( jet->auxdata<char>("baseline") == 1 ){ 
      jet_n_flow_nominal->Fill(Cut::baseline);
      if ( jet->auxdata<char>("passOR") == 1 ){ 
	jet_n_flow_nominal->Fill(Cut::passOR);
	if ( jet->auxdata<char>("signal") == 1 ){ 
	  jet_n_flow_nominal->Fill(Cut::signal);
	  jet_n_flow_nominal->Fill(Cut::trigmatch); //no trig matching for jets
	}	  
      }
    }
    jet_pt_nominal->Fill(jet->pt() * 1e-3);
  }


  // Set up the event weights
  // Base should include all weights that do not depend on individual objects
  double base_event_weight(1.);
  const xAOD::EventInfo* evtInfo(0);
  ATH_CHECK( evtStore()->retrieve(evtInfo) );
  bool isSim = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  if (isSim) {
    base_event_weight *= evtInfo->mcEventWeight();

    const xAOD::MissingETContainer* met_truth(0);
    CHECK( evtStore()->retrieve(met_truth, "MET_Truth") );

    ATH_MSG_DEBUG("Truth MET etx=" << (*met_truth)["NonInt"]->mpx()
                  << ", ety=" << (*met_truth)["NonInt"]->mpy()
                  << ", et=" << (*met_truth)["NonInt"]->met()
                  << ", sumet=" << (*met_truth)["NonInt"]->sumet());
    ATH_MSG_DEBUG("CST residuals: detx=" << (*metcst_nominal)["Final"]->mpx() - (*met_truth)["NonInt"]->mpx()
                  << ", dety=" << (*metcst_nominal)["Final"]->mpy() - (*met_truth)["NonInt"]->mpy());
    ATH_MSG_DEBUG("TST residuals: detx=" << (*mettst_nominal)["Final"]->mpx() - (*met_truth)["NonInt"]->mpx()
                  << ", dety=" << (*mettst_nominal)["Final"]->mpy() - (*met_truth)["NonInt"]->mpy());
  }

  // Additionally define a nominal weight for each object type
  double electrons_weight_nominal(1.);
  double photons_weight_nominal(1.);
  double muons_weight_nominal(1.);
  double jets_weight_nominal(1.);
  double btag_weight_nominal(1.);
  double event_weight_nominal(1.);
  double prw_weight_nominal(1.);

  bool isNominal(true);
  for (const auto& sysInfo : sysInfoList) {
    CP::SystematicSet sys = sysInfo.systset;
    if (m_SUSYTools->applySystematicVariation(sys) != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR( "Cannot configure SUSYTools for systematic var. %s" << sys.name() );
    } else {
      ATH_MSG_VERBOSE( "Variation \"" << sys.name() << "\" configured..." );
    }
    if (isNominal && (sysInfo.affectsKinematics || sysInfo.affectsWeights)) {
      ATH_MSG_ERROR("Expected nominal variation!");
      return StatusCode::FAILURE;
    }
    if (sysInfo.affectsKinematics || sysInfo.affectsWeights) isNominal = false;

    double event_weight = base_event_weight;
    bool syst_affectsEventWeight = (sysInfo.affectsType == ST::SystObjType::EventWeight);

    // If nominal, compute the nominal weight, otherwise recompute the weight

    //    ATH_MSG_INFO( "Variation \"" << sys.name() << "\" configured..." ); //MARTIN REMOVE THIS!

    double prw_weight=1.;
    if(isNominal){
      if(isSim) prw_weight_nominal = m_SUSYTools->GetPileupWeight();
      event_weight *= prw_weight_nominal;
      weight_event->SetBinContent(1, weight_event->GetBinContent(1)+event_weight);
    }
    else if (!syst_affectsEventWeight) {
      event_weight *= prw_weight_nominal;
    }
    else {
      if(isSim){
	CHECK(m_SUSYTools->ApplyPRWTool());
	prw_weight = m_SUSYTools->GetPileupWeight();
      }
      event_weight *= prw_weight;
      size_t iwbin = find(syst_event_weights.begin(), syst_event_weights.end(), sys.name()) - syst_event_weights.begin(); 
      if(iwbin < syst_event_weights.size()) {  weight_event->SetBinContent(iwbin+1, weight_event->GetBinContent(iwbin+1)+event_weight); }    
    }
    


    // Define the generic collection pointers
    xAOD::ElectronContainer* electrons(electrons_nominal);
    xAOD::ShallowAuxContainer* electrons_aux(electrons_nominal_aux);
    xAOD::PhotonContainer* photons(photons_nominal);
    xAOD::ShallowAuxContainer* photons_aux(photons_nominal_aux);
    xAOD::MuonContainer* muons(muons_nominal);
    xAOD::ShallowAuxContainer* muons_aux(muons_nominal_aux);
    xAOD::JetContainer* jets(jets_nominal);
    xAOD::ShallowAuxContainer* jets_aux(jets_nominal_aux);
    xAOD::MissingETContainer *mettst(mettst_nominal), *metcst(metcst_nominal);
    xAOD::MissingETAuxContainer *mettst_aux(mettst_nominal_aux), *metcst_aux(metcst_nominal_aux);

    bool syst_affectsElectrons = ST::testAffectsObject(xAOD::Type::Electron, sysInfo.affectsType);
    bool syst_affectsMuons     = ST::testAffectsObject(xAOD::Type::Muon, sysInfo.affectsType);
    bool syst_affectsPhotons   = ST::testAffectsObject(xAOD::Type::Photon, sysInfo.affectsType);
    //bool syst_affectsTaus      = ST::testAffectsObject(xAOD::Type::Tau, sysInfo.affectsType);
    bool syst_affectsJets      = ST::testAffectsObject(xAOD::Type::Jet, sysInfo.affectsType);
    bool syst_affectsBTag      = ST::testAffectsObject(xAOD::Type::BTag, sysInfo.affectsType);
    if (sysInfo.affectsKinematics) {
      // If necessary (kinematics affected), make a shallow copy with the variation applied
      // otherwise set the collection pointers to the nominal
      if (syst_affectsElectrons) {
        ATH_MSG_DEBUG("Get systematics-varied electrons");
        xAOD::ElectronContainer* electrons_syst(0);
        xAOD::ShallowAuxContainer* electrons_syst_aux(0);
        CHECK( m_SUSYTools->GetElectrons(electrons_syst, electrons_syst_aux) );
        electrons = electrons_syst;
        electrons_aux = electrons_syst_aux;
      }

      if (syst_affectsPhotons) {
        ATH_MSG_DEBUG("Get systematics-varied photons");
        xAOD::PhotonContainer* photons_syst(0);
        xAOD::ShallowAuxContainer* photons_syst_aux(0);
        CHECK( m_SUSYTools->GetPhotons(photons_syst, photons_syst_aux) );
        photons = photons_syst;
        photons_aux = photons_syst_aux;
      }

      if (syst_affectsMuons) {
        ATH_MSG_DEBUG("Get systematics-varied muons");
        xAOD::MuonContainer* muons_syst(0);
        xAOD::ShallowAuxContainer* muons_syst_aux(0);
        CHECK( m_SUSYTools->GetMuons(muons_syst, muons_syst_aux) );
        muons = muons_syst;
        muons_aux = muons_syst_aux;
      }

      if (syst_affectsJets) {
        ATH_MSG_DEBUG("Get systematics-varied jets");
        xAOD::JetContainer* jets_syst(0);
        xAOD::ShallowAuxContainer* jets_syst_aux(0);
        CHECK( m_SUSYTools->GetJetsSyst(*jets_nominal, jets_syst, jets_syst_aux) );
        jets = jets_syst;
        jets_aux = jets_syst_aux;
      }

      ATH_MSG_DEBUG("Get systematics-varied MET");
      xAOD::MissingETContainer* mettst_syst = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* mettst_syst_aux = new xAOD::MissingETAuxContainer;
      xAOD::MissingETContainer* metcst_syst = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* metcst_syst_aux = new xAOD::MissingETAuxContainer;
      mettst_syst->setStore(mettst_syst_aux);
      metcst_syst->setStore(metcst_syst_aux);
      mettst_syst->reserve(10);
      metcst_syst->reserve(10);
      //
      CHECK( m_SUSYTools->GetMET(*metcst_syst,
                                 jets,
                                 electrons,
                                 muons) );
      //
      CHECK( m_SUSYTools->GetMET(*mettst_syst,
                                 jets,
                                 electrons,
                                 muons,
                                 0, 0, true, true) );
      //
      mettst     = mettst_syst;
      mettst_aux = mettst_syst_aux;
      metcst     = metcst_syst;
      metcst_aux = metcst_syst_aux;
    }

    //--- Overlap Removal
    ATH_CHECK( m_SUSYTools->OverlapRemoval(electrons, muons, jets, photons) );


    //--- Electrons
    ATH_MSG_DEBUG("Working on electrons");
    float electrons_weight(1.);
    for ( const auto& el : *electrons ) {
      if( m_dataSource > 0 ){
	if (isNominal || syst_affectsElectrons) {
	  if ((el->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
	    electrons_weight *= m_SUSYTools->GetSignalElecSF( *el );
	  }
	}
      }
      ATH_MSG_VERBOSE( "  Electron passing baseline selection?" << (int) el->auxdata<char>("baseline") );
      ATH_MSG_VERBOSE( "  Electron passing signal selection?" << (int) el->auxdata<char>("signal") );
      if (el->auxdata< char >("signal") == 1)
        ATH_MSG_VERBOSE( "  Electron weight " << el->auxdata<float>("effscalefact") );
      
    }
    if (isNominal) {
      electrons_weight_nominal = electrons_weight;
      weight_electrons->SetBinContent(1, weight_electrons->GetBinContent(1)+electrons_weight);    
    }
    else if (!syst_affectsElectrons) {
      electrons_weight = electrons_weight_nominal;
    }
    else if ( sysInfo.affectsWeights ){
      size_t iwbin = find(syst_el_weights.begin(), syst_el_weights.end(), sys.name()) - syst_el_weights.begin(); 
      if(iwbin < syst_el_weights.size()) {  weight_electrons->SetBinContent(iwbin+1, weight_electrons->GetBinContent(iwbin+1)+electrons_weight); }    
    }
    
    event_weight *= electrons_weight;


    //--- Photons
    ATH_MSG_DEBUG("Working on photons");
    float photons_weight(1.);
    for ( const auto& el : *photons ) {
      if( m_dataSource > 0 ){
	if (isNominal || syst_affectsPhotons) {
	  if ((el->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
	    photons_weight *= m_SUSYTools->GetSignalPhotonSF( *el );
	  }
	}
      }
      ATH_MSG_VERBOSE( "  Photon passing baseline selection?" << (int) el->auxdata<char>("baseline") );
      ATH_MSG_VERBOSE( "  Photon passing signal selection?" << (int) el->auxdata<char>("signal") );
      if (el->auxdata< char >("signal") == 1)
        ATH_MSG_VERBOSE( "  Photon weight " << el->auxdata<float>("effscalefact") );
      
    }
    if (isNominal) {
      photons_weight_nominal = photons_weight;
      weight_photons->SetBinContent(1, weight_photons->GetBinContent(1)+photons_weight);    
    }
    else if (!syst_affectsPhotons) {
      photons_weight = photons_weight_nominal;
    }
    else if ( sysInfo.affectsWeights ){
      size_t iwbin = find(syst_ph_weights.begin(), syst_ph_weights.end(), sys.name()) - syst_ph_weights.begin(); 
      if(iwbin < syst_ph_weights.size()) {  weight_photons->SetBinContent(iwbin+1, weight_photons->GetBinContent(iwbin+1)+photons_weight); }    
    }
    
    event_weight *= photons_weight;



    ATH_MSG_DEBUG("Working on muons");
    float muons_weight(1.);
    for ( const auto& mu : *muons ) {
      if( m_dataSource > 0 ){
	if (isNominal || syst_affectsMuons) {
	  if ((mu->auxdata< char >("signal") == 1) && (isNominal || sysInfo.affectsWeights)) {
	    muons_weight *= m_SUSYTools->GetSignalMuonSF(*mu);;
	  }
	}
      }
      ATH_MSG_VERBOSE( "  Muon passing baseline selection?" << (int) mu->auxdata<char>("baseline") );
      ATH_MSG_VERBOSE( "  Muon passing signal selection?" << (int) mu->auxdata<char>("signal") );
      ATH_MSG_VERBOSE( "  Muon is a cosmic ray?" << (int) mu->auxdata<char>("cosmic") );
      if (mu->auxdata< char >("signal") == 1)
        ATH_MSG_VERBOSE( "  Muon weight " << mu->auxdata<float>("effscalefact") );
    }

    if (isNominal) {
      muons_weight_nominal = muons_weight;
      weight_muons->SetBinContent(1, weight_muons->GetBinContent(1)+muons_weight);    
    }
    else if (!syst_affectsMuons) {
      muons_weight = muons_weight_nominal;
    }
    else if ( sysInfo.affectsWeights ){
      size_t iwbin = find(syst_mu_weights.begin(), syst_mu_weights.end(), sys.name()) - syst_mu_weights.begin(); 
      if(iwbin < syst_mu_weights.size()) {  weight_muons->SetBinContent(iwbin+1, weight_muons->GetBinContent(iwbin+1)+muons_weight); }    
    }

    event_weight *= muons_weight;


    ATH_MSG_DEBUG("Working on jets");
    for ( const auto& jet : *jets ) {
      ATH_MSG_VERBOSE( " Jet is bad? " << (int) jet->auxdata<char>("bad") );
      ATH_MSG_VERBOSE( " Jet is baseline ? " << (int) jet->auxdata<char>("baseline") );
      ATH_MSG_VERBOSE( " Jet passes OR ? " << (int) jet->auxdata<char>("passOR") );
      if (jet->auxdata<char>("passOR")) {
        if (isNominal || syst_affectsJets || syst_affectsBTag) {
          m_SUSYTools->IsBJet(*jet);
        }
        ATH_MSG_VERBOSE( " Jet is bjet ? " << (int) jet->auxdata<char>("bjet") );
      }
    }

    float jet_weight(1.);    
    float btag_weight(1.);   
    if( m_dataSource > 0) { //isMC
      
      if (isNominal) { //btagging
	btag_weight_nominal = btag_weight = m_SUSYTools->BtagSF(jets);
	weight_btags->SetBinContent(1, weight_btags->GetBinContent(1)+btag_weight);    
      }
      else if (!syst_affectsBTag){
	btag_weight = btag_weight_nominal;
      }
      else{
	btag_weight = m_SUSYTools->BtagSF(jets);
	size_t iwbin = find(syst_btag_weights.begin(), syst_btag_weights.end(), sys.name()) - syst_btag_weights.begin(); 
	if(iwbin < syst_btag_weights.size()) {  weight_btags->SetBinContent(iwbin+1, weight_jets->GetBinContent(iwbin+1)+btag_weight); }    
      }

      if(isNominal){ //JVT
	jets_weight_nominal = jet_weight = m_SUSYTools->JVT_SF(jets);
	weight_jets->SetBinContent(1, weight_jets->GetBinContent(1)+jet_weight);
      }
      else if (!syst_affectsJets || (syst_affectsJets && !sysInfo.affectsWeights)){
	jet_weight = jets_weight_nominal;
      }
      else if ( syst_affectsJets && sysInfo.affectsWeights ){
	jet_weight = m_SUSYTools->JVT_SF(jets);
	size_t iwbin = find(syst_jet_weights.begin(), syst_jet_weights.end(), sys.name()) - syst_jet_weights.begin(); 
	if(iwbin < syst_jet_weights.size()) {  weight_jets->SetBinContent(iwbin+1, weight_jets->GetBinContent(iwbin+1)+jet_weight); }    
      }
    }

    event_weight *= btag_weight;
    ATH_MSG_DEBUG("Combined b-tagging scale factor: " << btag_weight);
    ATH_MSG_DEBUG("Combined jet scale factor: " << jet_weight);

    ATH_MSG_DEBUG("Full event weight: " << event_weight);
    if (isNominal) {event_weight_nominal = event_weight;}
    else if (sysInfo.affectsWeights) ATH_MSG_DEBUG("Difference with nominal weight: " << event_weight - event_weight_nominal);

    // Clean up the systematics copies
    if (sysInfo.affectsKinematics) {
      delete metcst;
      delete mettst;
      delete metcst_aux;
      delete mettst_aux;
      if (syst_affectsElectrons) {
        delete electrons;
        delete electrons_aux;
      }
      if (syst_affectsPhotons) {
        delete photons;
        delete photons_aux;
      }
      if (syst_affectsMuons) {
        delete muons;
        delete muons_aux;
      }
      if (syst_affectsJets) {
        delete jets;
        delete jets_aux;
      }
    }
    isNominal = false;
  }

  delete jets_nominal;
  delete jets_nominal_aux;
  delete electrons_nominal;
  delete electrons_nominal_aux;
  delete photons_nominal;
  delete photons_nominal_aux;
  delete muons_nominal;
  delete muons_nominal_aux;
  delete mettst_nominal;
  delete mettst_nominal_aux;
  delete metcst_nominal;
  delete metcst_nominal_aux;

  m_clock0.Stop();
  if (m_Nevts == 0) {
    m_clock1.Stop();
  } else {
    m_clock2.Stop();
  }
  ++m_Nevts;

  return StatusCode::SUCCESS;
}

//**********************************************************************


