/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)             
 *             Stefano Giagu (stefano.giagu@cern.ch)    
 *             Kunihiro Nagano (nagano@mail.cern.ch)
 */

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;

StatusCode HLTMuonMonTool::initMuCombDQA()
{
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::bookMuCombDQA()
{
  //histograms in each 10LBs 
  if( newRunFlag() || newLowStatFlag()){
    addHistogram( new TH2F("muComb_eta_vs_phi_in_10LBs",           "muComb eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), m_histdircoverage );

  }

  if( newRunFlag() ){

    //  basic EDM
    addHistogram( new TH1F("muComb_pt_all",          "muComb pt (GeV/c); p_{T}[GeV/c]; Entries",                  210, -105., 105.), m_histdirmucomb );
    addHistogram( new TH1F("muComb_pt",              "muComb pt (GeV/c); p_{T}[GeV/c]; Entries",                  210, -105., 105.), m_histdirmucomb );
    addHistogram( new TH1F("muComb_eta",             "muComb eta; #eta; Entries",                       108, -2.7,  2.7), m_histdirmucomb );
    addHistogram( new TH1F("muComb_phi",             "muComb phi; #phi[rad]; Entries",                       96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );
    addHistogram( new TH2F("muComb_eta_vs_phi",      "muComb eta vs phi; #eta ; #phi",           108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );
	 
	 addHistogram( new TH2F("muComb_eta_vs_phi_MCmatching_success",  "muComb eta vs phi (MC match success); #eta ; #phi", 108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );
	 addHistogram( new TH2F("muComb_eta_vs_phi_OFFmatching_failure", "muComb eta vs phi (MC match success Off match failure); #eta ; #phi",108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );
	 addHistogram( new TH2F("L2MuonSA_eta_vs_phi_MCmatching_failure",  "L2MuonSA eta vs phi (MC match failure Off match success); #eta ; #phi",           108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );

    // comparison muComb vs L2MuonSA
    addHistogram( new TH1F("muComb_ptratio_toMF",    "muComb pt / L2MuonSA pt; p_{T} ratio; Entries",          140, -0.5,  3), m_histdirmucomb );
    addHistogram( new TH1F("muComb_dR_toMF",         "muComb delta R to L2MuonSA; #DeltaR; Entries",           100,   0,  0.5), m_histdirmucomb );
    addHistogram( new TH1F("muComb_deta_toMF",       "muComb delta eta to L2MuonSA; #Delta#eta; Entries",      120, -0.2, 0.2), m_histdirmucomb );
    addHistogram( new TH1F("muComb_dphi_toMF",       "muComb delta phi to L2MuonSA; #Delta#phi; Entries",      120, -0.2, 0.2), m_histdirmucomb );
    addHistogram( new TH1F("muComb_MF_error",        "muComb pointer to MuonFeature Error",                  5,0,5),          m_histdirmucomb );
    hist("muComb_MF_error", m_histdirmucomb)->GetXaxis()->SetBinLabel(1,"No error");
    hist("muComb_MF_error", m_histdirmucomb)->GetXaxis()->SetBinLabel(2,"Missing Pointer to MF");
    hist("muComb_MF_error", m_histdirmucomb)->GetXaxis()->SetBinLabel(3,"MF reconstruction Error");
    hist("muComb_MF_error", m_histdirmucomb)->GetXaxis()->SetBinLabel(4,"CB ContainerSize Error");
    hist("muComb_MF_error", m_histdirmucomb)->GetXaxis()->SetBinLabel(5,"MF ContainerSize Error");

    // comparison IDtrk vs L2MuonSA
    addHistogram( new TH1F("muComb_ptratio_TRKtoMF", "muComb Trk pt / L2MuonSA pt; p_{T} ratio; Entries",      140, -0.5, 3), m_histdirmucomb );
    addHistogram( new TH1F("muComb_dR_TRKtoMF",      "muComb delta R TRK to L2MuonSA; #DeltaR; Entries",       100,    0, 0.5), m_histdirmucomb );
    addHistogram( new TH1F("muComb_deta_TRKtoMF",    "muComb delta eta TRK to L2MuonSA; #Delta#eta; Entries",  120, -0.2, 0.2), m_histdirmucomb );
    addHistogram( new TH1F("muComb_dphi_TRKtoMF",    "muComb delta phi TRK to L2MuonSA; #Delta#phi; Entries",  120, -0.2, 0.2), m_histdirmucomb );

    // ID variables
    addHistogram( new TH1F("muComb_TRKpt",           "muComb Trk pt (GeV/c); p_{T}[GeV/c]; Entries",      210, -105., 105.), m_histdirmucomb );
    addHistogram( new TH1F("muComb_TRKeta",          "muComb Trk eta; #eta; Entries",           108, -2.7, 2.7), m_histdirmucomb );
    addHistogram( new TH1F("muComb_TRKphi",          "muComb Trk phi; #phi[rad]; Entries",           96, -CLHEP::pi,CLHEP::pi), m_histdirmucomb );
    addHistogram( new TH1F("muComb_TRKz0",           "muComb Trk z0 (mm); z0[mm]; Entries",       150, -300, 300), m_histdirmucomb );
    addHistogram( new TH1F("muComb_TRKchi2",         "muComb Trk chi2; #chi^{2}; Entries",           50, 0, 10), m_histdirmucomb );

    // in case of failure 
    addHistogram( new TH1F("muComb_failed_MFpt",     "L2MuonSA pt (GeV/c) for muComb fails; p_{T}[GeV/c]; Entries",  210, -105., 105.), m_histdirmucomb );
    addHistogram( new TH1F("muComb_failed_MFeta",    "L2MuonSA eta for muComb fails; #eta; Entries",       108, -2.7,  2.7), m_histdirmucomb );
    addHistogram( new TH1F("muComb_failed_MFphi",    "L2MuonSA phi for muComb fails; #phi[rad]; Entries",   96, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );

    // Comparison to Offline
    addHistogram( new TH1F("muComb_dR_toOffl",          "dR between muComb and Offline; #DeltaR; Entries",           100,  0,  2), m_histdirmucomb );
    addHistogram( new TH1F("muComb_ptresol_toOffl",     "muComb pT resolution wrt Offline; p_{T} resol; Entries",        100, -2., 2.), m_histdirmucomb );
    addHistogram( new TH2F("muComb_ptresol_toOffl_eta", "muComb pT resolution wrt Offline in eta; #eta; p_{T} resol", 16, -3.2, 3.2, 100, -2, 2), m_histdirmucomb );

    // Efficiency wrt Offline
    addHistogram( new TH1F("muComb_effi_toOffl_pt",                "muComb effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",              26, 0, 52), m_histdireff);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_numer",          "muComb effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",        26, 0, 52), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_denom",          "muComb effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",        26, 0, 52), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_barrel",         "muComb effi pt (GeV/c) barrel; p_{T}[GeV/c]; Efficiency",       26, 0, 52), m_histdireff);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_barrel_numer",   "muComb effi pt (GeV/c) barrel numer; p_{T}[GeV/c]; Entries", 26, 0, 52), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_barrel_denom",   "muComb effi pt (GeV/c) barrel denom; p_{T}[GeV/c]; Entries", 26, 0, 52), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_endcap",         "muComb effi pt (GeV/c) endcap; p_{T}[GeV/c]; Efficiency",       26, 0, 52), m_histdireff);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_endcap_numer",   "muComb effi pt (GeV/c) endcap numer; p_{T}[GeV/c]; Entries", 26, 0, 52), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_pt_endcap_denom",   "muComb effi pt (GeV/c) endcap denom; p_{T}[GeV/c]; Entries", 26, 0, 52), m_histdirmucomb);

    addHistogram( new TH1F("muComb_effi_toOffl_eta",           "muComb effi eta; #eta; Efficiency",       32, -3.2, 3.2), m_histdireff );
    addHistogram( new TH1F("muComb_effi_toOffl_eta_numer",     "muComb effi eta numer; #eta; Entries", 32, -3.2, 3.2), m_histdirmucomb);
    addHistogram( new TH1F("muComb_effi_toOffl_eta_denom",     "muComb effi eta denom; #eta; Entries", 32, -3.2, 3.2), m_histdirmucomb);

    addHistogram( new TH1F("muComb_effi_toOffl_phi",           "muComb effi phi; #phi[rad]; Efficiency",       32, -CLHEP::pi, CLHEP::pi), m_histdireff );
    addHistogram( new TH1F("muComb_effi_toOffl_phi_numer",     "muComb effi phi numer; #phi[rad]; Entries", 32, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );
    addHistogram( new TH1F("muComb_effi_toOffl_phi_denom",     "muComb effi phi denom; #phi[rad]; Entries", 32, -CLHEP::pi, CLHEP::pi), m_histdirmucomb );

  }
  //else if( newLumiBlockFlag() ){  }

  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillMuCombDQA()
{
  hist("Common_Counter", m_histdir )->Fill(MUCOMB);

  const float DR_MATCHED         = 0.5;
  const float ETA_OF_BARREL      = 1.05;
  const float ZERO_LIMIT         = 0.00001;

  const float PT_ERR_NOLINK      = -104.5;
  const float PTRATIO_ERR_MFFAIL = -0.4;
  const float PTRATIO_ERR_NOLINK = -0.2;

  const float PT_HISTMAX         = 100.;
  const float PT_OVFL            = 101.5;

  const float EFFI_PT_HISTMAX    =  50;
  const float EFFI_PT_OVFL       =  51;

  const float PTRESOL_HISTMAX    =  1.9;
  const float PTRESOL_OVFL       =  1.95;

  const float PTRATIO_HISTMAX    = 2.9;
  const float PTRATIO_OVFL       = 2.95;

  const float DR_HISTMAX         = 0.45;
  const float DR_OVFL            = 0.48;

  const float DETA_HISTMAX       = 0.18;
  const float DETA_OVFL          = 0.19;

  const float DPHI_HISTMAX       = 0.18;
  const float DPHI_OVFL          = 0.19;

  // -----------------------------
  // Retrieve L2CombinedMuonContainer
  // -----------------------------
  const DataHandle<xAOD::L2CombinedMuonContainer> combContainer;
  const DataHandle<xAOD::L2CombinedMuonContainer> lastcombContainer;
  StatusCode sc_comb = evtStore()->retrieve(combContainer,lastcombContainer);
  if ( sc_comb.isFailure() ) {
    ATH_MSG_WARNING( "Failed to retrieve HLT muComb container" );
    return StatusCode::SUCCESS;    
  }
  
  ATH_MSG_DEBUG( " ====== START HLTMuon muComb MonTool ====== " ); 
   
  // -----------------------------
  // Dump combinedMuonFeature info
  // -----------------------------

  std::vector<const xAOD::L2CombinedMuon*> vec_combinedMuon;

  for(auto itr=combContainer; itr != lastcombContainer; itr++) {
    const xAOD::L2CombinedMuonContainer* comb(nullptr);
    ATH_CHECK( evtStore()->retrieve(comb, itr.key()));
    for(auto jtr=comb->begin();jtr!=comb->end();jtr++) {
      if( (*jtr)==nullptr ) continue;
      vec_combinedMuon.push_back( *jtr );
    }
  }

  int nMuComb = vec_combinedMuon.size();

  std::vector<const xAOD::L2CombinedMuon*>::const_iterator itComb;

  for(itComb=vec_combinedMuon.begin(); itComb != vec_combinedMuon.end(); itComb++) {

    float pt  = (*itComb)->pt() / CLHEP::GeV;  // convert to GeV
    float eta = (*itComb)->eta();
    float phi = (*itComb)->phi();

    ATH_MSG_DEBUG( " pt  " << pt ); 
    ATH_MSG_DEBUG( " eta " << eta );
    ATH_MSG_DEBUG( " phi " << phi );

     
    // get MF
    const xAOD::L2StandAloneMuon* ptr_mf = nullptr;
    if( (*itComb)->muSATrackLink().isValid() ) {
      ptr_mf = (*itComb)->muSATrack();
    }
    float mf_pt  = 0.;
    float mf_eta = 0.;
    float mf_phi = 0.;
    if( ptr_mf ) {
      mf_pt  = ptr_mf->pt();
      mf_eta = ptr_mf->eta();
      mf_phi = ptr_mf->phi();
    }

    // get IDTrack
    bool ID_con=false;
    /// FIXME: don't hardcode track collection names into the c++ code
    ///        these should be in a job option
    std::string ID_con_name="HLT_IDTrack_Muon_FTF"; // run 3 ID track container name
    if(getTDT()->getNavigationFormat() == "TriggerElement") ID_con_name="HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF"; // run 2 ID track container name
    const xAOD::TrackParticle *ptr_trk = nullptr;
    const xAOD::TrackParticleContainer* Idcbtrk = nullptr;
    StatusCode sc_idcb = m_storeGate->retrieve(Idcbtrk, ID_con_name.c_str());
    if (!sc_idcb.isFailure()) {
      ATH_MSG_DEBUG("the container: "<<ID_con_name<<" size:"<<Idcbtrk->size());
      if( Idcbtrk->size() > 0){
        ID_con=true;
      }else{
        ATH_MSG_DEBUG("HLTMuon: found the container of "<<ID_con_name<<", but the size: zero "); 
      }
    }else{
      ATH_MSG_DEBUG("HLTMuon can not find the container: "<<ID_con_name); 
    }
    if( ID_con && (*itComb)->idTrackLink().isValid() ) {
      ptr_trk = (*itComb)->idTrack();
    }
    
    float trk_pt   = 0.;
    float trk_eta  = 0;
    float trk_phi  = 0;
    float trk_z0   = 0;
    float trk_chi2 = 0;
    if( ptr_trk ) {
      trk_pt   = ptr_trk->pt() / CLHEP::GeV; // convert to GeV
      trk_eta  = ptr_trk->eta();
      trk_phi  = ptr_trk->phi0();
      trk_z0   = ptr_trk->z0();
      trk_chi2 = ptr_trk->chiSquared();
    }
     
    // values
    bool cmf_success = true;
    if( fabs(pt) < ZERO_LIMIT )  cmf_success = false;
	
    float pt_hist = pt;
    if( fabs(pt_hist) > PT_HISTMAX ) {
      if( fabs(pt_hist) > 0 ) { pt_hist =  PT_OVFL; }
      else                    { pt_hist = -PT_OVFL; }
    }

    float mf_pt_hist = mf_pt;
    if( ptr_mf == 0 ) mf_pt_hist = PT_ERR_NOLINK;
     
    float trk_pt_hist = trk_pt;
    if( ptr_trk == 0 ) trk_pt_hist = PT_ERR_NOLINK; 
     
    float ptratio_cmb_mf_hist = 0.;
    if( cmf_success ) {
      if( ptr_mf == 0 ) {
	ptratio_cmb_mf_hist = PTRATIO_ERR_NOLINK;
      }
      else {
	if( fabs(mf_pt) < ZERO_LIMIT ) { ptratio_cmb_mf_hist = PTRATIO_ERR_MFFAIL; }
	else { 
	  ptratio_cmb_mf_hist = fabs(pt / mf_pt); 
	  if( fabs(ptratio_cmb_mf_hist) > PTRATIO_HISTMAX ) { ptratio_cmb_mf_hist =  PTRATIO_OVFL; }
	}
      }
    }

    float ptratio_trk_mf_hist = 0.;
    if( cmf_success ) {
      if( ptr_trk == 0 || ptr_mf == 0 ) {
	ptratio_trk_mf_hist = PTRATIO_ERR_NOLINK;
      }
      else {
	if( fabs(mf_pt) < ZERO_LIMIT ) { ptratio_trk_mf_hist = PTRATIO_ERR_MFFAIL; }
	else { 
	  ptratio_trk_mf_hist = fabs(trk_pt / mf_pt); 
	  if( fabs(ptratio_trk_mf_hist) > PTRATIO_HISTMAX ) {ptratio_trk_mf_hist =  PTRATIO_OVFL; }
	}
      }
    }

    // fill in histos

    hist("muComb_pt_all", m_histdirmucomb)->Fill(pt_hist);

    if( ! cmf_success ) {
      hist("muComb_failed_MFpt", m_histdirmucomb)->Fill(mf_pt_hist);
      if( ptr_mf != 0 && fabs(mf_pt) > ZERO_LIMIT ) { 
	hist("muComb_failed_MFeta", m_histdirmucomb)->Fill(mf_eta);
	hist("muComb_failed_MFphi", m_histdirmucomb)->Fill(mf_phi);
      }
      continue;
    }

    // only for muComb succeeded

    hist("muComb_pt", m_histdirmucomb)->Fill(pt_hist);
    hist("muComb_eta", m_histdirmucomb)->Fill(eta);
    hist("muComb_phi", m_histdirmucomb)->Fill(phi);
    if(eta != 0 && phi != 0) { //tomoe added 10/06/2011
      hist2("muComb_eta_vs_phi", m_histdirmucomb)->Fill(eta, phi);
      hist2("muComb_eta_vs_phi_in_10LBs", m_histdircoverage)->Fill(eta, phi);
    }

    // muComb vs mFast
    if(ptratio_cmb_mf_hist != PTRATIO_ERR_NOLINK && ptratio_cmb_mf_hist != PTRATIO_ERR_MFFAIL){
	   hist("muComb_ptratio_toMF", m_histdirmucomb)->Fill(ptratio_cmb_mf_hist);
    }
    if( ptr_mf != 0 && fabs(mf_pt) > ZERO_LIMIT ) { 
      float deta = eta-mf_eta;
      float dphi = calc_dphi(phi, mf_phi);
      float dR   = calc_dR(eta, phi, mf_eta, mf_phi);
      if( fabs(deta) > DETA_HISTMAX ) deta = DETA_OVFL * deta / fabs(deta);
      if( fabs(dphi) > DPHI_HISTMAX ) dphi = DPHI_OVFL * dphi / fabs(dphi);
      if(        dR  > DR_HISTMAX )     dR = DR_OVFL;
      hist("muComb_deta_toMF", m_histdirmucomb)->Fill(deta);
      hist("muComb_dphi_toMF", m_histdirmucomb)->Fill(dphi);
      hist("muComb_dR_toMF", m_histdirmucomb)->Fill(dR);
    }

    // TRK, TRK vs L2MuonSA
    hist("muComb_TRKpt", m_histdirmucomb)->Fill(trk_pt_hist);
    if( ptr_trk != 0 ) {
      hist("muComb_TRKeta", m_histdirmucomb)->Fill(trk_eta);
      hist("muComb_TRKphi", m_histdirmucomb)->Fill(trk_phi);
      hist("muComb_TRKz0", m_histdirmucomb)->Fill(trk_z0);
      hist("muComb_TRKchi2", m_histdirmucomb)->Fill(trk_chi2);
      hist("muComb_ptratio_TRKtoMF", m_histdirmucomb)->Fill(ptratio_trk_mf_hist);
      if( ptr_mf != 0 && fabs(mf_pt) > ZERO_LIMIT ) { 
	float deta = trk_eta-mf_eta;
	float dphi = calc_dphi(trk_phi, mf_phi);
	float dR   = calc_dR(trk_eta, trk_phi, mf_eta, mf_phi);
	if( fabs(deta) > DETA_HISTMAX ) deta = DETA_OVFL * deta / fabs(deta);
	if( fabs(dphi) > DPHI_HISTMAX ) dphi = DPHI_OVFL * dphi / fabs(dphi);
	if(        dR  > DR_HISTMAX )     dR = DR_OVFL;
	hist("muComb_deta_TRKtoMF", m_histdirmucomb)->Fill(deta);
	hist("muComb_dphi_TRKtoMF", m_histdirmucomb)->Fill(dphi);
	hist("muComb_dR_TRKtoMF", m_histdirmucomb)->Fill(dR);
      }
    }

     
  } // loop over vecCombinedMuon

  // -----------------------------
  // Loop for each chain (Domae)
  // -----------------------------
  //For standard chains
  for(unsigned int nchain=0;nchain<m_chainsGeneric.size();nchain++) {
	 Trig::FeatureContainer fc = getTDT()->features(m_chainsGeneric[nchain]);
	 std::vector<Trig::Combination> combs = fc.getCombinations();
	 std::vector<Trig::Combination>::const_iterator p_comb;
	 for(p_comb=combs.begin();p_comb!=combs.end();++p_comb) {
	   std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > fs_MF = 
		  (*p_comb).get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
		std::vector<Trig::Feature<xAOD::L2CombinedMuonContainer> > fs_CB = 
		  p_comb->get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs);
      if(!fs_MF.size() || !fs_CB.size()) continue;
		
		//muComb Error
                const xAOD::L2StandAloneMuonContainer* mf_cont = fs_MF[0];

                float mf_pt  = mf_cont->at(0)->pt();
                float mf_eta = mf_cont->at(0)->eta();
                float mf_phi = mf_cont->at(0)->phi();

                const xAOD::L2CombinedMuonContainer* cb_cont = fs_CB[0];
		bool error = false;
		if(fs_CB.size() == 1 && fs_MF.size()>0){
		  if(cb_cont->at(0)->muSATrack()){
			  if(mf_pt != cb_cont->at(0)->muSATrack()->pt()){
				  hist("muComb_MF_error", m_histdirmucomb)->Fill(1);
				  error = true;
			  }
		  } else {
			  hist("muComb_MF_error", m_histdirmucomb)->Fill(1);
			  error = true;
		  }
		}
		if(fabs(mf_pt) < ZERO_LIMIT){
		  hist("muComb_MF_error", m_histdirmucomb)->Fill(2);
		  error = true;
		}
		if(fs_CB.size() > 1){
		  hist("muComb_MF_error", m_histdirmucomb)->Fill(3);
		  error = true;
		}
		if(fs_MF.size() > 1){
		  hist("muComb_MF_error", m_histdirmucomb)->Fill(4);
		  error = true;
		}
		if(!error){
                  hist("muComb_MF_error", m_histdirmucomb)->Fill(0);
		}
                else continue;
    
                //Plot muComb eta vs phi for standard chain
		float mc_pt =  cb_cont->at(0)->pt();
		float mc_eta = cb_cont->at(0)->eta();
		float mc_phi = cb_cont->at(0)->phi();
		bool off_mc_match = false;
		bool off_mf_match = false;
		bool mc_success = (fabs(mc_pt) > 0.00001)? true:false;
		bool mf_success = (fabs(mf_pt) > 0.00001)? true:false;
      for(int i_offl=0;i_offl<(int)m_RecMuonCB_pt.size();i_offl++) {
        float eta_offl = m_RecMuonCB_eta[i_offl];
        float phi_offl = m_RecMuonCB_phi[i_offl];
	float mf_dR = calc_dR(mf_eta,mf_phi,eta_offl,phi_offl);
        float mc_dR = calc_dR(mc_eta,mc_phi,eta_offl,phi_offl);
	if(mf_dR < 0.5 && mf_success) off_mf_match = true;
	if(mc_dR < 0.05 && mc_success) off_mc_match = true;
      }
      if(mc_eta != 0 && mc_phi != 0) {  //tomoe added 10/06/2011
	  if(mc_success){
		  hist2("muComb_eta_vs_phi_MCmatching_success", m_histdirmucomb)->Fill(mc_eta,mc_phi);
	   }
	  if(mc_success && !off_mc_match){
		  hist2("muComb_eta_vs_phi_OFFmatching_failure", m_histdirmucomb)->Fill(mc_eta,mc_phi);
	  }
      } 
      if(mf_eta != 0 && mf_phi != 0) {  //tomoe added 10/06/2011
	      if(!mc_success && off_mf_match){
		      hist2("L2MuonSA_eta_vs_phi_MCmatching_failure", m_histdirmucomb)->Fill(mf_eta,mf_phi);
	      } 
      } 
    }
  }

  //For passHLT chains
  for(unsigned int nchain=0;nchain<m_chainsL2passHLT.size();nchain++) {
	 Trig::FeatureContainer fc = getTDT()->features(m_chainsL2passHLT[nchain]);
	 std::vector<Trig::Combination> combs = fc.getCombinations();
    std::vector<Trig::Combination>::const_iterator p_comb;
      
	 for(p_comb=combs.begin();p_comb!=combs.end();++p_comb) {

	   std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > fs_MF = 
		(*p_comb).get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
		std::vector<Trig::Feature<xAOD::L2CombinedMuonContainer> > fs_CB = 
		  p_comb->get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs);

                float mf_pt  = 0;
		if( fs_MF.size()>0){
			const xAOD::L2StandAloneMuonContainer* mf_cont = fs_MF[0];
			mf_pt  = mf_cont->at(0)->pt();
		}
   
		bool error = false;
		if(fs_CB.size() == 1 && fs_MF.size()>0){
		  const xAOD::L2CombinedMuonContainer* cb_cont = fs_CB[0];
		  if(cb_cont->at(0)->muSATrack()){
		    if(mf_pt != cb_cont->at(0)->muSATrack()->pt()){
		       hist("muComb_MF_error", m_histdirmucomb)->Fill(1);
		       error = true;
		    }
		  } else {
		       hist("muComb_MF_error", m_histdirmucomb)->Fill(1);
		       error = true;
		  }
		}
		if(fs_CB.size() > 1){
		  hist("muComb_MF_error", m_histdirmucomb)->Fill(3);
		  error = true;
		}
		if(fs_MF.size() > 1){
		  hist("muComb_MF_error", m_histdirmucomb)->Fill(4);
		  error = true;
		}
		if(!error){
	     hist("muComb_MF_error", m_histdirmucomb)->Fill(0);
		}

	   else continue;
    }
  }
  
  // -----------------------------
  // Comparision wrt Offline
  // -----------------------------

  for(int i_offl=0;i_offl<(int)m_RecMuonCB_pt.size();i_offl++) {

    float pt_offl  = m_RecMuonCB_pt[i_offl];
    float eta_offl = m_RecMuonCB_eta[i_offl];
    float phi_offl = m_RecMuonCB_phi[i_offl];

    float pt_offl_hist = fabs(pt_offl);
    if( pt_offl_hist > EFFI_PT_HISTMAX )  pt_offl_hist = EFFI_PT_OVFL;

    hist("muComb_effi_toOffl_pt_denom", m_histdirmucomb)->Fill(pt_offl_hist);
    hist("muComb_effi_toOffl_eta_denom", m_histdirmucomb)->Fill(eta_offl);
    hist("muComb_effi_toOffl_phi_denom", m_histdirmucomb)->Fill(phi_offl);

    if( fabs(eta_offl) < ETA_OF_BARREL ) {
      hist("muComb_effi_toOffl_pt_barrel_denom", m_histdirmucomb)->Fill(pt_offl_hist);
    }
    else {
      hist("muComb_effi_toOffl_pt_endcap_denom", m_histdirmucomb)->Fill(pt_offl_hist);
    }

    // check whether matching combinedMuonFeature is there
    float dRmin    = 1000.;
    float  pt_comb  = 0.; 
    // float  eta_comb = 0.; 
    // float  phi_comb = 0.; 
    for(itComb=vec_combinedMuon.begin(); itComb != vec_combinedMuon.end(); itComb++) {
      float pt  = (*itComb)->pt() / CLHEP::GeV;  // convert to GeV
      if( fabs(pt) < ZERO_LIMIT )  continue;
      float eta  = (*itComb)->eta();
      float phi  = (*itComb)->phi();
      float dR = calc_dR(eta,phi,eta_offl,phi_offl);
      if( dR < dRmin ) {
	dRmin = dR;
	pt_comb = (*itComb)->pt() / CLHEP::GeV;
	// eta_comb = eta;
	// phi_comb = phi;
      }
    }

    hist("muComb_dR_toOffl", m_histdirmucomb)->Fill(dRmin);
    if( dRmin > DR_MATCHED ) continue; // not matched to muComb

    // efficiencies, resolutions

    float ptresol = fabs(pt_comb)/fabs(pt_offl) - 1;
    float ptresol_hist = ptresol;
    if( fabs(ptresol) > PTRESOL_HISTMAX ) ptresol_hist = ptresol_hist / fabs(ptresol_hist) * PTRESOL_OVFL;

    hist("muComb_ptresol_toOffl", m_histdirmucomb)->Fill(ptresol_hist);
    hist2("muComb_ptresol_toOffl_eta", m_histdirmucomb)->Fill(eta_offl, ptresol_hist);
    hist("muComb_effi_toOffl_pt_numer", m_histdirmucomb)->Fill(pt_offl_hist);
    hist("muComb_effi_toOffl_eta_numer", m_histdirmucomb)->Fill(eta_offl);
    hist("muComb_effi_toOffl_phi_numer", m_histdirmucomb)->Fill(phi_offl);
    if( fabs(eta_offl) < ETA_OF_BARREL ) {
      hist("muComb_effi_toOffl_pt_barrel_numer", m_histdirmucomb)->Fill(pt_offl_hist);
    }
    else {
      hist("muComb_effi_toOffl_pt_endcap_numer", m_histdirmucomb)->Fill(pt_offl_hist);
    }

  }

  // -----------------------------
  // -----------------------------

  if(nMuComb)hist("Common_Counter", m_histdir)->Fill((float)MUCOMBFOUND);

  // end
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::procMuCombDQA()
{
  if( endOfRunFlag() ){

    hist("muComb_effi_toOffl_pt", m_histdireff)->Sumw2();
    hist("muComb_effi_toOffl_pt", m_histdireff)->Divide( hist("muComb_effi_toOffl_pt_numer", m_histdirmucomb), hist("muComb_effi_toOffl_pt_denom", m_histdirmucomb), 1, 1, "B" );

    hist("muComb_effi_toOffl_pt_barrel", m_histdireff)->Sumw2();
    hist("muComb_effi_toOffl_pt_barrel", m_histdireff)->Divide( hist("muComb_effi_toOffl_pt_barrel_numer", m_histdirmucomb), hist("muComb_effi_toOffl_pt_barrel_denom", m_histdirmucomb), 1, 1, "B" );

    hist("muComb_effi_toOffl_pt_endcap", m_histdireff)->Sumw2();
    hist("muComb_effi_toOffl_pt_endcap", m_histdireff)->Divide( hist("muComb_effi_toOffl_pt_endcap_numer", m_histdirmucomb), hist("muComb_effi_toOffl_pt_endcap_denom", m_histdirmucomb), 1, 1, "B" );

    hist("muComb_effi_toOffl_eta", m_histdireff)->Sumw2();
    hist("muComb_effi_toOffl_eta", m_histdireff)->Divide( hist("muComb_effi_toOffl_eta_numer", m_histdirmucomb), hist("muComb_effi_toOffl_eta_denom", m_histdirmucomb), 1, 1, "B" );

    hist("muComb_effi_toOffl_phi", m_histdireff)->Sumw2();
    hist("muComb_effi_toOffl_phi", m_histdireff)->Divide( hist("muComb_effi_toOffl_phi_numer", m_histdirmucomb), hist("muComb_effi_toOffl_phi_denom", m_histdirmucomb), 1, 1, "B" );

  }
  //else if( endOfLumiBlockFlag() ){  }
  return StatusCode::SUCCESS;
}
