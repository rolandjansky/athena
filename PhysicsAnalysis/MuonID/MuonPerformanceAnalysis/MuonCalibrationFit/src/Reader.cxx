/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define Reader_cxx
#include "MuonCalibrationFit/Reader.h"

void Reader::Begin( TTree * /*tree*/ ) {
  TString option = GetOption();
  m_entry = 0;
  m_random = new TRandom3( 2988100123 );
}

void Reader::BeginFirstEvent() {
  TString type[ 3 ] = { "       Filling Data", "Filling Monte Carlo", " Filling Background" };
  int colors[ 3 ] = { ProgressBar::Red, ProgressBar::Blue, ProgressBar::Yellow };
  if( m_usePB ) m_prog = new ProgressBar( type[ m_type ] + " vectors", m_N_events, colors[ m_type ], 40 ); 
  //std::cout << std::endl;
}

void Reader::SlaveBegin( TTree * /*tree*/ ) {
  TString option = GetOption();
}

Bool_t Reader::Process( Long64_t entry ) {
  if( m_entry == 0 ) BeginFirstEvent();
  if( m_usePB ) m_prog->Update( m_entry++ ); 
  //:::
  b_EvtNumber->GetEntry( entry );
  b_EventWeight->GetEntry( entry );
  b_Pos_ID_Pt->GetEntry( entry );
  b_Pos_MS_Pt->GetEntry( entry );
  b_Pos_Eta->GetEntry( entry );
  b_Pos_Phi->GetEntry( entry );
  b_Pos_Charge->GetEntry( entry );
  b_Neg_ID_Pt->GetEntry( entry );
  b_Neg_MS_Pt->GetEntry( entry );
  b_Neg_Eta->GetEntry( entry );
  b_Neg_Phi->GetEntry( entry );
  b_Neg_Charge->GetEntry( entry );
  //:::
  //::: First check: eta acceptance
  if( TMath::Abs( Pos_Eta ) > 2.5 || TMath::Abs( Neg_Eta ) > 2.5 ) return kTRUE;
  //:::
  Bool_t isPosMuInROF = m_RegInfo->HasMuon( Pos_Eta, Pos_Phi );
  Bool_t isNegMuInROF = m_RegInfo->HasMuon( Neg_Eta, Neg_Phi );
  Bool_t isPosInCorrectedROF = kFALSE;
  Bool_t isNegInCorrectedROF = kFALSE;
  //:::
  //::: Main ones can be either ID or MS and then adjoint are None or ID respectively
  Float_t MainPosPt, AdjointPosPt, MainNegPt, AdjointNegPt;
  if( m_detector ) {
    MainPosPt = Pos_ID_Pt;
    AdjointPosPt = 0.0000001; // very small value, to make sure it doesn't get used anywhere
    MainNegPt = Neg_ID_Pt;
    AdjointNegPt = 0.0000001; // very small value, to make sure it doesn't get used anywhere
  }
  else {
    for( std::vector< Tools::RegionInfo* >::iterator It = m_IDRegInfo.begin(); It != m_IDRegInfo.end(); ++It ) {
      if( ( *It )->HasMuon( Pos_Eta, Pos_Phi ) ) {
        m_IDRegInfoForPos = *It;
      }
      if( ( *It )->HasMuon( Neg_Eta, Neg_Phi ) ) {
        m_IDRegInfoForNeg = *It;
      }
    }
    MainPosPt = Pos_MS_Pt;
    AdjointPosPt = Correct( Pos_ID_Pt, m_random->Gaus( 0, 1 ), m_IDRegInfoForPos );
    MainNegPt = Neg_MS_Pt;
    AdjointNegPt = Correct( Neg_ID_Pt, m_random->Gaus( 0, 1 ), m_IDRegInfoForNeg );
  }
  //:::
  for( std::vector< Tools::RegionInfo* >::iterator It = m_AllRegInfo.begin(); It != m_AllRegInfo.end(); ++It ) {
    if( ( *It )->Corrected && ( *It )->HasMuon( Pos_Eta, Pos_Phi ) ) {
      isPosInCorrectedROF = kTRUE;
      m_RegInfoForPos = *It;
    }
    if( ( *It )->Corrected && ( *It )->HasMuon( Neg_Eta, Neg_Phi ) ) {
      isNegInCorrectedROF = kTRUE;
      m_RegInfoForNeg = *It;
    }
  }
  bool use_only_both = ( !isPosInCorrectedROF && !isNegInCorrectedROF );
  //::://::://:::
  //::: Remember!!! First filled muon is the one that should be corrected!!!
  //::://::://:::
  //::: Filling Data vector
  if( m_type == Type::Data ) {
    if( use_only_both ) {
      if( isPosMuInROF && isNegMuInROF ) {
        Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      }
    }
    else if( isPosMuInROF && isNegMuInROF ) {
      Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
    else if( isPosMuInROF ) { 
      Add( m_SingleInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
    else if( isNegMuInROF ) {
      Add( m_SingleInfo, Tools::Info( MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
  }
  //::: Filling Monte Carlo vector
  else {
    if( use_only_both ) {
      if( isPosMuInROF && isNegMuInROF ) { 
        Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      }
    }
    else if( isPosMuInROF && isNegMuInROF ) { 
      Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
    else if( isPosMuInROF && isNegInCorrectedROF ) {
      Add( m_SingleInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, Correct( MainNegPt, m_random->Gaus( 0, 1 ), m_RegInfoForNeg ), AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
    else if( isNegMuInROF && isPosInCorrectedROF ) {
      Add( m_SingleInfo, Tools::Info( MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, Correct( MainPosPt, m_random->Gaus( 0, 1 ), m_RegInfoForPos ), AdjointPosPt, Pos_Eta, Pos_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
    }
  }
  //:::
  return kTRUE;
}

Float_t Reader::Correct( Float_t pt, Float_t r0, Tools::RegionInfo* rI ) {
  return rI->Correct( pt, r0 );
}

void Reader::Add( std::vector< Tools::Info >* info_vec, Tools::Info info ) {
  info_vec->push_back( info );
}

void Reader::SlaveTerminate() {
}

void Reader::Terminate() {
}
