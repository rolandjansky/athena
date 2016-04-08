#ifndef Ntuple_h
#define Ntuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

using namespace std;

class Ntuple {
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
   Int_t           L1Jet_nRoI;
   vector<long>    *L1Jet_JetRoIWord;
   vector<float>   *L1Jet_ET4x4;
   vector<float>   *L1Jet_ET6x6;
   vector<float>   *L1Jet_ET8x8;
   vector<long>    *L1Jet_Thresh;
   vector<float>   *L1Jet_eta;
   vector<float>   *L1Jet_phi;
   Long64_t        L1ET_EtMissHits;
   Long64_t        L1ET_EtSumHits;
   Float_t         L1ET_Ex;
   Float_t         L1ET_Ey;
   Float_t         L1ET_EtMiss;
   Float_t         L1ET_EtSum;
   Long64_t        L1ET_JetEtHits;
   Float_t         L1ET_JetEtSum;
   UInt_t          nCTP_ROI;
   vector<unsigned long> *CTP_ROI;
   UInt_t          nMuCTPI_ROI;
   vector<unsigned long> *muCTPI_ROI;
   UInt_t          nJetEnergy_ROI;
   vector<unsigned long> *JetEnergy_ROI;
   UInt_t          T2IdNtracks;
   vector<long>    *T2IdRoiID;
   vector<long>    *T2IdAlgo;
   vector<float>   *T2IdPt;
   vector<float>   *T2IdPhi0;
   vector<float>   *T2IdZ0;
   vector<float>   *T2IdD0;
   vector<float>   *T2IdPhic;
   vector<float>   *T2IdEtac;
   vector<float>   *T2IdEta;
   vector<float>   *T2IdErrPt;
   vector<float>   *T2IdErrPhi0;
   vector<float>   *T2IdErrEta;
   vector<float>   *T2IdErrD0;
   vector<float>   *T2IdErrZ0;
   vector<float>   *T2IdChi2;
   vector<long>    *T2IdNDoF;
   vector<long>    *T2IdNSihits;
   vector<long>    *T2IdNTrthits;
   vector<long>    *T2IdHPatt;
   vector<long>    *T2IdNstraw;
   vector<long>    *T2IdNtime;
   vector<long>    *T2IdNtr;
   vector<long>    *T2IdLastPlane;
   vector<long>    *T2IdFirstPlane;
   vector<long>    *T2IdNkineHit;
   vector<long>    *T2IdKineRef;
   vector<long>    *T2IdKineEnt;
   vector<long>    *T2IdNkineHitTRT;
   vector<long>    *T2IdKineRefTRT;
   vector<long>    *T2IdKineEntTRT;
   UInt_t          T2NVtx;
   vector<float>   *T2zVertex;
   Int_t           TrkEF_totalNumTracks;
   vector<long>    *TrkEF_RoIId;
   vector<float>   *TrkEF_d0;
   vector<float>   *TrkEF_z0;
   vector<float>   *TrkEF_phi;
   vector<float>   *TrkEF_eta;
   vector<float>   *TrkEF_qOverP;
   vector<float>   *TrkEF_Chi2;
   vector<long>    *TrkEF_Ndf;
   vector<float>   *TrkEF_sigd0;
   vector<float>   *TrkEF_sigz0;
   vector<float>   *TrkEF_sigpt;
   vector<float>   *TrkEF_sigphi;
   vector<float>   *TrkEF_sigeta;
   vector<float>   *TrkEF_sigqOverP;
   vector<float>   *TrkEF_covVert21;
   vector<float>   *TrkEF_covVert31;
   vector<float>   *TrkEF_covVert32;
   vector<float>   *TrkEF_covVert41;
   vector<float>   *TrkEF_covVert42;
   vector<float>   *TrkEF_covVert43;
   vector<float>   *TrkEF_covVert51;
   vector<float>   *TrkEF_covVert52;
   vector<float>   *TrkEF_covVert53;
   vector<float>   *TrkEF_covVert54;
   vector<float>   *TrkEF_px;
   vector<float>   *TrkEF_py;
   vector<float>   *TrkEF_pz;
   vector<float>   *TrkEF_pt;
   vector<long>    *TrkEF_numberOfBLayerHits;
   vector<long>    *TrkEF_numberOfPixelHits;
   vector<long>    *TrkEF_numberOfSCTHits;
   vector<long>    *TrkEF_numberOfTRTHits;
   vector<long>    *TrkEF_numberOfTRTHighThresholdHits;
   vector<long>    *TrkEF_numberOfBLayerSharedHits;
   vector<long>    *TrkEF_numberOfPixelSharedHits;
   vector<long>    *TrkEF_numberOfPixelHoles;
   vector<long>    *TrkEF_numberOfSCTSharedHits;
   vector<long>    *TrkEF_numberOfSCTHoles;
   vector<long>    *TrkEF_numberOfTRTOutliers;
   vector<long>    *TrkEF_numberOfTRTHighThresholdOutliers;
   vector<long>    *TrkEF_numberOfMdtHits;
   vector<long>    *TrkEF_numberOfTgcPhiHits;
   vector<long>    *TrkEF_numberOfRpcPhiHits;
   vector<long>    *TrkEF_numberOfCscPhiHits;
   vector<long>    *TrkEF_numberOfTgcEtaHits;
   vector<long>    *TrkEF_numberOfRpcEtaHits;
   vector<long>    *TrkEF_numberOfCscEtaHits;
   vector<long>    *TrkEF_numberOfGangedPixels;
   vector<long>    *TrkEF_numberOfOutliersOnTrack;
   vector<long>    *TrkEF_truthBarcode;
   vector<long>    *TrkEF_truthNt;
   vector<long>    *TrkEF_ParticlePdg;
   vector<long>    *TrkEF_ParentPdg;
   vector<long>    *TrkEF_NumSiKineHits;
   vector<long>    *TrkEF_NumTRTKineHits;

