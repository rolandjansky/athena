/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////
//                                //
//         trkana.h               //
//                                //
//       Salvador Marti           //
//       Vicente Lacuesta         //
//       Regina Moles             //
//       Carlos Escobar           //
//                                //
////////////////////////////////////

#include <iostream>
#include "TROOT.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TLorentzVector.h"
#include "TBox.h"
#include "TLine.h"
#include "TArc.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TMatrixD.h"
#include "TStopwatch.h"
#include "TGaxis.h"
#include "TSystem.h"
#include "TMarker.h"
#include "TPolyMarker.h"

// 
extern TFile *f0;
extern TTree *RecoTrk;
extern TTree *RHawNtu;
extern TTree *RHawMod;
extern TTree *GlblNtu;
extern TTree *TrkTrackNtu; 
extern TTree *CBNTupl;
extern TTree *SiClusterNtu;
extern TTree *TrkMatchingNtu;
//
extern TString *InputMonitorFile;
extern TString *DriverMacro;

// Constant variables
Bool_t DO_ZMASS_ANALYSIS=0;  // Do mass analysis??
Bool_t DO_HMASS_ANALYSIS=0;  // Do mass analysis??

const Float_t MassaZ = 91.2;
Int_t BarrelEndCapCode = 2;

Bool_t wanttodraw = false;          // Do you want to draw the plots?
Bool_t doFit = false;          
Bool_t doEigenAna = false;          
Bool_t doEventDisplay = false;          
Bool_t StoreHistos = false;

Bool_t doTruth=false;  // Process truth or not?
Bool_t doCosmics=false;  // Cosmics?

Bool_t SR1Ntuple = false;

Bool_t doCorrResidual = true;

//const Int_t ANA_LEVEL = 7; // Number of analysis levels
const Int_t ANA_LEVEL = 6; // M6 special March 2008

Char_t path[300] = ".";

Int_t IterNumber = -999;
Int_t nCPUs = 1;
Int_t hits2 = 0;

// Detector constants
const Int_t NSCT_Layers = 4;  // in SCT Barrel
const Int_t NPIX_Layers = 3;  // in PIXEL Barrel
const Int_t N_Endcaps = 2; // 2 endcaps
const Int_t NSCT_Discs=9; // in each endcap
const Int_t NPIX_Discs=3; // in each endcap
const Int_t NSCT_Sides=2; // SCT has 2 sides
const Int_t NSCT_Parts=3; // 1 barrel and 2 endcaps
const Int_t NPIX_Parts=3; // 1 barrel and 2 endcaps

// Program limits
const Int_t MAX_PARTICLES = 2000;
const Int_t MAX_HITS = 200;
//const Int_t MAX_HITS = 32;
const Int_t MAX_EVENTS = 100000;

// Histograms parameters
Int_t SCTHitsRange=17;
Int_t PIXHitsRange=17;
Int_t TRTHitsRange=40;

Int_t SharedHitsRange=17;
Int_t HolesRange=17;
Int_t HitsRange=40;
Int_t RecoTrackRange=14;
Int_t RecoTrackRangeWide=100;

Float_t PIXPhiResiRange=0.2;
Int_t PIXPhiResiBins=31;

Float_t PIXEtaResiRange=0.2;
Int_t PIXEtaResiBins=31;

Float_t PIXPlaneResiRange = 0.5;
Int_t   PIXPlaneResiBins=50;

Int_t PIXPhiPullBins = 75;
Float_t PIXPhiPullRange = 3.;

Int_t PIXEtaPullBins = 75;
Float_t PIXEtaPullRange = 3.;

Int_t PIXPhiErrBins=50;
Float_t PIXPhiErrRange=0.10;

Int_t PIXEtaErrBins=50;
Float_t PIXEtaErrRange=0.10;


//const Int_t SCTResiBins = 51;

Int_t SCTResiBins =150;
Float_t SCTResiRange = 0.050;


Int_t SCTPlaneResiBins =150;
Float_t SCTPlaneResiRange = 0.500;


Float_t SCTDelzRange = 70;

Int_t SCTPullBins = 51;
Float_t SCTPullRange = 3.;

Int_t SCTErrorBins=80;
Float_t SCTErrorRange = 0.080;

Int_t PtBins = 150;
Float_t PtRange = 75.;

Int_t EtaBins=101;
Float_t EtaRange=3;

Int_t EtaDifBins=101;
Float_t EtaDifRange=0.008;
Int_t EtaTrkDifBins=101;
Float_t EtaTrkDifRange=0.008;

Int_t QoverPtBins = 101;
Float_t QoverPtRange = 0.5;

Int_t QoverPtDifBins=81;
Float_t QoverPtDifRange=0.025;
Int_t QoverPtTrkDifBins=81;
Float_t QoverPtTrkDifRange=0.025;

Int_t PtDifBins=51; //RecoPt-TruthPt
Float_t PtDifRange=20.;
Int_t PtTrkDifBins=81; //RecoPt-TruthPt
Float_t PtTrkDifRange=15.;

Int_t Z0Bins=101;
//Float_t Z0Range=200.;
Float_t Z0Range=1500.;
Float_t Z0TruthRange=200.;
Float_t Z0trkTruthRange=300.;

Int_t Z0DifBins=81;
Float_t Z0DifRange=5.;
Int_t Z0TrkDifBins=81;
Float_t Z0TrkDifRange=5.;

Int_t D0Bins=61;
//Float_t D0Range=3;
Float_t D0Range=600;  

Int_t D0TruthBins=61;
Float_t D0TruthRange=1.;

Int_t D0TrkTruthBins=101;
Float_t D0trkTruthRange=0.75;

Int_t D0DifBins=75;
Float_t D0DifRange=1.5;
Int_t D0TrkDifBins=75;
Float_t D0TrkDifRange=1.5;

Int_t D0PullBins=51;
Float_t D0PullRange=5.;

Int_t D0SigBins=41;
Float_t D0SigRange=8.;

Int_t BinsdD0 = 41;
Float_t XmindD0 = 0.00;
Float_t XmaxdD0 = 0.10;

Int_t BinsdZ0 = 61;
Float_t XmindZ0 = 0.00;
Float_t XmaxdZ0 = 0.30;

Int_t Phi0Bins=61;
Float_t Phi0Range=TMath::Pi();

Int_t Phi0DifBins=81;
Float_t Phi0DifRange=0.008;
Int_t Phi0TrkDifBins=81;
Float_t Phi0TrkDifRange=0.008;

Int_t ThetaBins=45;
Float_t ThetaRange=TMath::Pi();

Int_t ThetaDifBins=101;
Float_t ThetaDifRange=0.008;
Int_t ThetaTrkDifBins=101;
Float_t ThetaTrkDifRange=0.008;

Int_t X0Bins=51;
Float_t X0Range=3.;

Int_t Y0Bins=51;
Float_t Y0Range=3.;

Int_t Chi2Bins=141;
Float_t Chi2Range=14.;

Float_t VtxCandRange=2.0;
Float_t VtxCandRangeZoom=0.18;
Float_t VtxZCandRange=300.;
Float_t VtxTruthRange=0.18;
Float_t VtxZTruthRange=300.;

Bool_t TrackLikeNtuple = 0;
Bool_t EventLikeNtuple = 0;

Int_t PrintLevel;

Long64_t FirstTrk;
Long64_t LastTrk;
Int_t PrintProgressAfterN;

Int_t iGlobalTag = 999;
Int_t iMisalignment = 999; // obsolete

// output file and directories
TFile *TrkanaFile;
TTree *GlblSummary;
TDirectoryFile *TrkanaDir;
TDirectory *SCTResPlaneDir;
TDirectory *PIXResPlaneDir;
TDirectory *MassAnalysisDir;
TDirectory *NewHistosDir;
TDirectory *HitsTracksDir;
TDirectory *PIXHitsDir;
TDirectory *SCTHitsDir;
TDirectory *PIXResDir;
TDirectory *SCTResDir;
TDirectory *TrackParametersDir;
TDirectory *TrkTrackDir;
TDirectory *SiClusterDir;
TDirectory *TrkMatchingDir;
TDirectory *CDFFitDir;
TDirectory *EigenDir;
TDirectory *BeamSpotDir;
TTree *SummaryTree;
TTree *OldSetupTree;

// common ntuple variables
Float_t RecQoverP;
Int_t   nHits;
Int_t   nShared;
Int_t   nHoles;
Int_t   DetectorType[MAX_HITS];

Float_t HitRadius[MAX_HITS];
Float_t HitZ[MAX_HITS];       
Float_t Chi2overNdof;
Int_t   nSCTHits;
Int_t   nPIXHits;

Float_t SCTRes[MAX_HITS];       
Float_t SCTResErr[MAX_HITS];       
Float_t PIXResPhi[MAX_HITS];       
Float_t PIXResEta[MAX_HITS];
Float_t SCTCorrRes[MAX_HITS];       
Float_t SCTCorrResErr[MAX_HITS];       
Float_t PIXCorrResPhi[MAX_HITS];       
Float_t PIXCorrResEta[MAX_HITS];
Float_t TrkCovariance[15];

