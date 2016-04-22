/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define Reader_cxx
#include "MuonCalibrationFit/Reader.h"

void Reader::Begin( TTree * /*tree*/ ) {
  TString option = GetOption();
  m_entry = 0;
  m_random = new TRandom3( 2988100123 );
  m_GRL = NULL;
  if( m_GRL_Name != "" ) m_GRL = new GRLReader( m_GRL_Name.Data() );
  if( m_JpsiRewFileName != "" ) {
    m_JpsiRewFile = new TFile( m_JpsiRewFileName ); 
    m_JpsiRewHist = ( TH1F* ) m_JpsiRewFile->Get( "RatioJpsiEta" );
    m_JpsiRewHistDummy = ( TH1F* ) m_JpsiRewHist->Clone( "RatioJpsiEtaDummy" );
    std::cout << "Using " << m_JpsiRewFileName << " for J/psi eta reweighting" << endl;
    //m_JpsiRewHist = ( TH2F* ) m_JpsiRewFile->Get( "Ratio2d" );
    //m_JpsiRewHistDummy = ( TH2F* ) m_JpsiRewHist->Clone( "JpsiRewHistDummy" );
  }
  else {
    m_JpsiRewFile = NULL; 
    m_JpsiRewHist = NULL; 
    m_JpsiRewHistDummy = NULL; 
  }
  if( m_ZRewFileName != "" ) {
    m_ZRewFile = new TFile( m_ZRewFileName ); 
    m_ZRewHist = ( TH1F* ) m_ZRewFile->Get( "RatioZEta" );
    m_ZRewHistDummy = ( TH1F* ) m_ZRewHist->Clone( "RatioZEtaDummy" );
    std::cout << "Using " << m_ZRewFileName << " for Z eta reweighting" << endl;
    //m_ZRewHist = ( TH2F* ) m_ZRewFile->Get( "Ratio2d" );
    //m_ZRewHistDummy = ( TH2F* ) m_ZRewHist->Clone( "ZRewHistDummy" );
  }
  else {
    m_ZRewFile = NULL; 
    m_ZRewHist = NULL; 
    m_ZRewHistDummy = NULL; 
  }
  if( m_PreviousCorrectionsForAll ) std::cout << "Applying previously found corrections to both muons!!!!!" << std::endl;
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
  m_Print = false;
  //:::
  b_RunNumber->GetEntry( entry );
  b_EvtNumber->GetEntry( entry );
  b_LumiBlock->GetEntry( entry );
  b_ChannelNumber->GetEntry( entry );
  b_EventWeight->GetEntry( entry );
  b_Trig_HLT_mu20_iloose_L1MU15->GetEntry( entry );
  b_Trig_HLT_mu26_imedium->GetEntry( entry );
  b_Trig_HLT_mu50->GetEntry( entry );
  b_Trig_HLT_2mu14->GetEntry( entry );
  b_Trig_HLT_2mu4_bJpsimumu->GetEntry( entry );
  b_Pos_Quality->GetEntry( entry );
  b_Pos_Author->GetEntry( entry );
  b_Pos_ID_Pt->GetEntry( entry );
  b_Pos_ME_Pt->GetEntry( entry );
  b_Pos_Eta->GetEntry( entry );
  b_Pos_Phi->GetEntry( entry );
  b_Pos_Charge->GetEntry( entry );
  b_Pos_d0sig->GetEntry( entry );
  b_Pos_z0->GetEntry( entry );
  b_Pos_PtCone20->GetEntry( entry );
  b_Pos_PrimarySector->GetEntry( entry );
  b_Neg_Quality->GetEntry( entry );
  b_Neg_Author->GetEntry( entry );
  b_Neg_ID_Pt->GetEntry( entry );
  b_Neg_ME_Pt->GetEntry( entry );
  b_Neg_Eta->GetEntry( entry );
  b_Neg_Phi->GetEntry( entry );
  b_Neg_Charge->GetEntry( entry );
  b_Neg_d0sig->GetEntry( entry );
  b_Neg_z0->GetEntry( entry );
  b_Neg_PtCone20->GetEntry( entry );
  b_Neg_PrimarySector->GetEntry( entry );
  //:::
  //if( RunNumber == 282455 && EvtNumber == 39142220 && LumiBlock == 264 ) m_Print = true; 
  //if( RunNumber == 282455 && EvtNumber == 288287664 && LumiBlock == 380 ) m_Print = true; 
  //if( RunNumber == 282455 && EvtNumber == 295407842 && LumiBlock == 382 ) m_Print = true; 
  //if( RunNumber == 282455 && EvtNumber == 63775689 && LumiBlock == 285 ) m_Print = true; 
  //if( RunNumber == 282455 && EvtNumber == 73032643 && LumiBlock == 288 ) m_Print = true; 
  if( RunNumber == 222525 && EvtNumber == 1544265 && LumiBlock == 29 ) m_Print = true; 
  if( RunNumber == 222525 && EvtNumber == 1543666 && LumiBlock == 14 ) m_Print = true; 
  if( RunNumber == 222525 && EvtNumber == 1543353 && LumiBlock == 15 ) m_Print = true; 
  if( m_Print ) {
    std::cout << "event found" << std::endl;
  }
  //::: Do not use EventWeight!!!!!!
  EventWeight = Tools::GetWeight( ChannelNumber );
  //:::
  TLorentzVector Pair, Pos, Neg;
  Pos.SetPtEtaPhiM( Pos_ID_Pt, Pos_Eta, Pos_Phi, MuonMass );
  Neg.SetPtEtaPhiM( Neg_ID_Pt, Neg_Eta, Neg_Phi, MuonMass );
  Pair = Pos + Neg;
  Float_t TempPairMass = Pair.M();
  Float_t TempPairPt = Pair.Pt();
  Float_t TempPairY = Pair.Rapidity();
  Float_t Pos_Pt = Pos_ME_Pt; 
  Float_t Neg_Pt = Neg_ME_Pt; 
  if( m_detector ) {
    Pos_Pt = Pos_ID_Pt; 
    Neg_Pt = Neg_ID_Pt; 
  }
  //::: Apply GRL 
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  if( m_GRL != NULL && ChannelNumber == 0 && ! m_GRL->PassGRL( RunNumber, LumiBlock ) ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //::: Eta acceptance!
  if( TMath::Abs( Pos_Eta ) > 2.5 || TMath::Abs( Neg_Eta ) > 2.5 ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //::: d0 significance!
  if( fabs( Pos_d0sig ) > 3 || fabs( Neg_d0sig ) > 3 ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //::: z0!
  if( fabs( Pos_z0 * TMath::Sin( Pos.Theta() ) ) > 0.5 || fabs( Neg_z0 * TMath::Sin( Neg.Theta() ) ) > 0.5 ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //::: Opposite charge!
  if( Pos_Charge * Neg_Charge > 0 ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //::: Quality!
  if( ( Pos_Quality > 2 ) || ( Neg_Quality > 2 ) ) return kTRUE;
  if( m_Print ) std::cout << RunNumber << "  " << EvtNumber << "  " << LumiBlock << std::endl;
  //if( ( Pos_Author != 1 ) || ( Neg_Author != 1 ) ) return kTRUE;
  //::: Pt, Trigger and Isolation!
  if( TempPairMass < 5 ) {
    if( !Trig_HLT_2mu4_bJpsimumu ) return kTRUE;
    if( Pos_Pt < m_MinMuonsPtForJpsi || Neg_Pt < m_MinMuonsPtForJpsi ) return kTRUE;
    if( Pos_Pt > m_MaxMuonsPtForJpsi || Neg_Pt > m_MaxMuonsPtForJpsi ) return kTRUE;
  }
  else { 
    if( !( Trig_HLT_mu20_iloose_L1MU15 || Trig_HLT_mu50 ) ) return kTRUE;
    if( ( Pos_PtCone20 / Pos_ID_Pt > 0.15 ) || ( Neg_PtCone20 / Neg_ID_Pt > 0.15 ) ) return kTRUE;
    Float_t LeadPt, SubPt;
    if( Pos_Pt > Neg_Pt ) {
      LeadPt = Pos_Pt;
      SubPt = Neg_Pt;
    }
    else { 
      LeadPt = Neg_Pt;
      SubPt = Pos_Pt;
    }
    if( LeadPt < m_MinLeadingMuonPtForZ || SubPt < m_MinSubLeadingMuonPtForZ ) return kTRUE;
  }
  if( m_Print ) {
    std::cout << "mass: " << TempPairMass << std::endl;
    std::cout << "pt1: " << Pos_Pt << " eta1: " << Pos_Eta << " phi1: " << Pos_Phi << std::endl;
    std::cout << "pt1: " << Neg_Pt << " eta1: " << Neg_Eta << " phi1: " << Neg_Phi << std::endl;
    std::cout << std::endl;
  }
  //:::
  Float_t Pos_Add( 1. ), Neg_Add( 1. );
  if( TMath::Abs( Pos_Eta ) > 2. ) Pos_Add = TMath::SinH( Pos_Eta );
  if( TMath::Abs( Neg_Eta ) > 2. ) Neg_Add = TMath::SinH( Neg_Eta );
  //:::
  Bool_t isPosMuInROF = m_RegInfo->HasMuon( Pos_Eta, Pos_Phi, Pos_PrimarySector );
  Bool_t isNegMuInROF = m_RegInfo->HasMuon( Neg_Eta, Neg_Phi, Neg_PrimarySector );
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
      if( ( *It )->HasMuon( Pos_Eta, Pos_Phi, Pos_PrimarySector ) ) {
        m_IDRegInfoForPos = *It;
      }
      if( ( *It )->HasMuon( Neg_Eta, Neg_Phi, Neg_PrimarySector ) ) {
        m_IDRegInfoForNeg = *It;
      }
    }
    MainPosPt = Pos_ME_Pt;
    AdjointPosPt = Correct( Pos_ID_Pt, m_random->Gaus( 0, 1 ), Pos_Add, m_IDRegInfoForPos );
    MainNegPt = Neg_ME_Pt;
    AdjointNegPt = Correct( Neg_ID_Pt, m_random->Gaus( 0, 1 ), Neg_Add, m_IDRegInfoForNeg );
    Pos_Add = 1.;
    Neg_Add = 1.;
  }
  //:::
  for( std::vector< Tools::RegionInfo* >::iterator It = m_AllRegInfo.begin(); It != m_AllRegInfo.end(); ++It ) {
    if( ( *It )->Corrected && ( *It )->HasMuon( Pos_Eta, Pos_Phi, Pos_PrimarySector ) ) {
      isPosInCorrectedROF = kTRUE;
      m_RegInfoForPos = *It;
      // m_RegInfoForPos->Print();
    }
    if( ( *It )->Corrected && ( *It )->HasMuon( Neg_Eta, Neg_Phi, Neg_PrimarySector ) ) {
      isNegInCorrectedROF = kTRUE;
      m_RegInfoForNeg = *It;
      // m_RegInfoForNeg->Print();
    }
  }
  bool use_only_both = ( !isPosInCorrectedROF && !isNegInCorrectedROF );
  bool both_corrected = ( isPosInCorrectedROF && isNegInCorrectedROF );

  //::: Fill Counters
  int key = ( m_type == Type::Data ) ? RunNumber : ChannelNumber;
  if( TempPairMass > 2.5 && TempPairMass < 4 ) FillCounter( key, m_Jpsi_Counter );
  else if( TempPairMass > 66 && TempPairMass < 116 ) FillCounter( key, m_Z_Counter );

  //::://::://:::
  //::: Remember!!! First filled muon is the one that should be corrected!!!
  //::://::://:::

  //::: Filling Data vector
  if( m_type == Type::Data ) {
    if( use_only_both ) {
      if( isPosMuInROF && isNegMuInROF ) {
        Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
        // std::cout << "data 1" << std::endl;
      }
    }
    else if( isPosMuInROF && isNegMuInROF ) {
      Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "data 2" << std::endl;
    }
    else if( isPosMuInROF && isNegInCorrectedROF ) { 
      Add( m_SingleInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "data 3" << std::endl;
    }
    else if( isNegMuInROF && isPosInCorrectedROF ) {
      Add( m_SingleInfo, Tools::Info( MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, EventWeight, EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "data 4" << std::endl;
    }
  }
  //::: Filling Monte Carlo vector
  else {
    if( both_corrected && m_PreviousCorrectionsForAll /*&& ( m_RegInfoForPos == m_RegInfoForNeg )*/ ) {
      if( isPosMuInROF && isNegMuInROF ) {
        Add( m_BothInfo, Tools::Info( Correct( MainPosPt, m_random->Gaus( 0, 1 ), Pos_Add, m_RegInfoForPos ), AdjointPosPt, Pos_Eta, Pos_Phi, Correct( MainNegPt, m_random->Gaus( 0, 1 ), Neg_Add, m_RegInfoForNeg ), AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
        // std::cout << "mc 1" << std::endl;
      }
      else if( isPosMuInROF ) { 
        Add( m_SingleInfo, Tools::Info( Correct( MainPosPt, m_random->Gaus( 0, 1 ), Pos_Add, m_RegInfoForPos ), AdjointPosPt, Pos_Eta, Pos_Phi, Correct( MainNegPt, m_random->Gaus( 0, 1 ), Neg_Add, m_RegInfoForNeg ), AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
        // std::cout << "mc 2" << std::endl;
      }
      else if( isNegMuInROF ) {
        Add( m_SingleInfo, Tools::Info( Correct( MainNegPt, m_random->Gaus( 0, 1 ), Neg_Add, m_RegInfoForNeg ), AdjointNegPt, Neg_Eta, Neg_Phi, Correct( MainPosPt, m_random->Gaus( 0, 1 ), Pos_Add, m_RegInfoForPos ), AdjointPosPt, Pos_Eta, Pos_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
        // std::cout << "mc 3" << std::endl;
      }
    }
    else if( use_only_both ) {
      if( isPosMuInROF && isNegMuInROF ) { 
        Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
        // std::cout << "mc 4" << std::endl;
      }
    }
    else if( isPosMuInROF && isNegMuInROF ) { 
      Add( m_BothInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "mc 5" << std::endl;
    }
    else if( isPosMuInROF && isNegInCorrectedROF ) {
      // std::cout << "Nature(1) ---> " << MainPosPt << " " << MainNegPt << std::endl;
      Add( m_SingleInfo, Tools::Info( MainPosPt, AdjointPosPt, Pos_Eta, Pos_Phi, Correct( MainNegPt, m_random->Gaus( 0, 1 ), Neg_Add, m_RegInfoForNeg ), AdjointNegPt, Neg_Eta, Neg_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "mc 6" << std::endl;
    }
    else if( isNegMuInROF && isPosInCorrectedROF ) {
      // std::cout << "Nature(2) ---> " << MainPosPt << " " << MainNegPt << std::endl;
      Add( m_SingleInfo, Tools::Info( MainNegPt, AdjointNegPt, Neg_Eta, Neg_Phi, Correct( MainPosPt, m_random->Gaus( 0, 1 ), Pos_Add, m_RegInfoForPos ), AdjointPosPt, Pos_Eta, Pos_Phi, EventWeight * GetWeight( TempPairMass, /*TempPairPt, TempPairY*/Pos_Eta, Neg_Eta ), EvtNumber, ( Pos_Charge * Neg_Charge < 0 ) ) ); 
      // std::cout << "mc 7" << std::endl;
    }
  }
  //:::
  return kTRUE;
}

Float_t Reader::Correct( Float_t pt, Float_t r0, Float_t add, Tools::RegionInfo* rI ) {
  float val = rI->Correct( pt, r0, add );
  //if( m_type == Type::MC ) std::cout << " Bef: " << pt << ", Aft: " << val << " " << rI->GetInfo() << std::endl;
  return val;
}

void Reader::Add( std::vector< Tools::Info >* info_vec, Tools::Info info ) {
  // std::cout << "  In Add ---> " << info.First_Mu_pt << " " << info.Second_Mu_pt << std::endl;
  info_vec->push_back( info );
}

Float_t Reader::GetWeight( Float_t pair_mass, Float_t eta1, Float_t eta2 ) { 
  if( ( m_JpsiRewHist == NULL ) && ( m_ZRewHist == NULL ) ) return 1.;
  else if( ( m_JpsiRewHist != NULL ) && pair_mass < 5 ) {
    return m_JpsiRewHist->GetBinContent( m_JpsiRewHistDummy->Fill( eta1 ) ) * m_JpsiRewHist->GetBinContent( m_JpsiRewHistDummy->Fill( eta2 ) ); 
  }
  else if( ( m_ZRewHist != NULL ) && pair_mass > 5 ) {
    return m_ZRewHist->GetBinContent( m_ZRewHistDummy->Fill( eta1 ) ) * m_ZRewHist->GetBinContent( m_ZRewHistDummy->Fill( eta2 ) ); 
  }
  else return 1.;
}
/*
Float_t Reader::GetWeight( Float_t pair_mass, Float_t pair_pt, Float_t pair_y ) { 
  if( ( m_JpsiRewHist == NULL ) && ( m_ZRewHist == NULL ) ) return 1.;
  else if( ( m_JpsiRewHist != NULL ) && pair_mass < 5 ) {
    return m_JpsiRewHist->GetBinContent( m_JpsiRewHistDummy->Fill( pair_pt, pair_y ) ); 
  }
  else if( ( m_ZRewHist != NULL ) && pair_mass > 5 ) {
    return m_ZRewHist->GetBinContent( m_ZRewHistDummy->Fill( pair_pt, pair_y ) ); 
  }
  else return 1.;
}
*/
void Reader::SlaveTerminate() {
}

void Reader::Terminate() {
  PrintCounter( "observed J/psi candidates", m_Jpsi_Counter );
  PrintCounter( "observed Z candidates", m_Z_Counter );
}

void Reader::FillCounter( int Key, std::map< int, int >& Map ) {
  if( Map.find( Key ) == Map.end() ) {
    Map[ Key ] = 0;
  }
  Map[ Key ]++;
}

void Reader::PrintCounter( TString name, std::map< int, int > Map ) {
  std::cout << "Printing Counter Map [ " << name << " ]..." << std::endl;
  for( auto& kv: Map ) { std::cout << kv.first << ": " << kv.second << ","; }
  std::cout << std::endl;
}
