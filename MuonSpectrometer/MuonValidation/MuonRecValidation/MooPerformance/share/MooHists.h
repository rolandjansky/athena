//////////////////////////////////////////////////////////////
// Moore Validation/Performance Plots
//
// MooHists.h
//
// Nektarios Ch. Benekos     Thomas Moore, Stephane Willocq 
// CERN/ATC                  Univ of Massachusetts, Amherst
//
// June, 2004
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Added code to distinguish between real and fake muons
//
// Vivek Jain: Feb 28, 2006
// Update MooHists.h to reflect CBNT ntuple made in 11.3.0
// To match MO reconstructed muons to MU and CB, use
// the links provided in the CBNT rather than the eta/phi window
//
// Victoria Rojo, SUNY Albany, June - July 2006:
// Added code to look at the number of hits in the large (non-overlapping) and
// small (non-overlapping) sectors, and overlapping regions of the barrel.
// Organized hit plots into a seperate function: MakeHitPlots.
//
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 23 16:16:04 2006 by ROOT version 4.04/02f
// from TTree t3333/Combined Ntuple
// found on file: mupt100_simdig1100_rec1130_8.root
//////////////////////////////////////////////////////////

#ifndef MooHists_h
#define MooHists_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TProfile.h>

class MooHists : public TSelector {

 private:

   TString treeName;
   //   TFile* MooPerformanceHists;
   Int_t NumEvents;
   Int_t count;
   Int_t count1;
   static const Int_t maxRecMo = 100; // max. number of Mo muons
   static const Int_t nMuonType = 4; // for real and diff. kinds of fake
   //
   static const Int_t nRec = 4; 
   static const Float_t innerDetEtaCut = 2.5;
   static const Float_t mooreEtaCut = 2.7;
   static const Float_t BrlEtaCut = 1.0;
   static const Float_t ECMdtEtaCut = 1.5;
   static const Float_t ECCscEtaCut = 2.0;
 
   static const Int_t nPhiBins = 17;
   Float_t phiBinsBrl[nPhiBins+1];
   Float_t phiBinsECCsc[nPhiBins+1];
   Float_t phiBinsECMdt[nPhiBins+1];

   static const Int_t nEtaBins = 15;
   Float_t etaBins[nEtaBins+1];

   Float_t a0ResRange[nRec];
   Float_t z0ResRange[nRec];
   Float_t phiResRange[nRec];

   // n Hits and Fit Quality

   TH1F *nAllHits;
   TH2F *nKineMo;  // VJ
   TH1F *nKineMo_onlyfake;  // VJ
   TH1F *detType;
   TH1F *rpcEtaStatName;
   TH1F *rpcPhiStatName;
   TH1F *tgcEtaStatName;
   TH1F *tgcPhiStatName;
   TH1F *cscEtaStatName;
   TH1F *cscPhiStatName;
   TH1F *mdtStatName;

   // Normalized hit residuals
   TH1F *allHitRes; //allEtaHitRes
   TH1F *rpcEtaHitRes;
   TH1F *rpcPhiHitRes;
   TH1F *tgcEtaHitRes;
   TH1F *tgcPhiHitRes;
   TH1F *cscEtaHitRes;
   TH1F *cscPhiHitRes;
   TH1F *mdtHitRes;

   // Hit uncertainties
   TH1F *rpcEtaHitSig;
   TH1F *rpcPhiHitSig;
   TH1F *tgcEtaHitSig;
   TH1F *tgcPhiHitSig;
   TH1F *cscEtaHitSig;
   TH1F *cscEtaHitSig2;
   TH1F *cscPhiHitSig;
   TH1F *cscPhiHitSig2;
   TH1F *mdtHitSig;

   // Material map
   TH2F *nScatXY;
   TH2F *nScatRZ;
   TH2F *x0ScatXY;
   TH2F *x0ScatRZ;

   // Number of hits per track
   TH1F *nMdtHits;
   TH1F *nPrecEtaHits;
   TH1F *nTrigEtaHits;
   TH1F *nPhiHits;

   TH1F *precHitsLayer;
   TH1F *precHitsLayerEta1;
   TH1F *precHitsLayerEta2;
   TH1F *precHitsLayerEta3;

   // following defined by Victoria Rojo
   // for looking at real and different kind of fake muons
   // See .C file for details
   TProfile *nMdtHitsVsEta_[nMuonType];
   TProfile *nPhiHitsVsEta_[nMuonType];
   TProfile *nStationsVsEta_[nMuonType];
   TProfile *nPrecEtaHitsVsEta_[nMuonType];
   TProfile *nTrigEtaHitsVsEta_[nMuonType];

   TH2F* nmMdtHitsVsEta_[nMuonType];
   TH2F* nmPhiHitsVsEta_[nMuonType];
   TH2F* nmStationsVsEta_[nMuonType];
   TH2F* nmEtaHitsVsEta_[nMuonType];

   TH2F* nMdtHitsEtaPhi_[nMuonType];
   TH2F* nPhiHitsEtaPhi_[nMuonType];
   TH2F* nStationsEtaPhi_[nMuonType];
   TH2F* nEtaHitsEtaPhi_[nMuonType];

   TH2F* nmMdtHitsVsPhi1_[nMuonType];
   TH2F* nmPhiHitsVsPhi1_[nMuonType];
   TH2F* nmEtaHitsVsPhi1_[nMuonType];
   TH2F* nmEtaHitsVsPhi1a_[nMuonType];
   TH2F* nmEtaHitsVsPhi1b_[nMuonType];
   TH2F* nmMdtHitsVsPhi2_[nMuonType];
   TH2F* nmPhiHitsVsPhi2_[nMuonType];
   TH2F* nmEtaHitsVsPhi2_[nMuonType];
   TH2F* nmEtaHitsVsPhi2a_[nMuonType];
   TH2F* nmEtaHitsVsPhi2b_[nMuonType];
   TH2F* nmMdtHitsVsPhi3_[nMuonType];
   TH2F* nmPhiHitsVsPhi3_[nMuonType];
   TH2F* nmEtaHitsVsPhi3_[nMuonType];
   TH2F* nmMdtHitsVsPhi4_[nMuonType];
   TH2F* nmPhiHitsVsPhi4_[nMuonType];
   TH2F* nmEtaHitsVsPhi4_[nMuonType];

   TH2F* nmEtaLargeSectorHitsVsEta_no_OL_[nMuonType];
   TH2F* nmEtaSmallSectorHitsVsEta_no_OL_[nMuonType];
   TH2F* nmEtaOverlapHitsVsEta_[nMuonType];
   TH2F* SectorVsPhi_[nMuonType];

   TH2F* nmEtaLargeSectorHitsVsEtaEC1_no_OL_[nMuonType];
   TH2F* nmEtaLargeSectorHitsVsEtaEC2_no_OL_[nMuonType];
   TH2F* nmEtaLargeSectorHitsVsEtaEC3_no_OL_[nMuonType];
   TH2F* nmEtaLargeSectorHitsVsEtaEC4_no_OL_[nMuonType];

   TH2F* nmEtaSmallSectorHitsVsEtaEC1_no_OL_[nMuonType];
   TH2F* nmEtaSmallSectorHitsVsEtaEC2_no_OL_[nMuonType];
   TH2F* nmEtaSmallSectorHitsVsEtaEC3_no_OL_[nMuonType];
   TH2F* nmEtaSmallSectorHitsVsEtaEC4_no_OL_[nMuonType];

   TH2F* nmEtaOverlapHitsVsEtaEC1_[nMuonType];
   TH2F* nmEtaOverlapHitsVsEtaEC2_[nMuonType];
   TH2F* nmEtaOverlapHitsVsEtaEC3_[nMuonType];
   TH2F* nmEtaOverlapHitsVsEtaEC4_[nMuonType];

   TH1F *nRecTrks[nRec];
   TH1F *nRecTrks1[nRec];
   TH1F *nRecTrks_good[nRec]; // These three
   TH2F* nRecTrksVsEta[nRec]; // defined by
   TH2F* NumRealMuons_wFakes[nRec];
   TH1F* rassocPlot[nRec];    // VJ
   TH1F *chi2[nRec];
   TH1F *chi2Brl[nRec];
   TH1F *chi2ECMdt[nRec];
   TH1F *chi2ECCsc[nRec];
   TH1F *chi2Prob[nRec];
   TH1F *chi2ProbBrl[nRec];
   TH1F *chi2ProbECMdt[nRec];
   TH1F *chi2ProbECCsc[nRec];

   // 1/pT Resolution and pull
   TH1F *pTRes[nRec];
   TH1F *pTPull[nRec];
   TH1F *pTResBrl[nRec];
   TH1F *pTPullBrl[nRec];
   TH1F *pTResECCsc[nRec];
   TH1F *pTPullECCsc[nRec];
   TH1F *pTResECMdt[nRec];
   TH1F *pTPullECMdt[nRec];

   TH1F *pTResMeanVsBrlPhi[nRec];
   TH1F *pTResWidthVsBrlPhi[nRec];
   TH1F *pTPullMeanVsBrlPhi[nRec];
   TH1F *pTPullWidthVsBrlPhi[nRec];
   TH1F *pTResFitsVsBrlPhi[nRec][nPhiBins]; 
   TH1F *pTPullFitsVsBrlPhi[nRec][nPhiBins];

   TH1F *pTResMeanVsECCscPhi[nRec];
   TH1F *pTResWidthVsECCscPhi[nRec];
   TH1F *pTPullMeanVsECCscPhi[nRec];
   TH1F *pTPullWidthVsECCscPhi[nRec];
   TH1F *pTResFitsVsECCscPhi[nRec][nPhiBins]; 
   TH1F *pTPullFitsVsECCscPhi[nRec][nPhiBins];

   TH1F *pTResMeanVsECMdtPhi[nRec];
   TH1F *pTResWidthVsECMdtPhi[nRec];
   TH1F *pTPullMeanVsECMdtPhi[nRec];
   TH1F *pTPullWidthVsECMdtPhi[nRec];
   TH1F *pTResFitsVsECMdtPhi[nRec][nPhiBins]; 
   TH1F *pTPullFitsVsECMdtPhi[nRec][nPhiBins];

   TH1F *pTResVsBrlPhiReg[nRec];
   TH1F *pTPullVsBrlPhiReg[nRec];
   TH1F *pTResVsECMdt1PhiReg[nRec];
   TH1F *pTPullVsECMdt1PhiReg[nRec];
   TH1F *pTResVsECMdt2PhiReg[nRec];
   TH1F *pTPullVsECMdt2PhiReg[nRec];
   TH1F *pTResVsECCscPhiReg[nRec];
   TH1F *pTPullVsECCscPhiReg[nRec];
   TH1F *pTResFitsVsBrlPhiReg[nRec][4];   //Phi regions defined in phiBin.C 
   TH1F *pTPullFitsVsBrlPhiReg[nRec][4];
   TH1F *pTResFitsVsECMdt1PhiReg[nRec][4];   
   TH1F *pTPullFitsVsECMdt1PhiReg[nRec][4];
   TH1F *pTResFitsVsECMdt2PhiReg[nRec][4];   
   TH1F *pTPullFitsVsECMdt2PhiReg[nRec][4];
   TH1F *pTResFitsVsECCscPhiReg[nRec][4];   
   TH1F *pTPullFitsVsECCscPhiReg[nRec][4];

   TH1F *pTResMeanVsEta[nRec];
   TH1F *pTResWidthVsEta[nRec];
   TH1F *pTPullMeanVsEta[nRec];
   TH1F *pTPullWidthVsEta[nRec];
   TH1F *pTResFitsVsEta[nRec][nEtaBins]; 
   TH1F *pTPullFitsVsEta[nRec][nEtaBins];

   // a0 Resolution and Pull
   TH1F *a0Res[nRec];
   TH1F *a0Pull[nRec];
   TH1F *a0ResBrl[nRec];
   TH1F *a0PullBrl[nRec];
   TH1F *a0ResECCsc[nRec];
   TH1F *a0PullECCsc[nRec];
   TH1F *a0ResECMdt[nRec];
   TH1F *a0PullECMdt[nRec];

   TH1F *a0PullMeanVsEta[nRec];
   TH1F *a0PullWidthVsEta[nRec];
   TH1F *a0PullFitsVsEta[nRec][nEtaBins];

   // z0 Resolution and Pull
   TH1F *z0Res[nRec];
   TH1F *z0Pull[nRec];
   TH1F *z0ResBrl[nRec];
   TH1F *z0PullBrl[nRec];
   TH1F *z0ResECCsc[nRec];
   TH1F *z0PullECCsc[nRec];
   TH1F *z0ResECMdt[nRec];
   TH1F *z0PullECMdt[nRec];