Float_t RecoPt;
Float_t RecoEta;
Float_t RecoTheta;
Float_t RecoD0;
Float_t RecoD0bs;
Float_t RecoZ0;
Float_t RecoPhi0;
short RecoQ;
Float_t RecoQoverPt;
Float_t RecoX0;
Float_t RecoY0;
Float_t RecodD0;
Float_t RecodZ0;
Float_t RecodQoverPt;
Float_t RecoD0Pull;
Float_t RecoD0Signif;
Float_t RecoChi2; // per a guardar el Chi2 de la trac,a
Int_t RecoNdof;

Float_t TruzPt;
Float_t TruzEta;
Float_t TruzTheta;
Float_t TruzD0;
Float_t TruzZ0;
Float_t TruzPhi0;
Float_t TruzQoverP;
Float_t TruzQoverPt;
Float_t TruzX0;
Float_t TruzY0;

// Time stamp
long Vtimestamp[MAX_PARTICLES];
long Timestamp;
long min_time = 1221000000;
long max_time = 1230000000;
Int_t timeBins = 1000;

// TrkTrack Branch
Float_t TRecoQoverPt;
Float_t TRecoPt;
Float_t TRecoEta;
Float_t TRecoPhi0;
Float_t TRecoTheta;
Float_t TRecoD0;
Float_t TRecoZ0;
Float_t TRecoQoverP;
short TRecoQ;
Float_t TTruzQoverPt;
Float_t TTruzPt;
Float_t TTruzEta;
Float_t TTruzPhi0;
Float_t TTruzTheta;
Float_t TTruzD0;
Float_t TTruzZ0;
Float_t TTruzQoverP;
Float_t TRecoCotTheta;
Float_t TTruzCotTheta;

Float_t TrkTruthVtxX;
Float_t TrkTruthVtxY;
Float_t TrkTruthVtxZ;

Float_t genParticleCharge;
Float_t genParticlePt;
Float_t genParticlePhi;
Float_t genParticleEta;

// TrkMatching ntuple
Float_t Trk_matching_delta_d0[MAX_PARTICLES];
Float_t Trk_matching_delta_phi0[MAX_PARTICLES];
Float_t Trk_matching_delta_eta[MAX_PARTICLES];
Float_t Trk_matching_delta_theta0[MAX_PARTICLES];
Float_t Trk_matching_delta_z0[MAX_PARTICLES];
Float_t Trk_matching_delta_qoverpt[MAX_PARTICLES];
Float_t Trk_matching_delta_pt[MAX_PARTICLES];
Float_t Trk_matching_delta_charge[MAX_PARTICLES];


Float_t matchingD0;
Float_t matchingPhi0;
Float_t matchingEta;
Float_t matchingTheta0;
Float_t matchingZ0;
Float_t matchingQoverPt;
Float_t matchingPt;
Float_t matchingCharge;

// SiCluter ntuple
const Int_t MaxSCTClusters=2000;
Int_t sct_clus_nclusters;
Int_t sct_clus_groupsize[MaxSCTClusters];
Int_t sct_clus_layer[MaxSCTClusters];
Int_t sct_clus_phi[MaxSCTClusters];
Int_t sct_clus_eta[MaxSCTClusters];
Float_t sct_clus_x[MaxSCTClusters];
Float_t sct_clus_y[MaxSCTClusters];

// Richard ntuple
Float_t RecoCotTheta;
Float_t TruzCotTheta;
Int_t ModType;
Int_t ident[MAX_HITS];
Int_t BarrelEndcap;
Int_t ModLayer;
Int_t ModRing;
Int_t ModSector;
Int_t ModSide;
Float_t RPhiResidual[MAX_HITS];
Float_t RPhiResError[MAX_HITS];
Float_t RZResidual[MAX_HITS];
Float_t RZResError[MAX_HITS];
Float_t RPhiCorrResidual[MAX_HITS];
Float_t REtaCorrResidual[MAX_HITS];
Float_t RDelzr[MAX_HITS];
Int_t RUsedHit[MAX_HITS];
Int_t is2Dresidual;
// original Summary branch
Int_t iSummaryEvents = 999;
Int_t iSummaryTracks = 999;
Int_t iSummaryUsedTracks = 999;
Int_t iSummaryHits = 999;
Int_t iSummaryPixelHits = 999;
Int_t iSummarySCTHits = 999;
Int_t iSummaryUsedHits = 999; 
Int_t iSummaryAlignOpt = 999;
Int_t iSummarySetup = 999;
Int_t iSummaryCosmics = 999;
Float_t iSummaryBField = 999;
Int_t iSummarySimulation = 999;
Int_t iSummaryMisalignment = 999;
Int_t iSummaryNoMCS = 999;
Int_t iSummaryNoMCSCosmics = 999;
Int_t iSummaryGlobalTag = 999;
Float_t iSummaryNtupleVersion = 1.0;
Int_t iSummaryBeamSpot = 999;
Int_t iSummaryTrkCons = 999;
Int_t iSummaryAlignProcessLevel = 999;
Int_t iSummaryMinimumHitsPerTrack = 999;
Int_t iSummaryMinimumPixelHitsPerTrack = 999;
Int_t iSummaryMinimumSCTHitsPerTrack = 999;

// new Summary branch
Int_t jSummaryEvents;
Int_t jSummaryTracks;
Int_t jSummaryUsedTracks;
Int_t jSummaryHits;
Int_t jSummaryPixelHits;
Int_t jSummarySCTHits;
Int_t jSummaryUsedHits;
Int_t jSummaryAlignOpt;
Int_t jSummarySetup;
Int_t jSummaryCosmics;
Float_t jSummaryBField;
Int_t jSummarySimulation;
Int_t jSummaryMisalignment;
Int_t jSummaryNoMCS;
Int_t jSummaryNoMCSCosmics;
Int_t jSummaryGlobalTag;
Float_t jSummaryNtupleVersion;
Int_t jSummaryAlignProcessLevel;
Int_t jSummaryMinimumHitsPerTrack;
Int_t jSummaryMinimumPixelHitsPerTrack;
Int_t jSummaryMinimumSCTHitsPerTrack;
Int_t jSummaryBeamSpot;
Int_t jSummaryTrkCons;

// Global ntuple
Float_t VRecoQoverPt[MAX_PARTICLES];
Float_t VRecoPhi0[MAX_PARTICLES];
Float_t VRecoCotTheta[MAX_PARTICLES];
Float_t VRecoD0[MAX_PARTICLES];
Float_t VRecoZ0[MAX_PARTICLES];
Float_t VRecoD0bs[MAX_PARTICLES];

Float_t VRecodD0[MAX_PARTICLES];
Float_t VRecodZ0[MAX_PARTICLES];

Float_t VTruzQoverPt[MAX_PARTICLES];
Float_t VTruzPhi0[MAX_PARTICLES];
Float_t VTruzCotTheta[MAX_PARTICLES];
Float_t VTruzD0[MAX_PARTICLES];
Float_t VTruzZ0[MAX_PARTICLES];

Int_t Used_Track[MAX_PARTICLES];
Int_t usedTrack;

// TrkTrack branch
Float_t VTRecoQoverP[MAX_PARTICLES];
Float_t VTRecoPhi0[MAX_PARTICLES];
Float_t VTRecoTheta0[MAX_PARTICLES];
Float_t VTRecoD0[MAX_PARTICLES];
Float_t VTRecoZ0[MAX_PARTICLES];
Float_t VTRecoPt[MAX_PARTICLES];
Float_t VTRecoD0bs[MAX_PARTICLES];

Float_t VTTruzQoverP[MAX_PARTICLES];
Float_t VTTruzPhi0[MAX_PARTICLES];
Float_t VTTruzTheta0[MAX_PARTICLES];
Float_t VTTruzD0[MAX_PARTICLES];
Float_t VTTruzZ0[MAX_PARTICLES];
Float_t VTTruzPt[MAX_PARTICLES];

Float_t VTrkTruthVtxX[MAX_PARTICLES];
Float_t VTrkTruthVtxY[MAX_PARTICLES];
Float_t VTrkTruthVtxZ[MAX_PARTICLES];

Float_t VgenParticleCharge[MAX_PARTICLES];
Float_t VgenParticlePt[MAX_PARTICLES];
Float_t VgenParticlePhi[MAX_PARTICLES];
Float_t VgenParticleEta[MAX_PARTICLES];

