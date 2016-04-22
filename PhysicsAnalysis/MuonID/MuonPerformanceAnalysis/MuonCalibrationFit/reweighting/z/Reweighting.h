/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Reweighting_h
#define Reweighting_h

#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TStyle.h>

#define MuonMass 0.10565837 

class Reweighting : public TSelector {
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
  Float_t         Pos_ME_Pt;
  Float_t         Pos_ME_Eta;
  Float_t         Pos_ME_Phi;
  Float_t         Pos_PtCone20;
  Int_t           Pos_Quality;
  Int_t           Pos_Author;
  Int_t           Pos_EnergyLossType;
  Int_t           Pos_PrimarySector;
  Int_t           Pos_SecondarySector;
  Float_t         Neg_Pt;
  Float_t         Neg_Eta;
  Float_t         Neg_Phi;
  Float_t         Neg_Charge;
  Float_t         Neg_ID_Pt;
  Float_t         Neg_ME_Pt;
  Float_t         Neg_ME_Eta;
  Float_t         Neg_ME_Phi;
  Float_t         Neg_PtCone20;
  Int_t           Neg_Quality;
  Int_t           Neg_Author;
  Int_t           Neg_EnergyLossType;
  Int_t           Neg_PrimarySector;
  Int_t           Neg_SecondarySector;
  //Float_t         MassID; // No longer there!
  //Float_t         MassMS; // No longer there!
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
  TBranch        *b_Pos_ME_Pt;   //!
  TBranch        *b_Pos_ME_Eta;   //!
  TBranch        *b_Pos_ME_Phi;   //!
  TBranch        *b_Pos_PtCone20;   //!
  TBranch        *b_Pos_Quality;   //!
  TBranch        *b_Pos_Author;   //!
  TBranch        *b_Pos_EnergyLossType;   //!
  TBranch        *b_Pos_PrimarySector;   //!
  TBranch        *b_Pos_SecondarySector;   //!
  TBranch        *b_Neg_Pt;   //!
  TBranch        *b_Neg_Eta;   //!
  TBranch        *b_Neg_Phi;   //!
  TBranch        *b_Neg_Charge;   //!
  TBranch        *b_Neg_ID_Pt;   //!
  TBranch        *b_Neg_ME_Pt;   //!
  TBranch        *b_Neg_ME_Eta;   //!
  TBranch        *b_Neg_ME_Phi;   //!
  TBranch        *b_Neg_PtCone20;   //!
  TBranch        *b_Neg_Quality;   //!
  TBranch        *b_Neg_Author;   //!
  TBranch        *b_Neg_EnergyLossType;   //!
  TBranch        *b_Neg_PrimarySector;   //!
  TBranch        *b_Neg_SecondarySector;   //!
  //TBranch        *b_MassID;   //!
  //TBranch        *b_MassMS;   //!
  //:::
  TFile* m_Input;
  TH2F*  m_RewHistPt;
  TH2F*  m_RewHistPtDummy;
  TFile* m_Output;
  TTree* m_Tree;
  Int_t m_Type;
  Float_t m_Weight;
  Float_t m_Lead_Pt, m_Lead_Eta, m_Lead_Phi;
  Float_t m_Sub_Pt, m_Sub_Eta, m_Sub_Phi;
  Float_t m_Pair_Pt, m_Pair_Eta, m_Pair_Phi, m_Pair_Mass, m_Pair_y, m_Pair_Theta, m_Pair_CosTheta, m_Pair_DeltaR, m_Pair_DeltaPhi;
  //:::
  Reweighting( TTree * /*tree*/ = 0 ) : fChain( 0 ) {}; 
  virtual ~Reweighting() { }
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
  Float_t GetWeight( Float_t varX, Float_t varY );
  //:::
};
#endif

#ifdef Reweighting_cxx
void Reweighting::Init( TTree *tree ) {
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
  fChain->SetBranchAddress( "Pos_ME_Pt", &Pos_ME_Pt, &b_Pos_ME_Pt );
  fChain->SetBranchAddress( "Pos_ME_Eta", &Pos_ME_Eta, &b_Pos_ME_Eta );
  fChain->SetBranchAddress( "Pos_ME_Phi", &Pos_ME_Phi, &b_Pos_ME_Phi );
  fChain->SetBranchAddress( "Pos_PtCone20", &Pos_PtCone20, &b_Pos_PtCone20 );
  fChain->SetBranchAddress( "Pos_Quality", &Pos_Quality, &b_Pos_Quality );
  fChain->SetBranchAddress( "Pos_Author", &Pos_Author, &b_Pos_Author );
  fChain->SetBranchAddress( "Pos_EnergyLossType", &Pos_EnergyLossType, &b_Pos_EnergyLossType );
  fChain->SetBranchAddress( "Pos_PrimarySector", &Pos_PrimarySector, &b_Pos_PrimarySector );
  fChain->SetBranchAddress( "Pos_SecondarySector", &Pos_SecondarySector, &b_Pos_SecondarySector );
  fChain->SetBranchAddress( "Neg_Pt", &Neg_Pt, &b_Neg_Pt );
  fChain->SetBranchAddress( "Neg_Eta", &Neg_Eta, &b_Neg_Eta );
  fChain->SetBranchAddress( "Neg_Phi", &Neg_Phi, &b_Neg_Phi );
  fChain->SetBranchAddress( "Neg_Charge", &Neg_Charge, &b_Neg_Charge );
  fChain->SetBranchAddress( "Neg_ID_Pt", &Neg_ID_Pt, &b_Neg_ID_Pt );
  fChain->SetBranchAddress( "Neg_ME_Pt", &Neg_ME_Pt, &b_Neg_ME_Pt );
  fChain->SetBranchAddress( "Neg_ME_Eta", &Neg_ME_Eta, &b_Neg_ME_Eta );
  fChain->SetBranchAddress( "Neg_ME_Phi", &Neg_ME_Phi, &b_Neg_ME_Phi );
  fChain->SetBranchAddress( "Neg_PtCone20", &Neg_PtCone20, &b_Neg_PtCone20 );
  fChain->SetBranchAddress( "Neg_Quality", &Neg_Quality, &b_Neg_Quality );
  fChain->SetBranchAddress( "Neg_Author", &Neg_Author, &b_Neg_Author);
  fChain->SetBranchAddress( "Neg_EnergyLossType", &Neg_EnergyLossType, &b_Neg_EnergyLossType );
  fChain->SetBranchAddress( "Neg_PrimarySector", &Neg_PrimarySector, &b_Neg_PrimarySector );
  fChain->SetBranchAddress( "Neg_SecondarySector", &Neg_SecondarySector, &b_Neg_SecondarySector );
  //fChain->SetBranchAddress( "MassID", &MassID, &b_MassID );
  //fChain->SetBranchAddress( "MassMS", &MassMS, &b_MassMS );
}

Bool_t Reweighting::Notify( ) {
  return kTRUE;
}

#endif // #ifdef Reweighting_cxx
