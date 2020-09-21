/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
 *             Heuijin Lim (hjlim@hep.anl.gov)
 *             Arantxa Ruiz (aranzazu.ruiz.martinez@cern.ch)
 */

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigMuonEvent/TileMuFeatureContainer.h"
//#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"

//#include "muonEvent/Muon.h"
//#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"

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

StatusCode HLTMuonMonTool::initTileMuDQA()
{
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::bookTileMuDQA()
{
  //histograms in each 10LBs 
  if( newRunFlag() || newLowStatFlag() ){

    addHistogram( new TH2F("TileMu_eta_vs_phi_in_10LBs",           "TileMu eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), m_histdircoverage );

  }

  if( newRunFlag() ){

    addHistogram(new TH1F("TileMu_N",  "TileMu Nmuons; number of muons; Entries", 10, 0., 10.), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_Eta","TileMu Eta; #eta; Entries", 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_Phi","TileMu Phi; #phi[rad]; Entries", 64, -3.14, 3.14), m_histdirtilemu);
    addHistogram(new TH2F("TileMu_Eta_Phi", "TileMu Eta vs. Phi; #eta; #phi[rad]", 34, -1.7, 1.7, 64,  -CLHEP::pi,  CLHEP::pi), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_Ene", "TileMu Energy(GeV); E[GeV]; Entries", 200, 0., 10.), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_EffwTrk", "TileMu Efficiency of Track-matching; ; Entries", 2, -0.5, 0.5), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_absPtTrk", "TileMu the matched track pt (GeV/c); p_{T}[GeV/c]; Entries", 200, 0., 20.), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_DelEtaTrkTR", "TileMu Eta(Tile)-EtaTR(Track); #Delta#eta; Entries", 200, -0.3, 0.3), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_DelPhiTrkTR",  "TileMu Phi(Tile)-PhiTR(Track); #Delta#phi[rad]; Entries", 200, -0.3, 0.3), m_histdirtilemu); 

    // Comparison wrt offline

    // Offline muons
    addHistogram(new TH1F("Rec_Eta", "TrigMuon_Rec_Eta; #eta; Entries", 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH1F("Rec_Phi", "TrigMuon_Rec_Phi; #phi[rad]; Entries", 64, -3.14, 3.14), m_histdirtilemu);
    addHistogram(new TH1F("Rec_Pt",  "TrigMuon_Rec_Pt; p_{T}[GeV/c]; Entries", 200, 0., 20.), m_histdirtilemu);

    // TileMuFeature correlations
    addHistogram(new TH2F("TileMu_RecCBMuon_EtaCor", "Correlation on #eta between TileMuFeature and Offline; Offline #eta; TileMu #eta", 34, -1.7,  1.7, 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH2F("TileMu_RecCBMuon_PhiCor", "Correlation on #phi between TileMuFeature and Offline; Offline #phi[rad]; TileMu #phi[rad]", 64, -3.14, 3.14, 64, -3.14, 3.14), m_histdirtilemu);

    // TileMuFeature delta
    addHistogram(new TH1F("TileMu_RecCBMuon_dEta", "#Delta#eta between TileMuFeature and Offline; #Delta#eta; Entries", 200, -1., 1.), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_RecCBMuon_dPhi", "#Delta#phi between TileMuFeature and Offline; #Delta#phi[rad]; Entries", 100, -0.5, 0.5), m_histdirtilemu);

    // TileMuFeature efficiencies
    addHistogram(new TH1F("Rec_Eta_Num", "Rec_Eta_Num; #eta; Entries", 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH1F("Rec_Phi_Num", "Rec_Phi_Num; #phi[rad]; Entries", 64, -3.14, 3.14), m_histdirtilemu);
    addHistogram(new TH1F("TileMu_RecCBMuon_EffEta", "Efficiency on #eta (TileMuFeature wrt Offline); #eta; Efficiency", 34, -1.7,  1.7), m_histdireff);
    addHistogram(new TH1F("TileMu_RecCBMuon_EffPhi", "Efficiency on #phi (TileMuFeature wrt Offline); #phi; Efficiency", 64, -3.14, 3.14), m_histdireff);

    // TileTrackMuFeature correlations
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_EtaCor", "Correlation on #eta between TileTrackMuFeature and Offline; Offline #eta; TileTrackMu #eta", 34, -1.7,  1.7, 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_PhiCor", "Correlation on #phi between TileTrackMuFeature and Offline; Offline #phi[rad]; TileTrackMu #phi[rad]", 64, -3.14, 3.14, 64, -3.14, 3.14), m_histdirtilemu);
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_PtCor", "Correlation on p_{T} between TileTrackMuFeature and Offline; Offline p_{T}[GeV/c]; TileTrackMu p_{T}[GeV/c]", 200, 0., 20., 200, 0., 20.), m_histdirtilemu);

    // TileTrackMuFeature delta
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dEta", "#Delta#eta between TileTrackMuFeature and Offline; #Delta#eta; Entries", 200, -1., 1.), m_histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dPhi", "#Delta#phi between TileTrackMuFeature and Offline; #Delta#phi; Entries", 100, -0.5, 0.5), m_histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dPt", "#Deltap_{T} between TileTrackMuFeature and Offline; #Deltap_{T}; Entries", 100, -0.5, 0.5), m_histdirtilemu);

    // TileTrackMuFeature efficiencies
    addHistogram(new TH1F("TileTrackMu_Eta", "TileTrackMu_Eta; #eta; Entries", 34, -1.7,  1.7), m_histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_Phi", "TileTrackMu_Phi; #phi; Entries", 64, -3.14, 3.14), m_histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_Pt",  "TileTrackMu_Pt; p_{T}; Entries", 200, 0., 20.), m_histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffEta", "Efficiency on #eta (TileTrackMuFeature wrt Offline); #eta; Efficiency", 34, -1.7,  1.7), m_histdireff);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffPhi", "Efficiency on #phi (TileTrackMuFeature wrt Offline); #phi; Efficiency", 64, -3.14, 3.14), m_histdireff);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffPt", "Efficiency on p_{T} (TileTrackMuFeature wrt Offline); p_{T}; Efficiency", 200, 0., 20.), m_histdireff);


  }else if( newLumiBlockFlag() ){
  }
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillTileMuDQA()
{
  hist("Common_Counter", m_histdir)->Fill((float)TILEMU);

  // Retrieve muons from TileMuFeatureContainer

  const DataHandle<TileMuFeatureContainer> TileMuFeat;
  const DataHandle<TileMuFeatureContainer> TileMuFeatEnd;

  StatusCode sc_TileMu = evtStore()->retrieve( TileMuFeat, TileMuFeatEnd );
  if( sc_TileMu.isFailure() ){
    ATH_MSG_VERBOSE( "Failed to retrieve HLT TileMu" );
    return StatusCode::SUCCESS;    
  } else {
    ATH_MSG_DEBUG( " ====== START HLTMuon TileMu MonTool ====== " );
  } 

  // Loop over TileMuFeature objects

  float NTileMu = 0.;

  for( auto itr=TileMuFeat; itr!= TileMuFeatEnd ; itr++ ) {
    const TileMuFeatureContainer* TileMuItrE(nullptr);
    ATH_CHECK( evtStore()->retrieve(TileMuItrE, itr.key()));
    for(auto TileMuItr=TileMuItrE->begin(); TileMuItr != TileMuItrE->end();TileMuItr++){
      if ( (*TileMuItr)==nullptr ) continue;
      // Extract the variables and fill the histograms
      float eta = (*TileMuItr)->eta();
      float phi = (*TileMuItr)->phi();
      float Ene = (*TileMuItr)->enedep().at(3);
      
      hist("TileMu_Eta", m_histdirtilemu)->Fill(eta);
      hist("TileMu_Phi", m_histdirtilemu)->Fill(phi); 
      hist2("TileMu_Eta_Phi", m_histdirtilemu)->Fill(eta, phi);
      hist2("TileMu_eta_vs_phi_in_10LBs", m_histdircoverage)->Fill(eta, phi);
      hist("TileMu_Ene", m_histdirtilemu)->Fill(Ene/1000.); 

      NTileMu++;

    }

  }

  hist("TileMu_N", m_histdirtilemu)->Fill(NTileMu);
  if(NTileMu) hist("Common_Counter", m_histdir)->Fill((float)TILEMUFOUND);

  // Retrieve muons from TileTrackMuFeatureContainer

  SG::ConstIterator< TileTrackMuFeatureContainer > TileTrackMu;
  SG::ConstIterator< TileTrackMuFeatureContainer > lastTileTrackMu;

  StatusCode sc = evtStore()->retrieve(TileTrackMu, lastTileTrackMu);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE( "Failed to retrieve HLT TileTrackMu" );
    return StatusCode::SUCCESS;    
  } else {
    ATH_MSG_DEBUG( " ====== START HLTMuon TileTrackMu MonTool ====== " );
  }

  // Loop over TileTrackMuFeature objects

  float NTileTrackMu = 0.;

  for( auto jtr=TileTrackMu; jtr != lastTileTrackMu ; jtr++ ) {  
    const TileTrackMuFeatureContainer* TileTrackMuItrE(nullptr);
    ATH_CHECK( evtStore()->retrieve(TileTrackMuItrE, jtr.key()));
    for(auto TileTrackMuItr =TileTrackMuItrE->begin(); TileTrackMuItr != TileTrackMuItrE->end(); TileTrackMuItr++ ) {        
      if( (*TileTrackMuItr)==nullptr ) continue;
      float absPtTrk = std::abs((*TileTrackMuItr)->PtTR_Trk());      

      ElementLink<TileMuFeatureContainer> TileMuEL = (*TileTrackMuItr)->TileMuLink();       
      const TileMuFeature* TileMu;
      if ( !TileMuEL.isValid() ) {
	ATH_MSG_WARNING( "In TileTrackMuFeatureContainer, No TileMuFeature (Something Wrong!)" );
	TileMu = 0;
      } else{
	TileMu = *TileMuEL;
      }

      float DelEtaTR = -9999.9;
      float DelPhiTR = -9999.9;
      if ( TileMu != 0 ) {  
	DelEtaTR = TileMu->eta() - (*TileTrackMuItr)->EtaTR_Trk();    
	DelPhiTR = TileMu->phi() - (*TileTrackMuItr)->PhiTR_Trk();
      }

      hist("TileMu_absPtTrk", m_histdirtilemu)->Fill( absPtTrk );
      hist("TileMu_DelEtaTrkTR", m_histdirtilemu)->Fill( DelEtaTR );
      hist("TileMu_DelPhiTrkTR", m_histdirtilemu)->Fill( DelPhiTR );
 
      NTileTrackMu++;

    } 

  }

  if (NTileMu > 0.) {
    hist("TileMu_EffwTrk", m_histdirtilemu)->Fill( NTileTrackMu/NTileMu );
    hist("Common_Counter", m_histdir)->Fill((float)TILEMUTRACKFOUND);
  }
  
  //****************************//
  // Trigger/Offline comparison //
  //****************************//

  float User_Pi = 3.141592;
  float DeltaEta_cut = 0.25;
  float DeltaPhi_cut = 0.12;
  float DeltaEta, DeltaPhi, muPhiTR;

  const xAOD::MuonContainer* muonCont;

  std::string muonKey = "Muons";

  sc = evtStore()->retrieve(muonCont, muonKey);
  if(sc.isFailure()){
    ATH_MSG_WARNING( "Container of muon particle with key " << muonKey << " not found in Store Gate" );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_VERBOSE( "Container of muon particle with key " << muonKey << " found in Store Gate with size " << muonCont->size() );

  xAOD::MuonContainer::const_iterator muonItr  = muonCont->begin();
  xAOD::MuonContainer::const_iterator muonItrE = muonCont->end();

  // Offline
  int isCombined=0;
  for ( ; muonItr != muonItrE; ++muonItr) {
    ATH_MSG_DEBUG( "Muon : "
                   << " eta = " << (*muonItr)->eta()
                   << " phi = " << (*muonItr)->phi()
                   << " pt  = " << fabs((*muonItr)->pt())
    		   << " Standalone  = " << ((*muonItr)->muonType()==xAOD::Muon::MuonStandAlone)
                   << " Combined  = " << ((*muonItr)->muonType()==xAOD::Muon::Combined)
                   << " Low pt  = " << ((*muonItr)->muonType()==xAOD::Muon::SegmentTagged && (*muonItr)->inDetTrackParticleLink()!=0));

    //                   << " Low pt  = " << (*muonItr)->isLowPtReconstructedMuon() );
    if( (*muonItr)->muonType()==xAOD::Muon::Combined ){
      if( fabs((*muonItr)->eta()) <= 1.4  &&  fabs((*muonItr)->pt()/CLHEP::GeV) >= 4. ) {
	m_eta_RecCB.push_back( (*muonItr)->eta() );
	m_phi_RecCB.push_back( (*muonItr)->phi() );
	m_pt_RecCB.push_back( (*muonItr)->pt()/CLHEP::GeV );
	hist("Rec_Eta", m_histdirtilemu)->Fill( (*muonItr)->eta() );
	hist("Rec_Phi", m_histdirtilemu)->Fill( (*muonItr)->phi() );
	hist("Rec_Pt", m_histdirtilemu)->Fill( (*muonItr)->pt()/CLHEP::GeV );
	isCombined++;
      }
    }
  }

  // TileMuFeature (TileMuId SA)
  NTileMu = 0.;
  sc_TileMu = evtStore()->retrieve( TileMuFeat, TileMuFeatEnd );
  if( sc_TileMu.isFailure() ){
    ATH_MSG_WARNING( "Failed to retrieve HLT TileMu" );
    return StatusCode::SUCCESS;
  }
  for( auto itr=TileMuFeat; itr!= TileMuFeatEnd ; itr++ ) {
    const TileMuFeatureContainer* TileMuItrE(nullptr);
    ATH_CHECK( evtStore()->retrieve(TileMuItrE, itr.key()));
    for(auto TileMuItr=TileMuItrE->begin(); TileMuItr != TileMuItrE->end();TileMuItr++){
      if ( (*TileMuItr)==nullptr ) continue;
      m_eta_Tile.push_back( (*TileMuItr)->eta() );
      m_phi_Tile.push_back( (*TileMuItr)->phi() );
      NTileMu++;
    } 
  }

  // Correlation TileMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileMu;j++) {
      hist2("TileMu_RecCBMuon_EtaCor", m_histdirtilemu)->Fill(m_eta_RecCB.at(i),m_eta_Tile.at(j));
      hist2("TileMu_RecCBMuon_PhiCor", m_histdirtilemu)->Fill(m_phi_RecCB.at(i),m_phi_Tile.at(j));
      hist("TileMu_RecCBMuon_dEta", m_histdirtilemu)->Fill(m_eta_Tile.at(j)-m_eta_RecCB.at(i));
      hist("TileMu_RecCBMuon_dPhi", m_histdirtilemu)->Fill(m_phi_Tile.at(j)-m_phi_RecCB.at(i));
    }
  }

  // Matching TileMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileMu;j++) {
      muPhiTR = m_phi_RecCB.at(i) - (1.23053e-04*(m_pt_RecCB.at(i)/fabs(m_pt_RecCB.at(i))) + 5.06985e-01/(m_pt_RecCB.at(i)));
      DeltaEta = m_eta_Tile.at(j) - m_eta_RecCB.at(i);
      DeltaPhi = m_phi_Tile.at(j) - muPhiTR;
      if (DeltaPhi >  User_Pi) DeltaPhi -= 2*User_Pi;
      if (DeltaPhi < -User_Pi) DeltaPhi += 2*User_Pi;
      if ( (fabs(DeltaEta) <= DeltaEta_cut)  &&  (fabs(DeltaPhi) <= DeltaPhi_cut) ) {
	hist("Rec_Eta_Num", m_histdirtilemu)->Fill( m_eta_RecCB.at(i) );
	hist("Rec_Phi_Num", m_histdirtilemu)->Fill( m_phi_RecCB.at(i) );
      }
    }
  }

  // TileTrackMuFeature (TileMuId combined with ID)
  NTileTrackMu = 0.;
  sc = evtStore()->retrieve(TileTrackMu, lastTileTrackMu);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Failed to retrieve HLT TileTrackMu" );
    return StatusCode::SUCCESS;
  }
  for( auto itr=TileTrackMu; itr!= lastTileTrackMu ; itr++ ) {
    const TileTrackMuFeatureContainer* TileTrackMuItrE(nullptr);
    ATH_CHECK( evtStore()->retrieve(TileTrackMuItrE, itr.key()));
    for(auto TileTrackMuItr=TileTrackMuItrE->begin(); TileTrackMuItr != TileTrackMuItrE->end();TileTrackMuItr++){
      if ( (*TileTrackMuItr)==nullptr ) continue;
      m_eta_TileTrack.push_back( (*TileTrackMuItr)->EtaTR_Trk() );
      m_phi_TileTrack.push_back( (*TileTrackMuItr)->PhiTR_Trk() );
      m_pt_TileTrack.push_back( (*TileTrackMuItr)->PtTR_Trk() );
      NTileTrackMu++;
    } 
  }

  // Correlation TileTrackMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileTrackMu;j++) {
      hist2("TileTrackMu_RecCBMuon_EtaCor", m_histdirtilemu)->Fill(m_eta_RecCB.at(i),m_eta_TileTrack.at(j));
      hist2("TileTrackMu_RecCBMuon_PhiCor", m_histdirtilemu)->Fill(m_phi_RecCB.at(i),m_phi_TileTrack.at(j));
      hist2("TileTrackMu_RecCBMuon_PtCor", m_histdirtilemu)->Fill(m_pt_RecCB.at(i),m_pt_TileTrack.at(j));
      hist("TileTrackMu_RecCBMuon_dEta", m_histdirtilemu)->Fill(m_eta_TileTrack.at(j)-m_eta_RecCB.at(i));
      hist("TileTrackMu_RecCBMuon_dPhi", m_histdirtilemu)->Fill(m_phi_TileTrack.at(j)-m_phi_RecCB.at(i));
      hist("TileTrackMu_RecCBMuon_dPt", m_histdirtilemu)->Fill(m_pt_TileTrack.at(j)-m_pt_RecCB.at(i));
    }
  }

  // Matching TileTrackMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileTrackMu;j++) {
      muPhiTR = m_phi_RecCB.at(i) - (1.23053e-04*(m_pt_RecCB.at(i)/fabs(m_pt_RecCB.at(i))) + 5.06985e-01/(m_pt_RecCB.at(i)));
      DeltaEta = m_eta_TileTrack.at(j) - m_eta_RecCB.at(i);
      DeltaPhi = m_phi_TileTrack.at(j) - muPhiTR;
      if (DeltaPhi >  User_Pi) DeltaPhi -= 2*User_Pi;
      if (DeltaPhi < -User_Pi) DeltaPhi += 2*User_Pi;
      if ( (fabs(DeltaEta) <= DeltaEta_cut)  &&  (fabs(DeltaPhi) <= DeltaPhi_cut) ) {
	hist("TileTrackMu_Eta", m_histdirtilemu)->Fill( m_eta_RecCB.at(i) );
	hist("TileTrackMu_Phi", m_histdirtilemu)->Fill( m_phi_RecCB.at(i) );
	hist("TileTrackMu_Pt", m_histdirtilemu)->Fill( m_pt_RecCB.at(i) );
      }
    }
  }

  // Clear Vectors

  m_eta_RecCB.clear();
  m_phi_RecCB.clear();
  m_pt_RecCB.clear();

  m_eta_Tile.clear();
  m_phi_Tile.clear();

  m_eta_TileTrack.clear();
  m_phi_TileTrack.clear();
  m_pt_TileTrack.clear();

  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::procTileMuDQA()
{
  if(endOfRunFlag()){

    // TileMuFeature efficiency on eta
    hist("TileMu_RecCBMuon_EffEta", m_histdireff)->Sumw2();
    hist("TileMu_RecCBMuon_EffEta", m_histdireff)->Divide( hist("Rec_Eta_Num", m_histdirtilemu), hist("Rec_Eta", m_histdirtilemu), 1, 1, "B" );

    // TileMuFeature efficiency on phi
    hist("TileMu_RecCBMuon_EffPhi", m_histdireff)->Sumw2();
    hist("TileMu_RecCBMuon_EffPhi", m_histdireff)->Divide( hist("Rec_Phi_Num", m_histdirtilemu), hist("Rec_Phi", m_histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on eta
    hist("TileTrackMu_RecCBMuon_EffEta", m_histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffEta", m_histdireff)->Divide( hist("TileTrackMu_Eta", m_histdirtilemu), hist("Rec_Eta", m_histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on phi
    hist("TileTrackMu_RecCBMuon_EffPhi", m_histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffPhi", m_histdireff)->Divide( hist("TileTrackMu_Phi", m_histdirtilemu), hist("Rec_Phi", m_histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on pt
    hist("TileTrackMu_RecCBMuon_EffPt", m_histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffPt", m_histdireff)->Divide( hist("TileTrackMu_Pt", m_histdirtilemu), hist("Rec_Pt", m_histdirtilemu), 1, 1, "B" );

    //removeHistogram("TileMu_Eta", m_histdir);
    //removeHistogram("TileMu_Phi", m_histdir);

    //removeHistogram("TileTrackMu_Eta", m_histdir);
    //removeHistogram("TileTrackMu_Phi", m_histdir);
    //removeHistogram("TileTrackMu_Pt", m_histdir);

    //removeHistogram("Rec_Eta", m_histdir);
    //removeHistogram("Rec_Phi", m_histdir);
    //removeHistogram("Rec_Pt", m_histdir);

  }
  //else if( endOfLumiBlockFlag() ){   }
  return StatusCode::SUCCESS;
}