Int_t   VnHits[MAX_PARTICLES];
Int_t   VnShared[MAX_PARTICLES];
Int_t   VnHoles[MAX_PARTICLES];
Int_t   VnPIXHits[MAX_PARTICLES];
Int_t   VnSCTHits[MAX_PARTICLES];
Int_t   VHit_DetType[MAX_PARTICLES][MAX_HITS];
Int_t   VModType[MAX_HITS];
Int_t   VHit_BEc[MAX_PARTICLES][MAX_HITS];
Int_t   VBEc[MAX_HITS];
Int_t   VHit_Layer[MAX_PARTICLES][MAX_HITS];
Int_t   VLayer[MAX_HITS];
Int_t   VHit_Ring[MAX_PARTICLES][MAX_HITS];
Int_t   VRing[MAX_HITS];
Int_t   VHit_Sector[MAX_PARTICLES][MAX_HITS];
Int_t   VSector[MAX_HITS];
Int_t   VHit_Side[MAX_PARTICLES][MAX_HITS];
Int_t   VSide[MAX_HITS];
Float_t VRes_rphi[MAX_PARTICLES][MAX_HITS];
Float_t VRes_rphi_error[MAX_PARTICLES][MAX_HITS];
Float_t VRes_corr_rphi[MAX_PARTICLES][MAX_HITS];
Float_t VRes_corr_eta[MAX_PARTICLES][MAX_HITS];
Float_t VDelzr[MAX_PARTICLES][MAX_HITS];
Double_t SR1_VRes_rphi[MAX_PARTICLES][MAX_HITS];
Double_t SR1_VRes_rphi_error[MAX_PARTICLES][MAX_HITS];
Float_t VRes_eta[MAX_PARTICLES][MAX_HITS];
Float_t VRes_eta_error[MAX_PARTICLES][MAX_HITS];
Int_t   VUsed_Hit[MAX_PARTICLES][MAX_HITS];

Int_t VtxCandidates = 0;
Float_t VtxCandidateX = 0.0;
Float_t VtxCandidateY = 0.0;
Float_t VtxCandidateZ = 0.0;

Int_t TotalUsedHits=0;

// Track analysis
const Float_t ZRange_SCTBarrel = 780.;
const Float_t ZRange_PIXBarrel = 450.;
Bool_t trk_HitInSCTLayer[NSCT_Layers];
Bool_t trk_HitInSCTDisc[NSCT_Discs];
Bool_t trk_HitInSCTDiscA[NSCT_Discs];
Bool_t trk_HitInSCTDiscC[NSCT_Discs];
Int_t  Hited_SCT_Layers;
Bool_t trk_HitInPIXLayer[NPIX_Layers];
Bool_t trk_HitInPIXDisc[NPIX_Discs];
Bool_t trk_HitInPIXDiscA[NPIX_Discs];
Bool_t trk_HitInPIXDiscC[NPIX_Discs];
Int_t  Hited_PIX_Layers;

Int_t NSCT_Hits;
Int_t usedNSCT_Hits;        //used hits in SCT (only level=1)
Int_t NSCT_HitsBarrel;      // Of NSCT_Hits, how many are just barrel
Int_t NSCT_HitsEndCap;      // Of NSCT_Hits, how many are in the endcaps
Int_t UsedSCTHit[MAX_HITS];
Int_t SCTHitType[MAX_HITS];  // 1= barrel, 2= endcap 
Int_t SCTHitLayDisc[MAX_HITS];
Int_t SCTHitRing[MAX_HITS];
Int_t SCTHitSector[MAX_HITS];
Int_t SCTHitSide[MAX_HITS];
Float_t SCTHitRes[MAX_HITS];
Float_t SCTHitPul[MAX_HITS];
Float_t SCTHitErr[MAX_HITS];
Float_t SCTHitDelzr[MAX_HITS];
Float_t SCTHitCorrRes[MAX_HITS];

Int_t NPIX_Hits;
Int_t usedNPIX_Hits;        //used hits in PIX (only level=1)
Int_t UsedPIXHit[MAX_HITS];
Int_t PIXHitType[MAX_HITS];  // 1= barrel, 2= endcap
Int_t PIXHitLayDisc[MAX_HITS];
Int_t PIXHitRing[MAX_HITS];
Int_t PIXHitSector[MAX_HITS];

Float_t PIXHitPhiRes[MAX_HITS];
Float_t PIXHitPhiErr[MAX_HITS];
Float_t PIXHitPhiPul[MAX_HITS];

Float_t PIXHitEtaRes[MAX_HITS];
Float_t PIXHitEtaErr[MAX_HITS];
Float_t PIXHitEtaPul[MAX_HITS];

Float_t PIXHitEtaCorrRes[MAX_HITS];
Float_t PIXHitPhiCorrRes[MAX_HITS];

Float_t PIXHitDelzr[MAX_HITS];

Int_t NTRT_Hits;
Int_t NShared_Hits;
Int_t NHoles_Hits;
//
Int_t EventNumber;
Int_t TotalEvents;
Int_t TotalTracks;
Bool_t NewEvent;

Int_t NTracks;
Int_t NTrkTracks;
Int_t NTracksInThisEvent[ANA_LEVEL];
Int_t GoodTrackLevel[MAX_PARTICLES];


// mass analysis variables

TLorentzVector *Particle4Mom[MAX_PARTICLES];
TLorentzVector *ParticleSpace[MAX_PARTICLES];

short ParticleQ[MAX_PARTICLES];
Bool_t MuPlusFound[ANA_LEVEL];
Bool_t MuMinusFound[ANA_LEVEL];
short MuPlus[ANA_LEVEL];
short MuMinus[ANA_LEVEL];
Bool_t GoodMassAnalysis[ANA_LEVEL];

TLorentzVector MuonPair[ANA_LEVEL];
TLorentzVector MuonPlus[ANA_LEVEL];
TLorentzVector MuonMinus[ANA_LEVEL];
TLorentzVector MuonPlusSpace[ANA_LEVEL];
TLorentzVector MuonMinusSpace[ANA_LEVEL];

TLorentzVector HZ4lept[ANA_LEVEL];
TLorentzVector RealZCandidate[ANA_LEVEL];
TLorentzVector VirtZCandidate[ANA_LEVEL];
Bool_t Good_HZ_Analysis[ANA_LEVEL];

// Beam Spot variables
float BeamSpotX;
float BeamSpotY;
float BeamSpotZ;
float BeamSpotTiltX;
float BeamSpotTiltY;
int LumiBlockID;

//Graphics
// graphic parameters
const Int_t CanDeltaX = 25;
const Int_t CanDeltaY = 25;
Int_t ThisCanX;
Int_t ThisCanY;
Int_t NCanvasOpen;

//Histograms
TH1F *hSTATS;

// Time histograms

TH2F *hSCTResVsTimestamp[ANA_LEVEL];
// PIX and SCT Histos
TH2F *hPIXvSCTHits[ANA_LEVEL];
TH1F *hSharedHits[ANA_LEVEL];
TH1F *hHolesHits[ANA_LEVEL];

TH1F *hNTrk[ANA_LEVEL];
TH1F *hNTrkWide[ANA_LEVEL];
TH1F *hNTrk_Event[ANA_LEVEL];                  // AlignTrks vs event
TH1F *hNTrkTrack[ANA_LEVEL];

TH1F *hHits[ANA_LEVEL];                        // Hits in track

// PIX Histos
TH1F *hPIXHits[ANA_LEVEL];                          // PIX hits by track
TH2F *hPIXHitsvEta[ANA_LEVEL];                      // PIX hits vs eta
TH1F *hPIXBarrelHits[ANA_LEVEL];                    // PIX Barrel hits by layer
TH1F *hPIXEndcapHits[NPIX_Discs][ANA_LEVEL];        // PIX Endcap A o C hits by disc
                                                    // tap=0 Endcap A
                                                    // tap=1 Endcap C