   Int_t           TrkEF_sctSP;
   Int_t           TrkEF_pixSP;
   Int_t           VxEF_numVertices;
   vector<int>     *VxEF_vtx_type;
   vector<float>   *VxEF_vtx_x;
   vector<float>   *VxEF_vtx_y;
   vector<float>   *VxEF_vtx_z;
   Int_t           VxEF_numOfTruthVertices;
   vector<float>   *VxEF_vtx_x_truth;
   vector<float>   *VxEF_vtx_y_truth;
   vector<float>   *VxEF_vtx_z_truth;
   vector<float>   *VxEF_vtx_x_res;
   vector<float>   *VxEF_vtx_y_res;
   vector<float>   *VxEF_vtx_z_res;
   vector<float>   *VxEF_sigvtx_x;
   vector<float>   *VxEF_sigvtx_y;
   vector<float>   *VxEF_sigvtx_z;
   vector<float>   *VxEF_vtxchi2;
   vector<long>    *VxEF_vtxndf;
   vector<float>   *VxEF_pt;
   vector<float>   *VxEF_vtxchi2prob;
   vector<long>    *VxEF_numTracksPerVertex;
   vector<long>    *VxEF_vtxType;
   Int_t           VxEF_totalNumTracks;
   vector<float>   *VxEF_chi2;
   vector<float>   *VxEF_d0;
   vector<float>   *VxEF_sigd0;
   vector<float>   *VxEF_z0;
   vector<float>   *VxEF_sigz0;
   vector<float>   *VxEF_phi;
   vector<float>   *VxEF_sigphi;
   vector<float>   *VxEF_theta;
   vector<float>   *VxEF_sigtheta;
   vector<float>   *VxEF_qOverP;
   vector<float>   *VxEF_sigqOverP;
   vector<float>   *VxEF_d0g;
   vector<float>   *VxEF_sigd0g;
   vector<float>   *VxEF_z0g;
   vector<float>   *VxEF_sigz0g;
   Int_t           SecVxEF_numVertices;
   vector<int>     *SecVxEF_vtx_type;
   vector<float>   *SecVxEF_vtx_x;
   vector<float>   *SecVxEF_vtx_y;
   vector<float>   *SecVxEF_vtx_z;
   Int_t           SecVxEF_numOfTruthVertices;
   vector<float>   *SecVxEF_vtx_x_truth;
   vector<float>   *SecVxEF_vtx_y_truth;
   vector<float>   *SecVxEF_vtx_z_truth;
   vector<float>   *SecVxEF_vtx_x_res;
   vector<float>   *SecVxEF_vtx_y_res;
   vector<float>   *SecVxEF_vtx_z_res;
   vector<float>   *SecVxEF_sigvtx_x;
   vector<float>   *SecVxEF_sigvtx_y;
   vector<float>   *SecVxEF_sigvtx_z;
   vector<float>   *SecVxEF_vtxchi2;
   vector<long>    *SecVxEF_vtxndf;
   vector<float>   *SecVxEF_pt;
   vector<float>   *SecVxEF_vtxchi2prob;
   vector<long>    *SecVxEF_numTracksPerVertex;
   Int_t           SecVxEF_totalNumTracks;
   vector<float>   *SecVxEF_chi2;
   vector<float>   *SecVxEF_d0;
   vector<float>   *SecVxEF_sigd0;
   vector<float>   *SecVxEF_z0;
   vector<float>   *SecVxEF_sigz0;
   vector<float>   *SecVxEF_phi;
   vector<float>   *SecVxEF_sigphi;
   vector<float>   *SecVxEF_theta;
   vector<float>   *SecVxEF_sigtheta;
   vector<float>   *SecVxEF_qOverP;
   vector<float>   *SecVxEF_sigqOverP;
   vector<float>   *SecVxEF_d0g;
   vector<float>   *SecVxEF_sigd0g;
   vector<float>   *SecVxEF_z0g;
   vector<float>   *SecVxEF_sigz0g;
   vector<float>   *SecVxEF_mass;
   vector<float>   *SecVxEF_massPiPi;
   vector<float>   *SecVxEF_energyAtVertex;
   vector<float>   *SecVxEF_energyFraction;
   vector<int>     *SecVxEF_n2trackvertices;
   vector<int>     *SecVxEF_nBadTracks;
   vector<int>     *SecVxEF_nTracksFromV0s;

   UInt_t          T2BjN;
   vector<float>   *T2BjEta;
   vector<float>   *T2BjPhi0;
   vector<float>   *T2BjPVtx;
   vector<float>   *T2BjXIP1D;
   vector<float>   *T2BjXIP2D;
   vector<float>   *T2BjXIP3D;
   vector<float>   *T2BjXChi2;
   vector<float>   *T2BjXSVtx;
   vector<float>   *T2BjPtJet;
   vector<float>   *T2BjXMVtx;
   vector<float>   *T2BjXEVtx;
   vector<float>   *T2BjXNVtx;

