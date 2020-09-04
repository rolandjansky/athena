/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Stefano Giagu (stefano.giagu@cern.ch)    
 *             Antonio Sidoti (antonio.sidoti@roma1.infn.it)    
 */

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigMuonEvent/IsoMuonFeatureContainer.h"

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

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;

StatusCode HLTMuonMonTool::initMuIsoDQA()
{
  return StatusCode::SUCCESS;  
  
}


StatusCode HLTMuonMonTool::bookMuIsoDQA()
{
  //histograms in each 10LBs 
  if( newRunFlag() || newLowStatFlag() ){

    addHistogram( new TH2F("muIso_eta_vs_phi_in_10LBs",           "muIso eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), m_histdircoverage );

  }

  if( newRunFlag() ){

    //  basic EDM
    addHistogram( new TH1F("muIso_pt",              "muIso pt (MeV/c); p_{T}[MeV/c]; Entries",              210, -105000., 105000.), m_histdirmuiso );
    addHistogram( new TH1F("muIso_eta",             "muIso eta; #eta; Entries",                             108, -2.7,  2.7), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_phi",             "muIso phi; #phi[rad]; Entries",                         96, -CLHEP::pi, CLHEP::pi), m_histdirmuiso  );
    addHistogram( new TH2F("muIso_eta_vs_phi",      "muIso eta vs phi; #eta ; #phi",                        108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), m_histdirmuiso );

    // CAL based ISO
    addHistogram( new TH1F("muIso_etinec",          "muIso EtInEC (MeV); E_{T}[MeV]; Entries",              100,-4000,15000), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_etoutec",         "muIso EtOutEC(MeV); E_{T}[MeV]; Entries",              100,-4000,15000), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_etinhc",          "muIso EtInHC (MeV); E_{T}[MeV]; Entries",              100,-4000,15000), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_etouthc",         "muIso EtOutHC (MeV); E_{T}[MeV]; Entries",             100,-4000,15000), m_histdirmuiso  );
	addHistogram( new TH1F("muIso_sumetcone",       "muIso SumEtCone Calo (MeV); E_{T}[MeV]; Entries",      100,-4000,15000), m_histdirmuiso  );  
	  
    // ID based ISO
    addHistogram( new TH1F("muIso_ntrk",            "muIso NTrk; number of tracks; Entries",                20,0,20), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_sumpt",           "muIso SumPt (MeV/c); #Sigmap_{T}[MeV/c]; Entries",     100,0,20000), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_ptmu",            "muIso muPtTrk (MeV/c); p_{T}[MeV/c]; Entries",         100,0,20000), m_histdirmuiso  );
	  
	addHistogram( new TH1F("muIso_sumptoverpt",     "muIso SumPt/muPt; ; Entries",                          100,-0.01,1.), m_histdirmuiso  );   

    // Comparison to Offline
    addHistogram( new TH1F("muIso_dR_toOffl",                "dR between muIso and Offline; #DeltaR; Entries",                 100,  0,  2), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_etcone_toOffl",            "Etcone04 muIso - Offline (MeV); #DeltaE_{T}[MeV]; Entries",      100,  -5000.,  5000.), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_ptcone_toOffl",            "Ptcone02 muIso - Offline (MeV/c); #Deltap_{T}[MeV/c]; Entries",  100,  -5000.,  5000.), m_histdirmuiso  );
    addHistogram( new TH1F("muIso_nucone_toOffl",            "NTrkcone02 muIso - Offline; #Deltantrk; Entries",                 40,  -20, 20), m_histdirmuiso  );

    // Efficiency wrt Offline
    addHistogram( new TH1F("muIso_effi_toOffl_pt",           "muIso effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",          26, 0, 52), m_histdireff);
    addHistogram( new TH1F("muIso_effi_toOffl_pt_numer",     "muIso effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",       26, 0, 52), m_histdirmuiso );
    addHistogram( new TH1F("muIso_effi_toOffl_pt_denom",     "muIso effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",       26, 0, 52), m_histdirmuiso );
    
  }
  //else if( newLumiBlockFlag() ){  }
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillMuIsoDQA()
{

  // Measure muIso in the events with non iso muon triggers
  if (!m_passedESNONISO) { 
    return StatusCode::SUCCESS;
  }
	
  hist("Common_Counter", m_histdir)->Fill((float)MUISO);

  const float PT_HISTMAX      = 100000.;
  const float PT_OVFL         = 101500.;

  // Retrieve IsoMuonFeatureContainer
  const DataHandle<IsoMuonFeatureContainer> isoContainer;
  const DataHandle<IsoMuonFeatureContainer> lastisoContainer;
  StatusCode sc_iso = evtStore()->retrieve(isoContainer,lastisoContainer);
  if ( sc_iso.isFailure() ) {
    ATH_MSG_DEBUG( "Failed to retrieve HLT muIso container" ); 
    return StatusCode::SUCCESS; 
  }

  ATH_MSG_DEBUG( " ====== START HLTMuon muIso MonTool ====== " ); 

  // -----------------------------
  // Dump IsoMuonFeature info
  // -----------------------------

  std::vector<const IsoMuonFeature*> vec_isoMuonFeatures;
  vec_isoMuonFeatures.clear();
  for(auto itr=isoContainer; itr!= lastisoContainer; itr++) {
    const IsoMuonFeatureContainer* iso(nullptr);
    ATH_CHECK( evtStore()->retrieve(iso, itr.key()));
    for(auto jtr=iso->begin(); jtr!= iso->end(); jtr++) {
      if( (*jtr)==nullptr ) continue;
      vec_isoMuonFeatures.push_back( *jtr );
    }
  }

  int nMuIso = vec_isoMuonFeatures.size();

  std::vector<const IsoMuonFeature*>::const_iterator itIso;

  for(itIso=vec_isoMuonFeatures.begin(); itIso != vec_isoMuonFeatures.end(); itIso++) {

    float pt         = (*itIso)->getPtMu();
    float pt_hist = pt;

    if( fabs(pt_hist) > PT_HISTMAX ) {
      if( pt_hist > 0 ) { pt_hist =  PT_OVFL; }
      else                    { pt_hist = -PT_OVFL; }
    }

    float eta        = (*itIso)->getEtaMu();
    float phi        = (*itIso)->getPhiMu();
    float EtInEC     = (*itIso)->getEtInnerConeEC();
    float EtOutEC    = (*itIso)->getEtOuterConeEC();
    float EtInHC     = (*itIso)->getEtInnerConeHC();
    float EtOutHC    = (*itIso)->getEtOuterConeHC();
    int   numTrack   = (*itIso)->getNTracksCone();
    float sumPtTrack = (*itIso)->getSumPtTracksCone();
    float PtMu       = (*itIso)->getPtMuTracksCone();
    // YY: tentatively commented out since it is never used
    // float LArWg      = (*itIso)->getLArWeight();
    // float TileWg     = (*itIso)->getTileWeight();
    // float ediEC      = (EtOutEC-EtInEC)*LArWg;
    // float ediHC      = (EtOutHC-EtInHC)*TileWg;
	  
	float caloiso    = EtOutEC-EtInEC+EtOutHC-EtInHC;
	float idiso      = 0.0;
	if (pt != 0.) idiso = (sumPtTrack-PtMu)/fabs(pt);
	  
    if( pt != 0. ) {
      hist("muIso_sumetcone", m_histdirmuiso ) ->Fill(caloiso);
	  hist("muIso_sumptoverpt", m_histdirmuiso ) ->Fill(idiso);
	  hist("muIso_pt", m_histdirmuiso ) ->Fill(pt_hist);
      hist("muIso_eta", m_histdirmuiso )->Fill(eta);
      hist("muIso_phi", m_histdirmuiso )->Fill(phi);
      hist2("muIso_eta_vs_phi", m_histdirmuiso )->Fill(eta, phi);
      hist2("muIso_eta_vs_phi_in_10LBs", m_histdircoverage )->Fill(eta, phi);
      hist("muIso_etinec", m_histdirmuiso )->Fill(EtInEC);
      hist("muIso_etoutec", m_histdirmuiso )->Fill(EtOutEC);
      hist("muIso_etinhc", m_histdirmuiso )->Fill(EtInHC);
      hist("muIso_etouthc", m_histdirmuiso )->Fill(EtOutHC);
      hist("muIso_ntrk", m_histdirmuiso )->Fill(numTrack);
      hist("muIso_sumpt", m_histdirmuiso )->Fill(sumPtTrack);
      hist("muIso_ptmu", m_histdirmuiso )->Fill(PtMu);
    }

  } // loop over vecIsoMuonFeatures

    // -----------------------------
    // Comparision wrt Offline
    // -----------------------------

  for(int i_offl=0;i_offl<(int)m_RecMuonCB_pt.size();i_offl++) {

    float pt_offl     = m_RecMuonCB_pt[i_offl];
    float eta_offl    = m_RecMuonCB_eta[i_offl];
    float phi_offl    = m_RecMuonCB_phi[i_offl];
    float eloss_offl  = m_RecMuonCB_eloss[i_offl];
    float etcone_offl = m_RecMuonCB_etcone[i_offl];
    float ptcone_offl = m_RecMuonCB_ptcone[i_offl];

    float etcone_offl_corr = etcone_offl + eloss_offl;

    float pt_offl_hist = fabs(pt_offl);
    hist("muIso_effi_toOffl_pt_denom", m_histdirmuiso )->Fill(pt_offl_hist);

    // match
    float  dRmin    = 100000000.;
    // float  pt_iso  = 0.; 
    // float  eta_iso = 0.; 
    // float  phi_iso = 0.; 
    float  etcone_iso = 0.; 
    float  ptcone_iso = 0.; 
    for(itIso=vec_isoMuonFeatures.begin(); itIso != vec_isoMuonFeatures.end(); itIso++) {
      // float pt         = (*itIso)->getPtMu();
      float eta        = (*itIso)->getEtaMu();
      float phi        = (*itIso)->getPhiMu();
      float EtOutEC    = (*itIso)->getEtOuterConeEC();
      float EtOutHC    = (*itIso)->getEtOuterConeHC();
      // int   numTrack   = (*itIso)->getNTracksCone();
      float sumPtTrack = (*itIso)->getSumPtTracksCone();
	  float PtMu       = (*itIso)->getPtMuTracksCone();
      float dR = calc_dR(eta,phi,eta_offl,phi_offl);
      if( dR < dRmin ) {
	dRmin   = dR;
	// pt_iso  = pt;
	// eta_iso = eta;
	// phi_iso = phi;
	etcone_iso = EtOutEC-EtOutEC + EtOutHC-EtOutHC;
	ptcone_iso = sumPtTrack-PtMu;
      }
    }

    hist("muIso_dR_toOffl", m_histdirmuiso )->Fill(dRmin);
    if( fabs(dRmin) > 0.4 ) continue; // not matched

    hist("muIso_effi_toOffl_pt_numer", m_histdirmuiso )->Fill(pt_offl_hist);

    float etcone_diff = etcone_iso - etcone_offl_corr;
    float ptcone_diff = ptcone_iso - ptcone_offl;
     
    hist("muIso_etcone_toOffl", m_histdirmuiso )->Fill(etcone_diff);
    hist("muIso_ptcone_toOffl", m_histdirmuiso )->Fill(ptcone_diff);
    hist("muIso_nucone_toOffl", m_histdirmuiso )->Fill(0);
  }

  // -----------------------------
  // -----------------------------

  if (nMuIso) hist("Common_Counter", m_histdir )->Fill((float)MUISOFOUND);

  // end
  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::procMuIsoDQA()
{
  if( endOfRunFlag() ){

    hist("muIso_effi_toOffl_pt", m_histdireff)->Sumw2();
    hist("muIso_effi_toOffl_pt", m_histdireff)->Divide( hist("muIso_effi_toOffl_pt_numer", m_histdirmuiso ), hist("muIso_effi_toOffl_pt_denom", m_histdirmuiso ), 1, 1, "B" ); 
  }
  //else if ( endOfLumiBlockFlag() ) {  }
  return StatusCode::SUCCESS;
}