TH1F *hPIXPhiRes[ANA_LEVEL];                        // PIX phi residuals
TH1F *hPIXEtaRes[ANA_LEVEL];                        // PIX eta residuals
TH1F *hPIXPhiCorrRes[ANA_LEVEL];                    // PIX phi corrected residuals
TH1F *hPIXEtaCorrRes[ANA_LEVEL];                    // PIX eta corrected residuals
TH1F *hPIXDelzr[ANA_LEVEL];                         // PIX delzr
TH1F *hPIXPhiResBa[ANA_LEVEL];                      // PIX phi residuals in barrel
TH1F *hPIXPhiResBaWide[ANA_LEVEL];                  // PIX phi residuals in barrel (wide)
TH1F *hPIXEtaResBa[ANA_LEVEL];                      // PIX eta residuals in barrel
TH1F *hPIXEtaResBaWide[ANA_LEVEL];                  // PIX eta residuals in barrel (wide)
TH1F *hPIXPhiResBaLay[NPIX_Layers][ANA_LEVEL];      // PIX phi residuals in barrel by layer
TH1F *hPIXPhiResBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX phi residuals in barrel by layer (wide)
TH1F *hPIXEtaResBaLay[NPIX_Layers][ANA_LEVEL];      // PIX eta residuals in barrel by layer
TH1F *hPIXEtaResBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX eta residuals in barrel by layer (wide)
TH1F *hPIXPhiCorrResBa[ANA_LEVEL];                      // PIX phi corrected residuals in barrel
TH1F *hPIXPhiCorrResBaWide[ANA_LEVEL];                  // PIX phi corrected residuals in barrel (wide)
TH1F *hPIXEtaCorrResBa[ANA_LEVEL];                      // PIX eta corrected residuals in barrel
TH1F *hPIXEtaCorrResBaWide[ANA_LEVEL];                  // PIX eta corrected residuals in barrel (wide)
TH1F *hPIXPhiCorrResBaLay[NPIX_Layers][ANA_LEVEL];      // PIX phi corrected residuals in barrel by layer
TH1F *hPIXPhiCorrResBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX phi corrected residuals in barrel by layer (wide)
TH1F *hPIXEtaCorrResBaLay[NPIX_Layers][ANA_LEVEL];      // PIX eta corrected residuals in barrel by layer
TH1F *hPIXEtaCorrResBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX eta corrected residuals in barrel by layer (wide)
TH1F *hPIXErrPhiBaLay[NPIX_Layers][ANA_LEVEL];      // PIX phi errors in barrel by layer
TH1F *hPIXErrEtaBaLay[NPIX_Layers][ANA_LEVEL];      // PIX eta errors in barrel by layer
TH1F *hPIXPhiPulBaLay[NPIX_Layers][ANA_LEVEL];      // PIX phi pull in barrel by layer
TH1F *hPIXPhiPulBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX phi pull in barrel by layer (wide)
TH1F *hPIXPhiPulBa[ANA_LEVEL];                      // PIX phi pull in barrel 
TH1F *hPIXEtaPulBaLay[NPIX_Layers][ANA_LEVEL];      // PIX eta pull in barrel by layer
TH1F *hPIXEtaPulBaLayWide[NPIX_Layers][ANA_LEVEL];  // PIX eta pull in barrel by layer (wide)
TH1F *hPIXEtaPulBa[ANA_LEVEL];                      // PIX phi pull in barrel
TProfile *hproPIXMeanResBaLay[ANA_LEVEL];           // PIX Barrel mean residuals by layer
TH1F *hPIXPhiResEndcap[ANA_LEVEL];                  // PIX phi residuals in endcaps
TH1F *hPIXEtaResEndcap[ANA_LEVEL];                  // PIX eta residuals in endcaps
TH1F *hPIXPhiCorrResEndcap[ANA_LEVEL];              // PIX phi corrected residuals in endcaps
TH1F *hPIXEtaCorrResEndcap[ANA_LEVEL];              // PIX eta corrected residuals in endcaps
TH1F *hPIXPhiPulEndcap[ANA_LEVEL];                  // PIX phi pull in endcaps
TH1F *hPIXEtaPulEndcap[ANA_LEVEL];                  // PIX eta pull in endcaps
TH1F *hPIXPhiResEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX phi corrected residuals in endcaps by disc
TH1F *hPIXPhiResEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX phi corrected residuals in endcaps by disc (wide)
TH1F *hPIXEtaResEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX eta corrected residuals in endcaps by disc
TH1F *hPIXEtaResEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX eta corrected residuals in endcaps by disc (wide)
TH1F *hPIXPhiCorrResEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX phi residuals in endcaps by disc
TH1F *hPIXPhiCorrResEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX phi residuals in endcaps by disc (wide)
TH1F *hPIXEtaCorrResEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX eta residuals in endcaps by disc
TH1F *hPIXEtaCorrResEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX eta residuals in endcaps by disc (wide)
TH1F *hPIXPhiPulEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX phi pulls in endcaps by disc
TH1F *hPIXPhiPulEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX phi pulls in endcaps by disc (wide)
TH1F *hPIXEtaPulEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      //PIX eta pulls in endcaps by disc
TH1F *hPIXEtaPulEndcapDiscWide[N_Endcaps][NPIX_Discs][ANA_LEVEL];  //PIX eta pulls in endcaps by disc (wide)
TProfile *hproPIXMeanResEndcap[N_Endcaps][ANA_LEVEL];              // PIX endcap mean residuals by disc
TH2F *hPIXHitMapBaLay[NPIX_Layers][ANA_LEVEL];                     // PIX Hit map in barrel by layer
TH2F *hPIXHitMapEndcapDisc[N_Endcaps][NPIX_Discs][ANA_LEVEL];      // PIX Hit map in endcaps by disc

TH1F *hPIXResPlane[NPIX_Parts][NPIX_Layers][17][52][ANA_LEVEL];     // PIX phi res per module
TH1F *hPIXEtaResPlane[NPIX_Parts][NPIX_Layers][17][52][ANA_LEVEL];  // PIX eta res per module

TH2F *hPIXResMeanMapBaLay[NSCT_Layers][ANA_LEVEL];               // Residual maps
TH2F *hPIXResMeanMapEaLay[NSCT_Discs][ANA_LEVEL];
TH2F *hPIXResMeanMapEcLay[NSCT_Discs][ANA_LEVEL];
TH2F *hPIXResSigmaMapBaLay[NSCT_Layers][ANA_LEVEL];              
TH2F *hPIXResSigmaMapEaLay[NSCT_Discs][ANA_LEVEL];
TH2F *hPIXResSigmaMapEcLay[NSCT_Discs][ANA_LEVEL];
TH2F *hPIXEtaResMeanMapBaLay[NSCT_Layers][ANA_LEVEL];               // Eta residual maps
TH2F *hPIXEtaResMeanMapEaLay[NSCT_Layers][ANA_LEVEL];               
TH2F *hPIXEtaResMeanMapEcLay[NSCT_Discs][ANA_LEVEL];
TH2F *hPIXEtaResSigmaMapBaLay[NSCT_Layers][ANA_LEVEL];              
TH2F *hPIXEtaResSigmaMapEaLay[NSCT_Layers][ANA_LEVEL];              
TH2F *hPIXEtaResSigmaMapEcLay[NSCT_Discs][ANA_LEVEL];


//SCT Histos
TH1F *hSCTHits[ANA_LEVEL];                          // SCT Hits by track
TH2F *hSCTHitsvEta[ANA_LEVEL];                      // SCT Hits vs Eta
TH1F *hSCTBarrelHits[ANA_LEVEL];                    // SCT Barrel hits by layer
TH1F *hSCTEndcapHits[N_Endcaps][ANA_LEVEL];         // SCT Endcap A o C hits by disc
                                                    // tap=0 Endcap A
                                                    // tap=1 Endcap C
TH1F *hSCTHitsBarrel[ANA_LEVEL];                    // Barrel SCT Hits by track
TH1F *hSCTHitsEndCap[ANA_LEVEL];                    // EndCap SCT Hits by track

TH1F *hSCTRes[ANA_LEVEL];                           // SCT residuals
TH1F *hSCTCorrRes[ANA_LEVEL];                       // SCT residuals corrected
TH1F *hSCTDelzr[ANA_LEVEL];                         // SCT delzr 
TH1F *hSCTResBa[ANA_LEVEL];                         // SCT residuals in barrel
TH1F *hSCTCorrResBa[ANA_LEVEL];                     // SCT corrected residuals in barrel
TH1F *hSCTResBaSide[NSCT_Sides][ANA_LEVEL];         // SCT residuals in barrel by side
TH1F *hSCTCorrResBaSide[NSCT_Sides][ANA_LEVEL];     // SCT corrected residuals in barrel by side

TH1F *hSCTResBaWide[ANA_LEVEL];                     // SCT residuals in barrel (wide)
TH1F *hSCTCorrResBaWide[ANA_LEVEL];                 // SCT corrected residuals in barrel (wide)
TH1F *hSCTPulBa[ANA_LEVEL];                         // SCT pulls in barrel
TH1F *hSCTPulBaWide[ANA_LEVEL];                     // SCT pulls in barrel (wide)
TH1F *hSCTResBaLay[NSCT_Layers][ANA_LEVEL];         // SCT residuals in barrel by layer
TH1F *hSCTCorrResBaLay[NSCT_Layers][ANA_LEVEL];     // SCT corrected residuals in barrel by layer