   UInt_t          EFBjN;
   vector<float>   *EFBjEta;
   vector<float>   *EFBjPhi0;
   vector<float>   *EFBjPVtx;
   vector<float>   *EFBjXIP1D;
   vector<float>   *EFBjXIP2D;
   vector<float>   *EFBjXIP3D;
   vector<float>   *EFBjXChi2;
   vector<float>   *EFBjXSVtx;
   vector<float>   *EFBjPtJet;
   vector<float>   *EFBjXMVtx;
   vector<float>   *EFBjXEVtx;
   vector<float>   *EFBjXNVtx;

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
   TBranch        *b_L1Jet_nRoI;   //!
   TBranch        *b_L1Jet_JetRoIWord;   //!
   TBranch        *b_L1Jet_ET4x4;   //!
   TBranch        *b_L1Jet_ET6x6;   //!
   TBranch        *b_L1Jet_ET8x8;   //!
   TBranch        *b_L1Jet_Thresh;   //!
   TBranch        *b_L1Jet_eta;   //!
   TBranch        *b_L1Jet_phi;   //!
   TBranch        *b_L1ET_EtMissHits;   //!
   TBranch        *b_L1ET_EtSumHits;   //!
   TBranch        *b_L1ET_Ex;   //!
   TBranch        *b_L1ET_Ey;   //!
   TBranch        *b_L1ET_EtMiss;   //!
   TBranch        *b_L1ET_EtSum;   //!
   TBranch        *b_L1ET_JetEtHits;   //!
   TBranch        *b_L1ET_JetEtSum;   //!
   TBranch        *b_nCTP_ROI;   //!
   TBranch        *b_CTP_ROI;   //!
   TBranch        *b_nMuCTPI_ROI;   //!
   TBranch        *b_muCTPI_ROI;   //!
   TBranch        *b_nJetEnergy_ROI;   //!
   TBranch        *b_JetEnergy_ROI;   //!
   TBranch        *b_T2IdNtracks;   //!
   TBranch        *b_T2IdRoiID;   //!
   TBranch        *b_T2IdAlgo;   //!
   TBranch        *b_T2IdPt;   //!
   TBranch        *b_T2IdPhi0;   //!
   TBranch        *b_T2IdZ0;   //!
   TBranch        *b_T2IdD0;   //!
   TBranch        *b_T2IdPhic;   //!
   TBranch        *b_T2IdEtac;   //!
   TBranch        *b_T2IdEta;   //!
   TBranch        *b_T2IdErrPt;   //!
   TBranch        *b_T2IdErrPhi0;   //!
   TBranch        *b_T2IdErrEta;   //!
   TBranch        *b_T2IdErrD0;   //!
   TBranch        *b_T2IdErrZ0;   //!
   TBranch        *b_T2IdChi2;   //!
   TBranch        *b_T2IdNDoF;   //!
   TBranch        *b_T2IdNSihits;   //!
   TBranch        *b_T2IdNTrthits;   //!
   TBranch        *b_T2IdHPatt;   //!
   TBranch        *b_T2IdNstraw;   //!
   TBranch        *b_T2IdNtime;   //!
   TBranch        *b_T2IdNtr;   //!
   TBranch        *b_T2IdLastPlane;   //!
   TBranch        *b_T2IdFirstPlane;   //!
   TBranch        *b_T2IdNkineHit;   //!
   TBranch        *b_T2IdKineRef;   //!
   TBranch        *b_T2IdKineEnt;   //!
   TBranch        *b_T2IdNkineHitTRT;   //!
   TBranch        *b_T2IdKineRefTRT;   //!
   TBranch        *b_T2IdKineEntTRT;   //!
   TBranch        *b_T2NVtx;   //!
   TBranch        *b_T2zVertex;   //!
   TBranch        *b_TrkEF_totalNumTracks;   //!
   TBranch        *b_TrkEF_RoIId;   //!
   TBranch        *b_TrkEF_d0;   //!
   TBranch        *b_TrkEF_z0;   //!
   TBranch        *b_TrkEF_phi;   //!
   TBranch        *b_TrkEF_eta;   //!
   TBranch        *b_TrkEF_qOverP;   //!
   TBranch        *b_TrkEF_Chi2;   //!
   TBranch        *b_TrkEF_Ndf;   //!
   TBranch        *b_TrkEF_sigd0;   //!
   TBranch        *b_TrkEF_sigz0;   //!
   TBranch        *b_TrkEF_sigpt;   //!
   TBranch        *b_TrkEF_sigphi;   //!
   TBranch        *b_TrkEF_sigeta;   //!
   TBranch        *b_TrkEF_sigqOverP;   //!
   TBranch        *b_TrkEF_covVert21;   //!
   TBranch        *b_TrkEF_covVert31;   //!
   TBranch        *b_TrkEF_covVert32;   //!
   TBranch        *b_TrkEF_covVert41;   //!
   TBranch        *b_TrkEF_covVert42;   //!
   TBranch        *b_TrkEF_covVert43;   //!
   TBranch        *b_TrkEF_covVert51;   //!
   TBranch        *b_TrkEF_covVert52;   //!
   TBranch        *b_TrkEF_covVert53;   //!
   TBranch        *b_TrkEF_covVert54;   //!
   TBranch        *b_TrkEF_px;   //!
   TBranch        *b_TrkEF_py;   //!
   TBranch        *b_TrkEF_pz;   //!
   TBranch        *b_TrkEF_pt;   //!
   TBranch        *b_TrkEF_numberOfBLayerHits;   //!
   TBranch        *b_TrkEF_numberOfPixelHits;   //!
   TBranch        *b_TrkEF_numberOfSCTHits;   //!
   TBranch        *b_TrkEF_numberOfTRTHits;   //!
   TBranch        *b_TrkEF_numberOfTRTHighThresholdHits;   //!
   TBranch        *b_TrkEF_numberOfBLayerSharedHits;   //!
   TBranch        *b_TrkEF_numberOfPixelSharedHits;   //!
   TBranch        *b_TrkEF_numberOfPixelHoles;   //!
   TBranch        *b_TrkEF_numberOfSCTSharedHits;   //!
   TBranch        *b_TrkEF_numberOfSCTHoles;   //!
   TBranch        *b_TrkEF_numberOfTRTOutliers;   //!
   TBranch        *b_TrkEF_numberOfTRTHighThresholdOutliers;   //!
   TBranch        *b_TrkEF_numberOfMdtHits;   //!
   TBranch        *b_TrkEF_numberOfTgcPhiHits;   //!
   TBranch        *b_TrkEF_numberOfRpcPhiHits;   //!
   TBranch        *b_TrkEF_numberOfCscPhiHits;   //!
   TBranch        *b_TrkEF_numberOfTgcEtaHits;   //!
   TBranch        *b_TrkEF_numberOfRpcEtaHits;   //!
   TBranch        *b_TrkEF_numberOfCscEtaHits;   //!
   TBranch        *b_TrkEF_numberOfGangedPixels;   //!
   TBranch        *b_TrkEF_numberOfOutliersOnTrack;   //!
   TBranch        *b_TrkEF_truthBarcode;   //!
   TBranch        *b_TrkEF_truthNt;   //!
   TBranch        *b_TrkEF_ParticlePdg;   //!
   TBranch        *b_TrkEF_ParentPdg;   //!
   TBranch        *b_TrkEF_NumSiKineHits;   //!
   TBranch        *b_TrkEF_NumTRTKineHits;   //!
   TBranch        *b_TrkEF_sctSP;   //!
   TBranch        *b_TrkEF_pixSP;   //!
   TBranch        *b_VxEF_numVertices;   //!
   TBranch        *b_VxEF_vtx_type;   //!
   TBranch        *b_VxEF_vtx_x;   //!
   TBranch        *b_VxEF_vtx_y;   //!
   TBranch        *b_VxEF_vtx_z;   //!
   TBranch        *b_VxEF_numOfTruthVertices;   //!
   TBranch        *b_VxEF_vtx_x_truth;   //!
   TBranch        *b_VxEF_vtx_y_truth;   //!
   TBranch        *b_VxEF_vtx_z_truth;   //!
   TBranch        *b_VxEF_vtx_x_res;   //!
   TBranch        *b_VxEF_vtx_y_res;   //!
   TBranch        *b_VxEF_vtx_z_res;   //!
   TBranch        *b_VxEF_sigvtx_x;   //!
   TBranch        *b_VxEF_sigvtx_y;   //!
   TBranch        *b_VxEF_sigvtx_z;   //!
   TBranch        *b_VxEF_vtxchi2;   //!
   TBranch        *b_VxEF_vtxndf;   //!
   TBranch        *b_VxEF_pt;   //!
   TBranch        *b_VxEF_vtxchi2prob;   //!
   TBranch        *b_VxEF_numTracksPerVertex;   //!
   TBranch        *b_VxEF_vtxType;   //!
   TBranch        *b_VxEF_totalNumTracks;   //!
   TBranch        *b_VxEF_chi2;   //!
   TBranch        *b_VxEF_d0;   //!
   TBranch        *b_VxEF_sigd0;   //!
   TBranch        *b_VxEF_z0;   //!
   TBranch        *b_VxEF_sigz0;   //!
   TBranch        *b_VxEF_phi;   //!
   TBranch        *b_VxEF_sigphi;   //!
   TBranch        *b_VxEF_theta;   //!
   TBranch        *b_VxEF_sigtheta;   //!
   TBranch        *b_VxEF_qOverP;   //!
   TBranch        *b_VxEF_sigqOverP;   //!
   TBranch        *b_VxEF_d0g;   //!
   TBranch        *b_VxEF_sigd0g;   //!
   TBranch        *b_VxEF_z0g;   //!
   TBranch        *b_VxEF_sigz0g;   //!
   TBranch        *b_SecVxEF_numVertices;   //!
   TBranch        *b_SecVxEF_vtx_type;   //!
   TBranch        *b_SecVxEF_vtx_x;   //!
   TBranch        *b_SecVxEF_vtx_y;   //!
   TBranch        *b_SecVxEF_vtx_z;   //!
   TBranch        *b_SecVxEF_numOfTruthVertices;   //!
   TBranch        *b_SecVxEF_vtx_x_truth;   //!
   TBranch        *b_SecVxEF_vtx_y_truth;   //!
   TBranch        *b_SecVxEF_vtx_z_truth;   //!
   TBranch        *b_SecVxEF_vtx_x_res;   //!
   TBranch        *b_SecVxEF_vtx_y_res;   //!
   TBranch        *b_SecVxEF_vtx_z_res;   //!
   TBranch        *b_SecVxEF_sigvtx_x;   //!
   TBranch        *b_SecVxEF_sigvtx_y;   //!
   TBranch        *b_SecVxEF_sigvtx_z;   //!
   TBranch        *b_SecVxEF_vtxchi2;   //!
   TBranch        *b_SecVxEF_vtxndf;   //!
   TBranch        *b_SecVxEF_pt;   //!
   TBranch        *b_SecVxEF_vtxchi2prob;   //!
   TBranch        *b_SecVxEF_numTracksPerVertex;   //!
   TBranch        *b_SecVxEF_totalNumTracks;   //!
   TBranch        *b_SecVxEF_chi2;   //!
   TBranch        *b_SecVxEF_d0;   //!
   TBranch        *b_SecVxEF_sigd0;   //!
   TBranch        *b_SecVxEF_z0;   //!
   TBranch        *b_SecVxEF_sigz0;   //!
   TBranch        *b_SecVxEF_phi;   //!
   TBranch        *b_SecVxEF_sigphi;   //!
   TBranch        *b_SecVxEF_theta;   //!
   TBranch        *b_SecVxEF_sigtheta;   //!
   TBranch        *b_SecVxEF_qOverP;   //!
   TBranch        *b_SecVxEF_sigqOverP;   //!
   TBranch        *b_SecVxEF_d0g;   //!
   TBranch        *b_SecVxEF_sigd0g;   //!
   TBranch        *b_SecVxEF_z0g;   //!
   TBranch        *b_SecVxEF_sigz0g;   //!
   TBranch        *b_SecVxEF_mass;   //!
   TBranch        *b_SecVxEF_massPiPi;   //!
   TBranch        *b_SecVxEF_energyAtVertex;   //!
   TBranch        *b_SecVxEF_energyFraction;   //!
   TBranch        *b_SecVxEF_n2trackvertices;   //!
   TBranch        *b_SecVxEF_nBadTracks;   //!
   TBranch        *b_SecVxEF_nTracksFromV0s;   //!

