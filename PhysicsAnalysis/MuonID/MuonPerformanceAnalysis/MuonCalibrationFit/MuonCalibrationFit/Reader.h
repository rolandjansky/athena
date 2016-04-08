/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Reader_h
#define Reader_h

#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TStyle.h>

#include "MuonCalibrationFit/ProgressBar.h"
#include "MuonCalibrationFit/Tools.h"

class Reader : public TSelector {
public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  //::: Declaration of leaf types
  UInt_t          RunNumber;
  ULong64_t       EvtNumber;
  UInt_t          LumiBlock;
  UInt_t          ChannelNumber;
  Float_t         EventWeight;
  Float_t         Pos_Pt;
  Float_t         Pos_Eta;
  Float_t         Pos_Phi;
  Float_t         Pos_Charge;
  Float_t         Pos_ID_Pt;
  Float_t         Pos_MS_Pt;
  Float_t         Pos_MS_Eta;
  Float_t         Pos_MS_Phi;
  Int_t           Pos_Quality;
  Float_t         Neg_Pt;
  Float_t         Neg_Eta;
  Float_t         Neg_Phi;
  Float_t         Neg_Charge;
  Float_t         Neg_ID_Pt;
  Float_t         Neg_MS_Pt;
  Float_t         Neg_MS_Eta;
  Float_t         Neg_MS_Phi;
  Int_t           Neg_Quality;
  Float_t         MassID;
  Float_t         MassMS;
  //::: List of branches
  TBranch        *b_RunNumber;   //!
  TBranch        *b_EvtNumber;   //!
  TBranch        *b_LumiBlock;   //!
  TBranch        *b_ChannelNumber;   //!
  TBranch        *b_EventWeight;   //!
  TBranch        *b_Pos_Pt;   //!
  TBranch        *b_Pos_Eta;   //!
  TBranch        *b_Pos_Phi;   //!
  TBranch        *b_Pos_Charge;   //!
  TBranch        *b_Pos_ID_Pt;   //!
  TBranch        *b_Pos_MS_Pt;   //!
  TBranch        *b_Pos_MS_Eta;   //!
  TBranch        *b_Pos_MS_Phi;   //!
  TBranch        *b_Pos_Quality;   //!
  TBranch        *b_Neg_Pt;   //!
  TBranch        *b_Neg_Eta;   //!
  TBranch        *b_Neg_Phi;   //!
  TBranch        *b_Neg_Charge;   //!
  TBranch        *b_Neg_ID_Pt;   //!
  TBranch        *b_Neg_MS_Pt;   //!
  TBranch        *b_Neg_MS_Eta;   //!
  TBranch        *b_Neg_MS_Phi;   //!
  TBranch        *b_Neg_Quality;   //!
  TBranch        *b_MassID;   //!
  TBranch        *b_MassMS;   //!
  //:::
  int          m_type;
  Long_t       m_N_events;
  Bool_t       m_detector;
  Bool_t       m_usePB;
  std::vector< Tools::RegionInfo* > m_AllRegInfo;
  std::vector< Tools::RegionInfo* > m_IDRegInfo;
  Tools::RegionInfo* m_RegInfo;
  Tools::RegionInfo* m_RegInfoForPos;
  Tools::RegionInfo* m_RegInfoForNeg;
  Tools::RegionInfo* m_IDRegInfoForPos;
  Tools::RegionInfo* m_IDRegInfoForNeg;
  std::vector< Tools::Info >* m_SingleInfo; 
  std::vector< Tools::Info >* m_BothInfo; 
  Long_t       m_entry;
  ProgressBar* m_prog;
  TRandom3*    m_random;
  //:::
  Reader( TTree * /*tree*/ = 0 ) : fChain( 0 ) { m_SingleInfo = NULL; m_BothInfo = NULL; }
  Reader( int type, Long_t n, Bool_t detector, Bool_t pb, std::vector< Tools::RegionInfo* > vec_reg, std::vector< Tools::RegionInfo* > vec_reg_ID, Tools::RegionInfo* reg, std::vector< Tools::Info >* vec_info_single, std::vector< Tools::Info >* vec_info_double ) : fChain( 0 ), m_type( type ), m_N_events( n ), m_detector( detector ), m_usePB( pb ), m_AllRegInfo( vec_reg ), m_IDRegInfo( vec_reg_ID ), m_RegInfo( reg ), m_SingleInfo( vec_info_single ), m_BothInfo( vec_info_double ) { }
  virtual ~Reader() { }
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin( TTree *tree );
  virtual void    SlaveBegin( TTree *tree );
  virtual void    Init( TTree *tree );
  virtual Bool_t  Notify();
  virtual Bool_t  Process( Long64_t entry );
  virtual Int_t   GetEntry( Long64_t entry, Int_t getall = 0 ) { return fChain ? fChain->GetTree()->GetEntry( entry, getall ) : 0; }
  virtual void    SetOption( const char *option ) { fOption = option; }
  virtual void    SetObject( TObject *obj ) { fObject = obj; }
  virtual void    SetInputList( TList *input ) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();
  //:::
  void BeginFirstEvent();
  void SetType( int val ) { m_type = val; }
  void SetAllRegionInfo( std::vector< Tools::RegionInfo* > r ) { m_AllRegInfo = r; }
  void SetIDRegionInfo( std::vector< Tools::RegionInfo* > r ) { m_IDRegInfo = r; }
  void SetRegionInfo( Tools::RegionInfo* r ) { m_RegInfo = r; }
  void SetSingleInfo( std::vector< Tools::Info >* v ) { m_SingleInfo = v; }
  void SetBothInfo( std::vector< Tools::Info >* v ) { m_BothInfo = v; }
  void SetNEvents( Long_t n ) { m_N_events = n; }
  void UseProgressBar( Bool_t val ) { m_usePB = val; }
  //:::
  std::vector< Tools::RegionInfo* > GetAllRegionInfo() { return m_AllRegInfo; }
  //:::
  Float_t Correct( Float_t pt, Float_t r0, Tools::RegionInfo* rI );
  //:::
  void Add( std::vector< Tools::Info >* info_vec, Tools::Info info );
  //ClassDef( Reader, 0 );
};
#endif