TH1F *hSCTResBaLaySide[NSCT_Layers][NSCT_Sides][ANA_LEVEL];         // SCT residuals in barrel by layer and side
TH1F *hSCTResBaLaySideWide[NSCT_Layers][NSCT_Sides][ANA_LEVEL];         // SCT residuals in barrel by layer and side (wide)
TH1F *hSCTResBaLayWide[NSCT_Layers][ANA_LEVEL];     // SCT residuals in barrel by layer (wide)
TH1F *hSCTCorrResBaLaySide[NSCT_Layers][NSCT_Sides][ANA_LEVEL];         // SCT corrected residuals in barrel by layer and side
TH1F *hSCTCorrResBaLaySideWide[NSCT_Layers][NSCT_Sides][ANA_LEVEL];         // SCT corrected residuals in barrel by layer and side (wide)
TH1F *hSCTCorrResBaLayWide[NSCT_Layers][ANA_LEVEL];     // SCT corrected residuals in barrel by layer (wide)
TH1F *hSCTErrBaLay[NSCT_Layers][ANA_LEVEL];         // SCT errors in barrel by layer
TH1F *hSCTPulBaLay[NSCT_Layers][ANA_LEVEL];         // SCT pulls in barrel by layer
TH1F *hSCTPulBaLayWide[NSCT_Layers][ANA_LEVEL];     // SCT pulls in barrel by layer (wide)
TProfile *hproSCTMeanResBaLay[ANA_LEVEL];           // SCT Barrel mean residuals by layer
TProfile *hproSCTMeanResBaLaySide[NSCT_Sides][ANA_LEVEL];           // SCT Barrel mean residuals by layer and side
TH1F *hSCTResEndcap[ANA_LEVEL];                     // SCT residuals in endcaps
TH1F *hSCTCorrResEndcap[ANA_LEVEL];                 // SCT corrected residuals in endcaps
TH1F *hSCTResEndcapDisc[N_Endcaps][NSCT_Discs][ANA_LEVEL];      // SCT Residuals in endcaps by disc
TH1F *hSCTResEndcapDiscWide[N_Endcaps][NSCT_Discs][ANA_LEVEL];  // SCT Residuals in endcaps by disc (wide)
TH1F *hSCTResEndcapDiscSide[N_Endcaps][NSCT_Discs][NSCT_Sides][ANA_LEVEL];      // SCT Residuals in endcaps by disc and side
TH1F *hSCTResEndcapDiscSideWide[N_Endcaps][NSCT_Discs][NSCT_Discs][ANA_LEVEL];  // SCT Residuals in endcaps by disc and side (wide)
TH1F *hSCTCorrResEndcapDisc[N_Endcaps][NSCT_Discs][ANA_LEVEL];      // SCT corrected Residuals in endcaps by disc
TH1F *hSCTCorrResEndcapDiscWide[N_Endcaps][NSCT_Discs][ANA_LEVEL];  // SCT corrected Residuals in endcaps by disc (wide)
TH1F *hSCTCorrResEndcapDiscSide[N_Endcaps][NSCT_Discs][NSCT_Sides][ANA_LEVEL];      // SCT corrected Residuals in endcaps by disc and side
TH1F *hSCTCorrResEndcapDiscSideWide[N_Endcaps][NSCT_Discs][NSCT_Discs][ANA_LEVEL];  // SCT corrected Residuals in endcaps by disc and side (wide)
TH1F *hSCTPulEndcap[ANA_LEVEL];                                // SCT pull un endcaps
TH1F *hSCTPulEndcapDisc[N_Endcaps][NSCT_Discs][ANA_LEVEL];      // SCT pulls in endcaps by disc
TH1F *hSCTPulEndcapDiscWide[N_Endcaps][NSCT_Discs][ANA_LEVEL];  // SCT pulls in endcaps by disc (wide)
TProfile *hproSCTMeanResEndcap[N_Endcaps][ANA_LEVEL];           // SCT endcap mean residuals by disc
TProfile *hproSCTMeanResEndcapSide[N_Endcaps][NSCT_Sides][ANA_LEVEL];           // SCT endcap mean residuals by disc and side
TH2F *hSCTHitMapBaLay[NSCT_Layers][ANA_LEVEL];                  // SCT Hit map in barrel by layer
TH2F *hSCTHitMapEndcapDisc[N_Endcaps][NSCT_Discs][ANA_LEVEL];       // SCT Hit map in endcaps by disc
TH1F *hSCTResPlaneSide0[NSCT_Parts][NSCT_Discs][17][56][ANA_LEVEL]; //SCT Residuals by plane, barrel-endcap, layer-disc, ring, sector
TH1F *hSCTResPlaneSide1[NSCT_Parts][NSCT_Discs][17][56][ANA_LEVEL]; //SCT Residuals by plane, barrel-endcap, layer-disc, ring, sector
TH2F *hSCTResMeanMapBaLay[NSCT_Layers][2][ANA_LEVEL];             //SCT Mean barrel residuals by layer and side
TH2F *hSCTResSigmaMapBaLay[NSCT_Layers][2][ANA_LEVEL];            //SCT Mean barrel sigma residuals by layer and side EndCap A
TH2F *hSCTResMeanMapEaLay[NSCT_Discs][2][ANA_LEVEL];             //SCT Mean barrel residuals by layer and side
TH2F *hSCTResSigmaMapEaLay[NSCT_Discs][2][ANA_LEVEL];            //SCT Mean barrel sigma residuals by layer and side EndCap C
TH2F *hSCTResMeanMapEcLay[NSCT_Discs][2][ANA_LEVEL];             //SCT Mean barrel residuals by layer and side
TH2F *hSCTResSigmaMapEcLay[NSCT_Discs][2][ANA_LEVEL];            //SCT Mean barrel sigma residuals by layer and side

// TRT Histos
TH1F *hTRTHits[ANA_LEVEL];

// Trk::Track extras
Int_t iNTrk_Hits;
Int_t iNTrkPIX_Hits;
Int_t iNTrkSCT_Hits;
Int_t iNTrkTRT_Hits;

Int_t iNTrk_sharedHits;
Int_t iNTrkPIX_sharedHits;
Int_t iNTrkSCT_sharedHits;

Int_t iNTrk_holes;
Int_t iNTrkPIX_holes;
Int_t iNTrkSCT_holes;

Float_t iTrk_chi2;
Int_t iTrk_ndof;
Float_t iTrk_chi2Prob;

Int_t NTrk_Hits[MAX_PARTICLES];
Int_t NTrkPIX_Hits[MAX_PARTICLES];
Int_t NTrkSCT_Hits[MAX_PARTICLES];
Int_t NTrkTRT_Hits[MAX_PARTICLES];

Int_t NTrk_sharedHits[MAX_PARTICLES];
Int_t NTrkPIX_sharedHits[MAX_PARTICLES];
Int_t NTrkSCT_sharedHits[MAX_PARTICLES];

Int_t NTrk_holes[MAX_PARTICLES];
Int_t NTrkPIX_holes[MAX_PARTICLES];
Int_t NTrkSCT_holes[MAX_PARTICLES];

Float_t Trk_chi2[MAX_PARTICLES];
Int_t Trk_ndof[MAX_PARTICLES];
Float_t  Trk_chi2Prob[MAX_PARTICLES];

TH1F *hTrkHits[ANA_LEVEL];     // hits by track in Trk::Track
TH1F *hTrkPIXHits[ANA_LEVEL];  // PIX hits by track in Trk::Track
TH1F *hTrkSCTHits[ANA_LEVEL];  // SCT hits by track in Trk::Track
TH1F *hTrkTRTHits[ANA_LEVEL];  // TRT hits by track in Trk::Track

TH1F *hTrksharedHits[ANA_LEVEL];     // shared hits by track in Trk::Track
TH1F *hTrkPIXsharedHits[ANA_LEVEL];  // PIX shared hits by track in Trk::Track
TH1F *hTrkSCTsharedHits[ANA_LEVEL];  // SCT shared hits by track in Trk::Track

TH1F *hTrkholes[ANA_LEVEL];      // holes by track in Trk::Track
TH1F *hTrkPIXholes[ANA_LEVEL];   // PIX holes by track in Trk::Track
TH1F *hTrkSCTholes[ANA_LEVEL];   // SCT holes by track in Trk::Track

TH1F *hTrkChi2[ANA_LEVEL];      // chi2/ndof by track in Trk::Track
/* TH1F *hTrkChi2[ANA_LEVEL];      // chi2 by track in Trk::Track */
/* TH1F *hTrkNDoF[ANA_LEVEL];      // number of degrees of freedom by track in Trk::Track */
/* TH1F *hTrkChi2Prob[ANA_LEVEL];  // chi2 probability by track in Trk::Track */

// Track parameters
// Pt
TH1F *hRecoPt[ANA_LEVEL];                      // Reco Pt
TH1F *hRecoPtAbs[ANA_LEVEL];                   // Reco |Pt|
TH1F *hRecoPtQPlus[ANA_LEVEL];                 // Reco Pt positive charge         
TH1F *hRecoPtQMinus[ANA_LEVEL];                // Reco Pt negative charge
TH2F *hResRecoPtSCTBarrel[NSCT_Layers][ANA_LEVEL]; // Residuals vs recoPt by layer(SCT barrel)

//***************
TH2F *hResRecoPtPIXPhiBarrel[NPIX_Layers][ANA_LEVEL]; // Residuals vs recoPt by layer(PIX Phi barrel)
TH2F *hResRecoPtPIXEtaBarrel[NPIX_Layers][ANA_LEVEL]; // Residuals vs recoPt by layer(PIX Eta barrel)