   TBranch        *b_T2BjN;   //!
   TBranch        *b_T2BjEta;   //!
   TBranch        *b_T2BjPhi0;   //!
   TBranch        *b_T2BjPVtx;   //!
   TBranch        *b_T2BjXIP1D;   //!
   TBranch        *b_T2BjXIP2D;   //!
   TBranch        *b_T2BjXIP3D;   //!
   TBranch        *b_T2BjXChi2;   //!
   TBranch        *b_T2BjXSVtx;   //!
   TBranch        *b_T2BjPtJet;   //!
   TBranch        *b_T2BjXMVtx;   //!
   TBranch        *b_T2BjXEVtx;   //!
   TBranch        *b_T2BjXNVtx;   //!

   TBranch        *b_EFBjN;   //!
   TBranch        *b_EFBjEta;   //!
   TBranch        *b_EFBjPhi0;   //!
   TBranch        *b_EFBjPVtx;   //!
   TBranch        *b_EFBjXIP1D;   //!
   TBranch        *b_EFBjXIP2D;   //!
   TBranch        *b_EFBjXIP3D;   //!
   TBranch        *b_EFBjXChi2;   //!
   TBranch        *b_EFBjXSVtx;   //!
   TBranch        *b_EFBjPtJet;   //!
   TBranch        *b_EFBjXMVtx;   //!
   TBranch        *b_EFBjXEVtx;   //!
   TBranch        *b_EFBjXNVtx;   //!

   Ntuple(TTree *tree=0);
   virtual ~Ntuple();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   Long64_t GetEntries();

};

#endif

#ifdef Ntuple_cxx
Ntuple::Ntuple(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("ntuple.root");
      if (!f) {
         f = new TFile("ntuple.root");
      }
      tree = (TTree*)gDirectory->Get("Ntuple");

   }
   Init(tree);
}

Ntuple::~Ntuple()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Ntuple::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Ntuple::LoadTree(Long64_t entry)
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

