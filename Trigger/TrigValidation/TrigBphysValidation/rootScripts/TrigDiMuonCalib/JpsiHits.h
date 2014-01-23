/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

struct TrackHits {

  Double_t        Pt;
  Double_t        Eta;
  Double_t        Phi;
  Double_t        Charge;
  Double_t        InnerBarrel_extEta;
  Double_t        InnerBarrel_extPhi;
  Double_t        InnerEndcap_extEta;
  Double_t        InnerEndcap_extPhi;
  Double_t        MiddleBarrel_extEta;
  Double_t        MiddleBarrel_extPhi;
  Double_t        MiddleEndcap_extEta;
  Double_t        MiddleEndcap_extPhi;
  Int_t           NAssociatedMdt;
  Int_t           NAssociatedRpcEta;
  Int_t           NAssociatedRpcPhi;
  Int_t           NAssociatedTgcRho;
  Int_t           NAssociatedTgcPhi;
  vector<double>  *InnerMdt_r;
  vector<double>  *InnerMdt_z;
  vector<bool>    *InnerMdt_associated;
  vector<double>  *MiddleMdt_r;
  vector<double>  *MiddleMdt_z;
  vector<bool>    *MiddleMdt_associated;
  vector<double>  *MiddleRpc_etaPad_x;
  vector<double>  *MiddleRpc_etaPad_y;
  vector<double>  *MiddleRpc_etaPad_z;
  vector<double>  *MiddleRpc_etaPad_r;
  vector<bool>    *MiddleRpc_etaPad_associated;
  vector<double>  *MiddleRpc_phiPad_x;
  vector<double>  *MiddleRpc_phiPad_y;
  vector<double>  *MiddleRpc_phiPad_z;
  vector<double>  *MiddleRpc_phiPad_r;
  vector<bool>    *MiddleRpc_phiPad_associated;
  vector<double>  *InnerTgc_phiHit_phi;
  vector<double>  *InnerTgc_phiHit_r;
  vector<double>  *InnerTgc_phiHit_z;
  vector<double>  *InnerTgc_phiHit_width;
  vector<bool>    *InnerTgc_phiHit_associated;
  vector<double>  *InnerTgc_rhoHit_phi;
  vector<double>  *InnerTgc_rhoHit_r;
  vector<double>  *InnerTgc_rhoHit_z;
  vector<double>  *InnerTgc_rhoHit_width;
  vector<bool>    *InnerTgc_rhoHit_associated;
  vector<double>  *MiddleTgc_phiHit_phi;
  vector<double>  *MiddleTgc_phiHit_r;
  vector<double>  *MiddleTgc_phiHit_z;
  vector<double>  *MiddleTgc_phiHit_width;
  vector<bool>    *MiddleTgc_phiHit_associated;
  vector<double>  *MiddleTgc_rhoHit_phi;
  vector<double>  *MiddleTgc_rhoHit_r;
  vector<double>  *MiddleTgc_rhoHit_z;
  vector<double>  *MiddleTgc_rhoHit_width;
  vector<bool>    *MiddleTgc_rhoHit_associated;
  vector<int>     *BTrigIndex;
  vector<int>     *BWhichTrack;
  vector<int>     *BTrigMatched;
  vector<int>     *BWhichTrackMatched;



}


struct JpsiHits {

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           LumiBlock;
   Int_t           EvtNumber;

   double          mass;


}