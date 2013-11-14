/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec 22 12:41:51 2009 by ROOT version 5.18/00b
// from TTree CollectionTree/CollectionTree
// found on file: ntuple.root
//////////////////////////////////////////////////////////

#ifndef AANTEvent_h
#define AANTEvent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

using namespace std;

class AANTEvent {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Char_t          Token[153];
   Int_t           Run;
   Int_t           Event;
   Int_t           Time;
   Int_t           LumiBlock;
   Int_t           BCID;
   Int_t           LVL1ID;
   Double_t        Weight;
   Int_t           IEvent;
   Int_t           StatusElement;
   Int_t           LVL1TriggerType;
   vector<unsigned int> *LVL1TriggerInfo;
   vector<unsigned int> *LVL2TriggerInfo;
   vector<unsigned int> *EventFilterInfo;
   vector<string>  *StreamTagName;
   vector<string>  *StreamTagType;
   UInt_t          NPar;
   vector<long>    *Type;
   vector<float>   *PtGen;
   vector<float>   *PhiGen;
   vector<float>   *EtaGen;
   vector<float>   *MGen;
   vector<float>   *Charge;
   vector<long>    *GenStat;
   vector<long>    *GenRef;
   vector<long>    *KMothNt;
   vector<long>    *KFDauNt;
   vector<long>    *KLDauNt;
   vector<long>    *KOriVNt;
   vector<long>    *KEndVNt;
   vector<long>    *BunchNum;
   vector<float>   *RVGen;
   vector<float>   *PhiVGen;
   vector<float>   *ZVGen;
   UInt_t          NVer;
   vector<long>    *BunchV;
   vector<float>   *RV;
   vector<float>   *PhiV;
   vector<float>   *ZV;
   vector<long>    *GenRefV;
   vector<long>    *KVMothNt;
   vector<long>    *KVFDauNt;
   vector<long>    *KVLDauNt;
   UInt_t          IVPrimary;
   Int_t           MuonEntRec_Num;
   vector<long>    *MuonEntRec_ID;
   vector<float>   *MuonEntRec_Ene;
   vector<float>   *MuonEntRec_Eta;
   vector<float>   *MuonEntRec_Phi;
   vector<float>   *MuonEntRec_PosX;
   vector<float>   *MuonEntRec_PosY;
   vector<float>   *MuonEntRec_PosZ;
   vector<float>   *MuonEntRec_Time;
   vector<long>    *MuonEntRec_BarCode;
   UInt_t          vxp_numVertices;
   vector<float>   *vxp_vtx_x;
   vector<float>   *vxp_vtx_y;
   vector<float>   *vxp_vtx_z;
   UInt_t          vxp_numOfTruthVertices;
   vector<float>   *vxp_vtx_x_truth;
   vector<float>   *vxp_vtx_y_truth;
   vector<float>   *vxp_vtx_z_truth;
   vector<float>   *vxp_vtx_x_res;
   vector<float>   *vxp_vtx_y_res;
   vector<float>   *vxp_vtx_z_res;
   vector<float>   *vxp_sigvtx_x;
   vector<float>   *vxp_sigvtx_y;
   vector<float>   *vxp_sigvtx_z;
   vector<float>   *vxp_vtxchi2;
   vector<float>   *vxp_vtxndf;
   vector<float>   *vxp_vtx_constraint_x;
   vector<float>   *vxp_vtx_constraint_y;
   vector<float>   *vxp_vtx_constraint_z;
   vector<float>   *vxp_sigvtx_constraint_x;
   vector<float>   *vxp_sigvtx_constraint_y;
   vector<float>   *vxp_sigvtx_constraint_z;
   vector<float>   *vxp_vtx_linearization_x;
   vector<float>   *vxp_vtx_linearization_y;
   vector<float>   *vxp_vtx_linearization_z;
   vector<float>   *vxp_vtx_seed_x;
   vector<float>   *vxp_vtx_seed_y;
   vector<float>   *vxp_vtx_seed_z;
   vector<float>   *vxp_vtxchi2prob;
   vector<long>    *vxp_numTracksPerVertex;
   UInt_t          vxp_totalNumTracks;
   vector<float>   *vxp_chi2;
   vector<float>   *vxp_ndf;
   vector<float>   *vxp_vertexCompatibility;
   vector<float>   *vxp_d0;
   vector<float>   *vxp_sigd0;
   vector<float>   *vxp_z0;
   vector<float>   *vxp_sigz0;
   vector<float>   *vxp_phi;
   vector<float>   *vxp_sigphi;
   vector<float>   *vxp_theta;
   vector<float>   *vxp_sigtheta;
   vector<float>   *vxp_qOverP;
   vector<float>   *vxp_sigqOverP;
   vector<float>   *vxp_d0g;
   vector<float>   *vxp_sigd0g;
   vector<float>   *vxp_z0g;
   vector<float>   *vxp_sigz0g;
   vector<float>   *vxp_refitted_d0;
   vector<float>   *vxp_refitted_sigd0;
   vector<float>   *vxp_refitted_z0;
   vector<float>   *vxp_refitted_sigz0;
   vector<float>   *vxp_refitted_phi;
   vector<float>   *vxp_refitted_sigphi;
   vector<float>   *vxp_refitted_theta;
   vector<float>   *vxp_refitted_sigtheta;
   vector<float>   *vxp_refitted_qOverP;
   vector<float>   *vxp_refitted_sigqOverP;
   UInt_t          Trk_totalNumTracks;
   vector<float>   *Trk_d0;
   vector<float>   *Trk_z0;
   vector<float>   *Trk_phi;
   vector<float>   *Trk_theta;
   vector<float>   *Trk_qOverP;
   vector<float>   *Trk_Chi2;
   vector<long>    *Trk_Ndf;
   vector<float>   *Trk_sigd0;
   vector<float>   *Trk_sigz0;
   vector<float>   *Trk_sigphi;
   vector<float>   *Trk_sigtheta;
   vector<float>   *Trk_sigqOverP;
   vector<float>   *Trk_covVert21;
   vector<float>   *Trk_covVert31;
   vector<float>   *Trk_covVert32;
   vector<float>   *Trk_covVert41;
   vector<float>   *Trk_covVert42;
   vector<float>   *Trk_covVert43;
   vector<float>   *Trk_covVert51;
   vector<float>   *Trk_covVert52;
   vector<float>   *Trk_covVert53;
   vector<float>   *Trk_covVert54;
   vector<float>   *Trk_px;
   vector<float>   *Trk_py;
   vector<float>   *Trk_pz;
   vector<float>   *Trk_pt;
   vector<long>    *Trk_numberOfBLayerHits;
   vector<long>    *Trk_numberOfPixelHits;
   vector<long>    *Trk_numberOfSCTHits;
   vector<long>    *Trk_numberOfTRTHits;
   vector<long>    *Trk_numberOfTRTHighThresholdHits;
   vector<long>    *Trk_numberOfBLayerSharedHits;
   vector<long>    *Trk_numberOfPixelSharedHits;
   vector<long>    *Trk_numberOfPixelHoles;
   vector<long>    *Trk_numberOfSCTSharedHits;
   vector<long>    *Trk_numberOfSCTHoles;
   vector<long>    *Trk_numberOfTRTOutliers;
   vector<long>    *Trk_numberOfTRTHighThresholdOutliers;
   vector<long>    *Trk_numberOfMdtHits;
   vector<long>    *Trk_numberOfTgcPhiHits;
   vector<long>    *Trk_numberOfRpcPhiHits;
   vector<long>    *Trk_numberOfCscPhiHits;
   vector<long>    *Trk_numberOfTgcEtaHits;
   vector<long>    *Trk_numberOfRpcEtaHits;
   vector<long>    *Trk_numberOfCscEtaHits;
   vector<long>    *Trk_numberOfGangedPixels;
   vector<long>    *Trk_numberOfOutliersOnTrack;
   vector<long>    *Trk_truthBarcode;
   vector<long>    *Trk_truthNt;
   vector<long>    *Trk_ParticlePdg;
   vector<long>    *Trk_ParentPdg;
   UInt_t          MboySeg_nseg;
   vector<float>   *MboySeg_Xpt;
   vector<float>   *MboySeg_Ypt;
   vector<float>   *MboySeg_Zpt;
   vector<float>   *MboySeg_Xvec;
   vector<float>   *MboySeg_Yvec;
   vector<float>   *MboySeg_Zvec;
   vector<float>   *MboySeg_QualityFactor;
   vector<long>    *MboySeg_NbreOfMultiLayer;
   vector<long>    *MboySeg_TrackSharing;
   vector<long>    *MboySeg_StationName;
   vector<long>    *MboySeg_Ndigits;
   UInt_t          MboyTrkMB_NTrk;
   UInt_t          MboyTrkVT_NTrk;
   vector<long>    *MboyTrkVT_Va;
   vector<float>   *MboyTrkVT_A0;
   vector<float>   *MboyTrkVT_Z;
   vector<float>   *MboyTrkVT_Phi;
   vector<float>   *MboyTrkVT_CotTh;
   vector<float>   *MboyTrkVT_PTInv;
   vector<float>   *MboyTrkVT_Cov11;
   vector<float>   *MboyTrkVT_Cov21;
   vector<float>   *MboyTrkVT_Cov22;
   vector<float>   *MboyTrkVT_Cov31;
   vector<float>   *MboyTrkVT_Cov32;
   vector<float>   *MboyTrkVT_Cov33;
   vector<float>   *MboyTrkVT_Cov41;
   vector<float>   *MboyTrkVT_Cov42;
   vector<float>   *MboyTrkVT_Cov43;
   vector<float>   *MboyTrkVT_Cov44;
   vector<float>   *MboyTrkVT_Cov51;
   vector<float>   *MboyTrkVT_Cov52;
   vector<float>   *MboyTrkVT_Cov53;
   vector<float>   *MboyTrkVT_Cov54;
   vector<float>   *MboyTrkVT_Cov55;
   vector<float>   *MboyTrkVT_Xpt;
   vector<float>   *MboyTrkVT_Ypt;
   vector<float>   *MboyTrkVT_Zpt;
   vector<float>   *MboyTrkVT_Xvec;
   vector<float>   *MboyTrkVT_Yvec;
   vector<float>   *MboyTrkVT_Zvec;
   vector<float>   *MboyTrkVT_Mom;
   vector<long>    *MboyTrkVT_digits;
   vector<long>    *MboyTrkVT_HitPosition;
   UInt_t          MboyTrkVT_nhits;
   vector<float>   *MboyTrkVT_TrHi_Rdigi;
   vector<float>   *MboyTrkVT_TrHi_Rtrack;
   vector<float>   *MboyTrkVT_TrHi_Xdca;
   vector<float>   *MboyTrkVT_TrHi_Ydca;
   vector<float>   *MboyTrkVT_TrHi_Zdca;
   vector<float>   *MboyTrkVT_TrHi_Sdca;
   vector<float>   *MboyTrkVT_TrHi_Reso;
   vector<float>   *MboyTrkVT_TrHi_RZSsim;
   vector<float>   *MboyTrkVT_TrHi_EZSfit;
   vector<long>    *MboyTrkVT_TrHi_JTYP;
   vector<long>    *MboyTrkVT_TrHi_JFF;
   vector<long>    *MboyTrkVT_TrHi_JZZ;
   vector<long>    *MboyTrkVT_TrHi_JOB;
   vector<long>    *MboyTrkVT_TrHi_JSPLI;
   vector<long>    *MboyTrkVT_TrHi_JSL;
   vector<long>    *MboyTrkVT_TrHi_JTUBE;
   vector<long>    *MboyTrkVT_TrHi_JSZ;
   vector<long>    *MboyTrkVT_TrHi_JSTRI;
   vector<float>   *MboyTrkVT_TrHi_DriftTime;
   vector<long>    *MboyTrkVT_TrHi_ChamberTech;
   vector<long>    *MboyTrkVT_TrHi_StationNber;
   vector<long>    *MboyTrkVT_TrHi_StationEta;
   vector<long>    *MboyTrkVT_TrHi_StationPhi;
   vector<long>    *MboyTrkVT_TrHi_Multilayer;
   vector<long>    *MboyTrkVT_TrHi_TubeLayer;
   vector<long>    *MboyTrkVT_TrHi_Tube;
   vector<long>    *MboyTrkVT_TrHi_DoubletR;
   vector<long>    *MboyTrkVT_TrHi_DoubletZ;
   vector<long>    *MboyTrkVT_TrHi_DoubletPhi;
   vector<long>    *MboyTrkVT_TrHi_GasGap;
   vector<long>    *MboyTrkVT_TrHi_MeasuresPhi;
   vector<long>    *MboyTrkVT_TrHi_Strip;
   vector<long>    *MboyTrkVT_TrHi_IsStrip;
   vector<long>    *MboyTrkVT_TrHi_Channel;
   vector<long>    *MboyTrkVT_TrHi_ChamberLayer;
   vector<long>    *MboyTrkVT_TrHi_WireLayer;
   vector<float>   *MboyTrkVT_TrHi_RpcTime;
   UInt_t          MboyTrkKE_NTrk;
   vector<long>    *MboyTrkKE_Va;
   vector<float>   *MboyTrkKE_Thept;
   vector<float>   *MboyTrkKE_Phipt;
   vector<float>   *MboyTrkKE_Thevc;
   vector<float>   *MboyTrkKE_Phivc;
   vector<float>   *MboyTrkKE_PInv;
   vector<float>   *MboyTrkKE_Cov11;
   vector<float>   *MboyTrkKE_Cov21;
   vector<float>   *MboyTrkKE_Cov22;
   vector<float>   *MboyTrkKE_Cov31;
   vector<float>   *MboyTrkKE_Cov32;
   vector<float>   *MboyTrkKE_Cov33;
   vector<float>   *MboyTrkKE_Cov41;
   vector<float>   *MboyTrkKE_Cov42;
   vector<float>   *MboyTrkKE_Cov43;
   vector<float>   *MboyTrkKE_Cov44;
   vector<float>   *MboyTrkKE_Cov51;
   vector<float>   *MboyTrkKE_Cov52;
   vector<float>   *MboyTrkKE_Cov53;
   vector<float>   *MboyTrkKE_Cov54;
   vector<float>   *MboyTrkKE_Cov55;
   vector<float>   *MboyTrkKE_Xpt;
   vector<float>   *MboyTrkKE_Ypt;
   vector<float>   *MboyTrkKE_Zpt;
   vector<float>   *MboyTrkKE_Xvec;
   vector<float>   *MboyTrkKE_Yvec;
   vector<float>   *MboyTrkKE_Zvec;
   vector<float>   *MboyTrkKE_Mom;
   vector<float>   *MboyTrkKE_ZCyl;
   vector<float>   *MboyTrkKE_RCyl;
   UInt_t          MboyTrkMS_NTrk;
   vector<long>    *MboyTrkMS_Va;
   vector<float>   *MboyTrkMS_Thept;
   vector<float>   *MboyTrkMS_Phipt;
   vector<float>   *MboyTrkMS_Thevc;
   vector<float>   *MboyTrkMS_Phivc;
   vector<float>   *MboyTrkMS_PInv;
   vector<float>   *MboyTrkMS_Cov11;
   vector<float>   *MboyTrkMS_Cov21;
   vector<float>   *MboyTrkMS_Cov22;
   vector<float>   *MboyTrkMS_Cov31;
   vector<float>   *MboyTrkMS_Cov32;
   vector<float>   *MboyTrkMS_Cov33;
   vector<float>   *MboyTrkMS_Cov41;
   vector<float>   *MboyTrkMS_Cov42;
   vector<float>   *MboyTrkMS_Cov43;
   vector<float>   *MboyTrkMS_Cov44;
   vector<float>   *MboyTrkMS_Cov51;
   vector<float>   *MboyTrkMS_Cov52;
   vector<float>   *MboyTrkMS_Cov53;
   vector<float>   *MboyTrkMS_Cov54;
   vector<float>   *MboyTrkMS_Cov55;
   vector<float>   *MboyTrkMS_Xpt;
   vector<float>   *MboyTrkMS_Ypt;
   vector<float>   *MboyTrkMS_Zpt;
   vector<float>   *MboyTrkMS_Xvec;
   vector<float>   *MboyTrkMS_Yvec;
   vector<float>   *MboyTrkMS_Zvec;
   vector<float>   *MboyTrkMS_Mom;
   vector<float>   *MboyTrkMS_ZCyl;
   vector<float>   *MboyTrkMS_RCyl;
   vector<float>   *MboyTrkMS_Chican;
   vector<float>   *MboyTrkMS_Chifit;
   vector<float>   *MboyTrkMS_Chiver;
   vector<float>   *MboyTrkMS_Chistat;
   vector<long>    *MboyTrkMS_Ndof;
   Float_t         MboyAdjT0DefaultTShift;
   UInt_t          MboyAdjT0NberOfData;
   vector<long>    *MboyAdjT0stationName;
   vector<long>    *MboyAdjT0stationEta;
   vector<long>    *MboyAdjT0stationPhi;
   vector<float>   *MboyAdjT0TShift;
   UInt_t          staco_nmuon;
   vector<float>   *staco_A0;
   vector<float>   *staco_Z;
   vector<float>   *staco_Phi;
   vector<float>   *staco_Theta;
   vector<float>   *staco_qOverP;
   vector<float>   *staco_chi2;
   vector<long>    *staco_Ndf;
   vector<float>   *staco_covr11;
   vector<float>   *staco_covr21;
   vector<float>   *staco_covr22;
   vector<float>   *staco_covr31;
   vector<float>   *staco_covr32;
   vector<float>   *staco_covr33;
   vector<float>   *staco_covr41;
   vector<float>   *staco_covr42;
   vector<float>   *staco_covr43;
   vector<float>   *staco_covr44;
   vector<float>   *staco_covr51;
   vector<float>   *staco_covr52;
   vector<float>   *staco_covr53;
   vector<float>   *staco_covr54;
   vector<float>   *staco_covr55;
   vector<float>   *staco_A0ID;
   vector<float>   *staco_ZID;
   vector<float>   *staco_PhiID;
   vector<float>   *staco_ThetaID;
   vector<float>   *staco_qOverPID;
   vector<float>   *staco_chi2ID;
   vector<long>    *staco_NdfID;
   vector<float>   *staco_covr11ID;
   vector<float>   *staco_covr12ID;
   vector<float>   *staco_covr22ID;
   vector<float>   *staco_covr13ID;
   vector<float>   *staco_covr23ID;
   vector<float>   *staco_covr33ID;
   vector<float>   *staco_covr14ID;
   vector<float>   *staco_covr24ID;
   vector<float>   *staco_covr34ID;
   vector<float>   *staco_covr44ID;
   vector<float>   *staco_covr15ID;
   vector<float>   *staco_covr25ID;
   vector<float>   *staco_covr35ID;
   vector<float>   *staco_covr45ID;
   vector<float>   *staco_covr55ID;
   vector<float>   *staco_A0MS;
   vector<float>   *staco_ZMS;
   vector<float>   *staco_PhiMS;
   vector<float>   *staco_ThetaMS;
   vector<float>   *staco_qOverPMS;
   vector<float>   *staco_chi2MS;
   vector<long>    *staco_NdfMS;
   vector<float>   *staco_covr11MS;
   vector<float>   *staco_covr12MS;
   vector<float>   *staco_covr22MS;
   vector<float>   *staco_covr13MS;
   vector<float>   *staco_covr23MS;
   vector<float>   *staco_covr33MS;
   vector<float>   *staco_covr14MS;
   vector<float>   *staco_covr24MS;
   vector<float>   *staco_covr34MS;
   vector<float>   *staco_covr44MS;
   vector<float>   *staco_covr15MS;
   vector<float>   *staco_covr25MS;
   vector<float>   *staco_covr35MS;
   vector<float>   *staco_covr45MS;
   vector<float>   *staco_covr55MS;
   vector<float>   *staco_xMSE;
   vector<float>   *staco_yMSE;
   vector<float>   *staco_zMSE;
   vector<float>   *staco_PxMSE;
   vector<float>   *staco_PyMSE;
   vector<float>   *staco_PzMSE;
   vector<float>   *staco_chi2MSE;
   vector<long>    *staco_NdfMSE;
   vector<float>   *staco_etcone10;
   vector<float>   *staco_etcone20;
   vector<float>   *staco_etcone30;
   vector<float>   *staco_etcone40;
   vector<float>   *staco_nucone10;
   vector<float>   *staco_nucone20;
   vector<float>   *staco_nucone30;
   vector<float>   *staco_nucone40;
   vector<float>   *staco_Eloss;
   vector<float>   *staco_ElossError;
   UInt_t          StacoExtr_nmuon;
   vector<float>   *StacoExtr_A0;
   vector<float>   *StacoExtr_Z;
   vector<float>   *StacoExtr_Phi;
   vector<float>   *StacoExtr_Theta;
   vector<float>   *StacoExtr_qOverP;
   vector<float>   *StacoExtr_Chi2;
   vector<long>    *StacoExtr_Ndf;
   vector<float>   *StacoExtr_covr11;
   vector<float>   *StacoExtr_covr21;
   vector<float>   *StacoExtr_covr22;
   vector<float>   *StacoExtr_covr31;
   vector<float>   *StacoExtr_covr32;
   vector<float>   *StacoExtr_covr33;
   vector<float>   *StacoExtr_covr41;
   vector<float>   *StacoExtr_covr42;
   vector<float>   *StacoExtr_covr43;
   vector<float>   *StacoExtr_covr44;
   vector<float>   *StacoExtr_covr51;
   vector<float>   *StacoExtr_covr52;
   vector<float>   *StacoExtr_covr53;
   vector<float>   *StacoExtr_covr54;
   vector<float>   *StacoExtr_covr55;
   vector<float>   *StacoExtr_etcone10;
   vector<float>   *StacoExtr_etcone20;
   vector<float>   *StacoExtr_etcone30;
   vector<float>   *StacoExtr_etcone40;
   vector<float>   *StacoExtr_nucone10;
   vector<float>   *StacoExtr_nucone20;
   vector<float>   *StacoExtr_nucone30;
   vector<float>   *StacoExtr_nucone40;
   vector<float>   *StacoExtr_Eloss;
   vector<float>   *StacoExtr_ElossError;
   UInt_t          MboyESD_nmuon;
   vector<float>   *MboyESD_A0;
   vector<float>   *MboyESD_Z;
   vector<float>   *MboyESD_Phi;
   vector<float>   *MboyESD_Theta;
   vector<float>   *MboyESD_qOverP;
   vector<float>   *MboyESD_Chi2;
   vector<long>    *MboyESD_Ndf;
   vector<float>   *MboyESD_covr11;
   vector<float>   *MboyESD_covr21;
   vector<float>   *MboyESD_covr22;
   vector<float>   *MboyESD_covr31;
   vector<float>   *MboyESD_covr32;
   vector<float>   *MboyESD_covr33;
   vector<float>   *MboyESD_covr41;
   vector<float>   *MboyESD_covr42;
   vector<float>   *MboyESD_covr43;
   vector<float>   *MboyESD_covr44;
   vector<float>   *MboyESD_covr51;
   vector<float>   *MboyESD_covr52;
   vector<float>   *MboyESD_covr53;
   vector<float>   *MboyESD_covr54;
   vector<float>   *MboyESD_covr55;
   vector<float>   *MboyESD_etcone10;
   vector<float>   *MboyESD_etcone20;
   vector<float>   *MboyESD_etcone30;
   vector<float>   *MboyESD_etcone40;
   vector<float>   *MboyESD_nucone10;
   vector<float>   *MboyESD_nucone20;
   vector<float>   *MboyESD_nucone30;
   vector<float>   *MboyESD_nucone40;
   vector<float>   *MboyESD_Eloss;
   vector<float>   *MboyESD_ElossError;
   UInt_t          MuTag_nmuon;
   vector<float>   *MuTag_A0;
   vector<float>   *MuTag_Z;
   vector<float>   *MuTag_Phi;
   vector<float>   *MuTag_Theta;
   vector<float>   *MuTag_qOverP;
   vector<float>   *MuTag_segmentChi2OverDoF;
   vector<float>   *MuTag_segmentDeltaPhi;
   vector<float>   *MuTag_segmentDeltaEta;
   vector<float>   *MuTag_innAngle;
   vector<float>   *MuTag_midAngle;
   vector<long>    *MuTag_numberOfMDTHits;
   vector<long>    *MuTag_numberOfRPCPhiHits;
   vector<long>    *MuTag_numberOfTGCPhiHits;
   vector<long>    *MuTag_MuonQuality;
   vector<long>    *MuTag_IsInner;
   vector<long>    *MuTag_SegMult;
   vector<long>    *MuTag_StationTypeSglTag;
   vector<float>   *MuTag_covr11;
   vector<float>   *MuTag_covr21;
   vector<float>   *MuTag_covr22;
   vector<float>   *MuTag_covr31;
   vector<float>   *MuTag_covr32;
   vector<float>   *MuTag_covr33;
   vector<float>   *MuTag_covr41;
   vector<float>   *MuTag_covr42;
   vector<float>   *MuTag_covr43;
   vector<float>   *MuTag_covr44;
   vector<float>   *MuTag_covr51;
   vector<float>   *MuTag_covr52;
   vector<float>   *MuTag_covr53;
   vector<float>   *MuTag_covr54;
   vector<float>   *MuTag_covr55;
   vector<float>   *MuTag_MatchChi2;
   vector<long>    *MuTag_MatchNdof;
   Float_t         MemRSS;
   Float_t         MemVSize;
   Float_t         TimeTotal;
   Float_t         TimeSum;
   Float_t         TimeAthMasterSeq;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_Token;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_LumiBlock;   //!
   TBranch        *b_BCID;   //!
   TBranch        *b_LVL1ID;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_IEvent;   //!
   TBranch        *b_StatusElement;   //!
   TBranch        *b_LVL1TriggerType;   //!
   TBranch        *b_LVL1TriggerInfo;   //!
   TBranch        *b_LVL2TriggerInfo;   //!
   TBranch        *b_EventFilterInfo;   //!
   TBranch        *b_StreamTagName;   //!
   TBranch        *b_StreamTagType;   //!
   TBranch        *b_NPar;   //!
   TBranch        *b_Type;   //!
   TBranch        *b_PtGen;   //!
   TBranch        *b_PhiGen;   //!
   TBranch        *b_EtaGen;   //!
   TBranch        *b_MGen;   //!
   TBranch        *b_Charge;   //!
   TBranch        *b_GenStat;   //!
   TBranch        *b_GenRef;   //!
   TBranch        *b_KMothNt;   //!
   TBranch        *b_KFDauNt;   //!
   TBranch        *b_KLDauNt;   //!
   TBranch        *b_KOriVNt;   //!
   TBranch        *b_KEndVNt;   //!
   TBranch        *b_BunchNum;   //!
   TBranch        *b_RVGen;   //!
   TBranch        *b_PhiVGen;   //!
   TBranch        *b_ZVGen;   //!
   TBranch        *b_NVer;   //!
   TBranch        *b_BunchV;   //!
   TBranch        *b_RV;   //!
   TBranch        *b_PhiV;   //!
   TBranch        *b_ZV;   //!
   TBranch        *b_GenRefV;   //!
   TBranch        *b_KVMothNt;   //!
   TBranch        *b_KVFDauNt;   //!
   TBranch        *b_KVLDauNt;   //!
   TBranch        *b_IVPrimary;   //!
   TBranch        *b_MuonEntRec_Num;   //!
   TBranch        *b_MuonEntRec_ID;   //!
   TBranch        *b_MuonEntRec_Ene;   //!
   TBranch        *b_MuonEntRec_Eta;   //!
   TBranch        *b_MuonEntRec_Phi;   //!
   TBranch        *b_MuonEntRec_PosX;   //!
   TBranch        *b_MuonEntRec_PosY;   //!
   TBranch        *b_MuonEntRec_PosZ;   //!
   TBranch        *b_MuonEntRec_Time;   //!
   TBranch        *b_MuonEntRec_BarCode;   //!
   TBranch        *b_vxp_numVertices;   //!
   TBranch        *b_vxp_vtx_x;   //!
   TBranch        *b_vxp_vtx_y;   //!
   TBranch        *b_vxp_vtx_z;   //!
   TBranch        *b_vxp_numOfTruthVertices;   //!
   TBranch        *b_vxp_vtx_x_truth;   //!
   TBranch        *b_vxp_vtx_y_truth;   //!
   TBranch        *b_vxp_vtx_z_truth;   //!
   TBranch        *b_vxp_vtx_x_res;   //!
   TBranch        *b_vxp_vtx_y_res;   //!
   TBranch        *b_vxp_vtx_z_res;   //!
   TBranch        *b_vxp_sigvtx_x;   //!
   TBranch        *b_vxp_sigvtx_y;   //!
   TBranch        *b_vxp_sigvtx_z;   //!
   TBranch        *b_vxp_vtxchi2;   //!
   TBranch        *b_vxp_vtxndf;   //!
   TBranch        *b_vxp_vtx_constraint_x;   //!
   TBranch        *b_vxp_vtx_constraint_y;   //!
   TBranch        *b_vxp_vtx_constraint_z;   //!
   TBranch        *b_vxp_sigvtx_constraint_x;   //!
   TBranch        *b_vxp_sigvtx_constraint_y;   //!
   TBranch        *b_vxp_sigvtx_constraint_z;   //!
   TBranch        *b_vxp_vtx_linearization_x;   //!
   TBranch        *b_vxp_vtx_linearization_y;   //!
   TBranch        *b_vxp_vtx_linearization_z;   //!
   TBranch        *b_vxp_vtx_seed_x;   //!
   TBranch        *b_vxp_vtx_seed_y;   //!
   TBranch        *b_vxp_vtx_seed_z;   //!
   TBranch        *b_vxp_vtxchi2prob;   //!
   TBranch        *b_vxp_numTracksPerVertex;   //!
   TBranch        *b_vxp_totalNumTracks;   //!
   TBranch        *b_vxp_chi2;   //!
   TBranch        *b_vxp_ndf;   //!
   TBranch        *b_vxp_vertexCompatibility;   //!
   TBranch        *b_vxp_d0;   //!
   TBranch        *b_vxp_sigd0;   //!
   TBranch        *b_vxp_z0;   //!
   TBranch        *b_vxp_sigz0;   //!
   TBranch        *b_vxp_phi;   //!
   TBranch        *b_vxp_sigphi;   //!
   TBranch        *b_vxp_theta;   //!
   TBranch        *b_vxp_sigtheta;   //!
   TBranch        *b_vxp_qOverP;   //!
   TBranch        *b_vxp_sigqOverP;   //!
   TBranch        *b_vxp_d0g;   //!
   TBranch        *b_vxp_sigd0g;   //!
   TBranch        *b_vxp_z0g;   //!
   TBranch        *b_vxp_sigz0g;   //!
   TBranch        *b_vxp_refitted_d0;   //!
   TBranch        *b_vxp_refitted_sigd0;   //!
   TBranch        *b_vxp_refitted_z0;   //!
   TBranch        *b_vxp_refitted_sigz0;   //!
   TBranch        *b_vxp_refitted_phi;   //!
   TBranch        *b_vxp_refitted_sigphi;   //!
   TBranch        *b_vxp_refitted_theta;   //!
   TBranch        *b_vxp_refitted_sigtheta;   //!
   TBranch        *b_vxp_refitted_qOverP;   //!
   TBranch        *b_vxp_refitted_sigqOverP;   //!
   TBranch        *b_Trk_totalNumTracks;   //!
   TBranch        *b_Trk_d0;   //!
   TBranch        *b_Trk_z0;   //!
   TBranch        *b_Trk_phi;   //!
   TBranch        *b_Trk_theta;   //!
   TBranch        *b_Trk_qOverP;   //!
   TBranch        *b_Trk_Chi2;   //!
   TBranch        *b_Trk_Ndf;   //!
   TBranch        *b_Trk_sigd0;   //!
   TBranch        *b_Trk_sigz0;   //!
   TBranch        *b_Trk_sigphi;   //!
   TBranch        *b_Trk_sigtheta;   //!
   TBranch        *b_Trk_sigqOverP;   //!
   TBranch        *b_Trk_covVert21;   //!
   TBranch        *b_Trk_covVert31;   //!
   TBranch        *b_Trk_covVert32;   //!
   TBranch        *b_Trk_covVert41;   //!
   TBranch        *b_Trk_covVert42;   //!
   TBranch        *b_Trk_covVert43;   //!
   TBranch        *b_Trk_covVert51;   //!
   TBranch        *b_Trk_covVert52;   //!
   TBranch        *b_Trk_covVert53;   //!
   TBranch        *b_Trk_covVert54;   //!
   TBranch        *b_Trk_px;   //!
   TBranch        *b_Trk_py;   //!
   TBranch        *b_Trk_pz;   //!
   TBranch        *b_Trk_pt;   //!
   TBranch        *b_Trk_numberOfBLayerHits;   //!
   TBranch        *b_Trk_numberOfPixelHits;   //!
   TBranch        *b_Trk_numberOfSCTHits;   //!
   TBranch        *b_Trk_numberOfTRTHits;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdHits;   //!
   TBranch        *b_Trk_numberOfBLayerSharedHits;   //!
   TBranch        *b_Trk_numberOfPixelSharedHits;   //!
   TBranch        *b_Trk_numberOfPixelHoles;   //!
   TBranch        *b_Trk_numberOfSCTSharedHits;   //!
   TBranch        *b_Trk_numberOfSCTHoles;   //!
   TBranch        *b_Trk_numberOfTRTOutliers;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdOutliers;   //!
   TBranch        *b_Trk_numberOfMdtHits;   //!
   TBranch        *b_Trk_numberOfTgcPhiHits;   //!
   TBranch        *b_Trk_numberOfRpcPhiHits;   //!
   TBranch        *b_Trk_numberOfCscPhiHits;   //!
   TBranch        *b_Trk_numberOfTgcEtaHits;   //!
   TBranch        *b_Trk_numberOfRpcEtaHits;   //!
   TBranch        *b_Trk_numberOfCscEtaHits;   //!
   TBranch        *b_Trk_numberOfGangedPixels;   //!
   TBranch        *b_Trk_numberOfOutliersOnTrack;   //!
   TBranch        *b_Trk_truthBarcode;   //!
   TBranch        *b_Trk_truthNt;   //!
   TBranch        *b_Trk_ParticlePdg;   //!
   TBranch        *b_Trk_ParentPdg;   //!
   TBranch        *b_MboySeg_nseg;   //!
   TBranch        *b_MboySeg_Xpt;   //!
   TBranch        *b_MboySeg_Ypt;   //!
   TBranch        *b_MboySeg_Zpt;   //!
   TBranch        *b_MboySeg_Xvec;   //!
   TBranch        *b_MboySeg_Yvec;   //!
   TBranch        *b_MboySeg_Zvec;   //!
   TBranch        *b_MboySeg_QualityFactor;   //!
   TBranch        *b_MboySeg_NbreOfMultiLayer;   //!
   TBranch        *b_MboySeg_TrackSharing;   //!
   TBranch        *b_MboySeg_StationName;   //!
   TBranch        *b_MboySeg_Ndigits;   //!
   TBranch        *b_MboyTrkMB_NTrk;   //!
   TBranch        *b_MboyTrkVT_NTrk;   //!
   TBranch        *b_MboyTrkVT_Va;   //!
   TBranch        *b_MboyTrkVT_A0;   //!
   TBranch        *b_MboyTrkVT_Z;   //!
   TBranch        *b_MboyTrkVT_Phi;   //!
   TBranch        *b_MboyTrkVT_CotTh;   //!
   TBranch        *b_MboyTrkVT_PTInv;   //!
   TBranch        *b_MboyTrkVT_Cov11;   //!
   TBranch        *b_MboyTrkVT_Cov21;   //!
   TBranch        *b_MboyTrkVT_Cov22;   //!
   TBranch        *b_MboyTrkVT_Cov31;   //!
   TBranch        *b_MboyTrkVT_Cov32;   //!
   TBranch        *b_MboyTrkVT_Cov33;   //!
   TBranch        *b_MboyTrkVT_Cov41;   //!
   TBranch        *b_MboyTrkVT_Cov42;   //!
   TBranch        *b_MboyTrkVT_Cov43;   //!
   TBranch        *b_MboyTrkVT_Cov44;   //!
   TBranch        *b_MboyTrkVT_Cov51;   //!
   TBranch        *b_MboyTrkVT_Cov52;   //!
   TBranch        *b_MboyTrkVT_Cov53;   //!
   TBranch        *b_MboyTrkVT_Cov54;   //!
   TBranch        *b_MboyTrkVT_Cov55;   //!
   TBranch        *b_MboyTrkVT_Xpt;   //!
   TBranch        *b_MboyTrkVT_Ypt;   //!
   TBranch        *b_MboyTrkVT_Zpt;   //!
   TBranch        *b_MboyTrkVT_Xvec;   //!
   TBranch        *b_MboyTrkVT_Yvec;   //!
   TBranch        *b_MboyTrkVT_Zvec;   //!
   TBranch        *b_MboyTrkVT_Mom;   //!
   TBranch        *b_MboyTrkVT_digits;   //!
   TBranch        *b_MboyTrkVT_HitPosition;   //!
   TBranch        *b_MboyTrkVT_nhits;   //!
   TBranch        *b_MboyTrkVT_TrHi_Rdigi;   //!
   TBranch        *b_MboyTrkVT_TrHi_Rtrack;   //!
   TBranch        *b_MboyTrkVT_TrHi_Xdca;   //!
   TBranch        *b_MboyTrkVT_TrHi_Ydca;   //!
   TBranch        *b_MboyTrkVT_TrHi_Zdca;   //!
   TBranch        *b_MboyTrkVT_TrHi_Sdca;   //!
   TBranch        *b_MboyTrkVT_TrHi_Reso;   //!
   TBranch        *b_MboyTrkVT_TrHi_RZSsim;   //!
   TBranch        *b_MboyTrkVT_TrHi_EZSfit;   //!
   TBranch        *b_MboyTrkVT_TrHi_JTYP;   //!
   TBranch        *b_MboyTrkVT_TrHi_JFF;   //!
   TBranch        *b_MboyTrkVT_TrHi_JZZ;   //!
   TBranch        *b_MboyTrkVT_TrHi_JOB;   //!
   TBranch        *b_MboyTrkVT_TrHi_JSPLI;   //!
   TBranch        *b_MboyTrkVT_TrHi_JSL;   //!
   TBranch        *b_MboyTrkVT_TrHi_JTUBE;   //!
   TBranch        *b_MboyTrkVT_TrHi_JSZ;   //!
   TBranch        *b_MboyTrkVT_TrHi_JSTRI;   //!
   TBranch        *b_MboyTrkVT_TrHi_DriftTime;   //!
   TBranch        *b_MboyTrkVT_TrHi_ChamberTech;   //!
   TBranch        *b_MboyTrkVT_TrHi_StationNber;   //!
   TBranch        *b_MboyTrkVT_TrHi_StationEta;   //!
   TBranch        *b_MboyTrkVT_TrHi_StationPhi;   //!
   TBranch        *b_MboyTrkVT_TrHi_Multilayer;   //!
   TBranch        *b_MboyTrkVT_TrHi_TubeLayer;   //!
   TBranch        *b_MboyTrkVT_TrHi_Tube;   //!
   TBranch        *b_MboyTrkVT_TrHi_DoubletR;   //!
   TBranch        *b_MboyTrkVT_TrHi_DoubletZ;   //!
   TBranch        *b_MboyTrkVT_TrHi_DoubletPhi;   //!
   TBranch        *b_MboyTrkVT_TrHi_GasGap;   //!
   TBranch        *b_MboyTrkVT_TrHi_MeasuresPhi;   //!
   TBranch        *b_MboyTrkVT_TrHi_Strip;   //!
   TBranch        *b_MboyTrkVT_TrHi_IsStrip;   //!
   TBranch        *b_MboyTrkVT_TrHi_Channel;   //!
   TBranch        *b_MboyTrkVT_TrHi_ChamberLayer;   //!
   TBranch        *b_MboyTrkVT_TrHi_WireLayer;   //!
   TBranch        *b_MboyTrkVT_TrHi_RpcTime;   //!
   TBranch        *b_MboyTrkKE_NTrk;   //!
   TBranch        *b_MboyTrkKE_Va;   //!
   TBranch        *b_MboyTrkKE_Thept;   //!
   TBranch        *b_MboyTrkKE_Phipt;   //!
   TBranch        *b_MboyTrkKE_Thevc;   //!
   TBranch        *b_MboyTrkKE_Phivc;   //!
   TBranch        *b_MboyTrkKE_PInv;   //!
   TBranch        *b_MboyTrkKE_Cov11;   //!
   TBranch        *b_MboyTrkKE_Cov21;   //!
   TBranch        *b_MboyTrkKE_Cov22;   //!
   TBranch        *b_MboyTrkKE_Cov31;   //!
   TBranch        *b_MboyTrkKE_Cov32;   //!
   TBranch        *b_MboyTrkKE_Cov33;   //!
   TBranch        *b_MboyTrkKE_Cov41;   //!
   TBranch        *b_MboyTrkKE_Cov42;   //!
   TBranch        *b_MboyTrkKE_Cov43;   //!
   TBranch        *b_MboyTrkKE_Cov44;   //!
   TBranch        *b_MboyTrkKE_Cov51;   //!
   TBranch        *b_MboyTrkKE_Cov52;   //!
   TBranch        *b_MboyTrkKE_Cov53;   //!
   TBranch        *b_MboyTrkKE_Cov54;   //!
   TBranch        *b_MboyTrkKE_Cov55;   //!
   TBranch        *b_MboyTrkKE_Xpt;   //!
   TBranch        *b_MboyTrkKE_Ypt;   //!
   TBranch        *b_MboyTrkKE_Zpt;   //!
   TBranch        *b_MboyTrkKE_Xvec;   //!
   TBranch        *b_MboyTrkKE_Yvec;   //!
   TBranch        *b_MboyTrkKE_Zvec;   //!
   TBranch        *b_MboyTrkKE_Mom;   //!
   TBranch        *b_MboyTrkKE_ZCyl;   //!
   TBranch        *b_MboyTrkKE_RCyl;   //!
   TBranch        *b_MboyTrkMS_NTrk;   //!
   TBranch        *b_MboyTrkMS_Va;   //!
   TBranch        *b_MboyTrkMS_Thept;   //!
   TBranch        *b_MboyTrkMS_Phipt;   //!
   TBranch        *b_MboyTrkMS_Thevc;   //!
   TBranch        *b_MboyTrkMS_Phivc;   //!
   TBranch        *b_MboyTrkMS_PInv;   //!
   TBranch        *b_MboyTrkMS_Cov11;   //!
   TBranch        *b_MboyTrkMS_Cov21;   //!
   TBranch        *b_MboyTrkMS_Cov22;   //!
   TBranch        *b_MboyTrkMS_Cov31;   //!
   TBranch        *b_MboyTrkMS_Cov32;   //!
   TBranch        *b_MboyTrkMS_Cov33;   //!
   TBranch        *b_MboyTrkMS_Cov41;   //!
   TBranch        *b_MboyTrkMS_Cov42;   //!
   TBranch        *b_MboyTrkMS_Cov43;   //!
   TBranch        *b_MboyTrkMS_Cov44;   //!
   TBranch        *b_MboyTrkMS_Cov51;   //!
   TBranch        *b_MboyTrkMS_Cov52;   //!
   TBranch        *b_MboyTrkMS_Cov53;   //!
   TBranch        *b_MboyTrkMS_Cov54;   //!
   TBranch        *b_MboyTrkMS_Cov55;   //!
   TBranch        *b_MboyTrkMS_Xpt;   //!
   TBranch        *b_MboyTrkMS_Ypt;   //!
   TBranch        *b_MboyTrkMS_Zpt;   //!
   TBranch        *b_MboyTrkMS_Xvec;   //!
   TBranch        *b_MboyTrkMS_Yvec;   //!
   TBranch        *b_MboyTrkMS_Zvec;   //!
   TBranch        *b_MboyTrkMS_Mom;   //!
   TBranch        *b_MboyTrkMS_ZCyl;   //!
   TBranch        *b_MboyTrkMS_RCyl;   //!
   TBranch        *b_MboyTrkMS_Chican;   //!
   TBranch        *b_MboyTrkMS_Chifit;   //!
   TBranch        *b_MboyTrkMS_Chiver;   //!
   TBranch        *b_MboyTrkMS_Chistat;   //!
   TBranch        *b_MboyTrkMS_Ndof;   //!
   TBranch        *b_MboyAdjT0DefaultTShift;   //!
   TBranch        *b_MboyAdjT0NberOfData;   //!
   TBranch        *b_MboyAdjT0stationName;   //!
   TBranch        *b_MboyAdjT0stationEta;   //!
   TBranch        *b_MboyAdjT0stationPhi;   //!
   TBranch        *b_MboyAdjT0TShift;   //!
   TBranch        *b_staco_nmuon;   //!
   TBranch        *b_staco_A0;   //!
   TBranch        *b_staco_Z;   //!
   TBranch        *b_staco_Phi;   //!
   TBranch        *b_staco_Theta;   //!
   TBranch        *b_staco_qOverP;   //!
   TBranch        *b_staco_chi2;   //!
   TBranch        *b_staco_Ndf;   //!
   TBranch        *b_staco_covr11;   //!
   TBranch        *b_staco_covr21;   //!
   TBranch        *b_staco_covr22;   //!
   TBranch        *b_staco_covr31;   //!
   TBranch        *b_staco_covr32;   //!
   TBranch        *b_staco_covr33;   //!
   TBranch        *b_staco_covr41;   //!
   TBranch        *b_staco_covr42;   //!
   TBranch        *b_staco_covr43;   //!
   TBranch        *b_staco_covr44;   //!
   TBranch        *b_staco_covr51;   //!
   TBranch        *b_staco_covr52;   //!
   TBranch        *b_staco_covr53;   //!
   TBranch        *b_staco_covr54;   //!
   TBranch        *b_staco_covr55;   //!
   TBranch        *b_staco_A0ID;   //!
   TBranch        *b_staco_ZID;   //!
   TBranch        *b_staco_PhiID;   //!
   TBranch        *b_staco_ThetaID;   //!
   TBranch        *b_staco_qOverPID;   //!
   TBranch        *b_staco_chi2ID;   //!
   TBranch        *b_staco_NdfID;   //!
   TBranch        *b_staco_covr11ID;   //!
   TBranch        *b_staco_covr12ID;   //!
   TBranch        *b_staco_covr22ID;   //!
   TBranch        *b_staco_covr13ID;   //!
   TBranch        *b_staco_covr23ID;   //!
   TBranch        *b_staco_covr33ID;   //!
   TBranch        *b_staco_covr14ID;   //!
   TBranch        *b_staco_covr24ID;   //!
   TBranch        *b_staco_covr34ID;   //!
   TBranch        *b_staco_covr44ID;   //!
   TBranch        *b_staco_covr15ID;   //!
   TBranch        *b_staco_covr25ID;   //!
   TBranch        *b_staco_covr35ID;   //!
   TBranch        *b_staco_covr45ID;   //!
   TBranch        *b_staco_covr55ID;   //!
   TBranch        *b_staco_A0MS;   //!
   TBranch        *b_staco_ZMS;   //!
   TBranch        *b_staco_PhiMS;   //!
   TBranch        *b_staco_ThetaMS;   //!
   TBranch        *b_staco_qOverPMS;   //!
   TBranch        *b_staco_chi2MS;   //!
   TBranch        *b_staco_NdfMS;   //!
   TBranch        *b_staco_covr11MS;   //!
   TBranch        *b_staco_covr12MS;   //!
   TBranch        *b_staco_covr22MS;   //!
   TBranch        *b_staco_covr13MS;   //!
   TBranch        *b_staco_covr23MS;   //!
   TBranch        *b_staco_covr33MS;   //!
   TBranch        *b_staco_covr14MS;   //!
   TBranch        *b_staco_covr24MS;   //!
   TBranch        *b_staco_covr34MS;   //!
   TBranch        *b_staco_covr44MS;   //!
   TBranch        *b_staco_covr15MS;   //!
   TBranch        *b_staco_covr25MS;   //!
   TBranch        *b_staco_covr35MS;   //!
   TBranch        *b_staco_covr45MS;   //!
   TBranch        *b_staco_covr55MS;   //!
   TBranch        *b_staco_xMSE;   //!
   TBranch        *b_staco_yMSE;   //!
   TBranch        *b_staco_zMSE;   //!
   TBranch        *b_staco_PxMSE;   //!
   TBranch        *b_staco_PyMSE;   //!
   TBranch        *b_staco_PzMSE;   //!
   TBranch        *b_staco_chi2MSE;   //!
   TBranch        *b_staco_NdfMSE;   //!
   TBranch        *b_staco_etcone10;   //!
   TBranch        *b_staco_etcone20;   //!
   TBranch        *b_staco_etcone30;   //!
   TBranch        *b_staco_etcone40;   //!
   TBranch        *b_staco_nucone10;   //!
   TBranch        *b_staco_nucone20;   //!
   TBranch        *b_staco_nucone30;   //!
   TBranch        *b_staco_nucone40;   //!
   TBranch        *b_staco_Eloss;   //!
   TBranch        *b_staco_ElossError;   //!
   TBranch        *b_StacoExtr_nmuon;   //!
   TBranch        *b_StacoExtr_A0;   //!
   TBranch        *b_StacoExtr_Z;   //!
   TBranch        *b_StacoExtr_Phi;   //!
   TBranch        *b_StacoExtr_Theta;   //!
   TBranch        *b_StacoExtr_qOverP;   //!
   TBranch        *b_StacoExtr_Chi2;   //!
   TBranch        *b_StacoExtr_Ndf;   //!
   TBranch        *b_StacoExtr_covr11;   //!
   TBranch        *b_StacoExtr_covr21;   //!
   TBranch        *b_StacoExtr_covr22;   //!
   TBranch        *b_StacoExtr_covr31;   //!
   TBranch        *b_StacoExtr_covr32;   //!
   TBranch        *b_StacoExtr_covr33;   //!
   TBranch        *b_StacoExtr_covr41;   //!
   TBranch        *b_StacoExtr_covr42;   //!
   TBranch        *b_StacoExtr_covr43;   //!
   TBranch        *b_StacoExtr_covr44;   //!
   TBranch        *b_StacoExtr_covr51;   //!
   TBranch        *b_StacoExtr_covr52;   //!
   TBranch        *b_StacoExtr_covr53;   //!
   TBranch        *b_StacoExtr_covr54;   //!
   TBranch        *b_StacoExtr_covr55;   //!
   TBranch        *b_StacoExtr_etcone10;   //!
   TBranch        *b_StacoExtr_etcone20;   //!
   TBranch        *b_StacoExtr_etcone30;   //!
   TBranch        *b_StacoExtr_etcone40;   //!
   TBranch        *b_StacoExtr_nucone10;   //!
   TBranch        *b_StacoExtr_nucone20;   //!
   TBranch        *b_StacoExtr_nucone30;   //!
   TBranch        *b_StacoExtr_nucone40;   //!
   TBranch        *b_StacoExtr_Eloss;   //!
   TBranch        *b_StacoExtr_ElossError;   //!
   TBranch        *b_MboyESD_nmuon;   //!
   TBranch        *b_MboyESD_A0;   //!
   TBranch        *b_MboyESD_Z;   //!
   TBranch        *b_MboyESD_Phi;   //!
   TBranch        *b_MboyESD_Theta;   //!
   TBranch        *b_MboyESD_qOverP;   //!
   TBranch        *b_MboyESD_Chi2;   //!
   TBranch        *b_MboyESD_Ndf;   //!
   TBranch        *b_MboyESD_covr11;   //!
   TBranch        *b_MboyESD_covr21;   //!
   TBranch        *b_MboyESD_covr22;   //!
   TBranch        *b_MboyESD_covr31;   //!
   TBranch        *b_MboyESD_covr32;   //!
   TBranch        *b_MboyESD_covr33;   //!
   TBranch        *b_MboyESD_covr41;   //!
   TBranch        *b_MboyESD_covr42;   //!
   TBranch        *b_MboyESD_covr43;   //!
   TBranch        *b_MboyESD_covr44;   //!
   TBranch        *b_MboyESD_covr51;   //!
   TBranch        *b_MboyESD_covr52;   //!
   TBranch        *b_MboyESD_covr53;   //!
   TBranch        *b_MboyESD_covr54;   //!
   TBranch        *b_MboyESD_covr55;   //!
   TBranch        *b_MboyESD_etcone10;   //!
   TBranch        *b_MboyESD_etcone20;   //!
   TBranch        *b_MboyESD_etcone30;   //!
   TBranch        *b_MboyESD_etcone40;   //!
   TBranch        *b_MboyESD_nucone10;   //!
   TBranch        *b_MboyESD_nucone20;   //!
   TBranch        *b_MboyESD_nucone30;   //!
   TBranch        *b_MboyESD_nucone40;   //!
   TBranch        *b_MboyESD_Eloss;   //!
   TBranch        *b_MboyESD_ElossError;   //!
   TBranch        *b_MuTag_nmuon;   //!
   TBranch        *b_MuTag_A0;   //!
   TBranch        *b_MuTag_Z;   //!
   TBranch        *b_MuTag_Phi;   //!
   TBranch        *b_MuTag_Theta;   //!
   TBranch        *b_MuTag_qOverP;   //!
   TBranch        *b_MuTag_segmentChi2OverDoF;   //!
   TBranch        *b_MuTag_segmentDeltaPhi;   //!
   TBranch        *b_MuTag_segmentDeltaEta;   //!
   TBranch        *b_MuTag_innAngle;   //!
   TBranch        *b_MuTag_midAngle;   //!
   TBranch        *b_MuTag_numberOfMDTHits;   //!
   TBranch        *b_MuTag_numberOfRPCPhiHits;   //!
   TBranch        *b_MuTag_numberOfTGCPhiHits;   //!
   TBranch        *b_MuTag_MuonQuality;   //!
   TBranch        *b_MuTag_IsInner;   //!
   TBranch        *b_MuTag_SegMult;   //!
   TBranch        *b_MuTag_StationTypeSglTag;   //!
   TBranch        *b_MuTag_covr11;   //!
   TBranch        *b_MuTag_covr21;   //!
   TBranch        *b_MuTag_covr22;   //!
   TBranch        *b_MuTag_covr31;   //!
   TBranch        *b_MuTag_covr32;   //!
   TBranch        *b_MuTag_covr33;   //!
   TBranch        *b_MuTag_covr41;   //!
   TBranch        *b_MuTag_covr42;   //!
   TBranch        *b_MuTag_covr43;   //!
   TBranch        *b_MuTag_covr44;   //!
   TBranch        *b_MuTag_covr51;   //!
   TBranch        *b_MuTag_covr52;   //!
   TBranch        *b_MuTag_covr53;   //!
   TBranch        *b_MuTag_covr54;   //!
   TBranch        *b_MuTag_covr55;   //!
   TBranch        *b_MuTag_MatchChi2;   //!
   TBranch        *b_MuTag_MatchNdof;   //!
   TBranch        *b_MemRSS;   //!
   TBranch        *b_MemVSize;   //!
   TBranch        *b_TimeTotal;   //!
   TBranch        *b_TimeSum;   //!
   TBranch        *b_TimeAthMasterSeq;   //!