//PIX EndCaps
TH2F *hResRecoPtPIXPhiEndCap[N_Endcaps][NPIX_Discs][ANA_LEVEL];
TH2F *hResRecoPtPIXEtaEndCap[N_Endcaps][NPIX_Discs][ANA_LEVEL];


//SCT EndCaps
TH2F *hResRecoPtSCTEndCap[N_Endcaps][NSCT_Discs][ANA_LEVEL];
//***************

TH2F *hPIXRes_RecoPhi0[ANA_LEVEL];             // PIX Residuals vs Phi0
TH2F *hPIXRes_RecoZ0[ANA_LEVEL];               // PIX Residuals vs Z0
TH2F *hPIXRes_Ring[ANA_LEVEL];                 // PIX Residuals vs Eta Ring
TH2F *hPIXRes_Sector[ANA_LEVEL];               // PIX Residuals vs Phi Sector
TH2F *hSCTRes_RecoPhi0[ANA_LEVEL];             // SCT Residuals vs Phi0
TH2F *hSCTRes_RecoZ0[ANA_LEVEL];               // SCT Residuals vs Z0
TH2F *hSCTRes_Ring[ANA_LEVEL];                 // SCT Residuals vs Eta Ring
TH2F *hSCTRes_Sector[ANA_LEVEL];               // SCT Residuals vs Phi Sector
TH1F *hRecoP[ANA_LEVEL];                       // Reco P
TH1F *hRecoPAbs[ANA_LEVEL];                    // Reco |P|
TH1F *hTruzPt[ANA_LEVEL];                      // truth Pt
TH1F *hRecoPt_TruzPt[ANA_LEVEL];               // Reco Pt - Truth Pt
TH1F *hRecoQoverPt[ANA_LEVEL];                 // Reco Q over Pt
TH1F *hRecoQoverP[ANA_LEVEL];                  // Reco Q over P
TH1F *hTruzQoverPt[ANA_LEVEL];                 // Truth Q over Pt
TH1F *hRecoQoverPt_TruzQoverPt[ANA_LEVEL];     // Reco Q over Pt - Truth Q over Pt
TH2F *hRecoQoverPt_TruzQoverPt_TruzQoverPt[ANA_LEVEL];  // Reco Q over Pt - Truth Q over Pt vs Truth Q over Pt
TH1F *hRecoQoverPt_TruzQoverPt_QPlus[ANA_LEVEL]; // Reco Q over Pt, positive charge
TH1F *hRecoQoverPt_TruzQoverPt_QMinus[ANA_LEVEL];// Reco Q over Pt, negative charge
// Eta
TH1F *hRecoEta[ANA_LEVEL];                     // Reco eta
TH1F *hTruzEta[ANA_LEVEL];                     // Truth eta
TH1F *hRecoEta_TruzEta[ANA_LEVEL];             // Reco Eta - Truth Eta
// D0
TH1F *hRecoD0[ANA_LEVEL];                      // Reco d0
TH1F *hRecoD0Wide[ANA_LEVEL];                  // Reco d0 (wide)
TH1F *hRecoD0Zoom[ANA_LEVEL];                  // Reco d0 (zoom)
TH1F *hTruzD0[ANA_LEVEL];                      // Truth d0
TH1F *hTruzD0Zoom[ANA_LEVEL];                  // Truth d0 (zoom)
TH1F *hRecoD0_TruzD0[ANA_LEVEL];               // Reco d0 - Truth d0
// d D0
TH1F *hRecodD0[ANA_LEVEL];                     // Reco delta d0
TH1F *hRecodD0Wide[ANA_LEVEL];                 // Reco delta d0 (wide)
TH1F *hRecoD0Pull[ANA_LEVEL];                  // Reco pull d0
TH1F *hRecoD0PullWide[ANA_LEVEL];              // Reco pull d0 (wide)
// d0 significance
TH1F *hRecoD0Signif[ANA_LEVEL];                // Reco d0 significance
TH1F *hRecoD0SignifWide[ANA_LEVEL];            // Reco d0 significance (wide)
// d0 wrt beam spot (as used in GX2 track constraint)
TH1F *hRecoD0bs[ANA_LEVEL];
TH2F *hRecoD0bs_RecoPhi0[ANA_LEVEL];           // reco d0 vs reco phi0
TH2F *hRecoD0bs_RecoEta[ANA_LEVEL];            // reco d0 vs reco eta
TH2F *hRecoD0bs_RecoZ0[ANA_LEVEL];             // reco d0 vs reco eta
TH2F *hRecoD0bs_RecoPt[ANA_LEVEL];             // reco d0 vs reco Pt
TProfile2D *hRecoD0bs_RecoPhi0_RecoZ0[ANA_LEVEL];// reco d0 vs reco phi0 and reco z0
// z0
TH1F *hRecoZ0[ANA_LEVEL];                      // Reco z0
TH1F *hTruzZ0[ANA_LEVEL];                      // Truth z0
TH1F *hRecoZ0_TruzZ0[ANA_LEVEL];               // Reco z0 - Truth z0
TH1F *hRecodZ0[ANA_LEVEL];                     // Error of Reco z0
// phi0
TH1F *hRecoPhi0[ANA_LEVEL];                    // Reco phi0
TH1F *hTruzPhi0[ANA_LEVEL];                    // TRuth phi0
TH1F *hRecoPhi0_TruzPhi0[ANA_LEVEL];           // Reco phi0 - Truth phi0
// Theta
TH1F *hRecoTheta[ANA_LEVEL];                   // Reco theta
TH1F *hTruzTheta[ANA_LEVEL];                   // Truth theta
TH1F *hRecoTheta_TruzTheta[ANA_LEVEL];         // Reco theta - Truth theta
// beam spot
TH2F *hRecoY0_RecoX0[ANA_LEVEL];               // reco y0 vs reco x0
TH1F *hRecoX0[ANA_LEVEL];                      // reco x0
TH1F *hRecoY0[ANA_LEVEL];                      // reco y0
TH2F *hTruzY0_TruzX0[ANA_LEVEL];               // truth y0 vs truth x0
TH2F *hRecoD0_RecoPhi0[ANA_LEVEL];             // reco d0 vs reco phi0
TH2F *hRecoD0_RecoPhi0_Narrow[ANA_LEVEL];      // reco d0 vs reco phi0 (narrow)
TProfile2D *hRecoD0_RecoPhi0_RecoZ0[ANA_LEVEL];// reco d0 vs reco phi0 and reco z0
TH2F *hRecoD0Pull_RecoPhi0[ANA_LEVEL];         // reco d0 pull vs reco phi0
TH2F *hRecodD0_RecoPhi0[ANA_LEVEL];            // reco d0 error vs reco phi0
TH2F *hRecodD0_RecoPhi0_Narrow[ANA_LEVEL];     // reco d0 error vs reco phi0 (narrow)
TH2F *hRecodD0_RecoPt[ANA_LEVEL];              // reco d0 error vs reco pt
TH2F *hRecodZ0_RecoPhi0[ANA_LEVEL];            // reco z0 error vs reco phi0
TH2F *hRecodZ0_RecoEta[ANA_LEVEL];            // reco z0 error vs reco eta
// impact parameter
TH2F *hRecoZ0_RecoEta[ANA_LEVEL];              // reco z0 vs reco eta
TH2F *hRecoZ0_RecoTheta[ANA_LEVEL];            // reco z0 vs reco theta
// Reco pt, Q over pt vs other
TH2F *hRecoPt_RecoEta[ANA_LEVEL];              // reco pt vs reco eta
TH2F *hRecoQoverPt_RecoEta[ANA_LEVEL];         // reco Q over pt vs reco eta
TH2F *hRecoPt_RecoPhi0[ANA_LEVEL];             // reco pt vs reco phi0
TH2F *hRecoPt_RecoPhi0_QPlus[ANA_LEVEL];       // reco pt vs reco phi0 - positive Q
TH2F *hRecoPt_RecoPhi0_QMinus[ANA_LEVEL];      // reco pt vs reco phi0 - negative Q
TH2F *hRecoPhi0_RecoPt[ANA_LEVEL];             // reco phi0 vs reco pt (cilindrical coord)
TH2F *hRecoQoverPt_RecoPhi0[ANA_LEVEL];        // reco Q over Pt vs reco phi0
TH2F *hRecoQoverPt_RecoPhi0_Pt10[ANA_LEVEL];   // reco Q over Pt (9<|Pt|<11)
TH2F *hRecoQoverPt_RecoPhi0_QPlus[ANA_LEVEL];  // reco Q over Pt vs reco phi0 - positive Q
TH2F *hRecoQoverPt_RecoPhi0_QMinus[ANA_LEVEL]; // reco Q over Pt vs reco phi0 - negative Q
TH1F *hRecoThetaFwd[ANA_LEVEL];                // reco theta forward
TH1F *hRecoThetaBwd[ANA_LEVEL];                // reco theta backward
// Chi2
TH1F *hCHI2[ANA_LEVEL];                        // Chi2
TH1F *hRecoChi2[ANA_LEVEL];                    // reco chi2 (same??)
TH1F *hRecoChi2Wide[ANA_LEVEL];                // reco chi2 wide range
TH2F *hRecoChi2_RecoPhi0[ANA_LEVEL];           // reco chi2 vs reco phi0
TH2F *hRecoChi2_RecoEta[ANA_LEVEL];            // reco chi2 vs reco eta0
TH2F *hRecoChi2_RecoPt[ANA_LEVEL];             // reco chi2 vs reco pT
TProfile *pRecoChi2_Res[ANA_LEVEL];            // reco chi2 vs # residuals in track
TH1F *pRecoChi2_Event[ANA_LEVEL];              // reco chi2 vs event
TProfile *pRecoChi2_Tracks[ANA_LEVEL];         // reco chi2 vs tracks in event