void Ntuple::Init(TTree *tree)
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
   L1Jet_JetRoIWord = 0;
   L1Jet_ET4x4 = 0;
   L1Jet_ET6x6 = 0;
   L1Jet_ET8x8 = 0;
   L1Jet_Thresh = 0;
   L1Jet_eta = 0;
   L1Jet_phi = 0;
   CTP_ROI = 0;
   muCTPI_ROI = 0;
   JetEnergy_ROI = 0;
   T2IdRoiID = 0;
   T2IdAlgo = 0;
   T2IdPt = 0;
   T2IdPhi0 = 0;
   T2IdZ0 = 0;
   T2IdD0 = 0;
   T2IdPhic = 0;
   T2IdEtac = 0;
   T2IdEta = 0;
   T2IdErrPt = 0;
   T2IdErrPhi0 = 0;
   T2IdErrEta = 0;
   T2IdErrD0 = 0;
   T2IdErrZ0 = 0;
   T2IdChi2 = 0;
   T2IdNDoF = 0;
   T2IdNSihits = 0;
   T2IdNTrthits = 0;
   T2IdHPatt = 0;
   T2IdNstraw = 0;
   T2IdNtime = 0;
   T2IdNtr = 0;
   T2IdLastPlane = 0;
   T2IdFirstPlane = 0;
   T2IdNkineHit = 0;
   T2IdKineRef = 0;
   T2IdKineEnt = 0;
   T2IdNkineHitTRT = 0;
   T2IdKineRefTRT = 0;
   T2IdKineEntTRT = 0;
   T2zVertex = 0;
   TrkEF_RoIId = 0;
   TrkEF_d0 = 0;
   TrkEF_z0 = 0;
   TrkEF_phi = 0;
   TrkEF_eta = 0;
   TrkEF_qOverP = 0;
   TrkEF_Chi2 = 0;
   TrkEF_Ndf = 0;
   TrkEF_sigd0 = 0;
   TrkEF_sigz0 = 0;
   TrkEF_sigpt = 0;
   TrkEF_sigphi = 0;
   TrkEF_sigeta = 0;
   TrkEF_sigqOverP = 0;
   TrkEF_covVert21 = 0;
   TrkEF_covVert31 = 0;
   TrkEF_covVert32 = 0;
   TrkEF_covVert41 = 0;
   TrkEF_covVert42 = 0;
   TrkEF_covVert43 = 0;
   TrkEF_covVert51 = 0;
   TrkEF_covVert52 = 0;
   TrkEF_covVert53 = 0;
   TrkEF_covVert54 = 0;
   TrkEF_px = 0;
   TrkEF_py = 0;
   TrkEF_pz = 0;
   TrkEF_pt = 0;
   TrkEF_numberOfBLayerHits = 0;
   TrkEF_numberOfPixelHits = 0;
   TrkEF_numberOfSCTHits = 0;
   TrkEF_numberOfTRTHits = 0;
   TrkEF_numberOfTRTHighThresholdHits = 0;
   TrkEF_numberOfBLayerSharedHits = 0;
   TrkEF_numberOfPixelSharedHits = 0;
   TrkEF_numberOfPixelHoles = 0;
   TrkEF_numberOfSCTSharedHits = 0;
   TrkEF_numberOfSCTHoles = 0;
   TrkEF_numberOfTRTOutliers = 0;
   TrkEF_numberOfTRTHighThresholdOutliers = 0;
   TrkEF_numberOfMdtHits = 0;
   TrkEF_numberOfTgcPhiHits = 0;
   TrkEF_numberOfRpcPhiHits = 0;
   TrkEF_numberOfCscPhiHits = 0;
   TrkEF_numberOfTgcEtaHits = 0;
   TrkEF_numberOfRpcEtaHits = 0;
   TrkEF_numberOfCscEtaHits = 0;
   TrkEF_numberOfGangedPixels = 0;
   TrkEF_numberOfOutliersOnTrack = 0;
   TrkEF_truthBarcode = 0;
   TrkEF_truthNt = 0;
   TrkEF_ParticlePdg = 0;
   TrkEF_ParentPdg = 0;
   TrkEF_NumSiKineHits = 0;
   TrkEF_NumTRTKineHits = 0;

   VxEF_vtx_type = 0;
   VxEF_vtx_x = 0;
   VxEF_vtx_y = 0;
   VxEF_vtx_z = 0;
   VxEF_vtx_x_truth = 0;
   VxEF_vtx_y_truth = 0;
   VxEF_vtx_z_truth = 0;
   VxEF_vtx_x_res = 0;
   VxEF_vtx_y_res = 0;
   VxEF_vtx_z_res = 0;
   VxEF_sigvtx_x = 0;
   VxEF_sigvtx_y = 0;
   VxEF_sigvtx_z = 0;
   VxEF_vtxchi2 = 0;
   VxEF_vtxndf = 0;
   VxEF_pt = 0;
   VxEF_vtxchi2prob = 0;
   VxEF_numTracksPerVertex = 0;
   VxEF_vtxType = 0;
   VxEF_chi2 = 0;
   VxEF_d0 = 0;
   VxEF_sigd0 = 0;
   VxEF_z0 = 0;
   VxEF_sigz0 = 0;
   VxEF_phi = 0;
   VxEF_sigphi = 0;
   VxEF_theta = 0;
   VxEF_sigtheta = 0;
   VxEF_qOverP = 0;
   VxEF_sigqOverP = 0;
   VxEF_d0g = 0;
   VxEF_sigd0g = 0;
   VxEF_z0g = 0;
   VxEF_sigz0g = 0;
   SecVxEF_vtx_type = 0;
   SecVxEF_vtx_x = 0;
   SecVxEF_vtx_y = 0;
   SecVxEF_vtx_z = 0;
   SecVxEF_vtx_x_truth = 0;
   SecVxEF_vtx_y_truth = 0;
   SecVxEF_vtx_z_truth = 0;
   SecVxEF_vtx_x_res = 0;
   SecVxEF_vtx_y_res = 0;
   SecVxEF_vtx_z_res = 0;
   SecVxEF_sigvtx_x = 0;
   SecVxEF_sigvtx_y = 0;
   SecVxEF_sigvtx_z = 0;
   SecVxEF_vtxchi2 = 0;
   SecVxEF_vtxndf = 0;
   SecVxEF_pt = 0;
   SecVxEF_vtxchi2prob = 0;
   SecVxEF_numTracksPerVertex = 0;
   SecVxEF_chi2 = 0;
   SecVxEF_d0 = 0;
   SecVxEF_sigd0 = 0;
   SecVxEF_z0 = 0;
   SecVxEF_sigz0 = 0;
   SecVxEF_phi = 0;
   SecVxEF_sigphi = 0;
   SecVxEF_theta = 0;
   SecVxEF_sigtheta = 0;
   SecVxEF_qOverP = 0;
   SecVxEF_sigqOverP = 0;
   SecVxEF_d0g = 0;
   SecVxEF_sigd0g = 0;
   SecVxEF_z0g = 0;
   SecVxEF_sigz0g = 0;
   SecVxEF_mass = 0;
   SecVxEF_massPiPi = 0;
   SecVxEF_energyAtVertex = 0;
   SecVxEF_energyFraction = 0;
   SecVxEF_n2trackvertices = 0;
   SecVxEF_nBadTracks = 0;
   SecVxEF_nTracksFromV0s = 0;

   T2BjEta = 0;
   T2BjPhi0 = 0;
   T2BjPVtx = 0;
   T2BjXIP1D = 0;
   T2BjXIP2D = 0;
   T2BjXIP3D = 0;
   T2BjXChi2 = 0;
   T2BjXSVtx = 0;
   T2BjPtJet = 0;
   T2BjXMVtx = 0;
   T2BjXEVtx = 0;
   T2BjXNVtx = 0;

   EFBjEta = 0;
   EFBjPhi0 = 0;
   EFBjPVtx = 0;
   EFBjXIP1D = 0;
   EFBjXIP2D = 0;
   EFBjXIP3D = 0;
   EFBjXChi2 = 0;
   EFBjXSVtx = 0;
   EFBjPtJet = 0;
   EFBjXMVtx = 0;
   EFBjXEVtx = 0;
   EFBjXNVtx = 0;

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
   fChain->SetBranchAddress("L1Jet_nRoI", &L1Jet_nRoI, &b_L1Jet_nRoI);
   fChain->SetBranchAddress("L1Jet_JetRoIWord", &L1Jet_JetRoIWord, &b_L1Jet_JetRoIWord);
   fChain->SetBranchAddress("L1Jet_ET4x4", &L1Jet_ET4x4, &b_L1Jet_ET4x4);
   fChain->SetBranchAddress("L1Jet_ET6x6", &L1Jet_ET6x6, &b_L1Jet_ET6x6);
   fChain->SetBranchAddress("L1Jet_ET8x8", &L1Jet_ET8x8, &b_L1Jet_ET8x8);
   fChain->SetBranchAddress("L1Jet_Thresh", &L1Jet_Thresh, &b_L1Jet_Thresh);
   fChain->SetBranchAddress("L1Jet_eta", &L1Jet_eta, &b_L1Jet_eta);
   fChain->SetBranchAddress("L1Jet_phi", &L1Jet_phi, &b_L1Jet_phi);
   fChain->SetBranchAddress("L1ET_EtMissHits", &L1ET_EtMissHits, &b_L1ET_EtMissHits);
   fChain->SetBranchAddress("L1ET_EtSumHits", &L1ET_EtSumHits, &b_L1ET_EtSumHits);
   fChain->SetBranchAddress("L1ET_Ex", &L1ET_Ex, &b_L1ET_Ex);
   fChain->SetBranchAddress("L1ET_Ey", &L1ET_Ey, &b_L1ET_Ey);
   fChain->SetBranchAddress("L1ET_EtMiss", &L1ET_EtMiss, &b_L1ET_EtMiss);
   fChain->SetBranchAddress("L1ET_EtSum", &L1ET_EtSum, &b_L1ET_EtSum);
   fChain->SetBranchAddress("L1ET_JetEtHits", &L1ET_JetEtHits, &b_L1ET_JetEtHits);
   fChain->SetBranchAddress("L1ET_JetEtSum", &L1ET_JetEtSum, &b_L1ET_JetEtSum);
   fChain->SetBranchAddress("nCTP_ROI", &nCTP_ROI, &b_nCTP_ROI);
   fChain->SetBranchAddress("CTP_ROI", &CTP_ROI, &b_CTP_ROI);
   fChain->SetBranchAddress("nMuCTPI_ROI", &nMuCTPI_ROI, &b_nMuCTPI_ROI);
   fChain->SetBranchAddress("muCTPI_ROI", &muCTPI_ROI, &b_muCTPI_ROI);
   fChain->SetBranchAddress("nJetEnergy_ROI", &nJetEnergy_ROI, &b_nJetEnergy_ROI);
   fChain->SetBranchAddress("JetEnergy_ROI", &JetEnergy_ROI, &b_JetEnergy_ROI);
   fChain->SetBranchAddress("T2IdNtracks", &T2IdNtracks, &b_T2IdNtracks);
   fChain->SetBranchAddress("T2IdRoiID", &T2IdRoiID, &b_T2IdRoiID);
   fChain->SetBranchAddress("T2IdAlgo", &T2IdAlgo, &b_T2IdAlgo);
   fChain->SetBranchAddress("T2IdPt", &T2IdPt, &b_T2IdPt);
   fChain->SetBranchAddress("T2IdPhi0", &T2IdPhi0, &b_T2IdPhi0);
   fChain->SetBranchAddress("T2IdZ0", &T2IdZ0, &b_T2IdZ0);
   fChain->SetBranchAddress("T2IdD0", &T2IdD0, &b_T2IdD0);
   fChain->SetBranchAddress("T2IdPhic", &T2IdPhic, &b_T2IdPhic);
   fChain->SetBranchAddress("T2IdEtac", &T2IdEtac, &b_T2IdEtac);
   fChain->SetBranchAddress("T2IdEta", &T2IdEta, &b_T2IdEta);
   fChain->SetBranchAddress("T2IdErrPt", &T2IdErrPt, &b_T2IdErrPt);
   fChain->SetBranchAddress("T2IdErrPhi0", &T2IdErrPhi0, &b_T2IdErrPhi0);
   fChain->SetBranchAddress("T2IdErrEta", &T2IdErrEta, &b_T2IdErrEta);
   fChain->SetBranchAddress("T2IdErrD0", &T2IdErrD0, &b_T2IdErrD0);
   fChain->SetBranchAddress("T2IdErrZ0", &T2IdErrZ0, &b_T2IdErrZ0);
   fChain->SetBranchAddress("T2IdChi2", &T2IdChi2, &b_T2IdChi2);
   fChain->SetBranchAddress("T2IdNDoF", &T2IdNDoF, &b_T2IdNDoF);
   fChain->SetBranchAddress("T2IdNSihits", &T2IdNSihits, &b_T2IdNSihits);
   fChain->SetBranchAddress("T2IdNTrthits", &T2IdNTrthits, &b_T2IdNTrthits);
   fChain->SetBranchAddress("T2IdHPatt", &T2IdHPatt, &b_T2IdHPatt);
   fChain->SetBranchAddress("T2IdNstraw", &T2IdNstraw, &b_T2IdNstraw);
   fChain->SetBranchAddress("T2IdNtime", &T2IdNtime, &b_T2IdNtime);
   fChain->SetBranchAddress("T2IdNtr", &T2IdNtr, &b_T2IdNtr);
   fChain->SetBranchAddress("T2IdLastPlane", &T2IdLastPlane, &b_T2IdLastPlane);
   fChain->SetBranchAddress("T2IdFirstPlane", &T2IdFirstPlane, &b_T2IdFirstPlane);
   fChain->SetBranchAddress("T2IdNkineHit", &T2IdNkineHit, &b_T2IdNkineHit);
   fChain->SetBranchAddress("T2IdKineRef", &T2IdKineRef, &b_T2IdKineRef);
   fChain->SetBranchAddress("T2IdKineEnt", &T2IdKineEnt, &b_T2IdKineEnt);
   fChain->SetBranchAddress("T2IdNkineHitTRT", &T2IdNkineHitTRT, &b_T2IdNkineHitTRT);
   fChain->SetBranchAddress("T2IdKineRefTRT", &T2IdKineRefTRT, &b_T2IdKineRefTRT);
   fChain->SetBranchAddress("T2IdKineEntTRT", &T2IdKineEntTRT, &b_T2IdKineEntTRT);
   fChain->SetBranchAddress("T2NVtx", &T2NVtx, &b_T2NVtx);
   fChain->SetBranchAddress("T2zVertex", &T2zVertex, &b_T2zVertex);
   fChain->SetBranchAddress("TrkEF_totalNumTracks", &TrkEF_totalNumTracks, &b_TrkEF_totalNumTracks);
   fChain->SetBranchAddress("TrkEF_RoIId", &TrkEF_RoIId, &b_TrkEF_RoIId);
   fChain->SetBranchAddress("TrkEF_d0", &TrkEF_d0, &b_TrkEF_d0);
   fChain->SetBranchAddress("TrkEF_z0", &TrkEF_z0, &b_TrkEF_z0);
   fChain->SetBranchAddress("TrkEF_phi", &TrkEF_phi, &b_TrkEF_phi);
   fChain->SetBranchAddress("TrkEF_eta", &TrkEF_eta, &b_TrkEF_eta);
   fChain->SetBranchAddress("TrkEF_qOverP", &TrkEF_qOverP, &b_TrkEF_qOverP);
   fChain->SetBranchAddress("TrkEF_Chi2", &TrkEF_Chi2, &b_TrkEF_Chi2);
   fChain->SetBranchAddress("TrkEF_Ndf", &TrkEF_Ndf, &b_TrkEF_Ndf);
   fChain->SetBranchAddress("TrkEF_sigd0", &TrkEF_sigd0, &b_TrkEF_sigd0);
   fChain->SetBranchAddress("TrkEF_sigz0", &TrkEF_sigz0, &b_TrkEF_sigz0);
   fChain->SetBranchAddress("TrkEF_sigpt", &TrkEF_sigpt, &b_TrkEF_sigpt);
   fChain->SetBranchAddress("TrkEF_sigphi", &TrkEF_sigphi, &b_TrkEF_sigphi);
   fChain->SetBranchAddress("TrkEF_sigeta", &TrkEF_sigeta, &b_TrkEF_sigeta);
   fChain->SetBranchAddress("TrkEF_sigqOverP", &TrkEF_sigqOverP, &b_TrkEF_sigqOverP);
   fChain->SetBranchAddress("TrkEF_covVert21", &TrkEF_covVert21, &b_TrkEF_covVert21);
   fChain->SetBranchAddress("TrkEF_covVert31", &TrkEF_covVert31, &b_TrkEF_covVert31);
   fChain->SetBranchAddress("TrkEF_covVert32", &TrkEF_covVert32, &b_TrkEF_covVert32);
   fChain->SetBranchAddress("TrkEF_covVert41", &TrkEF_covVert41, &b_TrkEF_covVert41);
   fChain->SetBranchAddress("TrkEF_covVert42", &TrkEF_covVert42, &b_TrkEF_covVert42);
   fChain->SetBranchAddress("TrkEF_covVert43", &TrkEF_covVert43, &b_TrkEF_covVert43);
   fChain->SetBranchAddress("TrkEF_covVert51", &TrkEF_covVert51, &b_TrkEF_covVert51);
   fChain->SetBranchAddress("TrkEF_covVert52", &TrkEF_covVert52, &b_TrkEF_covVert52);
   fChain->SetBranchAddress("TrkEF_covVert53", &TrkEF_covVert53, &b_TrkEF_covVert53);
   fChain->SetBranchAddress("TrkEF_covVert54", &TrkEF_covVert54, &b_TrkEF_covVert54);
   fChain->SetBranchAddress("TrkEF_px", &TrkEF_px, &b_TrkEF_px);
   fChain->SetBranchAddress("TrkEF_py", &TrkEF_py, &b_TrkEF_py);
   fChain->SetBranchAddress("TrkEF_pz", &TrkEF_pz, &b_TrkEF_pz);
   fChain->SetBranchAddress("TrkEF_pt", &TrkEF_pt, &b_TrkEF_pt);
   fChain->SetBranchAddress("TrkEF_numberOfBLayerHits", &TrkEF_numberOfBLayerHits, &b_TrkEF_numberOfBLayerHits);
   fChain->SetBranchAddress("TrkEF_numberOfPixelHits", &TrkEF_numberOfPixelHits, &b_TrkEF_numberOfPixelHits);
   fChain->SetBranchAddress("TrkEF_numberOfSCTHits", &TrkEF_numberOfSCTHits, &b_TrkEF_numberOfSCTHits);
   fChain->SetBranchAddress("TrkEF_numberOfTRTHits", &TrkEF_numberOfTRTHits, &b_TrkEF_numberOfTRTHits);
   fChain->SetBranchAddress("TrkEF_numberOfTRTHighThresholdHits", &TrkEF_numberOfTRTHighThresholdHits, &b_TrkEF_numberOfTRTHighThresholdHits);
   fChain->SetBranchAddress("TrkEF_numberOfBLayerSharedHits", &TrkEF_numberOfBLayerSharedHits, &b_TrkEF_numberOfBLayerSharedHits);
   fChain->SetBranchAddress("TrkEF_numberOfPixelSharedHits", &TrkEF_numberOfPixelSharedHits, &b_TrkEF_numberOfPixelSharedHits);
   fChain->SetBranchAddress("TrkEF_numberOfPixelHoles", &TrkEF_numberOfPixelHoles, &b_TrkEF_numberOfPixelHoles);
   fChain->SetBranchAddress("TrkEF_numberOfSCTSharedHits", &TrkEF_numberOfSCTSharedHits, &b_TrkEF_numberOfSCTSharedHits);
   fChain->SetBranchAddress("TrkEF_numberOfSCTHoles", &TrkEF_numberOfSCTHoles, &b_TrkEF_numberOfSCTHoles);
   fChain->SetBranchAddress("TrkEF_numberOfTRTOutliers", &TrkEF_numberOfTRTOutliers, &b_TrkEF_numberOfTRTOutliers);
   fChain->SetBranchAddress("TrkEF_numberOfTRTHighThresholdOutliers", &TrkEF_numberOfTRTHighThresholdOutliers, &b_TrkEF_numberOfTRTHighThresholdOutliers);
   fChain->SetBranchAddress("TrkEF_numberOfMdtHits", &TrkEF_numberOfMdtHits, &b_TrkEF_numberOfMdtHits);
   fChain->SetBranchAddress("TrkEF_numberOfTgcPhiHits", &TrkEF_numberOfTgcPhiHits, &b_TrkEF_numberOfTgcPhiHits);
   fChain->SetBranchAddress("TrkEF_numberOfRpcPhiHits", &TrkEF_numberOfRpcPhiHits, &b_TrkEF_numberOfRpcPhiHits);
   fChain->SetBranchAddress("TrkEF_numberOfCscPhiHits", &TrkEF_numberOfCscPhiHits, &b_TrkEF_numberOfCscPhiHits);
   fChain->SetBranchAddress("TrkEF_numberOfTgcEtaHits", &TrkEF_numberOfTgcEtaHits, &b_TrkEF_numberOfTgcEtaHits);
   fChain->SetBranchAddress("TrkEF_numberOfRpcEtaHits", &TrkEF_numberOfRpcEtaHits, &b_TrkEF_numberOfRpcEtaHits);
   fChain->SetBranchAddress("TrkEF_numberOfCscEtaHits", &TrkEF_numberOfCscEtaHits, &b_TrkEF_numberOfCscEtaHits);
   fChain->SetBranchAddress("TrkEF_numberOfGangedPixels", &TrkEF_numberOfGangedPixels, &b_TrkEF_numberOfGangedPixels);
   fChain->SetBranchAddress("TrkEF_numberOfOutliersOnTrack", &TrkEF_numberOfOutliersOnTrack, &b_TrkEF_numberOfOutliersOnTrack);
   fChain->SetBranchAddress("TrkEF_truthBarcode", &TrkEF_truthBarcode, &b_TrkEF_truthBarcode);
   fChain->SetBranchAddress("TrkEF_truthNt", &TrkEF_truthNt, &b_TrkEF_truthNt);
   fChain->SetBranchAddress("TrkEF_ParticlePdg", &TrkEF_ParticlePdg, &b_TrkEF_ParticlePdg);
   fChain->SetBranchAddress("TrkEF_ParentPdg", &TrkEF_ParentPdg, &b_TrkEF_ParentPdg);
   fChain->SetBranchAddress("TrkEF_NumSiKineHits", &TrkEF_NumSiKineHits, &b_TrkEF_NumSiKineHits);
   fChain->SetBranchAddress("TrkEF_NumTRTKineHits", &TrkEF_NumTRTKineHits, &b_TrkEF_NumTRTKineHits);
   fChain->SetBranchAddress("TrkEF_sctSP", &TrkEF_sctSP, &b_TrkEF_sctSP);
   fChain->SetBranchAddress("TrkEF_pixSP", &TrkEF_pixSP, &b_TrkEF_pixSP);
   fChain->SetBranchAddress("VxEF_numVertices", &VxEF_numVertices, &b_VxEF_numVertices);
   fChain->SetBranchAddress("VxEF_vtx_type", &VxEF_vtx_type, &b_VxEF_vtx_type);
   fChain->SetBranchAddress("VxEF_vtx_x", &VxEF_vtx_x, &b_VxEF_vtx_x);
   fChain->SetBranchAddress("VxEF_vtx_y", &VxEF_vtx_y, &b_VxEF_vtx_y);
   fChain->SetBranchAddress("VxEF_vtx_z", &VxEF_vtx_z, &b_VxEF_vtx_z);
   fChain->SetBranchAddress("VxEF_numOfTruthVertices", &VxEF_numOfTruthVertices, &b_VxEF_numOfTruthVertices);
   fChain->SetBranchAddress("VxEF_vtx_x_truth", &VxEF_vtx_x_truth, &b_VxEF_vtx_x_truth);
   fChain->SetBranchAddress("VxEF_vtx_y_truth", &VxEF_vtx_y_truth, &b_VxEF_vtx_y_truth);
   fChain->SetBranchAddress("VxEF_vtx_z_truth", &VxEF_vtx_z_truth, &b_VxEF_vtx_z_truth);
   fChain->SetBranchAddress("VxEF_vtx_x_res", &VxEF_vtx_x_res, &b_VxEF_vtx_x_res);
   fChain->SetBranchAddress("VxEF_vtx_y_res", &VxEF_vtx_y_res, &b_VxEF_vtx_y_res);
   fChain->SetBranchAddress("VxEF_vtx_z_res", &VxEF_vtx_z_res, &b_VxEF_vtx_z_res);
   fChain->SetBranchAddress("VxEF_sigvtx_x", &VxEF_sigvtx_x, &b_VxEF_sigvtx_x);
   fChain->SetBranchAddress("VxEF_sigvtx_y", &VxEF_sigvtx_y, &b_VxEF_sigvtx_y);
   fChain->SetBranchAddress("VxEF_sigvtx_z", &VxEF_sigvtx_z, &b_VxEF_sigvtx_z);
   fChain->SetBranchAddress("VxEF_vtxchi2", &VxEF_vtxchi2, &b_VxEF_vtxchi2);
   fChain->SetBranchAddress("VxEF_vtxndf", &VxEF_vtxndf, &b_VxEF_vtxndf);
   fChain->SetBranchAddress("VxEF_pt", &VxEF_pt, &b_VxEF_pt);
   fChain->SetBranchAddress("VxEF_vtxchi2prob", &VxEF_vtxchi2prob, &b_VxEF_vtxchi2prob);
   fChain->SetBranchAddress("VxEF_numTracksPerVertex", &VxEF_numTracksPerVertex, &b_VxEF_numTracksPerVertex);
   fChain->SetBranchAddress("VxEF_vtxType", &VxEF_vtxType, &b_VxEF_vtxType);
   fChain->SetBranchAddress("VxEF_totalNumTracks", &VxEF_totalNumTracks, &b_VxEF_totalNumTracks);
   fChain->SetBranchAddress("VxEF_chi2", &VxEF_chi2, &b_VxEF_chi2);
   fChain->SetBranchAddress("VxEF_d0", &VxEF_d0, &b_VxEF_d0);
   fChain->SetBranchAddress("VxEF_sigd0", &VxEF_sigd0, &b_VxEF_sigd0);
   fChain->SetBranchAddress("VxEF_z0", &VxEF_z0, &b_VxEF_z0);
   fChain->SetBranchAddress("VxEF_sigz0", &VxEF_sigz0, &b_VxEF_sigz0);
   fChain->SetBranchAddress("VxEF_phi", &VxEF_phi, &b_VxEF_phi);
   fChain->SetBranchAddress("VxEF_sigphi", &VxEF_sigphi, &b_VxEF_sigphi);
   fChain->SetBranchAddress("VxEF_theta", &VxEF_theta, &b_VxEF_theta);
   fChain->SetBranchAddress("VxEF_sigtheta", &VxEF_sigtheta, &b_VxEF_sigtheta);
   fChain->SetBranchAddress("VxEF_qOverP", &VxEF_qOverP, &b_VxEF_qOverP);
   fChain->SetBranchAddress("VxEF_sigqOverP", &VxEF_sigqOverP, &b_VxEF_sigqOverP);
   fChain->SetBranchAddress("VxEF_d0g", &VxEF_d0g, &b_VxEF_d0g);
   fChain->SetBranchAddress("VxEF_sigd0g", &VxEF_sigd0g, &b_VxEF_sigd0g);
   fChain->SetBranchAddress("VxEF_z0g", &VxEF_z0g, &b_VxEF_z0g);
   fChain->SetBranchAddress("VxEF_sigz0g", &VxEF_sigz0g, &b_VxEF_sigz0g);
   fChain->SetBranchAddress("SecVxEF_numVertices", &SecVxEF_numVertices, &b_SecVxEF_numVertices);
   fChain->SetBranchAddress("SecVxEF_vtx_type", &SecVxEF_vtx_type, &b_SecVxEF_vtx_type);
   fChain->SetBranchAddress("SecVxEF_vtx_x", &SecVxEF_vtx_x, &b_SecVxEF_vtx_x);
   fChain->SetBranchAddress("SecVxEF_vtx_y", &SecVxEF_vtx_y, &b_SecVxEF_vtx_y);
   fChain->SetBranchAddress("SecVxEF_vtx_z", &SecVxEF_vtx_z, &b_SecVxEF_vtx_z);
   fChain->SetBranchAddress("SecVxEF_numOfTruthVertices", &SecVxEF_numOfTruthVertices, &b_SecVxEF_numOfTruthVertices);
   fChain->SetBranchAddress("SecVxEF_vtx_x_truth", &SecVxEF_vtx_x_truth, &b_SecVxEF_vtx_x_truth);
   fChain->SetBranchAddress("SecVxEF_vtx_y_truth", &SecVxEF_vtx_y_truth, &b_SecVxEF_vtx_y_truth);
   fChain->SetBranchAddress("SecVxEF_vtx_z_truth", &SecVxEF_vtx_z_truth, &b_SecVxEF_vtx_z_truth);
   fChain->SetBranchAddress("SecVxEF_vtx_x_res", &SecVxEF_vtx_x_res, &b_SecVxEF_vtx_x_res);
   fChain->SetBranchAddress("SecVxEF_vtx_y_res", &SecVxEF_vtx_y_res, &b_SecVxEF_vtx_y_res);
   fChain->SetBranchAddress("SecVxEF_vtx_z_res", &SecVxEF_vtx_z_res, &b_SecVxEF_vtx_z_res);
   fChain->SetBranchAddress("SecVxEF_sigvtx_x", &SecVxEF_sigvtx_x, &b_SecVxEF_sigvtx_x);
   fChain->SetBranchAddress("SecVxEF_sigvtx_y", &SecVxEF_sigvtx_y, &b_SecVxEF_sigvtx_y);
   fChain->SetBranchAddress("SecVxEF_sigvtx_z", &SecVxEF_sigvtx_z, &b_SecVxEF_sigvtx_z);
   fChain->SetBranchAddress("SecVxEF_vtxchi2", &SecVxEF_vtxchi2, &b_SecVxEF_vtxchi2);
   fChain->SetBranchAddress("SecVxEF_vtxndf", &SecVxEF_vtxndf, &b_SecVxEF_vtxndf);
   fChain->SetBranchAddress("SecVxEF_pt", &SecVxEF_pt, &b_SecVxEF_pt);
   fChain->SetBranchAddress("SecVxEF_vtxchi2prob", &SecVxEF_vtxchi2prob, &b_SecVxEF_vtxchi2prob);
   fChain->SetBranchAddress("SecVxEF_numTracksPerVertex", &SecVxEF_numTracksPerVertex, &b_SecVxEF_numTracksPerVertex);
   fChain->SetBranchAddress("SecVxEF_totalNumTracks", &SecVxEF_totalNumTracks, &b_SecVxEF_totalNumTracks);
   fChain->SetBranchAddress("SecVxEF_chi2", &SecVxEF_chi2, &b_SecVxEF_chi2);
   fChain->SetBranchAddress("SecVxEF_d0", &SecVxEF_d0, &b_SecVxEF_d0);
   fChain->SetBranchAddress("SecVxEF_sigd0", &SecVxEF_sigd0, &b_SecVxEF_sigd0);
   fChain->SetBranchAddress("SecVxEF_z0", &SecVxEF_z0, &b_SecVxEF_z0);
   fChain->SetBranchAddress("SecVxEF_sigz0", &SecVxEF_sigz0, &b_SecVxEF_sigz0);
   fChain->SetBranchAddress("SecVxEF_phi", &SecVxEF_phi, &b_SecVxEF_phi);
   fChain->SetBranchAddress("SecVxEF_sigphi", &SecVxEF_sigphi, &b_SecVxEF_sigphi);
   fChain->SetBranchAddress("SecVxEF_theta", &SecVxEF_theta, &b_SecVxEF_theta);
   fChain->SetBranchAddress("SecVxEF_sigtheta", &SecVxEF_sigtheta, &b_SecVxEF_sigtheta);
   fChain->SetBranchAddress("SecVxEF_qOverP", &SecVxEF_qOverP, &b_SecVxEF_qOverP);
   fChain->SetBranchAddress("SecVxEF_sigqOverP", &SecVxEF_sigqOverP, &b_SecVxEF_sigqOverP);
   fChain->SetBranchAddress("SecVxEF_d0g", &SecVxEF_d0g, &b_SecVxEF_d0g);
   fChain->SetBranchAddress("SecVxEF_sigd0g", &SecVxEF_sigd0g, &b_SecVxEF_sigd0g);
   fChain->SetBranchAddress("SecVxEF_z0g", &SecVxEF_z0g, &b_SecVxEF_z0g);
   fChain->SetBranchAddress("SecVxEF_sigz0g", &SecVxEF_sigz0g, &b_SecVxEF_sigz0g);
   fChain->SetBranchAddress("SecVxEF_mass", &SecVxEF_mass, &b_SecVxEF_mass);
   fChain->SetBranchAddress("SecVxEF_massPiPi", &SecVxEF_massPiPi, &b_SecVxEF_massPiPi);
   fChain->SetBranchAddress("SecVxEF_energyAtVertex", &SecVxEF_energyAtVertex, &b_SecVxEF_energyAtVertex);
   fChain->SetBranchAddress("SecVxEF_energyFraction", &SecVxEF_energyFraction, &b_SecVxEF_energyFraction);
   fChain->SetBranchAddress("SecVxEF_n2trackvertices", &SecVxEF_n2trackvertices, &b_SecVxEF_n2trackvertices);
   fChain->SetBranchAddress("SecVxEF_nBadTracks", &SecVxEF_nBadTracks, &b_SecVxEF_nBadTracks);
   fChain->SetBranchAddress("SecVxEF_nTracksFromV0s", &SecVxEF_nTracksFromV0s, &b_SecVxEF_nTracksFromV0s);

   fChain->SetBranchAddress("T2BjN", &T2BjN, &b_T2BjN);
   fChain->SetBranchAddress("T2BjEta", &T2BjEta, &b_T2BjEta);
   fChain->SetBranchAddress("T2BjPhi0", &T2BjPhi0, &b_T2BjPhi0);
   fChain->SetBranchAddress("T2BjPVtx", &T2BjPVtx, &b_T2BjPVtx);
   fChain->SetBranchAddress("T2BjXIP1D", &T2BjXIP1D, &b_T2BjXIP1D);
   fChain->SetBranchAddress("T2BjXIP2D", &T2BjXIP2D, &b_T2BjXIP2D);
   fChain->SetBranchAddress("T2BjXIP3D", &T2BjXIP3D, &b_T2BjXIP3D);
   fChain->SetBranchAddress("T2BjXChi2", &T2BjXChi2, &b_T2BjXChi2);
   fChain->SetBranchAddress("T2BjXSVtx", &T2BjXSVtx, &b_T2BjXSVtx);
   fChain->SetBranchAddress("T2BjPtJet", &T2BjPtJet, &b_T2BjPtJet);
   fChain->SetBranchAddress("T2BjXMVtx", &T2BjXMVtx, &b_T2BjXMVtx);
   fChain->SetBranchAddress("T2BjXEVtx", &T2BjXEVtx, &b_T2BjXEVtx);
   fChain->SetBranchAddress("T2BjXNVtx", &T2BjXNVtx, &b_T2BjXNVtx);

   fChain->SetBranchAddress("EFBjN", &EFBjN, &b_EFBjN);
   fChain->SetBranchAddress("EFBjEta", &EFBjEta, &b_EFBjEta);
   fChain->SetBranchAddress("EFBjPhi0", &EFBjPhi0, &b_EFBjPhi0);
   fChain->SetBranchAddress("EFBjPVtx", &EFBjPVtx, &b_EFBjPVtx);
   fChain->SetBranchAddress("EFBjXIP1D", &EFBjXIP1D, &b_EFBjXIP1D);
   fChain->SetBranchAddress("EFBjXIP2D", &EFBjXIP2D, &b_EFBjXIP2D);
   fChain->SetBranchAddress("EFBjXIP3D", &EFBjXIP3D, &b_EFBjXIP3D);
   fChain->SetBranchAddress("EFBjXChi2", &EFBjXChi2, &b_EFBjXChi2);
   fChain->SetBranchAddress("EFBjXSVtx", &EFBjXSVtx, &b_EFBjXSVtx);
   fChain->SetBranchAddress("EFBjPtJet", &EFBjPtJet, &b_EFBjPtJet);
   fChain->SetBranchAddress("EFBjXMVtx", &EFBjXMVtx, &b_EFBjXMVtx);
   fChain->SetBranchAddress("EFBjXEVtx", &EFBjXEVtx, &b_EFBjXEVtx);
   fChain->SetBranchAddress("EFBjXNVtx", &EFBjXNVtx, &b_EFBjXNVtx);

   Notify();
}

Bool_t Ntuple::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Ntuple::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Ntuple::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Ntuple_cxx