   TH1F *z0PullMeanVsEta[nRec];
   TH1F *z0PullWidthVsEta[nRec];
   TH1F *z0PullFitsVsEta[nRec][nEtaBins];

   // phi Resolution and Pull
   TH1F *phiRes[nRec];
   TH1F *phiPull[nRec];
   TH1F *phiResBrl[nRec];
   TH1F *phiPullBrl[nRec];
   TH1F *phiResECCsc[nRec];
   TH1F *phiPullECCsc[nRec];
   TH1F *phiResECMdt[nRec];
   TH1F *phiPullECMdt[nRec];

   TH1F *phiResMeanVsEta[nRec];
   TH1F *phiResWidthVsEta[nRec];
   TH1F *phiResFitsVsEta[nRec][nEtaBins];
   TH1F *phiPullMeanVsEta[nRec];
   TH1F *phiPullWidthVsEta[nRec];
   TH1F *phiPullFitsVsEta[nRec][nEtaBins];

   // cot(Theta) Resolution and Pull
   TH1F *cotThetaRes[nRec];
   TH1F *cotThetaPull[nRec];
   TH1F *cotThetaResBrl[nRec];
   TH1F *cotThetaPullBrl[nRec];
   TH1F *cotThetaResECCsc[nRec];
   TH1F *cotThetaPullECCsc[nRec];
   TH1F *cotThetaResECMdt[nRec];
   TH1F *cotThetaPullECMdt[nRec];

   TH1F *cotThetaPullMeanVsEta[nRec];
   TH1F *cotThetaPullWidthVsEta[nRec];
   TH1F *cotThetaPullFitsVsEta[nRec][nEtaBins];

   // efficiency
   TH1F *effVsPhiBrl[nRec];
   TH1F *effVsPhiECCsc[nRec];
   TH1F *effVsPhiECMdt[nRec];
   TH1F *effVsPhi1Bin[nRec];
   TH1F *effVsEta[nRec];
   TH2F *effVsEtaPhi[nRec];

   // following plots defined by VJ
   TH1F *effVsEta_unNorm[nRec];
   TH2F *effVsEtaPhi_unNorm[nRec];
   TH1F *pT_real[nRec];
   TH1F *pT1_real[nRec];
   TH1F *pT2_real[nRec];
   TH2F *pTVsEta_real[nRec];
   TH2F *pT1VsEta_real[nRec];
   TH1F *Heta_real[nRec];
   TH1F *Hphi_real[nRec];
   TH1F *match_real[nRec];
   TH2F *matchVspT_real[nRec];
   TH2F *matchVspT1_real[nRec];
   TH2F *matchVseta_real[nRec];
   TH2F *matchVsnHits_real[nRec];
   TH1F *NumHitsPhi_real[nRec];
   TH1F *NumHitsMdt_real[nRec];
   TH1F *ProbChiSq_real[nRec];
   //
   TH1F *pT_fake[nRec];
   TH1F *pT1_fake[nRec];
   TH1F *pT2_fake[nRec];
   TH2F* pTVsEta_fake[nRec];
   TH2F* pT1VsEta_fake[nRec];
   TH1F *eta_fake[nRec];
   TH1F* phi_fake[nRec];
   TH1F* effVsEta_fake[nRec];
   TH1F* effVsEta_fake1[nRec];
   TH2F* effVsEtaPhi_fake[nRec];
   TH1F *NumHitsPhi_fake[nRec];
   TH1F *NumHitsMdt_fake[nRec];
   TH1F *ProbChiSq_fake[nRec];
   TH1F *ChiSq_fake[nRec];

   TH2F* pTVsEta_onlyfake[nRec];  // Five plots for when only fake muons in event
   TH2F* pT1VsEta_onlyfake[nRec];
   TH1F* phi_onlyfake[nRec];
   TH2F* effVsEtaPhi_onlyfake[nRec];
   TH2F* etaPhiGMo_onlyfake[nRec];

   //
   TH2F* pTVsEta_fakeEvt[nRec];
   TH2F* pT1VsEta_fakeEvt[nRec];
   TH1F* effVsEta_fakeEvt[nRec];
   TH1F* effVsEta_fake1Evt[nRec];
   TH2F* effVsEtaPhi_fakeEvt[nRec];

   TH1F *effVsPhiBrl_fakeEvt[nRec];
   TH1F *effVsPhiECCsc_fakeEvt[nRec];
   TH1F *effVsPhiECMdt_fakeEvt[nRec];
   TH1F *effVsPhiBrl_fake1Evt[nRec];
   TH1F *effVsPhiECCsc_fake1Evt[nRec];
   TH1F *effVsPhiECMdt_fake1Evt[nRec];


   //
   TH1F* statimo_fake[nRec];
   TH1F* statimo_real[nRec];
   TH1F* statimo_all[nRec];
   //
   TH2F* PhidiffVsEta_fake[nRec];
   TH2F* PhidiffVsPhi_fake[nRec];
   TH2F* EtadiffVsEta_fake[nRec];
   TH2F* EtadiffVsPhi_fake[nRec];
   //
   TH2F* chi2_5var[nRec];
   TH1F* chi2_5var1[nRec];
   TH2F* chi2_pT[nRec];
   TH2F* chi2_a0[nRec];
   TH2F* chi2_z0[nRec];
   TH2F* chi2_phi[nRec];
   TH2F* chi2_cth[nRec];
   //
   // diff. between Mu and Mo
   //
   TH2F* pT_diffMuMo;
   TH2F* pTprelossmu_diffMuMo;
   TH2F* pTelossgmo_diffMuMo;

   TH2F* prelossmu_eta;
   TH2F* elossgmo_eta;
   TH2F* prelossmu_phi;
   TH2F* elossgmo_phi;
   TH2F* prelossmu_elossgmo;

   TH2F* phi_diffMuMo;
   TH2F* eta_diffMuMo;
   TH2F* phi_diffMuMo1;
   TH2F* eta_diffMuMo1;

   TH2F* pT_phidiffMuMo;
   TH2F* pT1_phidiffMuMo;

   TH1F* match_MO_MU_hist;
   TH1F* match_MO_CB_hist;

   TH2F* eta_phi_diffMoCb;
   TH2F* eta_phi_diffMoCb1;
   //
   // end of plots defined by VJ
   // generated eta and phi
   TH1F *phiGMo[nRec];
   TH1F *phiGMoCutBrl[nRec];    
   TH1F *phiGMoCutECCsc[nRec]; 
   TH1F *phiGMoCutECMdt[nRec]; 
   TH1F *phiGMoCut1Bin[nRec]; // just for making eff vs pT graph
   TH1F *etaGMo[nRec];
   TH2F *etaPhiGMo[nRec];
   TH1F *pTGMo[nRec];    // defined by VJ
   TH1F *pT1GMo[nRec];  // VJ
   TH2F *pTVsEtaGMo[nRec];    // defined by VJ
   TH2F *pT1VsEtaGMo[nRec];  // VJ

public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   Int_t           RunNumber;
   Int_t           EventNumber;
   Char_t          StreamESD_ref;
   Char_t          StreamRDO_ref;
   Char_t          Token[153];
   Int_t           Run;
   Int_t           Event;
   Int_t           Time;
   Double_t        Weight;
   Int_t           IEvent;
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
   Int_t           Spcl_Num;
   vector<long>    *Spcl_ID;
   vector<float>   *Spcl_Pt;
   vector<float>   *Spcl_Eta;
   vector<float>   *Spcl_Phi;
   vector<float>   *Spcl_M;
   vector<long>    *Spcl_Stat;
   vector<float>   *Spcl_Isol;
   Float_t         Spcl_sumx;
   Float_t         Spcl_sumy;
   Float_t         Spcl_sumz;
   Float_t         Spcl_sume;
   Float_t         Spcl_x1;
   Float_t         Spcl_x2;
   Int_t           Spcl_id1;
   Int_t           Spcl_id2;
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
   vector<long>    *Trk_numberOfTrackSummaryTypes;
   vector<long>    *Trk_truthBarcode;
   vector<long>    *Trk_truthNt;
   vector<long>    *Trk_ParticlePdg;
   vector<long>    *Trk_ParentPdg;
   Int_t           Nh_Calo;
   Float_t         Eh_Calo;
   Int_t           Nh_EM;
   Float_t         Eh_EM;
   Int_t           Nh_HAD;
   Float_t         Eh_HAD;
   Int_t           Nh_PresB;
   vector<float>   *Eh_PresB;
   Int_t           Nh_EMB;
   vector<float>   *Eh_EMB;
   Int_t           Nh_EMEC;
   vector<float>   *Eh_EMEC;
   Int_t           Nh_Tile;
   vector<float>   *Eh_Tile;
   Int_t           Nh_TileGap;
   vector<float>   *Eh_TileGap;
   Int_t           Nh_HEC;
   vector<float>   *Eh_HEC;
   Int_t           Nh_FCal;
   vector<float>   *Eh_FCal;
   Int_t           Nh_PresE;
   vector<float>   *Eh_PresE;
   Int_t           Nh_Scint;
   vector<float>   *Eh_Scint;
   Int_t           nhit_em;
   Float_t         ecell_em;
   Int_t           nhit_hec;
   Float_t         ecell_hec;
   Int_t           nhit_fcal;
   Float_t         ecell_fcal;
   Int_t           nhit_tile;
   Float_t         ecell_tile;
   Int_t           Moore_nkine;
   vector<float>   *Moore_xvtxg;
   vector<float>   *Moore_yvtxg;
   vector<float>   *Moore_zvtxg;
   vector<float>   *Moore_a0g;
   vector<float>   *Moore_z0g;
   vector<float>   *Moore_phig;
   vector<float>   *Moore_thetag;
   vector<float>   *Moore_qpig;
   vector<float>   *Moore_etag;
   vector<int>     *Moore_rassociated;
   vector<float>   *Moore_matchdg;
   vector<float>   *Moore_xextrg;
   vector<float>   *Moore_yextrg;
   vector<float>   *Moore_zextrg;
   vector<float>   *Moore_a0extrg;
   vector<float>   *Moore_z0extrg;
   vector<float>   *Moore_phiextrg;
   vector<float>   *Moore_thetaextrg;
   vector<float>   *Moore_qpiextrg;
   vector<float>   *Moore_etaextrg;
   vector<float>   *Moore_barcodeg;
   vector<float>   *Moore_statusg;
   vector<float>   *Moore_elossg;
   vector<float>   *Moore_drphig;
   vector<float>   *Moore_drzg;
   vector<float>   *Moore_dphig;
   vector<float>   *Moore_dthetag;
   vector<float>   *Moore_a0pullg;
   vector<float>   *Moore_z0pullg;
   vector<float>   *Moore_phipullg;
   vector<float>   *Moore_thetapullg;
   vector<float>   *Moore_qpipullg;
   Int_t           Moore_nmuon;
   vector<int>     *Moore_phih;
   vector<int>     *Moore_etah;
   vector<int>     *Moore_mdth;
   vector<int>     *Moore_cscetah;
   vector<int>     *Moore_cscphih;
   vector<int>     *Moore_rpcetah;
   vector<int>     *Moore_rpcphih;
   vector<int>     *Moore_tgcetah;
   vector<int>     *Moore_tgcphih;
   vector<int>     *Moore_stati;
   vector<int>     *Moore_statr;
   vector<float>   *Moore_xvtxr;
   vector<float>   *Moore_yvtxr;
   vector<float>   *Moore_zvtxr;
   vector<float>   *Moore_a0r;
   vector<float>   *Moore_z0r;
   vector<float>   *Moore_phir;
   vector<float>   *Moore_thetar;
   vector<float>   *Moore_qpir;
   vector<float>   *Moore_etar;
   vector<float>   *Moore_chi2;
   vector<float>   *Moore_chi2pr;
   vector<float>   *Moore_origx;
   vector<float>   *Moore_origy;
   vector<float>   *Moore_origz;
   vector<float>   *Moore_covr11;
   vector<float>   *Moore_covr12;
   vector<float>   *Moore_covr22;
   vector<float>   *Moore_covr13;
   vector<float>   *Moore_covr23;
   vector<float>   *Moore_covr33;
   vector<float>   *Moore_covr14;
   vector<float>   *Moore_covr24;
   vector<float>   *Moore_covr34;
   vector<float>   *Moore_covr44;
   vector<float>   *Moore_covr15;
   vector<float>   *Moore_covr25;
   vector<float>   *Moore_covr35;
   vector<float>   *Moore_covr45;
   vector<float>   *Moore_covr55;
   vector<int>     *Moore_nhits;