// TrkTrack Branch
// Pt
TH1F *hTRecoPt[ANA_LEVEL];                      // Reco Pt
TH1F *hTRecoPtAbs[ANA_LEVEL];                   // Reco |Pt|
TH1F *hTRecoPtQPlus[ANA_LEVEL];                 // Reco Pt positive charge         
TH1F *hTRecoPtQMinus[ANA_LEVEL];                // Reco Pt negative charge

TH1F *hTRecoP[ANA_LEVEL];                       // Reco P
TH1F *hTRecoPAbs[ANA_LEVEL];                    // Reco |P|
TH1F *hTTruzPt[ANA_LEVEL];                      // truth Pt
TH1F *hTRecoPt_TruzPt[ANA_LEVEL];               // Reco Pt - Truth Pt
TH1F *hTRecoQoverPt[ANA_LEVEL];                 // Reco Q over Pt
TH1F *hTRecoQoverP[ANA_LEVEL];                  // Reco Q over P
TH1F *hTTruzQoverPt[ANA_LEVEL];                 // Truth Q over Pt
TH1F *hTRecoQoverPt_TruzQoverPt[ANA_LEVEL];     // Reco Q over Pt - Truth Q over Pt
TH2F *hTRecoQoverPt_TruzQoverPt_TruzQoverPt[ANA_LEVEL];  // Reco Q over Pt - Truth Q over Pt vs Truth Q over Pt
TH1F *hTRecoQoverPt_TruzQoverPt_QPlus[ANA_LEVEL]; // Reco Q over Pt, positive charge
TH1F *hTRecoQoverPt_TruzQoverPt_QMinus[ANA_LEVEL];// Reco Q over Pt, negative charge
// Eta
TH1F *hTRecoEta[ANA_LEVEL];                     // Reco eta
TH1F *hTTruzEta[ANA_LEVEL];                     // Truth eta
TH1F *hTRecoEta_TruzEta[ANA_LEVEL];             // Reco Eta - Truth Eta
// D0
TH1F *hTRecoD0[ANA_LEVEL];                      // Reco d0
TH1F *hTRecoD0Wide[ANA_LEVEL];                  // Reco d0 (wide)
TH1F *hTRecoD0Zoom[ANA_LEVEL];                  // Reco d0 (zoom)
TH1F *hTTruzD0[ANA_LEVEL];                      // Truth d0
TH1F *hTTruzD0Zoom[ANA_LEVEL];                  // Truth d0 (zoom)
TH1F *hTRecoD0_TruzD0[ANA_LEVEL];               // Reco d0 - Truth d0
// z0
TH1F *hTRecoZ0[ANA_LEVEL];                      // Reco z0
TH1F *hTTruzZ0[ANA_LEVEL];                      // Truth z0
TH1F *hTRecoZ0_TruzZ0[ANA_LEVEL];               // Reco z0 - Truth z0
// phi0
TH1F *hTRecoPhi0[ANA_LEVEL];                    // Reco phi0
TH1F *hTTruzPhi0[ANA_LEVEL];                    // TRuth phi0
TH1F *hTRecoPhi0_TruzPhi0[ANA_LEVEL];           // Reco phi0 - Truth phi0
// Theta
TH1F *hTRecoTheta[ANA_LEVEL];                   // Reco theta
TH1F *hTTruzTheta[ANA_LEVEL];                   // Truth theta
TH1F *hTRecoTheta_TruzTheta[ANA_LEVEL];         // Reco theta - Truth theta

// Vertex candidate
TH1F *hVtxX;                                   // Vertex candidate X
TH1F *hVtxY;                                   // Vertex candidate Y
TH1F *hVtxZ;                                   // Vertex candidate Z
TH1F *hVtxZoomX;                               // Vertex candidate X (Zoom)
TH1F *hVtxZoomY;                               // Vertex candidate Y (Zoom)
TH2F *hVtxX_VtxY;                              // Vertex candidate X vs Y
TH2F *hVtxX_VtxZ;                              // Vertex candidate X vs Z
TH2F *hVtxY_VtxZ;                              // Vertex candidate Y vs Z
TH2F *hZoomVtxX_VtxY;                          // Vertex candidate X vs Y (Zoom)
TH2F *hZoomVtxX_VtxZ;                          // Vertex candidate X vs Z (Zoom)
TH2F *hZoomVtxY_VtxZ;                          // Vertex candidate Y vs Z (Zoom)

TH1F *hTrkVtxX;                                // Production Vertex X
TH1F *hTrkVtxY;                                // Production Vertex Y
TH1F *hTrkVtxZ;                                // Production Vertex Z
TH2F *hTrkVtxXVtxY;                            // Production Vertex X Vs Y
TH2F *hTrkVtxXVtxZ;                            // Production Vertex X Vs Z
TH2F *hTrkVtxYVtxZ;                            // Production Vertex Y Vs Z

TH1F *hGenParticlePt;                          // generated signed pT
TH1F *hGenParticleEta;                         // generated eta
TH1F *hGenParticlePhi;                         // generated phi0
TH2F *hGenParticlePhiVsEta;                    // generated Phi Vs Eta
TH2F *hGenParticlePtVsPhi;                     // generated Pt Vs Phi
TH2F *hGenParticlePtVsEta;                     // generated Pt Vs Eta

// mass analisys
TH1F *hMuonPairMass[ANA_LEVEL];                // Muon pair reco invariant mass
TH1F *hMuonPairDPhi[ANA_LEVEL];                // Muon pair delta phi
TH1F *hMuonPairPt[ANA_LEVEL];                  // Muon pair reco pt
TH1F *hMuonRecoPt[ANA_LEVEL];                  // Muon reco pt
TH1F *hMuonPlusRecoPt[ANA_LEVEL];
TH1F *hMuonMinusRecoPt[ANA_LEVEL];
TH1F *hMuonRecoD0[ANA_LEVEL];
TH1F *hMuonDeltaRecoD0[ANA_LEVEL];

// histos del analisi H->ZZ-l+l-l+l-
TH1F *hHZ4LMass[ANA_LEVEL];
TH1F *hHZ4L_4LPt[ANA_LEVEL];
TH1F *hHZ4L_ZMass[ANA_LEVEL];
TH1F *hHZ4L_VMass[ANA_LEVEL];
TH1F *hHZ4L_RV_Z_DPhi[ANA_LEVEL];

// CDFfit - L1
TH2F *hCDFBigMat[ANA_LEVEL];            // big matrix
TH1F *hCDFBigVec[ANA_LEVEL];            // big vector

// SiCluter ntuple
TH1F *hSCTNClus[ANA_LEVEL];
TProfile *pSCTClusSize[NSCT_Layers][ANA_LEVEL];         
TProfile *pSCTClusSizeIncAngle[ANA_LEVEL];         

// Track Matching ntuple

