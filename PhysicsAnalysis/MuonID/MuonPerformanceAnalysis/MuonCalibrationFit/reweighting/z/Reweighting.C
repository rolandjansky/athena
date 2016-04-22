/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define Reweighting_cxx
#include "Reweighting.h"

void Reweighting::Begin( TTree * /*tree*/ ) {
  TString option = GetOption();
  m_Type = 0;
  if( option.Contains( "data" ) ) m_Type = 1;
  m_Output = new TFile( option.Data(), "recreate" );
  m_Tree = new TTree( "z_rew_tree", "" );
  //:::
  m_Tree->Branch( "Type", &m_Type );
  m_Tree->Branch( "Weight", &m_Weight );
  m_Tree->Branch( "Lead_Pt", &m_Lead_Pt );
  m_Tree->Branch( "Lead_Eta", &m_Lead_Eta );
  m_Tree->Branch( "Lead_Phi", &m_Lead_Phi );
  m_Tree->Branch( "Sub_Pt", &m_Sub_Pt );
  m_Tree->Branch( "Sub_Eta", &m_Sub_Eta );
  m_Tree->Branch( "Sub_Phi", &m_Sub_Phi );
  m_Tree->Branch( "Pair_Pt", &m_Pair_Pt );
  m_Tree->Branch( "Pair_Eta", &m_Pair_Eta );
  m_Tree->Branch( "Pair_Phi", &m_Pair_Phi );
  m_Tree->Branch( "Pair_Mass", &m_Pair_Mass );
  m_Tree->Branch( "Pair_y", &m_Pair_y );
  m_Tree->Branch( "Pair_Theta", &m_Pair_Theta );
  m_Tree->Branch( "Pair_DeltaR", &m_Pair_DeltaR );
  m_Tree->Branch( "Pair_DeltaPhi", &m_Pair_DeltaPhi );
  m_Tree->Branch( "Pair_CosTheta", &m_Pair_CosTheta );
  //:::
  m_Input = new TFile( "hists.root" );
  m_RewHistPt = ( TH2F* ) m_Input->Get( "Ratio2d" );
  m_RewHistPtDummy = ( TH2F* ) m_RewHistPt->Clone( "Ratio2dDummy" );
}

void Reweighting::SlaveBegin( TTree * /*tree*/ ) {
  TString option = GetOption();
}

Bool_t Reweighting::Process( Long64_t entry ) {
  //:::
  b_EvtNumber->GetEntry( entry );
  b_EventWeight->GetEntry( entry );
  b_Pos_Author->GetEntry( entry );
  b_Pos_ID_Pt->GetEntry( entry );
  b_Pos_ME_Pt->GetEntry( entry );
  b_Pos_Eta->GetEntry( entry );
  b_Pos_Phi->GetEntry( entry );
  b_Pos_Charge->GetEntry( entry );
  b_Pos_PtCone20->GetEntry( entry );
  b_Neg_Author->GetEntry( entry );
  b_Neg_ID_Pt->GetEntry( entry );
  b_Neg_ME_Pt->GetEntry( entry );
  b_Neg_Eta->GetEntry( entry );
  b_Neg_Phi->GetEntry( entry );
  b_Neg_Charge->GetEntry( entry );
  b_Neg_PtCone20->GetEntry( entry );
  //::: First check: eta acceptance!
  if( TMath::Abs( Pos_Eta ) > 2.5 || TMath::Abs( Neg_Eta ) > 2.5 ) return kTRUE;
  //::: Second check: oppositely charged!
  if( Pos_Charge * Neg_Charge > 0 ) return kTRUE;
  //::: Third check: isolated!
  if( ( Pos_PtCone20 / Pos_ID_Pt > 0.15 ) || ( Neg_PtCone20 / Neg_ID_Pt > 0.15 ) ) return kTRUE;
  //::: Fourth check: quality!
  if( ( Pos_Author != 1 ) || ( Neg_Author != 1 ) ) return kTRUE;
  //::: 
  if( ( Pos_ID_Pt < 8 ) || ( Neg_ID_Pt < 8 ) ) return kTRUE;
  //:::
  if( Pos_ID_Pt > Neg_ID_Pt ) {
    m_Lead_Pt = Pos_ID_Pt; 
    m_Lead_Eta = Pos_Eta;
    m_Lead_Phi = Pos_Phi;
    m_Sub_Pt = Neg_ID_Pt;
    m_Sub_Eta = Neg_Eta;
    m_Sub_Phi = Neg_Phi;
  }
  else {
    m_Lead_Pt = Neg_ID_Pt; 
    m_Lead_Eta = Neg_Eta;
    m_Lead_Phi = Neg_Phi;
    m_Sub_Pt = Pos_ID_Pt;
    m_Sub_Eta = Pos_Eta;
    m_Sub_Phi = Pos_Phi;
  }
  TLorentzVector Lead, Sub;
  Lead.SetPtEtaPhiM( m_Lead_Pt, m_Lead_Eta, m_Lead_Phi, MuonMass );
  Sub.SetPtEtaPhiM( m_Sub_Pt, m_Sub_Eta, m_Sub_Phi, MuonMass );
  m_Pair_Pt = ( Lead + Sub ).Pt();
  m_Pair_Eta = ( Lead + Sub ).Eta();
  m_Pair_Phi = ( Lead + Sub ).Phi();
  m_Pair_Mass = ( Lead + Sub ).M();
  m_Pair_y = ( Lead + Sub ).Rapidity();
  m_Pair_Theta = Lead.Angle( Sub.Vect() );
  m_Pair_DeltaR = Lead.DeltaR( Sub );
  m_Pair_DeltaPhi = Lead.DeltaPhi( Sub );
  m_Pair_CosTheta = TMath::Cos( m_Pair_Theta );
  m_Weight = GetWeight( m_Pair_Pt, m_Pair_y );
  //:::
  //::: Fifth check: Jpsi Mass cut!
  if( m_Pair_Mass > 66 && m_Pair_Mass < 116 ) m_Tree->Fill();
  //:::
  return kTRUE;
}
void Reweighting::SlaveTerminate() {
}

void Reweighting::Terminate() {
  m_Output->Write();
  m_Output->Close();
}

Float_t Reweighting::GetWeight( Float_t varX, Float_t varY ) {
  return m_RewHistPt->GetBinContent( m_RewHistPtDummy->Fill( varX, varY ) );
}