   //typedef std::vector<std::vector<float> > Matrix_t;
   typedef vector<vector<float> > Matrix_t;

   Matrix_t *Moore_hit_statname;
   Matrix_t *Moore_hit_stateta;
   Matrix_t *Moore_hit_statphi;
   Matrix_t *Moore_hit_statreg;
   Matrix_t *Moore_hit_tech;
   Matrix_t *Moore_hit_dtyp;
   Matrix_t *Moore_hit_dblr;
   Matrix_t *Moore_hit_dblz;
   Matrix_t *Moore_hit_dblp;
   Matrix_t *Moore_hit_ggap;
   Matrix_t *Moore_hit_strp;
   Matrix_t *Moore_hit_chnl;
   Matrix_t *Moore_hit_clyr;
   Matrix_t *Moore_hit_wlyr;
   Matrix_t *Moore_hit_mlyr;
   Matrix_t *Moore_hit_tlyr;
   Matrix_t *Moore_hit_tube;
   Matrix_t *Moore_hit_rho;
   Matrix_t *Moore_hit_phi;
   Matrix_t *Moore_hit_z;
   Matrix_t *Moore_hit_theta;
   Matrix_t *Moore_hit_eta;
   Matrix_t *Moore_hit_xloc;
   Matrix_t *Moore_hit_yloc;
   Matrix_t *Moore_hit_zloc;
   Matrix_t *Moore_hit_drd;
   Matrix_t *Moore_hit_sig;
   Matrix_t *Moore_hit_res;
   Matrix_t *Moore_hit_x0;
   Int_t           Muid_nmuonms;
   vector<float>   *Muid_a0rms;
   vector<float>   *Muid_z0rms;
   vector<float>   *Muid_phirms;
   vector<float>   *Muid_thetarms;
   vector<float>   *Muid_etarms;
   vector<float>   *Muid_pirms;
   vector<float>   *Muid_chi2ms;
   vector<long>    *Muid_mdtms;
   vector<long>    *Muid_cscetams;
   vector<long>    *Muid_cscphims;
   vector<long>    *Muid_rpcetams;
   vector<long>    *Muid_rpcphims;
   vector<long>    *Muid_tgcetams;
   vector<long>    *Muid_tgcphims;
   vector<float>   *Muid_covr11ms;
   vector<float>   *Muid_covr12ms;
   vector<float>   *Muid_covr22ms;
   vector<float>   *Muid_covr13ms;
   vector<float>   *Muid_covr23ms;
   vector<float>   *Muid_covr33ms;
   vector<float>   *Muid_covr14ms;
   vector<float>   *Muid_covr24ms;
   vector<float>   *Muid_covr34ms;
   vector<float>   *Muid_covr44ms;
   vector<float>   *Muid_covr15ms;
   vector<float>   *Muid_covr25ms;
   vector<float>   *Muid_covr35ms;
   vector<float>   *Muid_covr45ms;
   vector<float>   *Muid_covr55ms;
   Int_t           Muid_nmuonmu;
   vector<float>   *Muid_prelossmu;
   vector<float>   *Muid_erelossmu;
   vector<long>    *Muid_typeelossmu;
   vector<float>   *Muid_a0rmu;
   vector<float>   *Muid_z0rmu;
   vector<float>   *Muid_phirmu;
   vector<float>   *Muid_thetarmu;
   vector<float>   *Muid_etarmu;
   vector<float>   *Muid_pirmu;
   vector<float>   *Muid_chi2mu;
   vector<float>   *Muid_covr11mu;
   vector<float>   *Muid_covr12mu;
   vector<float>   *Muid_covr22mu;
   vector<float>   *Muid_covr13mu;
   vector<float>   *Muid_covr23mu;
   vector<float>   *Muid_covr33mu;
   vector<float>   *Muid_covr14mu;
   vector<float>   *Muid_covr24mu;
   vector<float>   *Muid_covr34mu;
   vector<float>   *Muid_covr44mu;
   vector<float>   *Muid_covr15mu;
   vector<float>   *Muid_covr25mu;
   vector<float>   *Muid_covr35mu;
   vector<float>   *Muid_covr45mu;
   vector<float>   *Muid_covr55mu;
   vector<long>    *Muid_mslinkmu;
   Int_t           Muid_nmuoncb;
   vector<float>   *Muid_chi2mcb;
   vector<long>    *Muid_loosecb;
   vector<float>   *Muid_a0rcb;
   vector<float>   *Muid_z0rcb;
   vector<float>   *Muid_phircb;
   vector<float>   *Muid_thetarcb;
   vector<float>   *Muid_etarcb;
   vector<float>   *Muid_pircb;
   vector<float>   *Muid_chi2cb;
   vector<float>   *Muid_chi2prcb;
   vector<float>   *Muid_covr11cb;
   vector<float>   *Muid_covr12cb;
   vector<float>   *Muid_covr22cb;
   vector<float>   *Muid_covr13cb;
   vector<float>   *Muid_covr23cb;
   vector<float>   *Muid_covr33cb;
   vector<float>   *Muid_covr14cb;
   vector<float>   *Muid_covr24cb;
   vector<float>   *Muid_covr34cb;
   vector<float>   *Muid_covr44cb;
   vector<float>   *Muid_covr15cb;
   vector<float>   *Muid_covr25cb;
   vector<float>   *Muid_covr35cb;
   vector<float>   *Muid_covr45cb;
   vector<float>   *Muid_covr55cb;
   vector<long>    *Muid_mslinkcb;
   vector<long>    *Muid_exlinkcb;
   vector<long>    *Muid_blayercb;
   vector<long>    *Muid_pixelcb;
   vector<long>    *Muid_sctcb;
   vector<long>    *Muid_trtcb;
   vector<long>    *Muid_trthighcb;
   vector<float>   *Muid_a0rid;
   vector<float>   *Muid_z0rid;
   vector<float>   *Muid_phirid;
   vector<float>   *Muid_thetarid;
   vector<float>   *Muid_etarid;
   vector<float>   *Muid_pirid;
   vector<float>   *Muid_chi2id;
   vector<float>   *Muid_pulla0cb;
   vector<float>   *Muid_pullz0cb;
   vector<float>   *Muid_pullphicb;
   vector<float>   *Muid_pullthecb;
   vector<float>   *Muid_pullmomcb;
   Float_t         MemRSS;
   Float_t         MemVSize;
   Float_t         TimeTotal;
   Float_t         TimeSum;
   Float_t         TimeEventCounter;
   Float_t         TimeDetStatusAlg;
   Float_t         TimeInDetPixelClusterization;
   Float_t         TimeInDetSCT_Clusterization;
   Float_t         TimeInDetTRT_RIO_Maker;
   Float_t         TimeInDetPRD_MultiTruthMaker;
   Float_t         TimeInDetSiTrackerSpacePointFinder;
   Float_t         TimeiPatRec;
   Float_t         TimeiPatTrackTruthAssociator;
   Float_t         TimeiPatStatistics;
   Float_t         TimeInDetLegacyCnvAlg;
   Float_t         TimeConvertedIPatTracksDetailedTruthMaker;
   Float_t         TimeConvertedIPatTracksTruthSelector;
   Float_t         TimeInDetCopyAlg;
   Float_t         TimeDetailedTrackTruthMaker;
   Float_t         TimeTrackTruthCollectionSelector;
   Float_t         TimeInDetPriVxFinder;
   Float_t         TimeInDetParticleCreation;
   Float_t         TimeInDetTrackParticleTruthMaker;
   Float_t         TimeInDetRecStatistics;
   Float_t         TimeInDetTrackClusterAssValidation;
   Float_t         TimeCaloCellMaker;
   Float_t         TimeMuonRdoToMuonDigit;
   Float_t         TimeCscRdoToCscPrepData;
   Float_t         TimeMdtRdoToMdtPrepData;
   Float_t         TimeRpcRdoToRpcPrepData;
   Float_t         TimeTgcRdoToTgcPrepData;
   Float_t         TimeCscThresholdClusterBuilder;
   Float_t         TimeTrackRecordFilter;
   Float_t         TimeMooMakePhiPatterns;
   Float_t         TimeMooMakeRzPatterns;
   Float_t         TimeMooCombinePatterns;
   Float_t         TimeMooCalibratedSegmentMaker;
   Float_t         TimeMooRzSegmentCombinationMaker;
   Float_t         TimePhiPatternToPhiSeg;
   Float_t         TimeCscPatternToCrudeSeg;
   Float_t         TimeMdtPatternToCrudeSeg;
   Float_t         TimeMuonSegmentComboToMooRzSegmentCombo;
   Float_t         TimeMuonSegmentToMooRzSegment;
   Float_t         TimeMooRoadMaker;
   Float_t         TimeMooMakeTracks;
   Float_t         TimeMooCnvAlg;
   Float_t         TimeMuidMooreTPCreator;
   Float_t         TimeMuidStandAlone;
   Float_t         TimeMuidExtrCnvAlg;
   Float_t         TimeMuidCombined;
   Float_t         TimeMuidCnvAlg;
   Float_t         TimeMuidStatistics;
   Float_t         TimeCBNT_AthenaAware;
   Float_t         TimeAANTupleStream;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_StreamESD_ref;   //!
   TBranch        *b_StreamRDO_ref;   //!
   TBranch        *b_Token;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_IEvent;   //!
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
   TBranch        *b_Spcl_Num;   //!
   TBranch        *b_Spcl_ID;   //!
   TBranch        *b_Spcl_Pt;   //!
   TBranch        *b_Spcl_Eta;   //!
   TBranch        *b_Spcl_Phi;   //!
   TBranch        *b_Spcl_M;   //!
   TBranch        *b_Spcl_Stat;   //!
   TBranch        *b_Spcl_Isol;   //!
   TBranch        *b_Spcl_sumx;   //!
   TBranch        *b_Spcl_sumy;   //!
   TBranch        *b_Spcl_sumz;   //!
   TBranch        *b_Spcl_sume;   //!
   TBranch        *b_Spcl_x1;   //!
   TBranch        *b_Spcl_x2;   //!
   TBranch        *b_Spcl_id1;   //!
   TBranch        *b_Spcl_id2;   //!
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
   TBranch        *b_Trk_numberOfTrackSummaryTypes;   //!
   TBranch        *b_Trk_truthBarcode;   //!
   TBranch        *b_Trk_truthNt;   //!
   TBranch        *b_Trk_ParticlePdg;   //!
   TBranch        *b_Trk_ParentPdg;   //!
   TBranch        *b_Nh_Calo;   //!
   TBranch        *b_Eh_Calo;   //!
   TBranch        *b_Nh_EM;   //!
   TBranch        *b_Eh_EM;   //!
   TBranch        *b_Nh_HAD;   //!
   TBranch        *b_Eh_HAD;   //!
   TBranch        *b_Nh_PresB;   //!
   TBranch        *b_Eh_PresB;   //!
   TBranch        *b_Nh_EMB;   //!
   TBranch        *b_Eh_EMB;   //!
   TBranch        *b_Nh_EMEC;   //!
   TBranch        *b_Eh_EMEC;   //!
   TBranch        *b_Nh_Tile;   //!
   TBranch        *b_Eh_Tile;   //!
   TBranch        *b_Nh_TileGap;   //!
   TBranch        *b_Eh_TileGap;   //!
   TBranch        *b_Nh_HEC;   //!
   TBranch        *b_Eh_HEC;   //!
   TBranch        *b_Nh_FCal;   //!
   TBranch        *b_Eh_FCal;   //!
   TBranch        *b_Nh_PresE;   //!
   TBranch        *b_Eh_PresE;   //!
   TBranch        *b_Nh_Scint;   //!
   TBranch        *b_Eh_Scint;   //!
   TBranch        *b_nhit_em;   //!
   TBranch        *b_ecell_em;   //!
   TBranch        *b_nhit_hec;   //!
   TBranch        *b_ecell_hec;   //!
   TBranch        *b_nhit_fcal;   //!
   TBranch        *b_ecell_fcal;   //!
   TBranch        *b_nhit_tile;   //!
   TBranch        *b_ecell_tile;   //!
   TBranch        *b_Moore_nkine;   //!
   TBranch        *b_Moore_xvtxg;   //!
   TBranch        *b_Moore_yvtxg;   //!
   TBranch        *b_Moore_zvtxg;   //!
   TBranch        *b_Moore_a0g;   //!
   TBranch        *b_Moore_z0g;   //!
   TBranch        *b_Moore_phig;   //!
   TBranch        *b_Moore_thetag;   //!
   TBranch        *b_Moore_qpig;   //!
   TBranch        *b_Moore_etag;   //!
   TBranch        *b_Moore_rassociated;   //!
   TBranch        *b_Moore_matchdg;   //!
   TBranch        *b_Moore_xextrg;   //!
   TBranch        *b_Moore_yextrg;   //!
   TBranch        *b_Moore_zextrg;   //!
   TBranch        *b_Moore_a0extrg;   //!
   TBranch        *b_Moore_z0extrg;   //!
   TBranch        *b_Moore_phiextrg;   //!
   TBranch        *b_Moore_thetaextrg;   //!
   TBranch        *b_Moore_qpiextrg;   //!
   TBranch        *b_Moore_etaextrg;   //!
   TBranch        *b_Moore_barcodeg;   //!
   TBranch        *b_Moore_statusg;   //!
   TBranch        *b_Moore_elossg;   //!
   TBranch        *b_Moore_drphig;   //!
   TBranch        *b_Moore_drzg;   //!
   TBranch        *b_Moore_dphig;   //!
   TBranch        *b_Moore_dthetag;   //!
   TBranch        *b_Moore_a0pullg;   //!
   TBranch        *b_Moore_z0pullg;   //!
   TBranch        *b_Moore_phipullg;   //!
   TBranch        *b_Moore_thetapullg;   //!
   TBranch        *b_Moore_qpipullg;   //!
   TBranch        *b_Moore_nmuon;   //!
   TBranch        *b_Moore_phih;   //!
   TBranch        *b_Moore_etah;   //!
   TBranch        *b_Moore_mdth;   //!
   TBranch        *b_Moore_cscetah;   //!
   TBranch        *b_Moore_cscphih;   //!
   TBranch        *b_Moore_rpcetah;   //!
   TBranch        *b_Moore_rpcphih;   //!
   TBranch        *b_Moore_tgcetah;   //!
   TBranch        *b_Moore_tgcphih;   //!
   TBranch        *b_Moore_stati;   //!
   TBranch        *b_Moore_statr;   //!
   TBranch        *b_Moore_xvtxr;   //!
   TBranch        *b_Moore_yvtxr;   //!
   TBranch        *b_Moore_zvtxr;   //!
   TBranch        *b_Moore_a0r;   //!
   TBranch        *b_Moore_z0r;   //!
   TBranch        *b_Moore_phir;   //!
   TBranch        *b_Moore_thetar;   //!
   TBranch        *b_Moore_qpir;   //!
   TBranch        *b_Moore_etar;   //!
   TBranch        *b_Moore_chi2;   //!
   TBranch        *b_Moore_chi2pr;   //!
   TBranch        *b_Moore_origx;   //!
   TBranch        *b_Moore_origy;   //!
   TBranch        *b_Moore_origz;   //!
   TBranch        *b_Moore_covr11;   //!
   TBranch        *b_Moore_covr12;   //!
   TBranch        *b_Moore_covr22;   //!
   TBranch        *b_Moore_covr13;   //!
   TBranch        *b_Moore_covr23;   //!
   TBranch        *b_Moore_covr33;   //!
   TBranch        *b_Moore_covr14;   //!
   TBranch        *b_Moore_covr24;   //!
   TBranch        *b_Moore_covr34;   //!
   TBranch        *b_Moore_covr44;   //!
   TBranch        *b_Moore_covr15;   //!
   TBranch        *b_Moore_covr25;   //!
   TBranch        *b_Moore_covr35;   //!
   TBranch        *b_Moore_covr45;   //!
   TBranch        *b_Moore_covr55;   //!
   TBranch        *b_Moore_nhits;   //!
   TBranch        *b_Moore_hit_statname;   //!
   TBranch        *b_Moore_hit_stateta;   //!
   TBranch        *b_Moore_hit_statphi;   //!
   TBranch        *b_Moore_hit_statreg;   //!
   TBranch        *b_Moore_hit_tech;   //!
   TBranch        *b_Moore_hit_dtyp;   //!
   TBranch        *b_Moore_hit_dblr;   //!
   TBranch        *b_Moore_hit_dblz;   //!
   TBranch        *b_Moore_hit_dblp;   //!
   TBranch        *b_Moore_hit_ggap;   //!
   TBranch        *b_Moore_hit_strp;   //!
   TBranch        *b_Moore_hit_chnl;   //!
   TBranch        *b_Moore_hit_clyr;   //!
   TBranch        *b_Moore_hit_wlyr;   //!
   TBranch        *b_Moore_hit_mlyr;   //!
   TBranch        *b_Moore_hit_tlyr;   //!
   TBranch        *b_Moore_hit_tube;   //!
   TBranch        *b_Moore_hit_rho;   //!
   TBranch        *b_Moore_hit_phi;   //!
   TBranch        *b_Moore_hit_z;   //!
   TBranch        *b_Moore_hit_theta;   //!
   TBranch        *b_Moore_hit_eta;   //!
   TBranch        *b_Moore_hit_xloc;   //!
   TBranch        *b_Moore_hit_yloc;   //!
   TBranch        *b_Moore_hit_zloc;   //!
   TBranch        *b_Moore_hit_drd;   //!
   TBranch        *b_Moore_hit_sig;   //!
   TBranch        *b_Moore_hit_res;   //!
   TBranch        *b_Moore_hit_x0;   //!
   TBranch        *b_Muid_nmuonms;   //!
   TBranch        *b_Muid_a0rms;   //!
   TBranch        *b_Muid_z0rms;   //!
   TBranch        *b_Muid_phirms;   //!
   TBranch        *b_Muid_thetarms;   //!
   TBranch        *b_Muid_etarms;   //!
   TBranch        *b_Muid_pirms;   //!
   TBranch        *b_Muid_chi2ms;   //!
   TBranch        *b_Muid_mdtms;   //!
   TBranch        *b_Muid_cscetams;   //!
   TBranch        *b_Muid_cscphims;   //!
   TBranch        *b_Muid_rpcetams;   //!
   TBranch        *b_Muid_rpcphims;   //!
   TBranch        *b_Muid_tgcetams;   //!
   TBranch        *b_Muid_tgcphims;   //!
   TBranch        *b_Muid_covr11ms;   //!
   TBranch        *b_Muid_covr12ms;   //!
   TBranch        *b_Muid_covr22ms;   //!
   TBranch        *b_Muid_covr13ms;   //!
   TBranch        *b_Muid_covr23ms;   //!
   TBranch        *b_Muid_covr33ms;   //!
   TBranch        *b_Muid_covr14ms;   //!
   TBranch        *b_Muid_covr24ms;   //!
   TBranch        *b_Muid_covr34ms;   //!
   TBranch        *b_Muid_covr44ms;   //!
   TBranch        *b_Muid_covr15ms;   //!
   TBranch        *b_Muid_covr25ms;   //!
   TBranch        *b_Muid_covr35ms;   //!
   TBranch        *b_Muid_covr45ms;   //!
   TBranch        *b_Muid_covr55ms;   //!
   TBranch        *b_Muid_nmuonmu;   //!
   TBranch        *b_Muid_prelossmu;   //!
   TBranch        *b_Muid_erelossmu;   //!
   TBranch        *b_Muid_typeelossmu;   //!
   TBranch        *b_Muid_a0rmu;   //!
   TBranch        *b_Muid_z0rmu;   //!
   TBranch        *b_Muid_phirmu;   //!
   TBranch        *b_Muid_thetarmu;   //!
   TBranch        *b_Muid_etarmu;   //!
   TBranch        *b_Muid_pirmu;   //!
   TBranch        *b_Muid_chi2mu;   //!
   TBranch        *b_Muid_covr11mu;   //!
   TBranch        *b_Muid_covr12mu;   //!
   TBranch        *b_Muid_covr22mu;   //!
   TBranch        *b_Muid_covr13mu;   //!
   TBranch        *b_Muid_covr23mu;   //!
   TBranch        *b_Muid_covr33mu;   //!
   TBranch        *b_Muid_covr14mu;   //!
   TBranch        *b_Muid_covr24mu;   //!
   TBranch        *b_Muid_covr34mu;   //!
   TBranch        *b_Muid_covr44mu;   //!
   TBranch        *b_Muid_covr15mu;   //!
   TBranch        *b_Muid_covr25mu;   //!
   TBranch        *b_Muid_covr35mu;   //!
   TBranch        *b_Muid_covr45mu;   //!
   TBranch        *b_Muid_covr55mu;   //!
   TBranch        *b_Muid_mslinkmu;   //!
   TBranch        *b_Muid_nmuoncb;   //!
   TBranch        *b_Muid_chi2mcb;   //!
   TBranch        *b_Muid_loosecb;   //!
   TBranch        *b_Muid_a0rcb;   //!
   TBranch        *b_Muid_z0rcb;   //!
   TBranch        *b_Muid_phircb;   //!
   TBranch        *b_Muid_thetarcb;   //!
   TBranch        *b_Muid_etarcb;   //!
   TBranch        *b_Muid_pircb;   //!
   TBranch        *b_Muid_chi2cb;   //!
   TBranch        *b_Muid_chi2prcb;   //!
   TBranch        *b_Muid_covr11cb;   //!
   TBranch        *b_Muid_covr12cb;   //!
   TBranch        *b_Muid_covr22cb;   //!
   TBranch        *b_Muid_covr13cb;   //!
   TBranch        *b_Muid_covr23cb;   //!
   TBranch        *b_Muid_covr33cb;   //!
   TBranch        *b_Muid_covr14cb;   //!
   TBranch        *b_Muid_covr24cb;   //!
   TBranch        *b_Muid_covr34cb;   //!
   TBranch        *b_Muid_covr44cb;   //!
   TBranch        *b_Muid_covr15cb;   //!
   TBranch        *b_Muid_covr25cb;   //!
   TBranch        *b_Muid_covr35cb;   //!
   TBranch        *b_Muid_covr45cb;   //!
   TBranch        *b_Muid_covr55cb;   //!
   TBranch        *b_Muid_mslinkcb;   //!
   TBranch        *b_Muid_exlinkcb;   //!
   TBranch        *b_Muid_blayercb;   //!
   TBranch        *b_Muid_pixelcb;   //!
   TBranch        *b_Muid_sctcb;   //!
   TBranch        *b_Muid_trtcb;   //!
   TBranch        *b_Muid_trthighcb;   //!
   TBranch        *b_Muid_a0rid;   //!
   TBranch        *b_Muid_z0rid;   //!
   TBranch        *b_Muid_phirid;   //!
   TBranch        *b_Muid_thetarid;   //!
   TBranch        *b_Muid_etarid;   //!
   TBranch        *b_Muid_pirid;   //!
   TBranch        *b_Muid_chi2id;   //!
   TBranch        *b_Muid_pulla0cb;   //!
   TBranch        *b_Muid_pullz0cb;   //!
   TBranch        *b_Muid_pullphicb;   //!
   TBranch        *b_Muid_pullthecb;   //!
   TBranch        *b_Muid_pullmomcb;   //!
   TBranch        *b_MemRSS;   //!
   TBranch        *b_MemVSize;   //!
   TBranch        *b_TimeTotal;   //!
   TBranch        *b_TimeSum;   //!
   TBranch        *b_TimeEventCounter;   //!
   TBranch        *b_TimeDetStatusAlg;   //!
   TBranch        *b_TimeInDetPixelClusterization;   //!
   TBranch        *b_TimeInDetSCT_Clusterization;   //!
   TBranch        *b_TimeInDetTRT_RIO_Maker;   //!
   TBranch        *b_TimeInDetPRD_MultiTruthMaker;   //!
   TBranch        *b_TimeInDetSiTrackerSpacePointFinder;   //!
   TBranch        *b_TimeiPatRec;   //!
   TBranch        *b_TimeiPatTrackTruthAssociator;   //!
   TBranch        *b_TimeiPatStatistics;   //!
   TBranch        *b_TimeInDetLegacyCnvAlg;   //!
   TBranch        *b_TimeConvertedIPatTracksDetailedTruthMaker;   //!
   TBranch        *b_TimeConvertedIPatTracksTruthSelector;   //!
   TBranch        *b_TimeInDetCopyAlg;   //!
   TBranch        *b_TimeDetailedTrackTruthMaker;   //!
   TBranch        *b_TimeTrackTruthCollectionSelector;   //!
   TBranch        *b_TimeInDetPriVxFinder;   //!
   TBranch        *b_TimeInDetParticleCreation;   //!
   TBranch        *b_TimeInDetTrackParticleTruthMaker;   //!
   TBranch        *b_TimeInDetRecStatistics;   //!
   TBranch        *b_TimeInDetTrackClusterAssValidation;   //!
   TBranch        *b_TimeCaloCellMaker;   //!
   TBranch        *b_TimeMuonRdoToMuonDigit;   //!
   TBranch        *b_TimeCscRdoToCscPrepData;   //!
   TBranch        *b_TimeMdtRdoToMdtPrepData;   //!
   TBranch        *b_TimeRpcRdoToRpcPrepData;   //!
   TBranch        *b_TimeTgcRdoToTgcPrepData;   //!
   TBranch        *b_TimeCscThresholdClusterBuilder;   //!
   TBranch        *b_TimeTrackRecordFilter;   //!
   TBranch        *b_TimeMooMakePhiPatterns;   //!
   TBranch        *b_TimeMooMakeRzPatterns;   //!
   TBranch        *b_TimeMooCombinePatterns;   //!
   TBranch        *b_TimeMooCalibratedSegmentMaker;   //!
   TBranch        *b_TimeMooRzSegmentCombinationMaker;   //!
   TBranch        *b_TimePhiPatternToPhiSeg;   //!
   TBranch        *b_TimeCscPatternToCrudeSeg;   //!
   TBranch        *b_TimeMdtPatternToCrudeSeg;   //!
   TBranch        *b_TimeMuonSegmentComboToMooRzSegmentCombo;   //!
   TBranch        *b_TimeMuonSegmentToMooRzSegment;   //!
   TBranch        *b_TimeMooRoadMaker;   //!
   TBranch        *b_TimeMooMakeTracks;   //!
   TBranch        *b_TimeMooCnvAlg;   //!
   TBranch        *b_TimeMuidMooreTPCreator;   //!
   TBranch        *b_TimeMuidStandAlone;   //!
   TBranch        *b_TimeMuidExtrCnvAlg;   //!
   TBranch        *b_TimeMuidCombined;   //!
   TBranch        *b_TimeMuidCnvAlg;   //!
   TBranch        *b_TimeMuidStatistics;   //!
   TBranch        *b_TimeCBNT_AthenaAware;   //!
   TBranch        *b_TimeAANTupleStream;   //!