TH1F *hMatchingD0[ANA_LEVEL];
TH1F *hMatchingPhi0[ANA_LEVEL];
TH1F *hMatchingEta[ANA_LEVEL];
TH1F *hMatchingTheta0[ANA_LEVEL];
TH1F *hMatchingZ0[ANA_LEVEL];
TH1F *hMatchingQoverPt[ANA_LEVEL];
TH1F *hMatchingPt[ANA_LEVEL];
TH1F *hMatchingCharge[ANA_LEVEL];
// Vs d0
TH2F *hMatchingD0VsD0[ANA_LEVEL];
TH2F *hMatchingZ0VsD0[ANA_LEVEL];
TH2F *hMatchingPhi0VsD0[ANA_LEVEL];
TH2F *hMatchingTheta0VsD0[ANA_LEVEL];
TH2F *hMatchingEtaVsD0[ANA_LEVEL];
TH2F *hMatchingQoverPtVsD0[ANA_LEVEL];
// Vs z0
TH2F *hMatchingD0VsZ[ANA_LEVEL];
TH2F *hMatchingZ0VsZ[ANA_LEVEL];
TH2F *hMatchingPhi0VsZ[ANA_LEVEL];
TH2F *hMatchingTheta0VsZ[ANA_LEVEL];
TH2F *hMatchingEtaVsZ[ANA_LEVEL];
TH2F *hMatchingQoverPtVsZ[ANA_LEVEL];
//Vs Phi0
TH2F *hMatchingD0VsPhi0[ANA_LEVEL];
TH2F *hMatchingZ0VsPhi0[ANA_LEVEL];
TH2F *hMatchingPhi0VsPhi0[ANA_LEVEL];
TH2F *hMatchingTheta0VsPhi0[ANA_LEVEL];
TH2F *hMatchingEtaVsPhi0[ANA_LEVEL];
TH2F *hMatchingQoverPtVsPhi0[ANA_LEVEL];
//Vs Eta
TH2F *hMatchingD0VsEta[ANA_LEVEL];
TH2F *hMatchingZ0VsEta[ANA_LEVEL];
TH2F *hMatchingPhi0VsEta[ANA_LEVEL];
TH2F *hMatchingTheta0VsEta[ANA_LEVEL];
TH2F *hMatchingEtaVsEta[ANA_LEVEL];
TH2F *hMatchingQoverPtVsEta[ANA_LEVEL];
//QoverPt
TH2F *hMatchingD0VsQoverPt[ANA_LEVEL];
TH2F *hMatchingZ0VsQoverPt[ANA_LEVEL];
TH2F *hMatchingPhi0VsQoverPt[ANA_LEVEL];
TH2F *hMatchingTheta0VsQoverPt[ANA_LEVEL];
TH2F *hMatchingEtaVsQoverPt[ANA_LEVEL];
TH2F *hMatchingQoverPtVsQoverPt[ANA_LEVEL];

// Beam Spot
TH1F *hBeamSpotX[ANA_LEVEL];
TH1F *hBeamSpotY[ANA_LEVEL];
TH1F *hBeamSpotZ[ANA_LEVEL];
TH1F *hBeamSpotTiltX[ANA_LEVEL];
TH1F *hBeamSpotTiltY[ANA_LEVEL];
TH1F *hLumiBlockID[ANA_LEVEL];



//TH1F* histo;
void ResFit(TH1F*);

///////////////////////////////////////
// declaration
///////////////////////////////////////
void trkana_init(int, int);
void trkana_init_branches();
void trkana_init_branches();
void trkana_init_branches_CBNT();
void trkana_init_branches_TrkVal();
void trkana_init_branches_Richard();
void trkana_init_branches_GlobalX2();
void trkana_init_branches_SiCluster();
void trkana_init_branches_TrkMatching();
void trkana_init_branches_Summary();
void trkana_init_branches_TrkTrack();
void trkana_init_stat();
void trkana_graf_style();
void trkana_init_file();
void trkana_init_histos();
void trkana_init_histos_BeamSpot(int);
void trkana_init_histos_CDFFit(int);
void trkana_init_histos_PIXandSCT(int);
void trkana_init_histos_PIX(int);
void trkana_init_histos_SCT(int);
void trkana_init_histos_SiCluster(int);
void trkana_init_histos_TrkMatching(int);
void trkana_init_histos_track(int);
void trkana_init_histos_TRT(int);
void trkana_init_histos_Zmass(int);
void trkana_init_histos_Hmass(int);
void trkana_init_histos_Vtx();
void trkana_init_histos_time(int);
Int_t trkana_sct_ring_from_int(int);
void trkana_loop();
void trkana_summary_loop();
void trkana_get_entry(int);
void trkana_event_like_analysis(int);
void trkana_event_like_track_analysis(int);
void trkana_event_like_trktrack_analysis(int);
void trkana_track_analysis();
void trkana_track_analysis_TrkVal();
void trkana_track_analysis_Richard();
void trkana_compute_track_vars();
void trkana_track_reset();
void trkana_event_reset();
void trkana_fill_histos();
void trkana_fill_histos_BeamSpot(int);
void trkana_fill_histos_cdffit_l1(int);
void trkana_fill_TrkTrack_histos();
void trkana_fill_SiCluster_histos();
void trkana_fill_event_histos();
void trkana_fill_TrkTrack_event_histos();
void trkana_fill_pass(int);
void trkana_fill_TrkTrack_pass(int);
void trkana_fill_SiCluster_pass(int);
void trkana_end();
void trkana_fit_residuals();
void trkana_fit_track_matching();
void trkana_mean_res();
void trkana_print_stat();
void trkana_def_track_4mom(const int);
void trkana_perform_H_4l_analysis();
void trkana_event_analysis();
void trkana_ompli_mapa_residus();
void trkana_track_dump();

void trkana_canvas_divide(TCanvas *);
void trkana_compute_track_vars();

void trkana_draw();
void trkana_draw_Chi2();
void trkana_draw_hitmap();
void trkana_draw_hits();
void trkana_draw_histo(TH1F* Histo[]);
void trkana_draw_histo2(TH2F* Histo[]);
void trkana_draw_histo2p(TH2F* Histo[]);
void trkana_draw_zmassana();
void trkana_draw_hmassana();
void trkana_draw_SCTHitErr();
void trkana_draw_SCTpull();
void trkana_draw_SCTres();
void trkana_draw_PIXresEta();
void trkana_draw_PIXresEta_Err();
void trkana_draw_PIXresEta_Pul();
void trkana_draw_PIXresPhi();
void trkana_draw_PIXresPhi_Err();
void trkana_draw_PIXresPhi_Pul();
void trkana_draw_PtViews();
void trkana_draw_PtViewsEta();
void trkana_draw_residuals();
void trkana_draw_residus_map(int);
void trkana_draw_recod0();
void trkana_draw_recotrk();
void trkana_draw_syst(TCanvas *, int, int, TH1F* Histo1[], TH1F* Histo2[]);
void trkana_draw_truztrk();
void trkana_draw_rectrk_trutrk();
void trkana_draw_SCT_disc_pulls();
void trkana_draw_more_residuals();
void trkana_draw_more_hits();
void trkana_event_display();
void trkana_EigenAna(const Char_t *);

void trkana_mean_res();
void trkana_draw_pro(TProfile* Profile[]);

void trkana_next_canvas(TCanvas *, int, int);

void SetMaximumOfHitmaps(TH2F *histo[][ANA_LEVEL], const int, const int);

void SCTMaps_SetXaxisLabels(TH2F *);

void checkForOldNtuples();
Int_t GetPrintLevel();
void  SetPrintLevel(Int_t);

// Canvas
TCanvas *CanTrkAna;
TCanvas *CanRecoTrk;
TCanvas *CanTruzTrk;
TCanvas *CanRecoD0; 
TCanvas *CanRecoBS;
TCanvas *CanHits;
TCanvas *CanRes;
TCanvas *CanResWide;
TCanvas *CanSCTRes;
TCanvas *CanSCTResWide;
TCanvas *CanSCTHitErr;
TCanvas *CanSCTPull;
TCanvas *CanSCTPullWide;
TCanvas *CanSCTResEndcapA;
TCanvas *CanSCTResEndcapAWide;
TCanvas *CanSCTResEndcapC;
TCanvas *CanSCTResEndcapCWide;
TCanvas *CanPIXPhiRes;
TCanvas *CanPIXPhiResWide;
TCanvas *CanPIXPhiResError;
TCanvas *CanPIXPhiPul;
TCanvas *CanPIXPhiPulWide;
TCanvas *CanPIXEtaRes;
TCanvas *CanPIXEtaResWide;
TCanvas *CanPIXEtaResError;
TCanvas *CanPIXEtaPul;
TCanvas *CanPIXEtaPulWide;
TCanvas *CanReco_Truz; 
TCanvas *CanPIXHitMapBa;
TCanvas *CanPIXHitMapEc;
TCanvas *CanSCTHitMapBa;
TCanvas *CanSCTHitMapECA;
TCanvas *CanSCTHitMapECC;
TCanvas *CanBarrelEndcapHits;

TCanvas *CanChi2;
TCanvas *CanSystPt;
TCanvas *CanSystTheta;
TCanvas *CanPtvPhi;
TCanvas *CanPtEta;
TCanvas *CanQoPtEta;
TCanvas *CanQPtPhi0;
TCanvas *CanQoPtPhi0;
TCanvas *CanD0Phi0;
TCanvas *CandD0Phi0;
TCanvas *CandD0Phi0N;

TCanvas *CanMassAna;
TCanvas *CanHZ4LAna;

TCanvas *CanTotalRes;
TCanvas *CanTotalResBE;
TCanvas *CanResLayer;
TCanvas *CanSCTResMeanMap[ANA_LEVEL];
TCanvas *CanSCTResSigmaMap[ANA_LEVEL];
TCanvas *CanSCTPullEndcapA;
TCanvas *CanSCTPullEndcapC;
//////////////////////////