#ifdef Reader_cxx
void Reader::Init( TTree *tree ) {
  if ( !tree ) return;
  fChain = tree;
  fChain->SetMakeClass( 1 );
  fChain->SetBranchAddress( "RunNumber", &RunNumber, &b_RunNumber );
  fChain->SetBranchAddress( "EvtNumber", &EvtNumber, &b_EvtNumber );
  fChain->SetBranchAddress( "LumiBlock", &LumiBlock, &b_LumiBlock );
  fChain->SetBranchAddress( "ChannelNumber", &ChannelNumber, &b_ChannelNumber );
  fChain->SetBranchAddress( "EventWeight", &EventWeight, &b_EventWeight );
  fChain->SetBranchAddress( "Pos_Pt", &Pos_Pt, &b_Pos_Pt );
  fChain->SetBranchAddress( "Pos_Eta", &Pos_Eta, &b_Pos_Eta );
  fChain->SetBranchAddress( "Pos_Phi", &Pos_Phi, &b_Pos_Phi );
  fChain->SetBranchAddress( "Pos_Charge", &Pos_Charge, &b_Pos_Charge );
  fChain->SetBranchAddress( "Pos_ID_Pt", &Pos_ID_Pt, &b_Pos_ID_Pt );
  fChain->SetBranchAddress( "Pos_MS_Pt", &Pos_MS_Pt, &b_Pos_MS_Pt );
  fChain->SetBranchAddress( "Pos_MS_Eta", &Pos_MS_Eta, &b_Pos_MS_Eta );
  fChain->SetBranchAddress( "Pos_MS_Phi", &Pos_MS_Phi, &b_Pos_MS_Phi );
  fChain->SetBranchAddress( "Pos_Quality", &Pos_Quality, &b_Pos_Quality );
  fChain->SetBranchAddress( "Neg_Pt", &Neg_Pt, &b_Neg_Pt );
  fChain->SetBranchAddress( "Neg_Eta", &Neg_Eta, &b_Neg_Eta );
  fChain->SetBranchAddress( "Neg_Phi", &Neg_Phi, &b_Neg_Phi );
  fChain->SetBranchAddress( "Neg_Charge", &Neg_Charge, &b_Neg_Charge );
  fChain->SetBranchAddress( "Neg_ID_Pt", &Neg_ID_Pt, &b_Neg_ID_Pt );
  fChain->SetBranchAddress( "Neg_MS_Pt", &Neg_MS_Pt, &b_Neg_MS_Pt );
  fChain->SetBranchAddress( "Neg_MS_Eta", &Neg_MS_Eta, &b_Neg_MS_Eta );
  fChain->SetBranchAddress( "Neg_MS_Phi", &Neg_MS_Phi, &b_Neg_MS_Phi );
  fChain->SetBranchAddress( "Neg_Quality", &Neg_Quality, &b_Neg_Quality );
  fChain->SetBranchAddress( "MassID", &MassID, &b_MassID );
  fChain->SetBranchAddress( "MassMS", &MassMS, &b_MassMS );
}

Bool_t Reader::Notify( ) {
  return kTRUE;
}

#endif // #ifdef Reader_cxx