   MooHists(TTree * /*tree*/ =0) { }
   virtual ~MooHists() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   void    gausFit(TH1F*);
   void    gaus2Fit(TH1F*, Int_t);

   // following methods are by VJ. The code is found in separate .C files
   // User methods to match reconstructed to generated track
   //
   Int_t   MooToKin_rassoc(Int_t);
   Int_t   MooToKin_rassoc_dist(Int_t);
   Int_t   MooToKin_etaPhi(Int_t);
   Int_t   MooToKin_5var(Int_t, Float_t, Float_t*);
   Int_t   MooToKin_pTvar(Int_t, Float_t, Float_t*);

   // Check number of Mdt/Phi hits to suppress fakes
   Int_t   MooCheck_MdtPhiHits(Int_t);

   // following by Victoria Rojo for looking at eta hits and making
   // hit plots
   Int_t   MooCheck_etahmo(Int_t); //
   void    MooMakeHitPlots(Int_t, Int_t, Int_t, Double_t, Double_t);

   ClassDef(MooHists,0);
};

#endif

#ifdef MooHists_cxx
void MooHists::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
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
   Spcl_ID = 0;
   Spcl_Pt = 0;
   Spcl_Eta = 0;
   Spcl_Phi = 0;
   Spcl_M = 0;
   Spcl_Stat = 0;
   Spcl_Isol = 0;
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
   Trk_numberOfTrackSummaryTypes = 0;
   Trk_truthBarcode = 0;
   Trk_truthNt = 0;
   Trk_ParticlePdg = 0;
   Trk_ParentPdg = 0;
   Eh_PresB = 0;
   Eh_EMB = 0;
   Eh_EMEC = 0;
   Eh_Tile = 0;
   Eh_TileGap = 0;
   Eh_HEC = 0;
   Eh_FCal = 0;
   Eh_PresE = 0;
   Eh_Scint = 0;
   Moore_xvtxg = 0;
   Moore_yvtxg = 0;
   Moore_zvtxg = 0;
   Moore_a0g = 0;
   Moore_z0g = 0;
   Moore_phig = 0;
   Moore_thetag = 0;
   Moore_qpig = 0;
   Moore_etag = 0;
   Moore_rassociated = 0;
   Moore_matchdg = 0;
   Moore_xextrg = 0;
   Moore_yextrg = 0;
   Moore_zextrg = 0;
   Moore_a0extrg = 0;
   Moore_z0extrg = 0;
   Moore_phiextrg = 0;
   Moore_thetaextrg = 0;
   Moore_qpiextrg = 0;
   Moore_etaextrg = 0;
   Moore_barcodeg = 0;
   Moore_statusg = 0;
   Moore_elossg = 0;
   Moore_drphig = 0;
   Moore_drzg = 0;
   Moore_dphig = 0;
   Moore_dthetag = 0;
   Moore_a0pullg = 0;
   Moore_z0pullg = 0;
   Moore_phipullg = 0;
   Moore_thetapullg = 0;
   Moore_qpipullg = 0;
   Moore_phih = 0;
   Moore_etah = 0;
   Moore_mdth = 0;
   Moore_cscetah = 0;
   Moore_cscphih = 0;
   Moore_rpcetah = 0;
   Moore_rpcphih = 0;
   Moore_tgcetah = 0;
   Moore_tgcphih = 0;
   Moore_stati = 0;
   Moore_statr = 0;
   Moore_xvtxr = 0;
   Moore_yvtxr = 0;
   Moore_zvtxr = 0;
   Moore_a0r = 0;
   Moore_z0r = 0;
   Moore_phir = 0;
   Moore_thetar = 0;
   Moore_qpir = 0;
   Moore_etar = 0;
   Moore_chi2 = 0;
   Moore_chi2pr = 0;
   Moore_origx = 0;
   Moore_origy = 0;
   Moore_origz = 0;
   Moore_covr11 = 0;
   Moore_covr12 = 0;
   Moore_covr22 = 0;
   Moore_covr13 = 0;
   Moore_covr23 = 0;
   Moore_covr33 = 0;
   Moore_covr14 = 0;
   Moore_covr24 = 0;
   Moore_covr34 = 0;
   Moore_covr44 = 0;
   Moore_covr15 = 0;
   Moore_covr25 = 0;
   Moore_covr35 = 0;
   Moore_covr45 = 0;
   Moore_covr55 = 0;
   Moore_nhits = 0;
   Moore_hit_statname = 0;
   Moore_hit_stateta = 0;
   Moore_hit_statphi = 0;
   Moore_hit_statreg = 0;
   Moore_hit_tech = 0;
   Moore_hit_dtyp = 0;
   Moore_hit_dblr = 0;
   Moore_hit_dblz = 0;
   Moore_hit_dblp = 0;
   Moore_hit_ggap = 0;
   Moore_hit_strp = 0;
   Moore_hit_chnl = 0;
   Moore_hit_clyr = 0;
   Moore_hit_wlyr = 0;
   Moore_hit_mlyr = 0;
   Moore_hit_tlyr = 0;
   Moore_hit_tube = 0;
   Moore_hit_rho = 0;
   Moore_hit_phi = 0;
   Moore_hit_z = 0;
   Moore_hit_theta = 0;
   Moore_hit_eta = 0;
   Moore_hit_xloc = 0;
   Moore_hit_yloc = 0;
   Moore_hit_zloc = 0;
   Moore_hit_drd = 0;
   Moore_hit_sig = 0;
   Moore_hit_res = 0;
   Moore_hit_x0 = 0;
   Muid_a0rms = 0;
   Muid_z0rms = 0;
   Muid_phirms = 0;
   Muid_thetarms = 0;
   Muid_etarms = 0;
   Muid_pirms = 0;
   Muid_chi2ms = 0;
   Muid_mdtms = 0;
   Muid_cscetams = 0;
   Muid_cscphims = 0;
   Muid_rpcetams = 0;
   Muid_rpcphims = 0;
   Muid_tgcetams = 0;
   Muid_tgcphims = 0;
   Muid_covr11ms = 0;
   Muid_covr12ms = 0;
   Muid_covr22ms = 0;
   Muid_covr13ms = 0;
   Muid_covr23ms = 0;
   Muid_covr33ms = 0;
   Muid_covr14ms = 0;
   Muid_covr24ms = 0;
   Muid_covr34ms = 0;
   Muid_covr44ms = 0;
   Muid_covr15ms = 0;
   Muid_covr25ms = 0;
   Muid_covr35ms = 0;
   Muid_covr45ms = 0;
   Muid_covr55ms = 0;
   Muid_prelossmu = 0;
   Muid_erelossmu = 0;
   Muid_typeelossmu = 0;
   Muid_a0rmu = 0;
   Muid_z0rmu = 0;
   Muid_phirmu = 0;
   Muid_thetarmu = 0;
   Muid_etarmu = 0;
   Muid_pirmu = 0;
   Muid_chi2mu = 0;
   Muid_covr11mu = 0;
   Muid_covr12mu = 0;
   Muid_covr22mu = 0;
   Muid_covr13mu = 0;
   Muid_covr23mu = 0;
   Muid_covr33mu = 0;
   Muid_covr14mu = 0;
   Muid_covr24mu = 0;
   Muid_covr34mu = 0;
   Muid_covr44mu = 0;
   Muid_covr15mu = 0;
   Muid_covr25mu = 0;
   Muid_covr35mu = 0;
   Muid_covr45mu = 0;
   Muid_covr55mu = 0;
   Muid_mslinkmu = 0;
   Muid_chi2mcb = 0;
   Muid_loosecb = 0;
   Muid_a0rcb = 0;
   Muid_z0rcb = 0;
   Muid_phircb = 0;
   Muid_thetarcb = 0;
   Muid_etarcb = 0;
   Muid_pircb = 0;
   Muid_chi2cb = 0;
   Muid_chi2prcb = 0;
   Muid_covr11cb = 0;
   Muid_covr12cb = 0;
   Muid_covr22cb = 0;
   Muid_covr13cb = 0;
   Muid_covr23cb = 0;
   Muid_covr33cb = 0;
   Muid_covr14cb = 0;
   Muid_covr24cb = 0;
   Muid_covr34cb = 0;
   Muid_covr44cb = 0;
   Muid_covr15cb = 0;
   Muid_covr25cb = 0;
   Muid_covr35cb = 0;
   Muid_covr45cb = 0;
   Muid_covr55cb = 0;
   Muid_mslinkcb = 0;
   Muid_exlinkcb = 0;
   Muid_blayercb = 0;
   Muid_pixelcb = 0;
   Muid_sctcb = 0;
   Muid_trtcb = 0;
   Muid_trthighcb = 0;
   Muid_a0rid = 0;
   Muid_z0rid = 0;
   Muid_phirid = 0;
   Muid_thetarid = 0;
   Muid_etarid = 0;
   Muid_pirid = 0;
   Muid_chi2id = 0;
   Muid_pulla0cb = 0;
   Muid_pullz0cb = 0;
   Muid_pullphicb = 0;
   Muid_pullthecb = 0;
   Muid_pullmomcb = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("StreamESD_ref", &StreamESD_ref, &b_StreamESD_ref);
   fChain->SetBranchAddress("StreamRDO_ref", &StreamRDO_ref, &b_StreamRDO_ref);
   fChain->SetBranchAddress("Token", Token, &b_Token);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("Weight", &Weight, &b_Weight);
   fChain->SetBranchAddress("IEvent", &IEvent, &b_IEvent);
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
   fChain->SetBranchAddress("Spcl_Num", &Spcl_Num, &b_Spcl_Num);
   fChain->SetBranchAddress("Spcl_ID", &Spcl_ID, &b_Spcl_ID);
   fChain->SetBranchAddress("Spcl_Pt", &Spcl_Pt, &b_Spcl_Pt);
   fChain->SetBranchAddress("Spcl_Eta", &Spcl_Eta, &b_Spcl_Eta);
   fChain->SetBranchAddress("Spcl_Phi", &Spcl_Phi, &b_Spcl_Phi);
   fChain->SetBranchAddress("Spcl_M", &Spcl_M, &b_Spcl_M);
   fChain->SetBranchAddress("Spcl_Stat", &Spcl_Stat, &b_Spcl_Stat);
   fChain->SetBranchAddress("Spcl_Isol", &Spcl_Isol, &b_Spcl_Isol);
   fChain->SetBranchAddress("Spcl_sumx", &Spcl_sumx, &b_Spcl_sumx);
   fChain->SetBranchAddress("Spcl_sumy", &Spcl_sumy, &b_Spcl_sumy);
   fChain->SetBranchAddress("Spcl_sumz", &Spcl_sumz, &b_Spcl_sumz);
   fChain->SetBranchAddress("Spcl_sume", &Spcl_sume, &b_Spcl_sume);
   fChain->SetBranchAddress("Spcl_x1", &Spcl_x1, &b_Spcl_x1);
   fChain->SetBranchAddress("Spcl_x2", &Spcl_x2, &b_Spcl_x2);
   fChain->SetBranchAddress("Spcl_id1", &Spcl_id1, &b_Spcl_id1);
   fChain->SetBranchAddress("Spcl_id2", &Spcl_id2, &b_Spcl_id2);
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
   fChain->SetBranchAddress("Trk_numberOfTrackSummaryTypes", &Trk_numberOfTrackSummaryTypes, &b_Trk_numberOfTrackSummaryTypes);
   fChain->SetBranchAddress("Trk_truthBarcode", &Trk_truthBarcode, &b_Trk_truthBarcode);
   fChain->SetBranchAddress("Trk_truthNt", &Trk_truthNt, &b_Trk_truthNt);
   fChain->SetBranchAddress("Trk_ParticlePdg", &Trk_ParticlePdg, &b_Trk_ParticlePdg);
   fChain->SetBranchAddress("Trk_ParentPdg", &Trk_ParentPdg, &b_Trk_ParentPdg);
   fChain->SetBranchAddress("Nh_Calo", &Nh_Calo, &b_Nh_Calo);
   fChain->SetBranchAddress("Eh_Calo", &Eh_Calo, &b_Eh_Calo);
   fChain->SetBranchAddress("Nh_EM", &Nh_EM, &b_Nh_EM);
   fChain->SetBranchAddress("Eh_EM", &Eh_EM, &b_Eh_EM);
   fChain->SetBranchAddress("Nh_HAD", &Nh_HAD, &b_Nh_HAD);
   fChain->SetBranchAddress("Eh_HAD", &Eh_HAD, &b_Eh_HAD);
   fChain->SetBranchAddress("Nh_PresB", &Nh_PresB, &b_Nh_PresB);
   fChain->SetBranchAddress("Eh_PresB", &Eh_PresB, &b_Eh_PresB);
   fChain->SetBranchAddress("Nh_EMB", &Nh_EMB, &b_Nh_EMB);
   fChain->SetBranchAddress("Eh_EMB", &Eh_EMB, &b_Eh_EMB);
   fChain->SetBranchAddress("Nh_EMEC", &Nh_EMEC, &b_Nh_EMEC);
   fChain->SetBranchAddress("Eh_EMEC", &Eh_EMEC, &b_Eh_EMEC);
   fChain->SetBranchAddress("Nh_Tile", &Nh_Tile, &b_Nh_Tile);
   fChain->SetBranchAddress("Eh_Tile", &Eh_Tile, &b_Eh_Tile);
   fChain->SetBranchAddress("Nh_TileGap", &Nh_TileGap, &b_Nh_TileGap);
   fChain->SetBranchAddress("Eh_TileGap", &Eh_TileGap, &b_Eh_TileGap);
   fChain->SetBranchAddress("Nh_HEC", &Nh_HEC, &b_Nh_HEC);
   fChain->SetBranchAddress("Eh_HEC", &Eh_HEC, &b_Eh_HEC);
   fChain->SetBranchAddress("Nh_FCal", &Nh_FCal, &b_Nh_FCal);
   fChain->SetBranchAddress("Eh_FCal", &Eh_FCal, &b_Eh_FCal);
   fChain->SetBranchAddress("Nh_PresE", &Nh_PresE, &b_Nh_PresE);
   fChain->SetBranchAddress("Eh_PresE", &Eh_PresE, &b_Eh_PresE);
   fChain->SetBranchAddress("Nh_Scint", &Nh_Scint, &b_Nh_Scint);
   fChain->SetBranchAddress("Eh_Scint", &Eh_Scint, &b_Eh_Scint);
   fChain->SetBranchAddress("nhit_em", &nhit_em, &b_nhit_em);
   fChain->SetBranchAddress("ecell_em", &ecell_em, &b_ecell_em);
   fChain->SetBranchAddress("nhit_hec", &nhit_hec, &b_nhit_hec);
   fChain->SetBranchAddress("ecell_hec", &ecell_hec, &b_ecell_hec);
   fChain->SetBranchAddress("nhit_fcal", &nhit_fcal, &b_nhit_fcal);
   fChain->SetBranchAddress("ecell_fcal", &ecell_fcal, &b_ecell_fcal);
   fChain->SetBranchAddress("nhit_tile", &nhit_tile, &b_nhit_tile);
   fChain->SetBranchAddress("ecell_tile", &ecell_tile, &b_ecell_tile);
   fChain->SetBranchAddress("Moore_nkine", &Moore_nkine, &b_Moore_nkine);
   fChain->SetBranchAddress("Moore_xvtxg", &Moore_xvtxg, &b_Moore_xvtxg);
   fChain->SetBranchAddress("Moore_yvtxg", &Moore_yvtxg, &b_Moore_yvtxg);
   fChain->SetBranchAddress("Moore_zvtxg", &Moore_zvtxg, &b_Moore_zvtxg);
   fChain->SetBranchAddress("Moore_a0g", &Moore_a0g, &b_Moore_a0g);
   fChain->SetBranchAddress("Moore_z0g", &Moore_z0g, &b_Moore_z0g);
   fChain->SetBranchAddress("Moore_phig", &Moore_phig, &b_Moore_phig);
   fChain->SetBranchAddress("Moore_thetag", &Moore_thetag, &b_Moore_thetag);
   fChain->SetBranchAddress("Moore_qpig", &Moore_qpig, &b_Moore_qpig);
   fChain->SetBranchAddress("Moore_etag", &Moore_etag, &b_Moore_etag);
   fChain->SetBranchAddress("Moore_rassociated", &Moore_rassociated, &b_Moore_rassociated);
   fChain->SetBranchAddress("Moore_matchdg", &Moore_matchdg, &b_Moore_matchdg);
   fChain->SetBranchAddress("Moore_xextrg", &Moore_xextrg, &b_Moore_xextrg);
   fChain->SetBranchAddress("Moore_yextrg", &Moore_yextrg, &b_Moore_yextrg);
   fChain->SetBranchAddress("Moore_zextrg", &Moore_zextrg, &b_Moore_zextrg);
   fChain->SetBranchAddress("Moore_a0extrg", &Moore_a0extrg, &b_Moore_a0extrg);
   fChain->SetBranchAddress("Moore_z0extrg", &Moore_z0extrg, &b_Moore_z0extrg);
   fChain->SetBranchAddress("Moore_phiextrg", &Moore_phiextrg, &b_Moore_phiextrg);
   fChain->SetBranchAddress("Moore_thetaextrg", &Moore_thetaextrg, &b_Moore_thetaextrg);
   fChain->SetBranchAddress("Moore_qpiextrg", &Moore_qpiextrg, &b_Moore_qpiextrg);
   fChain->SetBranchAddress("Moore_etaextrg", &Moore_etaextrg, &b_Moore_etaextrg);
   fChain->SetBranchAddress("Moore_barcodeg", &Moore_barcodeg, &b_Moore_barcodeg);
   fChain->SetBranchAddress("Moore_statusg", &Moore_statusg, &b_Moore_statusg);
   fChain->SetBranchAddress("Moore_elossg", &Moore_elossg, &b_Moore_elossg);
   fChain->SetBranchAddress("Moore_drphig", &Moore_drphig, &b_Moore_drphig);
   fChain->SetBranchAddress("Moore_drzg", &Moore_drzg, &b_Moore_drzg);
   fChain->SetBranchAddress("Moore_dphig", &Moore_dphig, &b_Moore_dphig);
   fChain->SetBranchAddress("Moore_dthetag", &Moore_dthetag, &b_Moore_dthetag);
   fChain->SetBranchAddress("Moore_a0pullg", &Moore_a0pullg, &b_Moore_a0pullg);
   fChain->SetBranchAddress("Moore_z0pullg", &Moore_z0pullg, &b_Moore_z0pullg);
   fChain->SetBranchAddress("Moore_phipullg", &Moore_phipullg, &b_Moore_phipullg);
   fChain->SetBranchAddress("Moore_thetapullg", &Moore_thetapullg, &b_Moore_thetapullg);
   fChain->SetBranchAddress("Moore_qpipullg", &Moore_qpipullg, &b_Moore_qpipullg);
   fChain->SetBranchAddress("Moore_nmuon", &Moore_nmuon, &b_Moore_nmuon);
   fChain->SetBranchAddress("Moore_phih", &Moore_phih, &b_Moore_phih);
   fChain->SetBranchAddress("Moore_etah", &Moore_etah, &b_Moore_etah);
   fChain->SetBranchAddress("Moore_mdth", &Moore_mdth, &b_Moore_mdth);
   fChain->SetBranchAddress("Moore_cscetah", &Moore_cscetah, &b_Moore_cscetah);
   fChain->SetBranchAddress("Moore_cscphih", &Moore_cscphih, &b_Moore_cscphih);
   fChain->SetBranchAddress("Moore_rpcetah", &Moore_rpcetah, &b_Moore_rpcetah);
   fChain->SetBranchAddress("Moore_rpcphih", &Moore_rpcphih, &b_Moore_rpcphih);
   fChain->SetBranchAddress("Moore_tgcetah", &Moore_tgcetah, &b_Moore_tgcetah);
   fChain->SetBranchAddress("Moore_tgcphih", &Moore_tgcphih, &b_Moore_tgcphih);
   fChain->SetBranchAddress("Moore_stati", &Moore_stati, &b_Moore_stati);
   fChain->SetBranchAddress("Moore_statr", &Moore_statr, &b_Moore_statr);
   fChain->SetBranchAddress("Moore_xvtxr", &Moore_xvtxr, &b_Moore_xvtxr);
   fChain->SetBranchAddress("Moore_yvtxr", &Moore_yvtxr, &b_Moore_yvtxr);
   fChain->SetBranchAddress("Moore_zvtxr", &Moore_zvtxr, &b_Moore_zvtxr);
   fChain->SetBranchAddress("Moore_a0r", &Moore_a0r, &b_Moore_a0r);
   fChain->SetBranchAddress("Moore_z0r", &Moore_z0r, &b_Moore_z0r);
   fChain->SetBranchAddress("Moore_phir", &Moore_phir, &b_Moore_phir);
   fChain->SetBranchAddress("Moore_thetar", &Moore_thetar, &b_Moore_thetar);
   fChain->SetBranchAddress("Moore_qpir", &Moore_qpir, &b_Moore_qpir);
   fChain->SetBranchAddress("Moore_etar", &Moore_etar, &b_Moore_etar);
   fChain->SetBranchAddress("Moore_chi2", &Moore_chi2, &b_Moore_chi2);
   fChain->SetBranchAddress("Moore_chi2pr", &Moore_chi2pr, &b_Moore_chi2pr);
   fChain->SetBranchAddress("Moore_origx", &Moore_origx, &b_Moore_origx);
   fChain->SetBranchAddress("Moore_origy", &Moore_origy, &b_Moore_origy);
   fChain->SetBranchAddress("Moore_origz", &Moore_origz, &b_Moore_origz);
   fChain->SetBranchAddress("Moore_covr11", &Moore_covr11, &b_Moore_covr11);
   fChain->SetBranchAddress("Moore_covr12", &Moore_covr12, &b_Moore_covr12);
   fChain->SetBranchAddress("Moore_covr22", &Moore_covr22, &b_Moore_covr22);
   fChain->SetBranchAddress("Moore_covr13", &Moore_covr13, &b_Moore_covr13);
   fChain->SetBranchAddress("Moore_covr23", &Moore_covr23, &b_Moore_covr23);
   fChain->SetBranchAddress("Moore_covr33", &Moore_covr33, &b_Moore_covr33);
   fChain->SetBranchAddress("Moore_covr14", &Moore_covr14, &b_Moore_covr14);
   fChain->SetBranchAddress("Moore_covr24", &Moore_covr24, &b_Moore_covr24);
   fChain->SetBranchAddress("Moore_covr34", &Moore_covr34, &b_Moore_covr34);
   fChain->SetBranchAddress("Moore_covr44", &Moore_covr44, &b_Moore_covr44);
   fChain->SetBranchAddress("Moore_covr15", &Moore_covr15, &b_Moore_covr15);
   fChain->SetBranchAddress("Moore_covr25", &Moore_covr25, &b_Moore_covr25);
   fChain->SetBranchAddress("Moore_covr35", &Moore_covr35, &b_Moore_covr35);
   fChain->SetBranchAddress("Moore_covr45", &Moore_covr45, &b_Moore_covr45);
   fChain->SetBranchAddress("Moore_covr55", &Moore_covr55, &b_Moore_covr55);
   fChain->SetBranchAddress("Moore_nhits", &Moore_nhits, &b_Moore_nhits);
   fChain->SetBranchAddress("Moore_hit_statname", &Moore_hit_statname, &b_Moore_hit_statname);
   fChain->SetBranchAddress("Moore_hit_stateta", &Moore_hit_stateta, &b_Moore_hit_stateta);
   fChain->SetBranchAddress("Moore_hit_statphi", &Moore_hit_statphi, &b_Moore_hit_statphi);
   fChain->SetBranchAddress("Moore_hit_statreg", &Moore_hit_statreg, &b_Moore_hit_statreg);
   fChain->SetBranchAddress("Moore_hit_tech", &Moore_hit_tech, &b_Moore_hit_tech);
   fChain->SetBranchAddress("Moore_hit_dtyp", &Moore_hit_dtyp, &b_Moore_hit_dtyp);
   fChain->SetBranchAddress("Moore_hit_dblr", &Moore_hit_dblr, &b_Moore_hit_dblr);
   fChain->SetBranchAddress("Moore_hit_dblz", &Moore_hit_dblz, &b_Moore_hit_dblz);
   fChain->SetBranchAddress("Moore_hit_dblp", &Moore_hit_dblp, &b_Moore_hit_dblp);
   fChain->SetBranchAddress("Moore_hit_ggap", &Moore_hit_ggap, &b_Moore_hit_ggap);
   fChain->SetBranchAddress("Moore_hit_strp", &Moore_hit_strp, &b_Moore_hit_strp);
   fChain->SetBranchAddress("Moore_hit_chnl", &Moore_hit_chnl, &b_Moore_hit_chnl);
   fChain->SetBranchAddress("Moore_hit_clyr", &Moore_hit_clyr, &b_Moore_hit_clyr);
   fChain->SetBranchAddress("Moore_hit_wlyr", &Moore_hit_wlyr, &b_Moore_hit_wlyr);
   fChain->SetBranchAddress("Moore_hit_mlyr", &Moore_hit_mlyr, &b_Moore_hit_mlyr);
   fChain->SetBranchAddress("Moore_hit_tlyr", &Moore_hit_tlyr, &b_Moore_hit_tlyr);
   fChain->SetBranchAddress("Moore_hit_tube", &Moore_hit_tube, &b_Moore_hit_tube);
   fChain->SetBranchAddress("Moore_hit_rho", &Moore_hit_rho, &b_Moore_hit_rho);
   fChain->SetBranchAddress("Moore_hit_phi", &Moore_hit_phi, &b_Moore_hit_phi);
   fChain->SetBranchAddress("Moore_hit_z", &Moore_hit_z, &b_Moore_hit_z);
   fChain->SetBranchAddress("Moore_hit_theta", &Moore_hit_theta, &b_Moore_hit_theta);
   fChain->SetBranchAddress("Moore_hit_eta", &Moore_hit_eta, &b_Moore_hit_eta);
   fChain->SetBranchAddress("Moore_hit_xloc", &Moore_hit_xloc, &b_Moore_hit_xloc);
   fChain->SetBranchAddress("Moore_hit_yloc", &Moore_hit_yloc, &b_Moore_hit_yloc);
   fChain->SetBranchAddress("Moore_hit_zloc", &Moore_hit_zloc, &b_Moore_hit_zloc);
   fChain->SetBranchAddress("Moore_hit_drd", &Moore_hit_drd, &b_Moore_hit_drd);
   fChain->SetBranchAddress("Moore_hit_sig", &Moore_hit_sig, &b_Moore_hit_sig);
   fChain->SetBranchAddress("Moore_hit_res", &Moore_hit_res, &b_Moore_hit_res);
   fChain->SetBranchAddress("Moore_hit_x0", &Moore_hit_x0, &b_Moore_hit_x0);
   fChain->SetBranchAddress("Muid_nmuonms", &Muid_nmuonms, &b_Muid_nmuonms);
   fChain->SetBranchAddress("Muid_a0rms", &Muid_a0rms, &b_Muid_a0rms);
   fChain->SetBranchAddress("Muid_z0rms", &Muid_z0rms, &b_Muid_z0rms);
   fChain->SetBranchAddress("Muid_phirms", &Muid_phirms, &b_Muid_phirms);
   fChain->SetBranchAddress("Muid_thetarms", &Muid_thetarms, &b_Muid_thetarms);
   fChain->SetBranchAddress("Muid_etarms", &Muid_etarms, &b_Muid_etarms);
   fChain->SetBranchAddress("Muid_pirms", &Muid_pirms, &b_Muid_pirms);
   fChain->SetBranchAddress("Muid_chi2ms", &Muid_chi2ms, &b_Muid_chi2ms);
   fChain->SetBranchAddress("Muid_mdtms", &Muid_mdtms, &b_Muid_mdtms);
   fChain->SetBranchAddress("Muid_cscetams", &Muid_cscetams, &b_Muid_cscetams);
   fChain->SetBranchAddress("Muid_cscphims", &Muid_cscphims, &b_Muid_cscphims);
   fChain->SetBranchAddress("Muid_rpcetams", &Muid_rpcetams, &b_Muid_rpcetams);
   fChain->SetBranchAddress("Muid_rpcphims", &Muid_rpcphims, &b_Muid_rpcphims);
   fChain->SetBranchAddress("Muid_tgcetams", &Muid_tgcetams, &b_Muid_tgcetams);
   fChain->SetBranchAddress("Muid_tgcphims", &Muid_tgcphims, &b_Muid_tgcphims);
   fChain->SetBranchAddress("Muid_covr11ms", &Muid_covr11ms, &b_Muid_covr11ms);
   fChain->SetBranchAddress("Muid_covr12ms", &Muid_covr12ms, &b_Muid_covr12ms);
   fChain->SetBranchAddress("Muid_covr22ms", &Muid_covr22ms, &b_Muid_covr22ms);
   fChain->SetBranchAddress("Muid_covr13ms", &Muid_covr13ms, &b_Muid_covr13ms);
   fChain->SetBranchAddress("Muid_covr23ms", &Muid_covr23ms, &b_Muid_covr23ms);
   fChain->SetBranchAddress("Muid_covr33ms", &Muid_covr33ms, &b_Muid_covr33ms);
   fChain->SetBranchAddress("Muid_covr14ms", &Muid_covr14ms, &b_Muid_covr14ms);
   fChain->SetBranchAddress("Muid_covr24ms", &Muid_covr24ms, &b_Muid_covr24ms);
   fChain->SetBranchAddress("Muid_covr34ms", &Muid_covr34ms, &b_Muid_covr34ms);
   fChain->SetBranchAddress("Muid_covr44ms", &Muid_covr44ms, &b_Muid_covr44ms);
   fChain->SetBranchAddress("Muid_covr15ms", &Muid_covr15ms, &b_Muid_covr15ms);
   fChain->SetBranchAddress("Muid_covr25ms", &Muid_covr25ms, &b_Muid_covr25ms);
   fChain->SetBranchAddress("Muid_covr35ms", &Muid_covr35ms, &b_Muid_covr35ms);
   fChain->SetBranchAddress("Muid_covr45ms", &Muid_covr45ms, &b_Muid_covr45ms);
   fChain->SetBranchAddress("Muid_covr55ms", &Muid_covr55ms, &b_Muid_covr55ms);
   fChain->SetBranchAddress("Muid_nmuonmu", &Muid_nmuonmu, &b_Muid_nmuonmu);
   fChain->SetBranchAddress("Muid_prelossmu", &Muid_prelossmu, &b_Muid_prelossmu);
   fChain->SetBranchAddress("Muid_erelossmu", &Muid_erelossmu, &b_Muid_erelossmu);
   fChain->SetBranchAddress("Muid_typeelossmu", &Muid_typeelossmu, &b_Muid_typeelossmu);
   fChain->SetBranchAddress("Muid_a0rmu", &Muid_a0rmu, &b_Muid_a0rmu);
   fChain->SetBranchAddress("Muid_z0rmu", &Muid_z0rmu, &b_Muid_z0rmu);
   fChain->SetBranchAddress("Muid_phirmu", &Muid_phirmu, &b_Muid_phirmu);
   fChain->SetBranchAddress("Muid_thetarmu", &Muid_thetarmu, &b_Muid_thetarmu);
   fChain->SetBranchAddress("Muid_etarmu", &Muid_etarmu, &b_Muid_etarmu);
   fChain->SetBranchAddress("Muid_pirmu", &Muid_pirmu, &b_Muid_pirmu);
   fChain->SetBranchAddress("Muid_chi2mu", &Muid_chi2mu, &b_Muid_chi2mu);
   fChain->SetBranchAddress("Muid_covr11mu", &Muid_covr11mu, &b_Muid_covr11mu);
   fChain->SetBranchAddress("Muid_covr12mu", &Muid_covr12mu, &b_Muid_covr12mu);
   fChain->SetBranchAddress("Muid_covr22mu", &Muid_covr22mu, &b_Muid_covr22mu);
   fChain->SetBranchAddress("Muid_covr13mu", &Muid_covr13mu, &b_Muid_covr13mu);
   fChain->SetBranchAddress("Muid_covr23mu", &Muid_covr23mu, &b_Muid_covr23mu);
   fChain->SetBranchAddress("Muid_covr33mu", &Muid_covr33mu, &b_Muid_covr33mu);
   fChain->SetBranchAddress("Muid_covr14mu", &Muid_covr14mu, &b_Muid_covr14mu);
   fChain->SetBranchAddress("Muid_covr24mu", &Muid_covr24mu, &b_Muid_covr24mu);
   fChain->SetBranchAddress("Muid_covr34mu", &Muid_covr34mu, &b_Muid_covr34mu);
   fChain->SetBranchAddress("Muid_covr44mu", &Muid_covr44mu, &b_Muid_covr44mu);
   fChain->SetBranchAddress("Muid_covr15mu", &Muid_covr15mu, &b_Muid_covr15mu);
   fChain->SetBranchAddress("Muid_covr25mu", &Muid_covr25mu, &b_Muid_covr25mu);
   fChain->SetBranchAddress("Muid_covr35mu", &Muid_covr35mu, &b_Muid_covr35mu);
   fChain->SetBranchAddress("Muid_covr45mu", &Muid_covr45mu, &b_Muid_covr45mu);
   fChain->SetBranchAddress("Muid_covr55mu", &Muid_covr55mu, &b_Muid_covr55mu);
   fChain->SetBranchAddress("Muid_mslinkmu", &Muid_mslinkmu, &b_Muid_mslinkmu);
   fChain->SetBranchAddress("Muid_nmuoncb", &Muid_nmuoncb, &b_Muid_nmuoncb);
   fChain->SetBranchAddress("Muid_chi2mcb", &Muid_chi2mcb, &b_Muid_chi2mcb);
   fChain->SetBranchAddress("Muid_loosecb", &Muid_loosecb, &b_Muid_loosecb);
   fChain->SetBranchAddress("Muid_a0rcb", &Muid_a0rcb, &b_Muid_a0rcb);
   fChain->SetBranchAddress("Muid_z0rcb", &Muid_z0rcb, &b_Muid_z0rcb);
   fChain->SetBranchAddress("Muid_phircb", &Muid_phircb, &b_Muid_phircb);
   fChain->SetBranchAddress("Muid_thetarcb", &Muid_thetarcb, &b_Muid_thetarcb);
   fChain->SetBranchAddress("Muid_etarcb", &Muid_etarcb, &b_Muid_etarcb);
   fChain->SetBranchAddress("Muid_pircb", &Muid_pircb, &b_Muid_pircb);
   fChain->SetBranchAddress("Muid_chi2cb", &Muid_chi2cb, &b_Muid_chi2cb);
   fChain->SetBranchAddress("Muid_chi2prcb", &Muid_chi2prcb, &b_Muid_chi2prcb);
   fChain->SetBranchAddress("Muid_covr11cb", &Muid_covr11cb, &b_Muid_covr11cb);
   fChain->SetBranchAddress("Muid_covr12cb", &Muid_covr12cb, &b_Muid_covr12cb);
   fChain->SetBranchAddress("Muid_covr22cb", &Muid_covr22cb, &b_Muid_covr22cb);
   fChain->SetBranchAddress("Muid_covr13cb", &Muid_covr13cb, &b_Muid_covr13cb);
   fChain->SetBranchAddress("Muid_covr23cb", &Muid_covr23cb, &b_Muid_covr23cb);
   fChain->SetBranchAddress("Muid_covr33cb", &Muid_covr33cb, &b_Muid_covr33cb);
   fChain->SetBranchAddress("Muid_covr14cb", &Muid_covr14cb, &b_Muid_covr14cb);
   fChain->SetBranchAddress("Muid_covr24cb", &Muid_covr24cb, &b_Muid_covr24cb);
   fChain->SetBranchAddress("Muid_covr34cb", &Muid_covr34cb, &b_Muid_covr34cb);
   fChain->SetBranchAddress("Muid_covr44cb", &Muid_covr44cb, &b_Muid_covr44cb);
   fChain->SetBranchAddress("Muid_covr15cb", &Muid_covr15cb, &b_Muid_covr15cb);
   fChain->SetBranchAddress("Muid_covr25cb", &Muid_covr25cb, &b_Muid_covr25cb);
   fChain->SetBranchAddress("Muid_covr35cb", &Muid_covr35cb, &b_Muid_covr35cb);
   fChain->SetBranchAddress("Muid_covr45cb", &Muid_covr45cb, &b_Muid_covr45cb);
   fChain->SetBranchAddress("Muid_covr55cb", &Muid_covr55cb, &b_Muid_covr55cb);
   fChain->SetBranchAddress("Muid_mslinkcb", &Muid_mslinkcb, &b_Muid_mslinkcb);
   fChain->SetBranchAddress("Muid_exlinkcb", &Muid_exlinkcb, &b_Muid_exlinkcb);
   fChain->SetBranchAddress("Muid_blayercb", &Muid_blayercb, &b_Muid_blayercb);
   fChain->SetBranchAddress("Muid_pixelcb", &Muid_pixelcb, &b_Muid_pixelcb);
   fChain->SetBranchAddress("Muid_sctcb", &Muid_sctcb, &b_Muid_sctcb);
   fChain->SetBranchAddress("Muid_trtcb", &Muid_trtcb, &b_Muid_trtcb);
   fChain->SetBranchAddress("Muid_trthighcb", &Muid_trthighcb, &b_Muid_trthighcb);
   fChain->SetBranchAddress("Muid_a0rid", &Muid_a0rid, &b_Muid_a0rid);
   fChain->SetBranchAddress("Muid_z0rid", &Muid_z0rid, &b_Muid_z0rid);
   fChain->SetBranchAddress("Muid_phirid", &Muid_phirid, &b_Muid_phirid);
   fChain->SetBranchAddress("Muid_thetarid", &Muid_thetarid, &b_Muid_thetarid);
   fChain->SetBranchAddress("Muid_etarid", &Muid_etarid, &b_Muid_etarid);
   fChain->SetBranchAddress("Muid_pirid", &Muid_pirid, &b_Muid_pirid);
   fChain->SetBranchAddress("Muid_chi2id", &Muid_chi2id, &b_Muid_chi2id);
   fChain->SetBranchAddress("Muid_pulla0cb", &Muid_pulla0cb, &b_Muid_pulla0cb);
   fChain->SetBranchAddress("Muid_pullz0cb", &Muid_pullz0cb, &b_Muid_pullz0cb);
   fChain->SetBranchAddress("Muid_pullphicb", &Muid_pullphicb, &b_Muid_pullphicb);
   fChain->SetBranchAddress("Muid_pullthecb", &Muid_pullthecb, &b_Muid_pullthecb);
   fChain->SetBranchAddress("Muid_pullmomcb", &Muid_pullmomcb, &b_Muid_pullmomcb);
   fChain->SetBranchAddress("MemRSS", &MemRSS, &b_MemRSS);
   fChain->SetBranchAddress("MemVSize", &MemVSize, &b_MemVSize);
   fChain->SetBranchAddress("TimeTotal", &TimeTotal, &b_TimeTotal);
   fChain->SetBranchAddress("TimeSum", &TimeSum, &b_TimeSum);
   fChain->SetBranchAddress("TimeEventCounter", &TimeEventCounter, &b_TimeEventCounter);
   fChain->SetBranchAddress("TimeDetStatusAlg", &TimeDetStatusAlg, &b_TimeDetStatusAlg);
   fChain->SetBranchAddress("TimeInDetPixelClusterization", &TimeInDetPixelClusterization, &b_TimeInDetPixelClusterization);
   fChain->SetBranchAddress("TimeInDetSCT_Clusterization", &TimeInDetSCT_Clusterization, &b_TimeInDetSCT_Clusterization);
   fChain->SetBranchAddress("TimeInDetTRT_RIO_Maker", &TimeInDetTRT_RIO_Maker, &b_TimeInDetTRT_RIO_Maker);
   fChain->SetBranchAddress("TimeInDetPRD_MultiTruthMaker", &TimeInDetPRD_MultiTruthMaker, &b_TimeInDetPRD_MultiTruthMaker);
   fChain->SetBranchAddress("TimeInDetSiTrackerSpacePointFinder", &TimeInDetSiTrackerSpacePointFinder, &b_TimeInDetSiTrackerSpacePointFinder);
   fChain->SetBranchAddress("TimeiPatRec", &TimeiPatRec, &b_TimeiPatRec);
   fChain->SetBranchAddress("TimeiPatTrackTruthAssociator", &TimeiPatTrackTruthAssociator, &b_TimeiPatTrackTruthAssociator);
   fChain->SetBranchAddress("TimeiPatStatistics", &TimeiPatStatistics, &b_TimeiPatStatistics);
   fChain->SetBranchAddress("TimeInDetLegacyCnvAlg", &TimeInDetLegacyCnvAlg, &b_TimeInDetLegacyCnvAlg);
   fChain->SetBranchAddress("TimeConvertedIPatTracksDetailedTruthMaker", &TimeConvertedIPatTracksDetailedTruthMaker, &b_TimeConvertedIPatTracksDetailedTruthMaker);
   fChain->SetBranchAddress("TimeConvertedIPatTracksTruthSelector", &TimeConvertedIPatTracksTruthSelector, &b_TimeConvertedIPatTracksTruthSelector);
   fChain->SetBranchAddress("TimeInDetCopyAlg", &TimeInDetCopyAlg, &b_TimeInDetCopyAlg);
   fChain->SetBranchAddress("TimeDetailedTrackTruthMaker", &TimeDetailedTrackTruthMaker, &b_TimeDetailedTrackTruthMaker);
   fChain->SetBranchAddress("TimeTrackTruthCollectionSelector", &TimeTrackTruthCollectionSelector, &b_TimeTrackTruthCollectionSelector);
   fChain->SetBranchAddress("TimeInDetPriVxFinder", &TimeInDetPriVxFinder, &b_TimeInDetPriVxFinder);
   fChain->SetBranchAddress("TimeInDetParticleCreation", &TimeInDetParticleCreation, &b_TimeInDetParticleCreation);
   fChain->SetBranchAddress("TimeInDetTrackParticleTruthMaker", &TimeInDetTrackParticleTruthMaker, &b_TimeInDetTrackParticleTruthMaker);
   fChain->SetBranchAddress("TimeInDetRecStatistics", &TimeInDetRecStatistics, &b_TimeInDetRecStatistics);
   fChain->SetBranchAddress("TimeInDetTrackClusterAssValidation", &TimeInDetTrackClusterAssValidation, &b_TimeInDetTrackClusterAssValidation);
   fChain->SetBranchAddress("TimeCaloCellMaker", &TimeCaloCellMaker, &b_TimeCaloCellMaker);
   fChain->SetBranchAddress("TimeMuonRdoToMuonDigit", &TimeMuonRdoToMuonDigit, &b_TimeMuonRdoToMuonDigit);
   fChain->SetBranchAddress("TimeCscRdoToCscPrepData", &TimeCscRdoToCscPrepData, &b_TimeCscRdoToCscPrepData);
   fChain->SetBranchAddress("TimeMdtRdoToMdtPrepData", &TimeMdtRdoToMdtPrepData, &b_TimeMdtRdoToMdtPrepData);
   fChain->SetBranchAddress("TimeRpcRdoToRpcPrepData", &TimeRpcRdoToRpcPrepData, &b_TimeRpcRdoToRpcPrepData);
   fChain->SetBranchAddress("TimeTgcRdoToTgcPrepData", &TimeTgcRdoToTgcPrepData, &b_TimeTgcRdoToTgcPrepData);
   fChain->SetBranchAddress("TimeCscThresholdClusterBuilder", &TimeCscThresholdClusterBuilder, &b_TimeCscThresholdClusterBuilder);
   fChain->SetBranchAddress("TimeTrackRecordFilter", &TimeTrackRecordFilter, &b_TimeTrackRecordFilter);
   fChain->SetBranchAddress("TimeMooMakePhiPatterns", &TimeMooMakePhiPatterns, &b_TimeMooMakePhiPatterns);
   fChain->SetBranchAddress("TimeMooMakeRzPatterns", &TimeMooMakeRzPatterns, &b_TimeMooMakeRzPatterns);
   fChain->SetBranchAddress("TimeMooCombinePatterns", &TimeMooCombinePatterns, &b_TimeMooCombinePatterns);
   fChain->SetBranchAddress("TimeMooCalibratedSegmentMaker", &TimeMooCalibratedSegmentMaker, &b_TimeMooCalibratedSegmentMaker);
   fChain->SetBranchAddress("TimeMooRzSegmentCombinationMaker", &TimeMooRzSegmentCombinationMaker, &b_TimeMooRzSegmentCombinationMaker);
   fChain->SetBranchAddress("TimePhiPatternToPhiSeg", &TimePhiPatternToPhiSeg, &b_TimePhiPatternToPhiSeg);
   fChain->SetBranchAddress("TimeCscPatternToCrudeSeg", &TimeCscPatternToCrudeSeg, &b_TimeCscPatternToCrudeSeg);
   fChain->SetBranchAddress("TimeMdtPatternToCrudeSeg", &TimeMdtPatternToCrudeSeg, &b_TimeMdtPatternToCrudeSeg);
   fChain->SetBranchAddress("TimeMuonSegmentComboToMooRzSegmentCombo", &TimeMuonSegmentComboToMooRzSegmentCombo, &b_TimeMuonSegmentComboToMooRzSegmentCombo);
   fChain->SetBranchAddress("TimeMuonSegmentToMooRzSegment", &TimeMuonSegmentToMooRzSegment, &b_TimeMuonSegmentToMooRzSegment);
   fChain->SetBranchAddress("TimeMooRoadMaker", &TimeMooRoadMaker, &b_TimeMooRoadMaker);
   fChain->SetBranchAddress("TimeMooMakeTracks", &TimeMooMakeTracks, &b_TimeMooMakeTracks);
   fChain->SetBranchAddress("TimeMooCnvAlg", &TimeMooCnvAlg, &b_TimeMooCnvAlg);
   fChain->SetBranchAddress("TimeMuidMooreTPCreator", &TimeMuidMooreTPCreator, &b_TimeMuidMooreTPCreator);
   fChain->SetBranchAddress("TimeMuidStandAlone", &TimeMuidStandAlone, &b_TimeMuidStandAlone);
   fChain->SetBranchAddress("TimeMuidExtrCnvAlg", &TimeMuidExtrCnvAlg, &b_TimeMuidExtrCnvAlg);
   fChain->SetBranchAddress("TimeMuidCombined", &TimeMuidCombined, &b_TimeMuidCombined);
   fChain->SetBranchAddress("TimeMuidCnvAlg", &TimeMuidCnvAlg, &b_TimeMuidCnvAlg);
   fChain->SetBranchAddress("TimeMuidStatistics", &TimeMuidStatistics, &b_TimeMuidStatistics);
   fChain->SetBranchAddress("TimeCBNT_AthenaAware", &TimeCBNT_AthenaAware, &b_TimeCBNT_AthenaAware);
   fChain->SetBranchAddress("TimeAANTupleStream", &TimeAANTupleStream, &b_TimeAANTupleStream);
}

Bool_t MooHists::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef MooHists_cxx
