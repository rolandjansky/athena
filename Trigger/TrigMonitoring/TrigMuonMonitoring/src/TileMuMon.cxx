/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
 *             Heuijin Lim (hjlim@hep.anl.gov)
 *             Arantxa Ruiz (aranzazu.ruiz.martinez@cern.ch)
 */

#include "GaudiKernel/IJobOptionsSvc.h"
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
  if( newRun || newLowStat ){

    addHistogram( new TH2F("TileMu_eta_vs_phi_in_10LBs",           "TileMu eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), histdircoverage );

  }

  if( newRun ){

    addHistogram(new TH1F("TileMu_N",  "TileMu Nmuons; number of muons; Entries", 10, 0., 10.), histdirtilemu);
    addHistogram(new TH1F("TileMu_Eta","TileMu Eta; #eta; Entries", 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH1F("TileMu_Phi","TileMu Phi; #phi[rad]; Entries", 64, -3.14, 3.14), histdirtilemu);
    addHistogram(new TH2F("TileMu_Eta_Phi", "TileMu Eta vs. Phi; #eta; #phi[rad]", 34, -1.7, 1.7, 64,  -CLHEP::pi,  CLHEP::pi), histdirtilemu);
    addHistogram(new TH1F("TileMu_Ene", "TileMu Energy(GeV); E[GeV]; Entries", 200, 0., 10.), histdirtilemu);
    addHistogram(new TH1F("TileMu_EffwTrk", "TileMu Efficiency of Track-matching; ; Entries", 2, -0.5, 0.5), histdirtilemu);
    addHistogram(new TH1F("TileMu_absPtTrk", "TileMu the matched track pt (GeV/c); p_{T}[GeV/c]; Entries", 200, 0., 20.), histdirtilemu);
    addHistogram(new TH1F("TileMu_DelEtaTrkTR", "TileMu Eta(Tile)-EtaTR(Track); #Delta#eta; Entries", 200, -0.3, 0.3), histdirtilemu);
    addHistogram(new TH1F("TileMu_DelPhiTrkTR",  "TileMu Phi(Tile)-PhiTR(Track); #Delta#phi[rad]; Entries", 200, -0.3, 0.3), histdirtilemu); 

    // Comparison wrt offline

    // Offline muons
    addHistogram(new TH1F("Rec_Eta", "TrigMuon_Rec_Eta; #eta; Entries", 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH1F("Rec_Phi", "TrigMuon_Rec_Phi; #phi[rad]; Entries", 64, -3.14, 3.14), histdirtilemu);
    addHistogram(new TH1F("Rec_Pt",  "TrigMuon_Rec_Pt; p_{T}[GeV/c]; Entries", 200, 0., 20.), histdirtilemu);

    // TileMuFeature correlations
    addHistogram(new TH2F("TileMu_RecCBMuon_EtaCor", "Correlation on #eta between TileMuFeature and Offline; Offline #eta; TileMu #eta", 34, -1.7,  1.7, 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH2F("TileMu_RecCBMuon_PhiCor", "Correlation on #phi between TileMuFeature and Offline; Offline #phi[rad]; TileMu #phi[rad]", 64, -3.14, 3.14, 64, -3.14, 3.14), histdirtilemu);

    // TileMuFeature delta
    addHistogram(new TH1F("TileMu_RecCBMuon_dEta", "#Delta#eta between TileMuFeature and Offline; #Delta#eta; Entries", 200, -1., 1.), histdirtilemu);
    addHistogram(new TH1F("TileMu_RecCBMuon_dPhi", "#Delta#phi between TileMuFeature and Offline; #Delta#phi[rad]; Entries", 100, -0.5, 0.5), histdirtilemu);

    // TileMuFeature efficiencies
    addHistogram(new TH1F("Rec_Eta_Num", "Rec_Eta_Num; #eta; Entries", 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH1F("Rec_Phi_Num", "Rec_Phi_Num; #phi[rad]; Entries", 64, -3.14, 3.14), histdirtilemu);
    addHistogram(new TH1F("TileMu_RecCBMuon_EffEta", "Efficiency on #eta (TileMuFeature wrt Offline); #eta; Efficiency", 34, -1.7,  1.7), histdireff);
    addHistogram(new TH1F("TileMu_RecCBMuon_EffPhi", "Efficiency on #phi (TileMuFeature wrt Offline); #phi; Efficiency", 64, -3.14, 3.14), histdireff);

    // TileTrackMuFeature correlations
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_EtaCor", "Correlation on #eta between TileTrackMuFeature and Offline; Offline #eta; TileTrackMu #eta", 34, -1.7,  1.7, 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_PhiCor", "Correlation on #phi between TileTrackMuFeature and Offline; Offline #phi[rad]; TileTrackMu #phi[rad]", 64, -3.14, 3.14, 64, -3.14, 3.14), histdirtilemu);
    addHistogram(new TH2F("TileTrackMu_RecCBMuon_PtCor", "Correlation on p_{T} between TileTrackMuFeature and Offline; Offline p_{T}[GeV/c]; TileTrackMu p_{T}[GeV/c]", 200, 0., 20., 200, 0., 20.), histdirtilemu);

    // TileTrackMuFeature delta
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dEta", "#Delta#eta between TileTrackMuFeature and Offline; #Delta#eta; Entries", 200, -1., 1.), histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dPhi", "#Delta#phi between TileTrackMuFeature and Offline; #Delta#phi; Entries", 100, -0.5, 0.5), histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_dPt", "#Deltap_{T} between TileTrackMuFeature and Offline; #Deltap_{T}; Entries", 100, -0.5, 0.5), histdirtilemu);

    // TileTrackMuFeature efficiencies
    addHistogram(new TH1F("TileTrackMu_Eta", "TileTrackMu_Eta; #eta; Entries", 34, -1.7,  1.7), histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_Phi", "TileTrackMu_Phi; #phi; Entries", 64, -3.14, 3.14), histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_Pt",  "TileTrackMu_Pt; p_{T}; Entries", 200, 0., 20.), histdirtilemu);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffEta", "Efficiency on #eta (TileTrackMuFeature wrt Offline); #eta; Efficiency", 34, -1.7,  1.7), histdireff);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffPhi", "Efficiency on #phi (TileTrackMuFeature wrt Offline); #phi; Efficiency", 64, -3.14, 3.14), histdireff);
    addHistogram(new TH1F("TileTrackMu_RecCBMuon_EffPt", "Efficiency on p_{T} (TileTrackMuFeature wrt Offline); p_{T}; Efficiency", 200, 0., 20.), histdireff);


  }else if( newLumiBlock ){
  }
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillTileMuDQA()
{
  hist("Common_Counter", histdir)->Fill((float)TILEMU);

  // Retrieve muons from TileMuFeatureContainer

  const DataHandle<TileMuFeatureContainer> TileMuFeat;
  const DataHandle<TileMuFeatureContainer> TileMuFeatEnd;

  StatusCode sc_TileMu = m_storeGate->retrieve( TileMuFeat, TileMuFeatEnd );
  if( sc_TileMu.isFailure() ){
    ATH_MSG_VERBOSE( "Failed to retrieve HLT TileMu" );
  } else {
    ATH_MSG_DEBUG( " ====== START HLTMuon TileMu MonTool ====== " );
  } 

  // Loop over TileMuFeature objects

  float NTileMu = 0.;

  for( ; TileMuFeat != TileMuFeatEnd ; ++TileMuFeat ) {

    TileMuFeatureContainer::const_iterator TileMuItr = TileMuFeat->begin();
    TileMuFeatureContainer::const_iterator TileMuItrE = TileMuFeat->end();

    for(;TileMuItr != TileMuItrE; ++TileMuItr){

      // Extract the variables and fill the histograms
      float eta = (*TileMuItr)->eta();
      float phi = (*TileMuItr)->phi();
      float Ene = (*TileMuItr)->enedep().at(3);
      
      hist("TileMu_Eta", histdirtilemu)->Fill(eta);
      hist("TileMu_Phi", histdirtilemu)->Fill(phi); 
      hist2("TileMu_Eta_Phi", histdirtilemu)->Fill(eta, phi);
      hist2("TileMu_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta, phi);
      hist("TileMu_Ene", histdirtilemu)->Fill(Ene/1000.); 

      NTileMu++;

    }

  }

  hist("TileMu_N", histdirtilemu)->Fill(NTileMu);
  if(NTileMu) hist("Common_Counter", histdir)->Fill((float)TILEMUFOUND);

  // Retrieve muons from TileTrackMuFeatureContainer

  const DataHandle< TileTrackMuFeatureContainer > TileTrackMu;
  const DataHandle< TileTrackMuFeatureContainer > lastTileTrackMu;

  StatusCode sc = m_storeGate->retrieve(TileTrackMu, lastTileTrackMu);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE( "Failed to retrieve HLT TileTrackMu" );
  } else {
    ATH_MSG_DEBUG( " ====== START HLTMuon TileTrackMu MonTool ====== " );
  }

  // Loop over TileTrackMuFeature objects

  float NTileTrackMu = 0.;

  for( ; TileTrackMu != lastTileTrackMu ; ++TileTrackMu ) {  

    TileTrackMuFeatureContainer::const_iterator TileTrackMuItr = TileTrackMu->begin();
    TileTrackMuFeatureContainer::const_iterator TileTrackMuItrE= TileTrackMu->end();

    for( ; TileTrackMuItr != TileTrackMuItrE; ++TileTrackMuItr ) {        

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

      hist("TileMu_absPtTrk", histdirtilemu)->Fill( absPtTrk );
      hist("TileMu_DelEtaTrkTR", histdirtilemu)->Fill( DelEtaTR );
      hist("TileMu_DelPhiTrkTR", histdirtilemu)->Fill( DelPhiTR );
 
      NTileTrackMu++;

    } 

  }

  if (NTileMu > 0.) {
    hist("TileMu_EffwTrk", histdirtilemu)->Fill( NTileTrackMu/NTileMu );
    hist("Common_Counter", histdir)->Fill((float)TILEMUTRACKFOUND);
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

  sc = m_storeGate->retrieve(muonCont, muonKey);
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
	eta_RecCB.push_back( (*muonItr)->eta() );
	phi_RecCB.push_back( (*muonItr)->phi() );
	pt_RecCB.push_back( (*muonItr)->pt()/CLHEP::GeV );
	hist("Rec_Eta", histdirtilemu)->Fill( (*muonItr)->eta() );
	hist("Rec_Phi", histdirtilemu)->Fill( (*muonItr)->phi() );
	hist("Rec_Pt", histdirtilemu)->Fill( (*muonItr)->pt()/CLHEP::GeV );
	isCombined++;
      }
    }
  }

  // TileMuFeature (TileMuId SA)
  NTileMu = 0.;
  sc_TileMu = m_storeGate->retrieve( TileMuFeat, TileMuFeatEnd );
  if( sc_TileMu.isFailure() ){
    ATH_MSG_WARNING( "Failed to retrieve HLT TileMu" );
    return StatusCode::SUCCESS;
  }
  for( ; TileMuFeat != TileMuFeatEnd ; ++TileMuFeat ) {
    TileMuFeatureContainer::const_iterator TileMuItr = TileMuFeat->begin();
    TileMuFeatureContainer::const_iterator TileMuItrE = TileMuFeat->end();
    for(;TileMuItr != TileMuItrE; ++TileMuItr){
      eta_Tile.push_back( (*TileMuItr)->eta() );
      phi_Tile.push_back( (*TileMuItr)->phi() );
      NTileMu++;
    } 
  }

  // Correlation TileMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileMu;j++) {
      hist2("TileMu_RecCBMuon_EtaCor", histdirtilemu)->Fill(eta_RecCB.at(i),eta_Tile.at(j));
      hist2("TileMu_RecCBMuon_PhiCor", histdirtilemu)->Fill(phi_RecCB.at(i),phi_Tile.at(j));
      hist("TileMu_RecCBMuon_dEta", histdirtilemu)->Fill(eta_Tile.at(j)-eta_RecCB.at(i));
      hist("TileMu_RecCBMuon_dPhi", histdirtilemu)->Fill(phi_Tile.at(j)-phi_RecCB.at(i));
    }
  }

  // Matching TileMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileMu;j++) {
      muPhiTR = phi_RecCB.at(i) - (1.23053e-04*(pt_RecCB.at(i)/fabs(pt_RecCB.at(i))) + 5.06985e-01/(pt_RecCB.at(i)));
      DeltaEta = eta_Tile.at(j) - eta_RecCB.at(i);
      DeltaPhi = phi_Tile.at(j) - muPhiTR;
      if (DeltaPhi >  User_Pi) DeltaPhi -= 2*User_Pi;
      if (DeltaPhi < -User_Pi) DeltaPhi += 2*User_Pi;
      if ( (fabs(DeltaEta) <= DeltaEta_cut)  &&  (fabs(DeltaPhi) <= DeltaPhi_cut) ) {
	hist("Rec_Eta_Num", histdirtilemu)->Fill( eta_RecCB.at(i) );
	hist("Rec_Phi_Num", histdirtilemu)->Fill( phi_RecCB.at(i) );
      }
    }
  }

  // TileTrackMuFeature (TileMuId combined with ID)
  NTileTrackMu = 0.;
  sc = m_storeGate->retrieve(TileTrackMu, lastTileTrackMu);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Failed to retrieve HLT TileTrackMu" );
    return StatusCode::SUCCESS;
  }
  for( ; TileTrackMu != lastTileTrackMu ; ++TileTrackMu ) {  
    TileTrackMuFeatureContainer::const_iterator TileTrackMuItr = TileTrackMu->begin();
    TileTrackMuFeatureContainer::const_iterator TileTrackMuItrE= TileTrackMu->end();
    for( ; TileTrackMuItr != TileTrackMuItrE; ++TileTrackMuItr ) {        
      eta_TileTrack.push_back( (*TileTrackMuItr)->EtaTR_Trk() );
      phi_TileTrack.push_back( (*TileTrackMuItr)->PhiTR_Trk() );
      pt_TileTrack.push_back( (*TileTrackMuItr)->PtTR_Trk() );
      NTileTrackMu++;
    } 
  }

  // Correlation TileTrackMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileTrackMu;j++) {
      hist2("TileTrackMu_RecCBMuon_EtaCor", histdirtilemu)->Fill(eta_RecCB.at(i),eta_TileTrack.at(j));
      hist2("TileTrackMu_RecCBMuon_PhiCor", histdirtilemu)->Fill(phi_RecCB.at(i),phi_TileTrack.at(j));
      hist2("TileTrackMu_RecCBMuon_PtCor", histdirtilemu)->Fill(pt_RecCB.at(i),pt_TileTrack.at(j));
      hist("TileTrackMu_RecCBMuon_dEta", histdirtilemu)->Fill(eta_TileTrack.at(j)-eta_RecCB.at(i));
      hist("TileTrackMu_RecCBMuon_dPhi", histdirtilemu)->Fill(phi_TileTrack.at(j)-phi_RecCB.at(i));
      hist("TileTrackMu_RecCBMuon_dPt", histdirtilemu)->Fill(pt_TileTrack.at(j)-pt_RecCB.at(i));
    }
  }

  // Matching TileTrackMuFeature-Offline
  for(int i=0;i<isCombined;i++) {
    for(int j=0;j<NTileTrackMu;j++) {
      muPhiTR = phi_RecCB.at(i) - (1.23053e-04*(pt_RecCB.at(i)/fabs(pt_RecCB.at(i))) + 5.06985e-01/(pt_RecCB.at(i)));
      DeltaEta = eta_TileTrack.at(j) - eta_RecCB.at(i);
      DeltaPhi = phi_TileTrack.at(j) - muPhiTR;
      if (DeltaPhi >  User_Pi) DeltaPhi -= 2*User_Pi;
      if (DeltaPhi < -User_Pi) DeltaPhi += 2*User_Pi;
      if ( (fabs(DeltaEta) <= DeltaEta_cut)  &&  (fabs(DeltaPhi) <= DeltaPhi_cut) ) {
	hist("TileTrackMu_Eta", histdirtilemu)->Fill( eta_RecCB.at(i) );
	hist("TileTrackMu_Phi", histdirtilemu)->Fill( phi_RecCB.at(i) );
	hist("TileTrackMu_Pt", histdirtilemu)->Fill( pt_RecCB.at(i) );
      }
    }
  }

  // Clear Vectors

  eta_RecCB.clear();
  phi_RecCB.clear();
  pt_RecCB.clear();

  eta_Tile.clear();
  phi_Tile.clear();

  eta_TileTrack.clear();
  phi_TileTrack.clear();
  pt_TileTrack.clear();

  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::procTileMuDQA()
{
  if( endOfRun ){

    // TileMuFeature efficiency on eta
    hist("TileMu_RecCBMuon_EffEta", histdireff)->Sumw2();
    hist("TileMu_RecCBMuon_EffEta", histdireff)->Divide( hist("Rec_Eta_Num", histdirtilemu), hist("Rec_Eta", histdirtilemu), 1, 1, "B" );

    // TileMuFeature efficiency on phi
    hist("TileMu_RecCBMuon_EffPhi", histdireff)->Sumw2();
    hist("TileMu_RecCBMuon_EffPhi", histdireff)->Divide( hist("Rec_Phi_Num", histdirtilemu), hist("Rec_Phi", histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on eta
    hist("TileTrackMu_RecCBMuon_EffEta", histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffEta", histdireff)->Divide( hist("TileTrackMu_Eta", histdirtilemu), hist("Rec_Eta", histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on phi
    hist("TileTrackMu_RecCBMuon_EffPhi", histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffPhi", histdireff)->Divide( hist("TileTrackMu_Phi", histdirtilemu), hist("Rec_Phi", histdirtilemu), 1, 1, "B" );

    // TileTrackMuFeature efficiency on pt
    hist("TileTrackMu_RecCBMuon_EffPt", histdireff)->Sumw2();
    hist("TileTrackMu_RecCBMuon_EffPt", histdireff)->Divide( hist("TileTrackMu_Pt", histdirtilemu), hist("Rec_Pt", histdirtilemu), 1, 1, "B" );

    //removeHistogram("TileMu_Eta", histdir);
    //removeHistogram("TileMu_Phi", histdir);

    //removeHistogram("TileTrackMu_Eta", histdir);
    //removeHistogram("TileTrackMu_Phi", histdir);
    //removeHistogram("TileTrackMu_Pt", histdir);

    //removeHistogram("Rec_Eta", histdir);
    //removeHistogram("Rec_Phi", histdir);
    //removeHistogram("Rec_Pt", histdir);

  }else if( endOfLumiBlock ){
  }
  return StatusCode::SUCCESS;
}