   AANTEvent(TTree *tree=0);
   virtual ~AANTEvent();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef AANTEvent_cxx
AANTEvent::AANTEvent(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("ntuple.root");
      if (!f) {
         f = new TFile("ntuple.root");
      }
      tree = (TTree*)gDirectory->Get("CollectionTree");

   }
   Init(tree);
}

AANTEvent::~AANTEvent()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t AANTEvent::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t AANTEvent::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void AANTEvent::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   LVL1TriggerInfo = 0;
   LVL2TriggerInfo = 0;
   EventFilterInfo = 0;
   StreamTagName = 0;
   StreamTagType = 0;
   Type = 0;
   PtGen = 0;
   PhiGen = 0;
   EtaGen = 0;
   MGen = 0;
   Charge = 0;
   GenStat = 0;
   GenRef = 0;
   KMothNt = 0;
   KFDauNt = 0;
   KLDauNt = 0;
   KOriVNt = 0;
   KEndVNt = 0;
   BunchNum = 0;
   RVGen = 0;
   PhiVGen = 0;
   ZVGen = 0;
   BunchV = 0;
   RV = 0;
   PhiV = 0;
   ZV = 0;
   GenRefV = 0;
   KVMothNt = 0;
   KVFDauNt = 0;
   KVLDauNt = 0;
   MuonEntRec_ID = 0;
   MuonEntRec_Ene = 0;
   MuonEntRec_Eta = 0;
   MuonEntRec_Phi = 0;
   MuonEntRec_PosX = 0;
   MuonEntRec_PosY = 0;
   MuonEntRec_PosZ = 0;
   MuonEntRec_Time = 0;
   MuonEntRec_BarCode = 0;
   vxp_vtx_x = 0;
   vxp_vtx_y = 0;
   vxp_vtx_z = 0;
   vxp_vtx_x_truth = 0;
   vxp_vtx_y_truth = 0;
   vxp_vtx_z_truth = 0;
   vxp_vtx_x_res = 0;
   vxp_vtx_y_res = 0;
   vxp_vtx_z_res = 0;
   vxp_sigvtx_x = 0;
   vxp_sigvtx_y = 0;
   vxp_sigvtx_z = 0;
   vxp_vtxchi2 = 0;
   vxp_vtxndf = 0;
   vxp_vtx_constraint_x = 0;
   vxp_vtx_constraint_y = 0;
   vxp_vtx_constraint_z = 0;
   vxp_sigvtx_constraint_x = 0;
   vxp_sigvtx_constraint_y = 0;
   vxp_sigvtx_constraint_z = 0;
   vxp_vtx_linearization_x = 0;
   vxp_vtx_linearization_y = 0;
   vxp_vtx_linearization_z = 0;
   vxp_vtx_seed_x = 0;
   vxp_vtx_seed_y = 0;
   vxp_vtx_seed_z = 0;
   vxp_vtxchi2prob = 0;
   vxp_numTracksPerVertex = 0;
   vxp_chi2 = 0;
   vxp_ndf = 0;
   vxp_vertexCompatibility = 0;
   vxp_d0 = 0;
   vxp_sigd0 = 0;
   vxp_z0 = 0;
   vxp_sigz0 = 0;
   vxp_phi = 0;
   vxp_sigphi = 0;
   vxp_theta = 0;
   vxp_sigtheta = 0;
   vxp_qOverP = 0;
   vxp_sigqOverP = 0;
   vxp_d0g = 0;
   vxp_sigd0g = 0;
   vxp_z0g = 0;
   vxp_sigz0g = 0;
   vxp_refitted_d0 = 0;
   vxp_refitted_sigd0 = 0;
   vxp_refitted_z0 = 0;
   vxp_refitted_sigz0 = 0;
   vxp_refitted_phi = 0;
   vxp_refitted_sigphi = 0;
   vxp_refitted_theta = 0;
   vxp_refitted_sigtheta = 0;
   vxp_refitted_qOverP = 0;
   vxp_refitted_sigqOverP = 0;
   Trk_d0 = 0;
   Trk_z0 = 0;
   Trk_phi = 0;
   Trk_theta = 0;
   Trk_qOverP = 0;
   Trk_Chi2 = 0;
   Trk_Ndf = 0;
   Trk_sigd0 = 0;
   Trk_sigz0 = 0;
   Trk_sigphi = 0;
   Trk_sigtheta = 0;
   Trk_sigqOverP = 0;
   Trk_covVert21 = 0;
   Trk_covVert31 = 0;
   Trk_covVert32 = 0;
   Trk_covVert41 = 0;
   Trk_covVert42 = 0;
   Trk_covVert43 = 0;
   Trk_covVert51 = 0;
   Trk_covVert52 = 0;
   Trk_covVert53 = 0;
   Trk_covVert54 = 0;
   Trk_px = 0;
   Trk_py = 0;
   Trk_pz = 0;
   Trk_pt = 0;
   Trk_numberOfBLayerHits = 0;
   Trk_numberOfPixelHits = 0;
   Trk_numberOfSCTHits = 0;
   Trk_numberOfTRTHits = 0;
   Trk_numberOfTRTHighThresholdHits = 0;
   Trk_numberOfBLayerSharedHits = 0;
   Trk_numberOfPixelSharedHits = 0;
   Trk_numberOfPixelHoles = 0;
   Trk_numberOfSCTSharedHits = 0;
   Trk_numberOfSCTHoles = 0;
   Trk_numberOfTRTOutliers = 0;
   Trk_numberOfTRTHighThresholdOutliers = 0;
   Trk_numberOfMdtHits = 0;
   Trk_numberOfTgcPhiHits = 0;
   Trk_numberOfRpcPhiHits = 0;
   Trk_numberOfCscPhiHits = 0;
   Trk_numberOfTgcEtaHits = 0;
   Trk_numberOfRpcEtaHits = 0;
   Trk_numberOfCscEtaHits = 0;
   Trk_numberOfGangedPixels = 0;
   Trk_numberOfOutliersOnTrack = 0;
   Trk_truthBarcode = 0;
   Trk_truthNt = 0;
   Trk_ParticlePdg = 0;
   Trk_ParentPdg = 0;
   MboySeg_Xpt = 0;
   MboySeg_Ypt = 0;
   MboySeg_Zpt = 0;
   MboySeg_Xvec = 0;
   MboySeg_Yvec = 0;
   MboySeg_Zvec = 0;
   MboySeg_QualityFactor = 0;
   MboySeg_NbreOfMultiLayer = 0;
   MboySeg_TrackSharing = 0;
   MboySeg_StationName = 0;
   MboySeg_Ndigits = 0;
   MboyTrkVT_Va = 0;
   MboyTrkVT_A0 = 0;
   MboyTrkVT_Z = 0;
   MboyTrkVT_Phi = 0;
   MboyTrkVT_CotTh = 0;
   MboyTrkVT_PTInv = 0;
   MboyTrkVT_Cov11 = 0;
   MboyTrkVT_Cov21 = 0;
   MboyTrkVT_Cov22 = 0;
   MboyTrkVT_Cov31 = 0;
   MboyTrkVT_Cov32 = 0;
   MboyTrkVT_Cov33 = 0;
   MboyTrkVT_Cov41 = 0;
   MboyTrkVT_Cov42 = 0;
   MboyTrkVT_Cov43 = 0;
   MboyTrkVT_Cov44 = 0;
   MboyTrkVT_Cov51 = 0;
   MboyTrkVT_Cov52 = 0;
   MboyTrkVT_Cov53 = 0;
   MboyTrkVT_Cov54 = 0;
   MboyTrkVT_Cov55 = 0;
   MboyTrkVT_Xpt = 0;
   MboyTrkVT_Ypt = 0;
   MboyTrkVT_Zpt = 0;
   MboyTrkVT_Xvec = 0;
   MboyTrkVT_Yvec = 0;
   MboyTrkVT_Zvec = 0;
   MboyTrkVT_Mom = 0;
   MboyTrkVT_digits = 0;
   MboyTrkVT_HitPosition = 0;
   MboyTrkVT_TrHi_Rdigi = 0;
   MboyTrkVT_TrHi_Rtrack = 0;
   MboyTrkVT_TrHi_Xdca = 0;
   MboyTrkVT_TrHi_Ydca = 0;
   MboyTrkVT_TrHi_Zdca = 0;
   MboyTrkVT_TrHi_Sdca = 0;
   MboyTrkVT_TrHi_Reso = 0;
   MboyTrkVT_TrHi_RZSsim = 0;
   MboyTrkVT_TrHi_EZSfit = 0;
   MboyTrkVT_TrHi_JTYP = 0;
   MboyTrkVT_TrHi_JFF = 0;
   MboyTrkVT_TrHi_JZZ = 0;
   MboyTrkVT_TrHi_JOB = 0;
   MboyTrkVT_TrHi_JSPLI = 0;
   MboyTrkVT_TrHi_JSL = 0;
   MboyTrkVT_TrHi_JTUBE = 0;
   MboyTrkVT_TrHi_JSZ = 0;
   MboyTrkVT_TrHi_JSTRI = 0;
   MboyTrkVT_TrHi_DriftTime = 0;
   MboyTrkVT_TrHi_ChamberTech = 0;
   MboyTrkVT_TrHi_StationNber = 0;
   MboyTrkVT_TrHi_StationEta = 0;
   MboyTrkVT_TrHi_StationPhi = 0;
   MboyTrkVT_TrHi_Multilayer = 0;
   MboyTrkVT_TrHi_TubeLayer = 0;
   MboyTrkVT_TrHi_Tube = 0;
   MboyTrkVT_TrHi_DoubletR = 0;
   MboyTrkVT_TrHi_DoubletZ = 0;
   MboyTrkVT_TrHi_DoubletPhi = 0;
   MboyTrkVT_TrHi_GasGap = 0;
   MboyTrkVT_TrHi_MeasuresPhi = 0;
   MboyTrkVT_TrHi_Strip = 0;
   MboyTrkVT_TrHi_IsStrip = 0;
   MboyTrkVT_TrHi_Channel = 0;
   MboyTrkVT_TrHi_ChamberLayer = 0;
   MboyTrkVT_TrHi_WireLayer = 0;
   MboyTrkVT_TrHi_RpcTime = 0;
   MboyTrkKE_Va = 0;
   MboyTrkKE_Thept = 0;
   MboyTrkKE_Phipt = 0;
   MboyTrkKE_Thevc = 0;
   MboyTrkKE_Phivc = 0;
   MboyTrkKE_PInv = 0;
   MboyTrkKE_Cov11 = 0;
   MboyTrkKE_Cov21 = 0;
   MboyTrkKE_Cov22 = 0;
   MboyTrkKE_Cov31 = 0;
   MboyTrkKE_Cov32 = 0;
   MboyTrkKE_Cov33 = 0;
   MboyTrkKE_Cov41 = 0;
   MboyTrkKE_Cov42 = 0;
   MboyTrkKE_Cov43 = 0;
   MboyTrkKE_Cov44 = 0;
   MboyTrkKE_Cov51 = 0;
   MboyTrkKE_Cov52 = 0;
   MboyTrkKE_Cov53 = 0;
   MboyTrkKE_Cov54 = 0;
   MboyTrkKE_Cov55 = 0;
   MboyTrkKE_Xpt = 0;
   MboyTrkKE_Ypt = 0;
   MboyTrkKE_Zpt = 0;
   MboyTrkKE_Xvec = 0;
   MboyTrkKE_Yvec = 0;
   MboyTrkKE_Zvec = 0;
   MboyTrkKE_Mom = 0;
   MboyTrkKE_ZCyl = 0;
   MboyTrkKE_RCyl = 0;
   MboyTrkMS_Va = 0;
   MboyTrkMS_Thept = 0;
   MboyTrkMS_Phipt = 0;
   MboyTrkMS_Thevc = 0;
   MboyTrkMS_Phivc = 0;
   MboyTrkMS_PInv = 0;
   MboyTrkMS_Cov11 = 0;
   MboyTrkMS_Cov21 = 0;
   MboyTrkMS_Cov22 = 0;
   MboyTrkMS_Cov31 = 0;
   MboyTrkMS_Cov32 = 0;
   MboyTrkMS_Cov33 = 0;
   MboyTrkMS_Cov41 = 0;
   MboyTrkMS_Cov42 = 0;
   MboyTrkMS_Cov43 = 0;
   MboyTrkMS_Cov44 = 0;
   MboyTrkMS_Cov51 = 0;
   MboyTrkMS_Cov52 = 0;
   MboyTrkMS_Cov53 = 0;
   MboyTrkMS_Cov54 = 0;
   MboyTrkMS_Cov55 = 0;
   MboyTrkMS_Xpt = 0;
   MboyTrkMS_Ypt = 0;
   MboyTrkMS_Zpt = 0;
   MboyTrkMS_Xvec = 0;
   MboyTrkMS_Yvec = 0;
   MboyTrkMS_Zvec = 0;
   MboyTrkMS_Mom = 0;
   MboyTrkMS_ZCyl = 0;
   MboyTrkMS_RCyl = 0;
   MboyTrkMS_Chican = 0;
   MboyTrkMS_Chifit = 0;
   MboyTrkMS_Chiver = 0;
   MboyTrkMS_Chistat = 0;
   MboyTrkMS_Ndof = 0;
   MboyAdjT0stationName = 0;
   MboyAdjT0stationEta = 0;
   MboyAdjT0stationPhi = 0;
   MboyAdjT0TShift = 0;
   staco_A0 = 0;
   staco_Z = 0;
   staco_Phi = 0;
   staco_Theta = 0;
   staco_qOverP = 0;
   staco_chi2 = 0;
   staco_Ndf = 0;
   staco_covr11 = 0;
   staco_covr21 = 0;
   staco_covr22 = 0;
   staco_covr31 = 0;
   staco_covr32 = 0;
   staco_covr33 = 0;
   staco_covr41 = 0;
   staco_covr42 = 0;
   staco_covr43 = 0;
   staco_covr44 = 0;
   staco_covr51 = 0;
   staco_covr52 = 0;
   staco_covr53 = 0;
   staco_covr54 = 0;
   staco_covr55 = 0;
   staco_A0ID = 0;
   staco_ZID = 0;
   staco_PhiID = 0;
   staco_ThetaID = 0;
   staco_qOverPID = 0;
   staco_chi2ID = 0;
   staco_NdfID = 0;
   staco_covr11ID = 0;
   staco_covr12ID = 0;
   staco_covr22ID = 0;
   staco_covr13ID = 0;
   staco_covr23ID = 0;
   staco_covr33ID = 0;
   staco_covr14ID = 0;
   staco_covr24ID = 0;
   staco_covr34ID = 0;
   staco_covr44ID = 0;
   staco_covr15ID = 0;
   staco_covr25ID = 0;
   staco_covr35ID = 0;
   staco_covr45ID = 0;
   staco_covr55ID = 0;
   staco_A0MS = 0;
   staco_ZMS = 0;
   staco_PhiMS = 0;
   staco_ThetaMS = 0;
   staco_qOverPMS = 0;
   staco_chi2MS = 0;
   staco_NdfMS = 0;
   staco_covr11MS = 0;
   staco_covr12MS = 0;
   staco_covr22MS = 0;
   staco_covr13MS = 0;
   staco_covr23MS = 0;
   staco_covr33MS = 0;
   staco_covr14MS = 0;
   staco_covr24MS = 0;
   staco_covr34MS = 0;
   staco_covr44MS = 0;
   staco_covr15MS = 0;
   staco_covr25MS = 0;
   staco_covr35MS = 0;
   staco_covr45MS = 0;
   staco_covr55MS = 0;
   staco_xMSE = 0;
   staco_yMSE = 0;
   staco_zMSE = 0;
   staco_PxMSE = 0;
   staco_PyMSE = 0;
   staco_PzMSE = 0;
   staco_chi2MSE = 0;
   staco_NdfMSE = 0;
   staco_etcone10 = 0;
   staco_etcone20 = 0;
   staco_etcone30 = 0;
   staco_etcone40 = 0;
   staco_nucone10 = 0;
   staco_nucone20 = 0;
   staco_nucone30 = 0;
   staco_nucone40 = 0;
   staco_Eloss = 0;
   staco_ElossError = 0;
   StacoExtr_A0 = 0;
   StacoExtr_Z = 0;
   StacoExtr_Phi = 0;
   StacoExtr_Theta = 0;
   StacoExtr_qOverP = 0;
   StacoExtr_Chi2 = 0;
   StacoExtr_Ndf = 0;
   StacoExtr_covr11 = 0;
   StacoExtr_covr21 = 0;
   StacoExtr_covr22 = 0;
   StacoExtr_covr31 = 0;
   StacoExtr_covr32 = 0;
   StacoExtr_covr33 = 0;
   StacoExtr_covr41 = 0;
   StacoExtr_covr42 = 0;
   StacoExtr_covr43 = 0;
   StacoExtr_covr44 = 0;
   StacoExtr_covr51 = 0;
   StacoExtr_covr52 = 0;
   StacoExtr_covr53 = 0;
   StacoExtr_covr54 = 0;
   StacoExtr_covr55 = 0;
   StacoExtr_etcone10 = 0;
   StacoExtr_etcone20 = 0;
   StacoExtr_etcone30 = 0;
   StacoExtr_etcone40 = 0;
   StacoExtr_nucone10 = 0;
   StacoExtr_nucone20 = 0;
   StacoExtr_nucone30 = 0;
   StacoExtr_nucone40 = 0;
   StacoExtr_Eloss = 0;
   StacoExtr_ElossError = 0;
   MboyESD_A0 = 0;
   MboyESD_Z = 0;
   MboyESD_Phi = 0;
   MboyESD_Theta = 0;
   MboyESD_qOverP = 0;
   MboyESD_Chi2 = 0;
   MboyESD_Ndf = 0;
   MboyESD_covr11 = 0;
   MboyESD_covr21 = 0;
   MboyESD_covr22 = 0;
   MboyESD_covr31 = 0;
   MboyESD_covr32 = 0;
   MboyESD_covr33 = 0;
   MboyESD_covr41 = 0;
   MboyESD_covr42 = 0;
   MboyESD_covr43 = 0;
   MboyESD_covr44 = 0;
   MboyESD_covr51 = 0;
   MboyESD_covr52 = 0;
   MboyESD_covr53 = 0;
   MboyESD_covr54 = 0;
   MboyESD_covr55 = 0;
   MboyESD_etcone10 = 0;
   MboyESD_etcone20 = 0;
   MboyESD_etcone30 = 0;
   MboyESD_etcone40 = 0;
   MboyESD_nucone10 = 0;
   MboyESD_nucone20 = 0;
   MboyESD_nucone30 = 0;
   MboyESD_nucone40 = 0;
   MboyESD_Eloss = 0;
   MboyESD_ElossError = 0;
   MuTag_A0 = 0;
   MuTag_Z = 0;
   MuTag_Phi = 0;
   MuTag_Theta = 0;
   MuTag_qOverP = 0;
   MuTag_segmentChi2OverDoF = 0;
   MuTag_segmentDeltaPhi = 0;
   MuTag_segmentDeltaEta = 0;
   MuTag_innAngle = 0;
   MuTag_midAngle = 0;
   MuTag_numberOfMDTHits = 0;
   MuTag_numberOfRPCPhiHits = 0;
   MuTag_numberOfTGCPhiHits = 0;
   MuTag_MuonQuality = 0;
   MuTag_IsInner = 0;
   MuTag_SegMult = 0;
   MuTag_StationTypeSglTag = 0;
   MuTag_covr11 = 0;
   MuTag_covr21 = 0;
   MuTag_covr22 = 0;
   MuTag_covr31 = 0;
   MuTag_covr32 = 0;
   MuTag_covr33 = 0;
   MuTag_covr41 = 0;
   MuTag_covr42 = 0;
   MuTag_covr43 = 0;
   MuTag_covr44 = 0;
   MuTag_covr51 = 0;
   MuTag_covr52 = 0;
   MuTag_covr53 = 0;
   MuTag_covr54 = 0;
   MuTag_covr55 = 0;
   MuTag_MatchChi2 = 0;
   MuTag_MatchNdof = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("Token", Token, &b_Token);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("LumiBlock", &LumiBlock, &b_LumiBlock);
   fChain->SetBranchAddress("BCID", &BCID, &b_BCID);
   fChain->SetBranchAddress("LVL1ID", &LVL1ID, &b_LVL1ID);
   fChain->SetBranchAddress("Weight", &Weight, &b_Weight);
   fChain->SetBranchAddress("IEvent", &IEvent, &b_IEvent);
   fChain->SetBranchAddress("StatusElement", &StatusElement, &b_StatusElement);
   fChain->SetBranchAddress("LVL1TriggerType", &LVL1TriggerType, &b_LVL1TriggerType);
   fChain->SetBranchAddress("LVL1TriggerInfo", &LVL1TriggerInfo, &b_LVL1TriggerInfo);
   fChain->SetBranchAddress("LVL2TriggerInfo", &LVL2TriggerInfo, &b_LVL2TriggerInfo);
   fChain->SetBranchAddress("EventFilterInfo", &EventFilterInfo, &b_EventFilterInfo);
   fChain->SetBranchAddress("StreamTagName", &StreamTagName, &b_StreamTagName);
   fChain->SetBranchAddress("StreamTagType", &StreamTagType, &b_StreamTagType);
   fChain->SetBranchAddress("NPar", &NPar, &b_NPar);
   fChain->SetBranchAddress("Type", &Type, &b_Type);
   fChain->SetBranchAddress("PtGen", &PtGen, &b_PtGen);
   fChain->SetBranchAddress("PhiGen", &PhiGen, &b_PhiGen);
   fChain->SetBranchAddress("EtaGen", &EtaGen, &b_EtaGen);
   fChain->SetBranchAddress("MGen", &MGen, &b_MGen);
   fChain->SetBranchAddress("Charge", &Charge, &b_Charge);
   fChain->SetBranchAddress("GenStat", &GenStat, &b_GenStat);
   fChain->SetBranchAddress("GenRef", &GenRef, &b_GenRef);
   fChain->SetBranchAddress("KMothNt", &KMothNt, &b_KMothNt);
   fChain->SetBranchAddress("KFDauNt", &KFDauNt, &b_KFDauNt);
   fChain->SetBranchAddress("KLDauNt", &KLDauNt, &b_KLDauNt);
   fChain->SetBranchAddress("KOriVNt", &KOriVNt, &b_KOriVNt);
   fChain->SetBranchAddress("KEndVNt", &KEndVNt, &b_KEndVNt);
   fChain->SetBranchAddress("BunchNum", &BunchNum, &b_BunchNum);
   fChain->SetBranchAddress("RVGen", &RVGen, &b_RVGen);
   fChain->SetBranchAddress("PhiVGen", &PhiVGen, &b_PhiVGen);
   fChain->SetBranchAddress("ZVGen", &ZVGen, &b_ZVGen);
   fChain->SetBranchAddress("NVer", &NVer, &b_NVer);
   fChain->SetBranchAddress("BunchV", &BunchV, &b_BunchV);
   fChain->SetBranchAddress("RV", &RV, &b_RV);
   fChain->SetBranchAddress("PhiV", &PhiV, &b_PhiV);
   fChain->SetBranchAddress("ZV", &ZV, &b_ZV);
   fChain->SetBranchAddress("GenRefV", &GenRefV, &b_GenRefV);
   fChain->SetBranchAddress("KVMothNt", &KVMothNt, &b_KVMothNt);
   fChain->SetBranchAddress("KVFDauNt", &KVFDauNt, &b_KVFDauNt);
   fChain->SetBranchAddress("KVLDauNt", &KVLDauNt, &b_KVLDauNt);
   fChain->SetBranchAddress("IVPrimary", &IVPrimary, &b_IVPrimary);
   fChain->SetBranchAddress("MuonEntRec_Num", &MuonEntRec_Num, &b_MuonEntRec_Num);
   fChain->SetBranchAddress("MuonEntRec_ID", &MuonEntRec_ID, &b_MuonEntRec_ID);
   fChain->SetBranchAddress("MuonEntRec_Ene", &MuonEntRec_Ene, &b_MuonEntRec_Ene);
   fChain->SetBranchAddress("MuonEntRec_Eta", &MuonEntRec_Eta, &b_MuonEntRec_Eta);
   fChain->SetBranchAddress("MuonEntRec_Phi", &MuonEntRec_Phi, &b_MuonEntRec_Phi);
   fChain->SetBranchAddress("MuonEntRec_PosX", &MuonEntRec_PosX, &b_MuonEntRec_PosX);
   fChain->SetBranchAddress("MuonEntRec_PosY", &MuonEntRec_PosY, &b_MuonEntRec_PosY);
   fChain->SetBranchAddress("MuonEntRec_PosZ", &MuonEntRec_PosZ, &b_MuonEntRec_PosZ);
   fChain->SetBranchAddress("MuonEntRec_Time", &MuonEntRec_Time, &b_MuonEntRec_Time);
   fChain->SetBranchAddress("MuonEntRec_BarCode", &MuonEntRec_BarCode, &b_MuonEntRec_BarCode);
   fChain->SetBranchAddress("vxp_numVertices", &vxp_numVertices, &b_vxp_numVertices);
   fChain->SetBranchAddress("vxp_vtx_x", &vxp_vtx_x, &b_vxp_vtx_x);
   fChain->SetBranchAddress("vxp_vtx_y", &vxp_vtx_y, &b_vxp_vtx_y);
   fChain->SetBranchAddress("vxp_vtx_z", &vxp_vtx_z, &b_vxp_vtx_z);
   fChain->SetBranchAddress("vxp_numOfTruthVertices", &vxp_numOfTruthVertices, &b_vxp_numOfTruthVertices);
   fChain->SetBranchAddress("vxp_vtx_x_truth", &vxp_vtx_x_truth, &b_vxp_vtx_x_truth);
   fChain->SetBranchAddress("vxp_vtx_y_truth", &vxp_vtx_y_truth, &b_vxp_vtx_y_truth);
   fChain->SetBranchAddress("vxp_vtx_z_truth", &vxp_vtx_z_truth, &b_vxp_vtx_z_truth);
   fChain->SetBranchAddress("vxp_vtx_x_res", &vxp_vtx_x_res, &b_vxp_vtx_x_res);
   fChain->SetBranchAddress("vxp_vtx_y_res", &vxp_vtx_y_res, &b_vxp_vtx_y_res);
   fChain->SetBranchAddress("vxp_vtx_z_res", &vxp_vtx_z_res, &b_vxp_vtx_z_res);
   fChain->SetBranchAddress("vxp_sigvtx_x", &vxp_sigvtx_x, &b_vxp_sigvtx_x);
   fChain->SetBranchAddress("vxp_sigvtx_y", &vxp_sigvtx_y, &b_vxp_sigvtx_y);
   fChain->SetBranchAddress("vxp_sigvtx_z", &vxp_sigvtx_z, &b_vxp_sigvtx_z);
   fChain->SetBranchAddress("vxp_vtxchi2", &vxp_vtxchi2, &b_vxp_vtxchi2);
   fChain->SetBranchAddress("vxp_vtxndf", &vxp_vtxndf, &b_vxp_vtxndf);
   fChain->SetBranchAddress("vxp_vtx_constraint_x", &vxp_vtx_constraint_x, &b_vxp_vtx_constraint_x);
   fChain->SetBranchAddress("vxp_vtx_constraint_y", &vxp_vtx_constraint_y, &b_vxp_vtx_constraint_y);
   fChain->SetBranchAddress("vxp_vtx_constraint_z", &vxp_vtx_constraint_z, &b_vxp_vtx_constraint_z);
   fChain->SetBranchAddress("vxp_sigvtx_constraint_x", &vxp_sigvtx_constraint_x, &b_vxp_sigvtx_constraint_x);
   fChain->SetBranchAddress("vxp_sigvtx_constraint_y", &vxp_sigvtx_constraint_y, &b_vxp_sigvtx_constraint_y);
   fChain->SetBranchAddress("vxp_sigvtx_constraint_z", &vxp_sigvtx_constraint_z, &b_vxp_sigvtx_constraint_z);
   fChain->SetBranchAddress("vxp_vtx_linearization_x", &vxp_vtx_linearization_x, &b_vxp_vtx_linearization_x);
   fChain->SetBranchAddress("vxp_vtx_linearization_y", &vxp_vtx_linearization_y, &b_vxp_vtx_linearization_y);
   fChain->SetBranchAddress("vxp_vtx_linearization_z", &vxp_vtx_linearization_z, &b_vxp_vtx_linearization_z);
   fChain->SetBranchAddress("vxp_vtx_seed_x", &vxp_vtx_seed_x, &b_vxp_vtx_seed_x);
   fChain->SetBranchAddress("vxp_vtx_seed_y", &vxp_vtx_seed_y, &b_vxp_vtx_seed_y);
   fChain->SetBranchAddress("vxp_vtx_seed_z", &vxp_vtx_seed_z, &b_vxp_vtx_seed_z);
   fChain->SetBranchAddress("vxp_vtxchi2prob", &vxp_vtxchi2prob, &b_vxp_vtxchi2prob);
   fChain->SetBranchAddress("vxp_numTracksPerVertex", &vxp_numTracksPerVertex, &b_vxp_numTracksPerVertex);
   fChain->SetBranchAddress("vxp_totalNumTracks", &vxp_totalNumTracks, &b_vxp_totalNumTracks);
   fChain->SetBranchAddress("vxp_chi2", &vxp_chi2, &b_vxp_chi2);
   fChain->SetBranchAddress("vxp_ndf", &vxp_ndf, &b_vxp_ndf);
   fChain->SetBranchAddress("vxp_vertexCompatibility", &vxp_vertexCompatibility, &b_vxp_vertexCompatibility);
   fChain->SetBranchAddress("vxp_d0", &vxp_d0, &b_vxp_d0);
   fChain->SetBranchAddress("vxp_sigd0", &vxp_sigd0, &b_vxp_sigd0);
   fChain->SetBranchAddress("vxp_z0", &vxp_z0, &b_vxp_z0);
   fChain->SetBranchAddress("vxp_sigz0", &vxp_sigz0, &b_vxp_sigz0);
   fChain->SetBranchAddress("vxp_phi", &vxp_phi, &b_vxp_phi);
   fChain->SetBranchAddress("vxp_sigphi", &vxp_sigphi, &b_vxp_sigphi);
   fChain->SetBranchAddress("vxp_theta", &vxp_theta, &b_vxp_theta);
   fChain->SetBranchAddress("vxp_sigtheta", &vxp_sigtheta, &b_vxp_sigtheta);
   fChain->SetBranchAddress("vxp_qOverP", &vxp_qOverP, &b_vxp_qOverP);
   fChain->SetBranchAddress("vxp_sigqOverP", &vxp_sigqOverP, &b_vxp_sigqOverP);
   fChain->SetBranchAddress("vxp_d0g", &vxp_d0g, &b_vxp_d0g);
   fChain->SetBranchAddress("vxp_sigd0g", &vxp_sigd0g, &b_vxp_sigd0g);
   fChain->SetBranchAddress("vxp_z0g", &vxp_z0g, &b_vxp_z0g);
   fChain->SetBranchAddress("vxp_sigz0g", &vxp_sigz0g, &b_vxp_sigz0g);
   fChain->SetBranchAddress("vxp_refitted_d0", &vxp_refitted_d0, &b_vxp_refitted_d0);
   fChain->SetBranchAddress("vxp_refitted_sigd0", &vxp_refitted_sigd0, &b_vxp_refitted_sigd0);
   fChain->SetBranchAddress("vxp_refitted_z0", &vxp_refitted_z0, &b_vxp_refitted_z0);
   fChain->SetBranchAddress("vxp_refitted_sigz0", &vxp_refitted_sigz0, &b_vxp_refitted_sigz0);
   fChain->SetBranchAddress("vxp_refitted_phi", &vxp_refitted_phi, &b_vxp_refitted_phi);
   fChain->SetBranchAddress("vxp_refitted_sigphi", &vxp_refitted_sigphi, &b_vxp_refitted_sigphi);
   fChain->SetBranchAddress("vxp_refitted_theta", &vxp_refitted_theta, &b_vxp_refitted_theta);
   fChain->SetBranchAddress("vxp_refitted_sigtheta", &vxp_refitted_sigtheta, &b_vxp_refitted_sigtheta);
   fChain->SetBranchAddress("vxp_refitted_qOverP", &vxp_refitted_qOverP, &b_vxp_refitted_qOverP);
   fChain->SetBranchAddress("vxp_refitted_sigqOverP", &vxp_refitted_sigqOverP, &b_vxp_refitted_sigqOverP);
   fChain->SetBranchAddress("Trk_totalNumTracks", &Trk_totalNumTracks, &b_Trk_totalNumTracks);
   fChain->SetBranchAddress("Trk_d0", &Trk_d0, &b_Trk_d0);
   fChain->SetBranchAddress("Trk_z0", &Trk_z0, &b_Trk_z0);
   fChain->SetBranchAddress("Trk_phi", &Trk_phi, &b_Trk_phi);
   fChain->SetBranchAddress("Trk_theta", &Trk_theta, &b_Trk_theta);
   fChain->SetBranchAddress("Trk_qOverP", &Trk_qOverP, &b_Trk_qOverP);
   fChain->SetBranchAddress("Trk_Chi2", &Trk_Chi2, &b_Trk_Chi2);
   fChain->SetBranchAddress("Trk_Ndf", &Trk_Ndf, &b_Trk_Ndf);
   fChain->SetBranchAddress("Trk_sigd0", &Trk_sigd0, &b_Trk_sigd0);
   fChain->SetBranchAddress("Trk_sigz0", &Trk_sigz0, &b_Trk_sigz0);
   fChain->SetBranchAddress("Trk_sigphi", &Trk_sigphi, &b_Trk_sigphi);
   fChain->SetBranchAddress("Trk_sigtheta", &Trk_sigtheta, &b_Trk_sigtheta);
   fChain->SetBranchAddress("Trk_sigqOverP", &Trk_sigqOverP, &b_Trk_sigqOverP);
   fChain->SetBranchAddress("Trk_covVert21", &Trk_covVert21, &b_Trk_covVert21);
   fChain->SetBranchAddress("Trk_covVert31", &Trk_covVert31, &b_Trk_covVert31);
   fChain->SetBranchAddress("Trk_covVert32", &Trk_covVert32, &b_Trk_covVert32);
   fChain->SetBranchAddress("Trk_covVert41", &Trk_covVert41, &b_Trk_covVert41);
   fChain->SetBranchAddress("Trk_covVert42", &Trk_covVert42, &b_Trk_covVert42);
   fChain->SetBranchAddress("Trk_covVert43", &Trk_covVert43, &b_Trk_covVert43);
   fChain->SetBranchAddress("Trk_covVert51", &Trk_covVert51, &b_Trk_covVert51);
   fChain->SetBranchAddress("Trk_covVert52", &Trk_covVert52, &b_Trk_covVert52);
   fChain->SetBranchAddress("Trk_covVert53", &Trk_covVert53, &b_Trk_covVert53);
   fChain->SetBranchAddress("Trk_covVert54", &Trk_covVert54, &b_Trk_covVert54);
   fChain->SetBranchAddress("Trk_px", &Trk_px, &b_Trk_px);
   fChain->SetBranchAddress("Trk_py", &Trk_py, &b_Trk_py);
   fChain->SetBranchAddress("Trk_pz", &Trk_pz, &b_Trk_pz);
   fChain->SetBranchAddress("Trk_pt", &Trk_pt, &b_Trk_pt);
   fChain->SetBranchAddress("Trk_numberOfBLayerHits", &Trk_numberOfBLayerHits, &b_Trk_numberOfBLayerHits);
   fChain->SetBranchAddress("Trk_numberOfPixelHits", &Trk_numberOfPixelHits, &b_Trk_numberOfPixelHits);
   fChain->SetBranchAddress("Trk_numberOfSCTHits", &Trk_numberOfSCTHits, &b_Trk_numberOfSCTHits);
   fChain->SetBranchAddress("Trk_numberOfTRTHits", &Trk_numberOfTRTHits, &b_Trk_numberOfTRTHits);
   fChain->SetBranchAddress("Trk_numberOfTRTHighThresholdHits", &Trk_numberOfTRTHighThresholdHits, &b_Trk_numberOfTRTHighThresholdHits);
   fChain->SetBranchAddress("Trk_numberOfBLayerSharedHits", &Trk_numberOfBLayerSharedHits, &b_Trk_numberOfBLayerSharedHits);
   fChain->SetBranchAddress("Trk_numberOfPixelSharedHits", &Trk_numberOfPixelSharedHits, &b_Trk_numberOfPixelSharedHits);
   fChain->SetBranchAddress("Trk_numberOfPixelHoles", &Trk_numberOfPixelHoles, &b_Trk_numberOfPixelHoles);
   fChain->SetBranchAddress("Trk_numberOfSCTSharedHits", &Trk_numberOfSCTSharedHits, &b_Trk_numberOfSCTSharedHits);
   fChain->SetBranchAddress("Trk_numberOfSCTHoles", &Trk_numberOfSCTHoles, &b_Trk_numberOfSCTHoles);
   fChain->SetBranchAddress("Trk_numberOfTRTOutliers", &Trk_numberOfTRTOutliers, &b_Trk_numberOfTRTOutliers);
   fChain->SetBranchAddress("Trk_numberOfTRTHighThresholdOutliers", &Trk_numberOfTRTHighThresholdOutliers, &b_Trk_numberOfTRTHighThresholdOutliers);
   fChain->SetBranchAddress("Trk_numberOfMdtHits", &Trk_numberOfMdtHits, &b_Trk_numberOfMdtHits);
   fChain->SetBranchAddress("Trk_numberOfTgcPhiHits", &Trk_numberOfTgcPhiHits, &b_Trk_numberOfTgcPhiHits);
   fChain->SetBranchAddress("Trk_numberOfRpcPhiHits", &Trk_numberOfRpcPhiHits, &b_Trk_numberOfRpcPhiHits);
   fChain->SetBranchAddress("Trk_numberOfCscPhiHits", &Trk_numberOfCscPhiHits, &b_Trk_numberOfCscPhiHits);
   fChain->SetBranchAddress("Trk_numberOfTgcEtaHits", &Trk_numberOfTgcEtaHits, &b_Trk_numberOfTgcEtaHits);
   fChain->SetBranchAddress("Trk_numberOfRpcEtaHits", &Trk_numberOfRpcEtaHits, &b_Trk_numberOfRpcEtaHits);
   fChain->SetBranchAddress("Trk_numberOfCscEtaHits", &Trk_numberOfCscEtaHits, &b_Trk_numberOfCscEtaHits);
   fChain->SetBranchAddress("Trk_numberOfGangedPixels", &Trk_numberOfGangedPixels, &b_Trk_numberOfGangedPixels);
   fChain->SetBranchAddress("Trk_numberOfOutliersOnTrack", &Trk_numberOfOutliersOnTrack, &b_Trk_numberOfOutliersOnTrack);
   fChain->SetBranchAddress("Trk_truthBarcode", &Trk_truthBarcode, &b_Trk_truthBarcode);
   fChain->SetBranchAddress("Trk_truthNt", &Trk_truthNt, &b_Trk_truthNt);
   fChain->SetBranchAddress("Trk_ParticlePdg", &Trk_ParticlePdg, &b_Trk_ParticlePdg);
   fChain->SetBranchAddress("Trk_ParentPdg", &Trk_ParentPdg, &b_Trk_ParentPdg);
   fChain->SetBranchAddress("MboySeg_nseg", &MboySeg_nseg, &b_MboySeg_nseg);
   fChain->SetBranchAddress("MboySeg_Xpt", &MboySeg_Xpt, &b_MboySeg_Xpt);
   fChain->SetBranchAddress("MboySeg_Ypt", &MboySeg_Ypt, &b_MboySeg_Ypt);
   fChain->SetBranchAddress("MboySeg_Zpt", &MboySeg_Zpt, &b_MboySeg_Zpt);
   fChain->SetBranchAddress("MboySeg_Xvec", &MboySeg_Xvec, &b_MboySeg_Xvec);
   fChain->SetBranchAddress("MboySeg_Yvec", &MboySeg_Yvec, &b_MboySeg_Yvec);
   fChain->SetBranchAddress("MboySeg_Zvec", &MboySeg_Zvec, &b_MboySeg_Zvec);
   fChain->SetBranchAddress("MboySeg_QualityFactor", &MboySeg_QualityFactor, &b_MboySeg_QualityFactor);
   fChain->SetBranchAddress("MboySeg_NbreOfMultiLayer", &MboySeg_NbreOfMultiLayer, &b_MboySeg_NbreOfMultiLayer);
   fChain->SetBranchAddress("MboySeg_TrackSharing", &MboySeg_TrackSharing, &b_MboySeg_TrackSharing);
   fChain->SetBranchAddress("MboySeg_StationName", &MboySeg_StationName, &b_MboySeg_StationName);
   fChain->SetBranchAddress("MboySeg_Ndigits", &MboySeg_Ndigits, &b_MboySeg_Ndigits);
   fChain->SetBranchAddress("MboyTrkMB_NTrk", &MboyTrkMB_NTrk, &b_MboyTrkMB_NTrk);
   fChain->SetBranchAddress("MboyTrkVT_NTrk", &MboyTrkVT_NTrk, &b_MboyTrkVT_NTrk);
   fChain->SetBranchAddress("MboyTrkVT_Va", &MboyTrkVT_Va, &b_MboyTrkVT_Va);
   fChain->SetBranchAddress("MboyTrkVT_A0", &MboyTrkVT_A0, &b_MboyTrkVT_A0);
   fChain->SetBranchAddress("MboyTrkVT_Z", &MboyTrkVT_Z, &b_MboyTrkVT_Z);
   fChain->SetBranchAddress("MboyTrkVT_Phi", &MboyTrkVT_Phi, &b_MboyTrkVT_Phi);
   fChain->SetBranchAddress("MboyTrkVT_CotTh", &MboyTrkVT_CotTh, &b_MboyTrkVT_CotTh);
   fChain->SetBranchAddress("MboyTrkVT_PTInv", &MboyTrkVT_PTInv, &b_MboyTrkVT_PTInv);
   fChain->SetBranchAddress("MboyTrkVT_Cov11", &MboyTrkVT_Cov11, &b_MboyTrkVT_Cov11);
   fChain->SetBranchAddress("MboyTrkVT_Cov21", &MboyTrkVT_Cov21, &b_MboyTrkVT_Cov21);
   fChain->SetBranchAddress("MboyTrkVT_Cov22", &MboyTrkVT_Cov22, &b_MboyTrkVT_Cov22);
   fChain->SetBranchAddress("MboyTrkVT_Cov31", &MboyTrkVT_Cov31, &b_MboyTrkVT_Cov31);
   fChain->SetBranchAddress("MboyTrkVT_Cov32", &MboyTrkVT_Cov32, &b_MboyTrkVT_Cov32);
   fChain->SetBranchAddress("MboyTrkVT_Cov33", &MboyTrkVT_Cov33, &b_MboyTrkVT_Cov33);
   fChain->SetBranchAddress("MboyTrkVT_Cov41", &MboyTrkVT_Cov41, &b_MboyTrkVT_Cov41);
   fChain->SetBranchAddress("MboyTrkVT_Cov42", &MboyTrkVT_Cov42, &b_MboyTrkVT_Cov42);
   fChain->SetBranchAddress("MboyTrkVT_Cov43", &MboyTrkVT_Cov43, &b_MboyTrkVT_Cov43);
   fChain->SetBranchAddress("MboyTrkVT_Cov44", &MboyTrkVT_Cov44, &b_MboyTrkVT_Cov44);
   fChain->SetBranchAddress("MboyTrkVT_Cov51", &MboyTrkVT_Cov51, &b_MboyTrkVT_Cov51);
   fChain->SetBranchAddress("MboyTrkVT_Cov52", &MboyTrkVT_Cov52, &b_MboyTrkVT_Cov52);
   fChain->SetBranchAddress("MboyTrkVT_Cov53", &MboyTrkVT_Cov53, &b_MboyTrkVT_Cov53);
   fChain->SetBranchAddress("MboyTrkVT_Cov54", &MboyTrkVT_Cov54, &b_MboyTrkVT_Cov54);
   fChain->SetBranchAddress("MboyTrkVT_Cov55", &MboyTrkVT_Cov55, &b_MboyTrkVT_Cov55);
   fChain->SetBranchAddress("MboyTrkVT_Xpt", &MboyTrkVT_Xpt, &b_MboyTrkVT_Xpt);
   fChain->SetBranchAddress("MboyTrkVT_Ypt", &MboyTrkVT_Ypt, &b_MboyTrkVT_Ypt);
   fChain->SetBranchAddress("MboyTrkVT_Zpt", &MboyTrkVT_Zpt, &b_MboyTrkVT_Zpt);
   fChain->SetBranchAddress("MboyTrkVT_Xvec", &MboyTrkVT_Xvec, &b_MboyTrkVT_Xvec);
   fChain->SetBranchAddress("MboyTrkVT_Yvec", &MboyTrkVT_Yvec, &b_MboyTrkVT_Yvec);
   fChain->SetBranchAddress("MboyTrkVT_Zvec", &MboyTrkVT_Zvec, &b_MboyTrkVT_Zvec);
   fChain->SetBranchAddress("MboyTrkVT_Mom", &MboyTrkVT_Mom, &b_MboyTrkVT_Mom);
   fChain->SetBranchAddress("MboyTrkVT_digits", &MboyTrkVT_digits, &b_MboyTrkVT_digits);
   fChain->SetBranchAddress("MboyTrkVT_HitPosition", &MboyTrkVT_HitPosition, &b_MboyTrkVT_HitPosition);
   fChain->SetBranchAddress("MboyTrkVT_nhits", &MboyTrkVT_nhits, &b_MboyTrkVT_nhits);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Rdigi", &MboyTrkVT_TrHi_Rdigi, &b_MboyTrkVT_TrHi_Rdigi);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Rtrack", &MboyTrkVT_TrHi_Rtrack, &b_MboyTrkVT_TrHi_Rtrack);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Xdca", &MboyTrkVT_TrHi_Xdca, &b_MboyTrkVT_TrHi_Xdca);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Ydca", &MboyTrkVT_TrHi_Ydca, &b_MboyTrkVT_TrHi_Ydca);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Zdca", &MboyTrkVT_TrHi_Zdca, &b_MboyTrkVT_TrHi_Zdca);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Sdca", &MboyTrkVT_TrHi_Sdca, &b_MboyTrkVT_TrHi_Sdca);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Reso", &MboyTrkVT_TrHi_Reso, &b_MboyTrkVT_TrHi_Reso);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_RZSsim", &MboyTrkVT_TrHi_RZSsim, &b_MboyTrkVT_TrHi_RZSsim);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_EZSfit", &MboyTrkVT_TrHi_EZSfit, &b_MboyTrkVT_TrHi_EZSfit);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JTYP", &MboyTrkVT_TrHi_JTYP, &b_MboyTrkVT_TrHi_JTYP);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JFF", &MboyTrkVT_TrHi_JFF, &b_MboyTrkVT_TrHi_JFF);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JZZ", &MboyTrkVT_TrHi_JZZ, &b_MboyTrkVT_TrHi_JZZ);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JOB", &MboyTrkVT_TrHi_JOB, &b_MboyTrkVT_TrHi_JOB);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JSPLI", &MboyTrkVT_TrHi_JSPLI, &b_MboyTrkVT_TrHi_JSPLI);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JSL", &MboyTrkVT_TrHi_JSL, &b_MboyTrkVT_TrHi_JSL);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JTUBE", &MboyTrkVT_TrHi_JTUBE, &b_MboyTrkVT_TrHi_JTUBE);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JSZ", &MboyTrkVT_TrHi_JSZ, &b_MboyTrkVT_TrHi_JSZ);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_JSTRI", &MboyTrkVT_TrHi_JSTRI, &b_MboyTrkVT_TrHi_JSTRI);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_DriftTime", &MboyTrkVT_TrHi_DriftTime, &b_MboyTrkVT_TrHi_DriftTime);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_ChamberTech", &MboyTrkVT_TrHi_ChamberTech, &b_MboyTrkVT_TrHi_ChamberTech);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_StationNber", &MboyTrkVT_TrHi_StationNber, &b_MboyTrkVT_TrHi_StationNber);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_StationEta", &MboyTrkVT_TrHi_StationEta, &b_MboyTrkVT_TrHi_StationEta);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_StationPhi", &MboyTrkVT_TrHi_StationPhi, &b_MboyTrkVT_TrHi_StationPhi);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Multilayer", &MboyTrkVT_TrHi_Multilayer, &b_MboyTrkVT_TrHi_Multilayer);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_TubeLayer", &MboyTrkVT_TrHi_TubeLayer, &b_MboyTrkVT_TrHi_TubeLayer);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Tube", &MboyTrkVT_TrHi_Tube, &b_MboyTrkVT_TrHi_Tube);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_DoubletR", &MboyTrkVT_TrHi_DoubletR, &b_MboyTrkVT_TrHi_DoubletR);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_DoubletZ", &MboyTrkVT_TrHi_DoubletZ, &b_MboyTrkVT_TrHi_DoubletZ);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_DoubletPhi", &MboyTrkVT_TrHi_DoubletPhi, &b_MboyTrkVT_TrHi_DoubletPhi);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_GasGap", &MboyTrkVT_TrHi_GasGap, &b_MboyTrkVT_TrHi_GasGap);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_MeasuresPhi", &MboyTrkVT_TrHi_MeasuresPhi, &b_MboyTrkVT_TrHi_MeasuresPhi);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Strip", &MboyTrkVT_TrHi_Strip, &b_MboyTrkVT_TrHi_Strip);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_IsStrip", &MboyTrkVT_TrHi_IsStrip, &b_MboyTrkVT_TrHi_IsStrip);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_Channel", &MboyTrkVT_TrHi_Channel, &b_MboyTrkVT_TrHi_Channel);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_ChamberLayer", &MboyTrkVT_TrHi_ChamberLayer, &b_MboyTrkVT_TrHi_ChamberLayer);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_WireLayer", &MboyTrkVT_TrHi_WireLayer, &b_MboyTrkVT_TrHi_WireLayer);
   fChain->SetBranchAddress("MboyTrkVT_TrHi_RpcTime", &MboyTrkVT_TrHi_RpcTime, &b_MboyTrkVT_TrHi_RpcTime);
   fChain->SetBranchAddress("MboyTrkKE_NTrk", &MboyTrkKE_NTrk, &b_MboyTrkKE_NTrk);
   fChain->SetBranchAddress("MboyTrkKE_Va", &MboyTrkKE_Va, &b_MboyTrkKE_Va);
   fChain->SetBranchAddress("MboyTrkKE_Thept", &MboyTrkKE_Thept, &b_MboyTrkKE_Thept);
   fChain->SetBranchAddress("MboyTrkKE_Phipt", &MboyTrkKE_Phipt, &b_MboyTrkKE_Phipt);
   fChain->SetBranchAddress("MboyTrkKE_Thevc", &MboyTrkKE_Thevc, &b_MboyTrkKE_Thevc);
   fChain->SetBranchAddress("MboyTrkKE_Phivc", &MboyTrkKE_Phivc, &b_MboyTrkKE_Phivc);
   fChain->SetBranchAddress("MboyTrkKE_PInv", &MboyTrkKE_PInv, &b_MboyTrkKE_PInv);
   fChain->SetBranchAddress("MboyTrkKE_Cov11", &MboyTrkKE_Cov11, &b_MboyTrkKE_Cov11);
   fChain->SetBranchAddress("MboyTrkKE_Cov21", &MboyTrkKE_Cov21, &b_MboyTrkKE_Cov21);
   fChain->SetBranchAddress("MboyTrkKE_Cov22", &MboyTrkKE_Cov22, &b_MboyTrkKE_Cov22);
   fChain->SetBranchAddress("MboyTrkKE_Cov31", &MboyTrkKE_Cov31, &b_MboyTrkKE_Cov31);
   fChain->SetBranchAddress("MboyTrkKE_Cov32", &MboyTrkKE_Cov32, &b_MboyTrkKE_Cov32);
   fChain->SetBranchAddress("MboyTrkKE_Cov33", &MboyTrkKE_Cov33, &b_MboyTrkKE_Cov33);
   fChain->SetBranchAddress("MboyTrkKE_Cov41", &MboyTrkKE_Cov41, &b_MboyTrkKE_Cov41);
   fChain->SetBranchAddress("MboyTrkKE_Cov42", &MboyTrkKE_Cov42, &b_MboyTrkKE_Cov42);
   fChain->SetBranchAddress("MboyTrkKE_Cov43", &MboyTrkKE_Cov43, &b_MboyTrkKE_Cov43);
   fChain->SetBranchAddress("MboyTrkKE_Cov44", &MboyTrkKE_Cov44, &b_MboyTrkKE_Cov44);
   fChain->SetBranchAddress("MboyTrkKE_Cov51", &MboyTrkKE_Cov51, &b_MboyTrkKE_Cov51);
   fChain->SetBranchAddress("MboyTrkKE_Cov52", &MboyTrkKE_Cov52, &b_MboyTrkKE_Cov52);
   fChain->SetBranchAddress("MboyTrkKE_Cov53", &MboyTrkKE_Cov53, &b_MboyTrkKE_Cov53);
   fChain->SetBranchAddress("MboyTrkKE_Cov54", &MboyTrkKE_Cov54, &b_MboyTrkKE_Cov54);
   fChain->SetBranchAddress("MboyTrkKE_Cov55", &MboyTrkKE_Cov55, &b_MboyTrkKE_Cov55);
   fChain->SetBranchAddress("MboyTrkKE_Xpt", &MboyTrkKE_Xpt, &b_MboyTrkKE_Xpt);
   fChain->SetBranchAddress("MboyTrkKE_Ypt", &MboyTrkKE_Ypt, &b_MboyTrkKE_Ypt);
   fChain->SetBranchAddress("MboyTrkKE_Zpt", &MboyTrkKE_Zpt, &b_MboyTrkKE_Zpt);
   fChain->SetBranchAddress("MboyTrkKE_Xvec", &MboyTrkKE_Xvec, &b_MboyTrkKE_Xvec);
   fChain->SetBranchAddress("MboyTrkKE_Yvec", &MboyTrkKE_Yvec, &b_MboyTrkKE_Yvec);
   fChain->SetBranchAddress("MboyTrkKE_Zvec", &MboyTrkKE_Zvec, &b_MboyTrkKE_Zvec);
   fChain->SetBranchAddress("MboyTrkKE_Mom", &MboyTrkKE_Mom, &b_MboyTrkKE_Mom);
   fChain->SetBranchAddress("MboyTrkKE_ZCyl", &MboyTrkKE_ZCyl, &b_MboyTrkKE_ZCyl);
   fChain->SetBranchAddress("MboyTrkKE_RCyl", &MboyTrkKE_RCyl, &b_MboyTrkKE_RCyl);
   fChain->SetBranchAddress("MboyTrkMS_NTrk", &MboyTrkMS_NTrk, &b_MboyTrkMS_NTrk);
   fChain->SetBranchAddress("MboyTrkMS_Va", &MboyTrkMS_Va, &b_MboyTrkMS_Va);
   fChain->SetBranchAddress("MboyTrkMS_Thept", &MboyTrkMS_Thept, &b_MboyTrkMS_Thept);
   fChain->SetBranchAddress("MboyTrkMS_Phipt", &MboyTrkMS_Phipt, &b_MboyTrkMS_Phipt);
   fChain->SetBranchAddress("MboyTrkMS_Thevc", &MboyTrkMS_Thevc, &b_MboyTrkMS_Thevc);
   fChain->SetBranchAddress("MboyTrkMS_Phivc", &MboyTrkMS_Phivc, &b_MboyTrkMS_Phivc);
   fChain->SetBranchAddress("MboyTrkMS_PInv", &MboyTrkMS_PInv, &b_MboyTrkMS_PInv);
   fChain->SetBranchAddress("MboyTrkMS_Cov11", &MboyTrkMS_Cov11, &b_MboyTrkMS_Cov11);
   fChain->SetBranchAddress("MboyTrkMS_Cov21", &MboyTrkMS_Cov21, &b_MboyTrkMS_Cov21);
   fChain->SetBranchAddress("MboyTrkMS_Cov22", &MboyTrkMS_Cov22, &b_MboyTrkMS_Cov22);
   fChain->SetBranchAddress("MboyTrkMS_Cov31", &MboyTrkMS_Cov31, &b_MboyTrkMS_Cov31);
   fChain->SetBranchAddress("MboyTrkMS_Cov32", &MboyTrkMS_Cov32, &b_MboyTrkMS_Cov32);
   fChain->SetBranchAddress("MboyTrkMS_Cov33", &MboyTrkMS_Cov33, &b_MboyTrkMS_Cov33);
   fChain->SetBranchAddress("MboyTrkMS_Cov41", &MboyTrkMS_Cov41, &b_MboyTrkMS_Cov41);
   fChain->SetBranchAddress("MboyTrkMS_Cov42", &MboyTrkMS_Cov42, &b_MboyTrkMS_Cov42);
   fChain->SetBranchAddress("MboyTrkMS_Cov43", &MboyTrkMS_Cov43, &b_MboyTrkMS_Cov43);
   fChain->SetBranchAddress("MboyTrkMS_Cov44", &MboyTrkMS_Cov44, &b_MboyTrkMS_Cov44);
   fChain->SetBranchAddress("MboyTrkMS_Cov51", &MboyTrkMS_Cov51, &b_MboyTrkMS_Cov51);
   fChain->SetBranchAddress("MboyTrkMS_Cov52", &MboyTrkMS_Cov52, &b_MboyTrkMS_Cov52);
   fChain->SetBranchAddress("MboyTrkMS_Cov53", &MboyTrkMS_Cov53, &b_MboyTrkMS_Cov53);
   fChain->SetBranchAddress("MboyTrkMS_Cov54", &MboyTrkMS_Cov54, &b_MboyTrkMS_Cov54);
   fChain->SetBranchAddress("MboyTrkMS_Cov55", &MboyTrkMS_Cov55, &b_MboyTrkMS_Cov55);
   fChain->SetBranchAddress("MboyTrkMS_Xpt", &MboyTrkMS_Xpt, &b_MboyTrkMS_Xpt);
   fChain->SetBranchAddress("MboyTrkMS_Ypt", &MboyTrkMS_Ypt, &b_MboyTrkMS_Ypt);
   fChain->SetBranchAddress("MboyTrkMS_Zpt", &MboyTrkMS_Zpt, &b_MboyTrkMS_Zpt);
   fChain->SetBranchAddress("MboyTrkMS_Xvec", &MboyTrkMS_Xvec, &b_MboyTrkMS_Xvec);
   fChain->SetBranchAddress("MboyTrkMS_Yvec", &MboyTrkMS_Yvec, &b_MboyTrkMS_Yvec);
   fChain->SetBranchAddress("MboyTrkMS_Zvec", &MboyTrkMS_Zvec, &b_MboyTrkMS_Zvec);
   fChain->SetBranchAddress("MboyTrkMS_Mom", &MboyTrkMS_Mom, &b_MboyTrkMS_Mom);
   fChain->SetBranchAddress("MboyTrkMS_ZCyl", &MboyTrkMS_ZCyl, &b_MboyTrkMS_ZCyl);
   fChain->SetBranchAddress("MboyTrkMS_RCyl", &MboyTrkMS_RCyl, &b_MboyTrkMS_RCyl);
   fChain->SetBranchAddress("MboyTrkMS_Chican", &MboyTrkMS_Chican, &b_MboyTrkMS_Chican);
   fChain->SetBranchAddress("MboyTrkMS_Chifit", &MboyTrkMS_Chifit, &b_MboyTrkMS_Chifit);
   fChain->SetBranchAddress("MboyTrkMS_Chiver", &MboyTrkMS_Chiver, &b_MboyTrkMS_Chiver);
   fChain->SetBranchAddress("MboyTrkMS_Chistat", &MboyTrkMS_Chistat, &b_MboyTrkMS_Chistat);
   fChain->SetBranchAddress("MboyTrkMS_Ndof", &MboyTrkMS_Ndof, &b_MboyTrkMS_Ndof);
   fChain->SetBranchAddress("MboyAdjT0DefaultTShift", &MboyAdjT0DefaultTShift, &b_MboyAdjT0DefaultTShift);
   fChain->SetBranchAddress("MboyAdjT0NberOfData", &MboyAdjT0NberOfData, &b_MboyAdjT0NberOfData);
   fChain->SetBranchAddress("MboyAdjT0stationName", &MboyAdjT0stationName, &b_MboyAdjT0stationName);
   fChain->SetBranchAddress("MboyAdjT0stationEta", &MboyAdjT0stationEta, &b_MboyAdjT0stationEta);
   fChain->SetBranchAddress("MboyAdjT0stationPhi", &MboyAdjT0stationPhi, &b_MboyAdjT0stationPhi);
   fChain->SetBranchAddress("MboyAdjT0TShift", &MboyAdjT0TShift, &b_MboyAdjT0TShift);
   fChain->SetBranchAddress("staco_nmuon", &staco_nmuon, &b_staco_nmuon);
   fChain->SetBranchAddress("staco_A0", &staco_A0, &b_staco_A0);
   fChain->SetBranchAddress("staco_Z", &staco_Z, &b_staco_Z);
   fChain->SetBranchAddress("staco_Phi", &staco_Phi, &b_staco_Phi);
   fChain->SetBranchAddress("staco_Theta", &staco_Theta, &b_staco_Theta);
   fChain->SetBranchAddress("staco_qOverP", &staco_qOverP, &b_staco_qOverP);
   fChain->SetBranchAddress("staco_chi2", &staco_chi2, &b_staco_chi2);
   fChain->SetBranchAddress("staco_Ndf", &staco_Ndf, &b_staco_Ndf);
   fChain->SetBranchAddress("staco_covr11", &staco_covr11, &b_staco_covr11);
   fChain->SetBranchAddress("staco_covr21", &staco_covr21, &b_staco_covr21);
   fChain->SetBranchAddress("staco_covr22", &staco_covr22, &b_staco_covr22);
   fChain->SetBranchAddress("staco_covr31", &staco_covr31, &b_staco_covr31);
   fChain->SetBranchAddress("staco_covr32", &staco_covr32, &b_staco_covr32);
   fChain->SetBranchAddress("staco_covr33", &staco_covr33, &b_staco_covr33);
   fChain->SetBranchAddress("staco_covr41", &staco_covr41, &b_staco_covr41);
   fChain->SetBranchAddress("staco_covr42", &staco_covr42, &b_staco_covr42);
   fChain->SetBranchAddress("staco_covr43", &staco_covr43, &b_staco_covr43);
   fChain->SetBranchAddress("staco_covr44", &staco_covr44, &b_staco_covr44);
   fChain->SetBranchAddress("staco_covr51", &staco_covr51, &b_staco_covr51);
   fChain->SetBranchAddress("staco_covr52", &staco_covr52, &b_staco_covr52);
   fChain->SetBranchAddress("staco_covr53", &staco_covr53, &b_staco_covr53);
   fChain->SetBranchAddress("staco_covr54", &staco_covr54, &b_staco_covr54);
   fChain->SetBranchAddress("staco_covr55", &staco_covr55, &b_staco_covr55);
   fChain->SetBranchAddress("staco_A0ID", &staco_A0ID, &b_staco_A0ID);
   fChain->SetBranchAddress("staco_ZID", &staco_ZID, &b_staco_ZID);
   fChain->SetBranchAddress("staco_PhiID", &staco_PhiID, &b_staco_PhiID);
   fChain->SetBranchAddress("staco_ThetaID", &staco_ThetaID, &b_staco_ThetaID);
   fChain->SetBranchAddress("staco_qOverPID", &staco_qOverPID, &b_staco_qOverPID);
   fChain->SetBranchAddress("staco_chi2ID", &staco_chi2ID, &b_staco_chi2ID);
   fChain->SetBranchAddress("staco_NdfID", &staco_NdfID, &b_staco_NdfID);
   fChain->SetBranchAddress("staco_covr11ID", &staco_covr11ID, &b_staco_covr11ID);
   fChain->SetBranchAddress("staco_covr12ID", &staco_covr12ID, &b_staco_covr12ID);
   fChain->SetBranchAddress("staco_covr22ID", &staco_covr22ID, &b_staco_covr22ID);
   fChain->SetBranchAddress("staco_covr13ID", &staco_covr13ID, &b_staco_covr13ID);
   fChain->SetBranchAddress("staco_covr23ID", &staco_covr23ID, &b_staco_covr23ID);
   fChain->SetBranchAddress("staco_covr33ID", &staco_covr33ID, &b_staco_covr33ID);
   fChain->SetBranchAddress("staco_covr14ID", &staco_covr14ID, &b_staco_covr14ID);
   fChain->SetBranchAddress("staco_covr24ID", &staco_covr24ID, &b_staco_covr24ID);
   fChain->SetBranchAddress("staco_covr34ID", &staco_covr34ID, &b_staco_covr34ID);
   fChain->SetBranchAddress("staco_covr44ID", &staco_covr44ID, &b_staco_covr44ID);
   fChain->SetBranchAddress("staco_covr15ID", &staco_covr15ID, &b_staco_covr15ID);
   fChain->SetBranchAddress("staco_covr25ID", &staco_covr25ID, &b_staco_covr25ID);
   fChain->SetBranchAddress("staco_covr35ID", &staco_covr35ID, &b_staco_covr35ID);
   fChain->SetBranchAddress("staco_covr45ID", &staco_covr45ID, &b_staco_covr45ID);
   fChain->SetBranchAddress("staco_covr55ID", &staco_covr55ID, &b_staco_covr55ID);
   fChain->SetBranchAddress("staco_A0MS", &staco_A0MS, &b_staco_A0MS);
   fChain->SetBranchAddress("staco_ZMS", &staco_ZMS, &b_staco_ZMS);
   fChain->SetBranchAddress("staco_PhiMS", &staco_PhiMS, &b_staco_PhiMS);
   fChain->SetBranchAddress("staco_ThetaMS", &staco_ThetaMS, &b_staco_ThetaMS);
   fChain->SetBranchAddress("staco_qOverPMS", &staco_qOverPMS, &b_staco_qOverPMS);
   fChain->SetBranchAddress("staco_chi2MS", &staco_chi2MS, &b_staco_chi2MS);
   fChain->SetBranchAddress("staco_NdfMS", &staco_NdfMS, &b_staco_NdfMS);
   fChain->SetBranchAddress("staco_covr11MS", &staco_covr11MS, &b_staco_covr11MS);
   fChain->SetBranchAddress("staco_covr12MS", &staco_covr12MS, &b_staco_covr12MS);
   fChain->SetBranchAddress("staco_covr22MS", &staco_covr22MS, &b_staco_covr22MS);
   fChain->SetBranchAddress("staco_covr13MS", &staco_covr13MS, &b_staco_covr13MS);
   fChain->SetBranchAddress("staco_covr23MS", &staco_covr23MS, &b_staco_covr23MS);
   fChain->SetBranchAddress("staco_covr33MS", &staco_covr33MS, &b_staco_covr33MS);
   fChain->SetBranchAddress("staco_covr14MS", &staco_covr14MS, &b_staco_covr14MS);
   fChain->SetBranchAddress("staco_covr24MS", &staco_covr24MS, &b_staco_covr24MS);
   fChain->SetBranchAddress("staco_covr34MS", &staco_covr34MS, &b_staco_covr34MS);
   fChain->SetBranchAddress("staco_covr44MS", &staco_covr44MS, &b_staco_covr44MS);
   fChain->SetBranchAddress("staco_covr15MS", &staco_covr15MS, &b_staco_covr15MS);
   fChain->SetBranchAddress("staco_covr25MS", &staco_covr25MS, &b_staco_covr25MS);
   fChain->SetBranchAddress("staco_covr35MS", &staco_covr35MS, &b_staco_covr35MS);
   fChain->SetBranchAddress("staco_covr45MS", &staco_covr45MS, &b_staco_covr45MS);
   fChain->SetBranchAddress("staco_covr55MS", &staco_covr55MS, &b_staco_covr55MS);
   fChain->SetBranchAddress("staco_xMSE", &staco_xMSE, &b_staco_xMSE);
   fChain->SetBranchAddress("staco_yMSE", &staco_yMSE, &b_staco_yMSE);
   fChain->SetBranchAddress("staco_zMSE", &staco_zMSE, &b_staco_zMSE);
   fChain->SetBranchAddress("staco_PxMSE", &staco_PxMSE, &b_staco_PxMSE);
   fChain->SetBranchAddress("staco_PyMSE", &staco_PyMSE, &b_staco_PyMSE);
   fChain->SetBranchAddress("staco_PzMSE", &staco_PzMSE, &b_staco_PzMSE);
   fChain->SetBranchAddress("staco_chi2MSE", &staco_chi2MSE, &b_staco_chi2MSE);
   fChain->SetBranchAddress("staco_NdfMSE", &staco_NdfMSE, &b_staco_NdfMSE);
   fChain->SetBranchAddress("staco_etcone10", &staco_etcone10, &b_staco_etcone10);
   fChain->SetBranchAddress("staco_etcone20", &staco_etcone20, &b_staco_etcone20);
   fChain->SetBranchAddress("staco_etcone30", &staco_etcone30, &b_staco_etcone30);
   fChain->SetBranchAddress("staco_etcone40", &staco_etcone40, &b_staco_etcone40);
   fChain->SetBranchAddress("staco_nucone10", &staco_nucone10, &b_staco_nucone10);
   fChain->SetBranchAddress("staco_nucone20", &staco_nucone20, &b_staco_nucone20);
   fChain->SetBranchAddress("staco_nucone30", &staco_nucone30, &b_staco_nucone30);
   fChain->SetBranchAddress("staco_nucone40", &staco_nucone40, &b_staco_nucone40);
   fChain->SetBranchAddress("staco_Eloss", &staco_Eloss, &b_staco_Eloss);
   fChain->SetBranchAddress("staco_ElossError", &staco_ElossError, &b_staco_ElossError);
   fChain->SetBranchAddress("StacoExtr_nmuon", &StacoExtr_nmuon, &b_StacoExtr_nmuon);
   fChain->SetBranchAddress("StacoExtr_A0", &StacoExtr_A0, &b_StacoExtr_A0);
   fChain->SetBranchAddress("StacoExtr_Z", &StacoExtr_Z, &b_StacoExtr_Z);
   fChain->SetBranchAddress("StacoExtr_Phi", &StacoExtr_Phi, &b_StacoExtr_Phi);
   fChain->SetBranchAddress("StacoExtr_Theta", &StacoExtr_Theta, &b_StacoExtr_Theta);
   fChain->SetBranchAddress("StacoExtr_qOverP", &StacoExtr_qOverP, &b_StacoExtr_qOverP);
   fChain->SetBranchAddress("StacoExtr_Chi2", &StacoExtr_Chi2, &b_StacoExtr_Chi2);
   fChain->SetBranchAddress("StacoExtr_Ndf", &StacoExtr_Ndf, &b_StacoExtr_Ndf);
   fChain->SetBranchAddress("StacoExtr_covr11", &StacoExtr_covr11, &b_StacoExtr_covr11);
   fChain->SetBranchAddress("StacoExtr_covr21", &StacoExtr_covr21, &b_StacoExtr_covr21);
   fChain->SetBranchAddress("StacoExtr_covr22", &StacoExtr_covr22, &b_StacoExtr_covr22);
   fChain->SetBranchAddress("StacoExtr_covr31", &StacoExtr_covr31, &b_StacoExtr_covr31);
   fChain->SetBranchAddress("StacoExtr_covr32", &StacoExtr_covr32, &b_StacoExtr_covr32);
   fChain->SetBranchAddress("StacoExtr_covr33", &StacoExtr_covr33, &b_StacoExtr_covr33);
   fChain->SetBranchAddress("StacoExtr_covr41", &StacoExtr_covr41, &b_StacoExtr_covr41);
   fChain->SetBranchAddress("StacoExtr_covr42", &StacoExtr_covr42, &b_StacoExtr_covr42);
   fChain->SetBranchAddress("StacoExtr_covr43", &StacoExtr_covr43, &b_StacoExtr_covr43);
   fChain->SetBranchAddress("StacoExtr_covr44", &StacoExtr_covr44, &b_StacoExtr_covr44);
   fChain->SetBranchAddress("StacoExtr_covr51", &StacoExtr_covr51, &b_StacoExtr_covr51);
   fChain->SetBranchAddress("StacoExtr_covr52", &StacoExtr_covr52, &b_StacoExtr_covr52);
   fChain->SetBranchAddress("StacoExtr_covr53", &StacoExtr_covr53, &b_StacoExtr_covr53);
   fChain->SetBranchAddress("StacoExtr_covr54", &StacoExtr_covr54, &b_StacoExtr_covr54);
   fChain->SetBranchAddress("StacoExtr_covr55", &StacoExtr_covr55, &b_StacoExtr_covr55);
   fChain->SetBranchAddress("StacoExtr_etcone10", &StacoExtr_etcone10, &b_StacoExtr_etcone10);
   fChain->SetBranchAddress("StacoExtr_etcone20", &StacoExtr_etcone20, &b_StacoExtr_etcone20);
   fChain->SetBranchAddress("StacoExtr_etcone30", &StacoExtr_etcone30, &b_StacoExtr_etcone30);
   fChain->SetBranchAddress("StacoExtr_etcone40", &StacoExtr_etcone40, &b_StacoExtr_etcone40);
   fChain->SetBranchAddress("StacoExtr_nucone10", &StacoExtr_nucone10, &b_StacoExtr_nucone10);
   fChain->SetBranchAddress("StacoExtr_nucone20", &StacoExtr_nucone20, &b_StacoExtr_nucone20);
   fChain->SetBranchAddress("StacoExtr_nucone30", &StacoExtr_nucone30, &b_StacoExtr_nucone30);
   fChain->SetBranchAddress("StacoExtr_nucone40", &StacoExtr_nucone40, &b_StacoExtr_nucone40);
   fChain->SetBranchAddress("StacoExtr_Eloss", &StacoExtr_Eloss, &b_StacoExtr_Eloss);
   fChain->SetBranchAddress("StacoExtr_ElossError", &StacoExtr_ElossError, &b_StacoExtr_ElossError);
   fChain->SetBranchAddress("MboyESD_nmuon", &MboyESD_nmuon, &b_MboyESD_nmuon);
   fChain->SetBranchAddress("MboyESD_A0", &MboyESD_A0, &b_MboyESD_A0);
   fChain->SetBranchAddress("MboyESD_Z", &MboyESD_Z, &b_MboyESD_Z);
   fChain->SetBranchAddress("MboyESD_Phi", &MboyESD_Phi, &b_MboyESD_Phi);
   fChain->SetBranchAddress("MboyESD_Theta", &MboyESD_Theta, &b_MboyESD_Theta);
   fChain->SetBranchAddress("MboyESD_qOverP", &MboyESD_qOverP, &b_MboyESD_qOverP);
   fChain->SetBranchAddress("MboyESD_Chi2", &MboyESD_Chi2, &b_MboyESD_Chi2);
   fChain->SetBranchAddress("MboyESD_Ndf", &MboyESD_Ndf, &b_MboyESD_Ndf);
   fChain->SetBranchAddress("MboyESD_covr11", &MboyESD_covr11, &b_MboyESD_covr11);
   fChain->SetBranchAddress("MboyESD_covr21", &MboyESD_covr21, &b_MboyESD_covr21);
   fChain->SetBranchAddress("MboyESD_covr22", &MboyESD_covr22, &b_MboyESD_covr22);
   fChain->SetBranchAddress("MboyESD_covr31", &MboyESD_covr31, &b_MboyESD_covr31);
   fChain->SetBranchAddress("MboyESD_covr32", &MboyESD_covr32, &b_MboyESD_covr32);
   fChain->SetBranchAddress("MboyESD_covr33", &MboyESD_covr33, &b_MboyESD_covr33);
   fChain->SetBranchAddress("MboyESD_covr41", &MboyESD_covr41, &b_MboyESD_covr41);
   fChain->SetBranchAddress("MboyESD_covr42", &MboyESD_covr42, &b_MboyESD_covr42);
   fChain->SetBranchAddress("MboyESD_covr43", &MboyESD_covr43, &b_MboyESD_covr43);
   fChain->SetBranchAddress("MboyESD_covr44", &MboyESD_covr44, &b_MboyESD_covr44);
   fChain->SetBranchAddress("MboyESD_covr51", &MboyESD_covr51, &b_MboyESD_covr51);
   fChain->SetBranchAddress("MboyESD_covr52", &MboyESD_covr52, &b_MboyESD_covr52);
   fChain->SetBranchAddress("MboyESD_covr53", &MboyESD_covr53, &b_MboyESD_covr53);
   fChain->SetBranchAddress("MboyESD_covr54", &MboyESD_covr54, &b_MboyESD_covr54);
   fChain->SetBranchAddress("MboyESD_covr55", &MboyESD_covr55, &b_MboyESD_covr55);
   fChain->SetBranchAddress("MboyESD_etcone10", &MboyESD_etcone10, &b_MboyESD_etcone10);
   fChain->SetBranchAddress("MboyESD_etcone20", &MboyESD_etcone20, &b_MboyESD_etcone20);
   fChain->SetBranchAddress("MboyESD_etcone30", &MboyESD_etcone30, &b_MboyESD_etcone30);
   fChain->SetBranchAddress("MboyESD_etcone40", &MboyESD_etcone40, &b_MboyESD_etcone40);
   fChain->SetBranchAddress("MboyESD_nucone10", &MboyESD_nucone10, &b_MboyESD_nucone10);
   fChain->SetBranchAddress("MboyESD_nucone20", &MboyESD_nucone20, &b_MboyESD_nucone20);
   fChain->SetBranchAddress("MboyESD_nucone30", &MboyESD_nucone30, &b_MboyESD_nucone30);
   fChain->SetBranchAddress("MboyESD_nucone40", &MboyESD_nucone40, &b_MboyESD_nucone40);
   fChain->SetBranchAddress("MboyESD_Eloss", &MboyESD_Eloss, &b_MboyESD_Eloss);
   fChain->SetBranchAddress("MboyESD_ElossError", &MboyESD_ElossError, &b_MboyESD_ElossError);
   fChain->SetBranchAddress("MuTag_nmuon", &MuTag_nmuon, &b_MuTag_nmuon);
   fChain->SetBranchAddress("MuTag_A0", &MuTag_A0, &b_MuTag_A0);
   fChain->SetBranchAddress("MuTag_Z", &MuTag_Z, &b_MuTag_Z);
   fChain->SetBranchAddress("MuTag_Phi", &MuTag_Phi, &b_MuTag_Phi);
   fChain->SetBranchAddress("MuTag_Theta", &MuTag_Theta, &b_MuTag_Theta);
   fChain->SetBranchAddress("MuTag_qOverP", &MuTag_qOverP, &b_MuTag_qOverP);
   fChain->SetBranchAddress("MuTag_segmentChi2OverDoF", &MuTag_segmentChi2OverDoF, &b_MuTag_segmentChi2OverDoF);
   fChain->SetBranchAddress("MuTag_segmentDeltaPhi", &MuTag_segmentDeltaPhi, &b_MuTag_segmentDeltaPhi);
   fChain->SetBranchAddress("MuTag_segmentDeltaEta", &MuTag_segmentDeltaEta, &b_MuTag_segmentDeltaEta);
   fChain->SetBranchAddress("MuTag_innAngle", &MuTag_innAngle, &b_MuTag_innAngle);
   fChain->SetBranchAddress("MuTag_midAngle", &MuTag_midAngle, &b_MuTag_midAngle);
   fChain->SetBranchAddress("MuTag_numberOfMDTHits", &MuTag_numberOfMDTHits, &b_MuTag_numberOfMDTHits);
   fChain->SetBranchAddress("MuTag_numberOfRPCPhiHits", &MuTag_numberOfRPCPhiHits, &b_MuTag_numberOfRPCPhiHits);
   fChain->SetBranchAddress("MuTag_numberOfTGCPhiHits", &MuTag_numberOfTGCPhiHits, &b_MuTag_numberOfTGCPhiHits);
   fChain->SetBranchAddress("MuTag_MuonQuality", &MuTag_MuonQuality, &b_MuTag_MuonQuality);
   fChain->SetBranchAddress("MuTag_IsInner", &MuTag_IsInner, &b_MuTag_IsInner);
   fChain->SetBranchAddress("MuTag_SegMult", &MuTag_SegMult, &b_MuTag_SegMult);
   fChain->SetBranchAddress("MuTag_StationTypeSglTag", &MuTag_StationTypeSglTag, &b_MuTag_StationTypeSglTag);
   fChain->SetBranchAddress("MuTag_covr11", &MuTag_covr11, &b_MuTag_covr11);
   fChain->SetBranchAddress("MuTag_covr21", &MuTag_covr21, &b_MuTag_covr21);
   fChain->SetBranchAddress("MuTag_covr22", &MuTag_covr22, &b_MuTag_covr22);
   fChain->SetBranchAddress("MuTag_covr31", &MuTag_covr31, &b_MuTag_covr31);
   fChain->SetBranchAddress("MuTag_covr32", &MuTag_covr32, &b_MuTag_covr32);
   fChain->SetBranchAddress("MuTag_covr33", &MuTag_covr33, &b_MuTag_covr33);
   fChain->SetBranchAddress("MuTag_covr41", &MuTag_covr41, &b_MuTag_covr41);
   fChain->SetBranchAddress("MuTag_covr42", &MuTag_covr42, &b_MuTag_covr42);
   fChain->SetBranchAddress("MuTag_covr43", &MuTag_covr43, &b_MuTag_covr43);
   fChain->SetBranchAddress("MuTag_covr44", &MuTag_covr44, &b_MuTag_covr44);
   fChain->SetBranchAddress("MuTag_covr51", &MuTag_covr51, &b_MuTag_covr51);
   fChain->SetBranchAddress("MuTag_covr52", &MuTag_covr52, &b_MuTag_covr52);
   fChain->SetBranchAddress("MuTag_covr53", &MuTag_covr53, &b_MuTag_covr53);
   fChain->SetBranchAddress("MuTag_covr54", &MuTag_covr54, &b_MuTag_covr54);
   fChain->SetBranchAddress("MuTag_covr55", &MuTag_covr55, &b_MuTag_covr55);
   fChain->SetBranchAddress("MuTag_MatchChi2", &MuTag_MatchChi2, &b_MuTag_MatchChi2);
   fChain->SetBranchAddress("MuTag_MatchNdof", &MuTag_MatchNdof, &b_MuTag_MatchNdof);
   fChain->SetBranchAddress("MemRSS", &MemRSS, &b_MemRSS);
   fChain->SetBranchAddress("MemVSize", &MemVSize, &b_MemVSize);
   fChain->SetBranchAddress("TimeTotal", &TimeTotal, &b_TimeTotal);
   fChain->SetBranchAddress("TimeSum", &TimeSum, &b_TimeSum);
   fChain->SetBranchAddress("TimeAthMasterSeq", &TimeAthMasterSeq, &b_TimeAthMasterSeq);
   Notify();
}

Bool_t AANTEvent::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void AANTEvent::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t AANTEvent::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef AANTEvent_cxx
