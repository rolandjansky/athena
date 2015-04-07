/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct  6 06:14:07 2011 by ROOT version 5.28/00e
// from TChain POOLCollectionTree/
//////////////////////////////////////////////////////////

#ifndef HITAGprinter_h
#define HITAGprinter_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class HITAGprinter {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Char_t          StreamESD_ref[153];
   Char_t          StreamRAW_ref[137];
   UInt_t          RunNumber;
   UInt_t          EventNumber;
   UInt_t          EventTime;
   UInt_t          EventTimeNanoSec;
   UInt_t          LumiBlockN;
   UInt_t          BunchId;
   UInt_t          PixelFlags;
   UInt_t          SCTFlags;
   UInt_t          TRTFlags;
   UInt_t          LArFlags;
   UInt_t          TileFlags;
   UInt_t          MuonFlags;
   UInt_t          ForwardDetFlags;
   UInt_t          CoreFlags;
   Char_t          IsSimulation;
   Char_t          IsCalibration;
   Char_t          IsTestBeam;
   Float_t         RandomNumber;
   UInt_t          NTrk;
   UInt_t          NVtx;
   Float_t         VtxX;
   Float_t         VtxY;
   Float_t         VtxZ;
   Float_t         VtxChiSq;
   UInt_t          VtxNDOF;
   Int_t           NVtxTight;
   Float_t         LooseElectronEta1;
   Float_t         LooseElectronEta2;
   Float_t         LooseElectronEta3;
   Float_t         LooseElectronEta4;
   Char_t          LooseElectronIsForward1;
   Char_t          LooseElectronIsForward2;
   Char_t          LooseElectronIsForward3;
   Char_t          LooseElectronIsForward4;
   Float_t         LooseElectronPhi1;
   Float_t         LooseElectronPhi2;
   Float_t         LooseElectronPhi3;
   Float_t         LooseElectronPhi4;
   Float_t         LooseElectronPt1;
   Float_t         LooseElectronPt2;
   Float_t         LooseElectronPt3;
   Float_t         LooseElectronPt4;
   UInt_t          LooseElectronTightness1;
   UInt_t          LooseElectronTightness2;
   UInt_t          LooseElectronTightness3;
   UInt_t          LooseElectronTightness4;
   UInt_t          NLooseElectron;
   Float_t         LoosePhotonEta1;
   Float_t         LoosePhotonEta2;
   Float_t         LoosePhotonPhi1;
   Float_t         LoosePhotonPhi2;
   Float_t         LoosePhotonPt1;
   Float_t         LoosePhotonPt2;
   UInt_t          LoosePhotonTightness1;
   UInt_t          LoosePhotonTightness2;
   UInt_t          NConvertedLoosePhoton;
   UInt_t          NLoosePhoton;
   Int_t           LooseMuonCSCHits1;
   Int_t           LooseMuonCSCHits2;
   Int_t           LooseMuonCSCHits3;
   Int_t           LooseMuonCSCHits4;
   Float_t         LooseMuonEta1;
   Float_t         LooseMuonEta2;
   Float_t         LooseMuonEta3;
   Float_t         LooseMuonEta4;
   Float_t         LooseMuonIsolEt1;
   Float_t         LooseMuonIsolEt2;
   Float_t         LooseMuonIsolEt3;
   Float_t         LooseMuonIsolEt4;
   UInt_t          LooseMuonIsolN1;
   UInt_t          LooseMuonIsolN2;
   UInt_t          LooseMuonIsolN3;
   UInt_t          LooseMuonIsolN4;
   Int_t           LooseMuonMDTHits1;
   Int_t           LooseMuonMDTHits2;
   Int_t           LooseMuonMDTHits3;
   Int_t           LooseMuonMDTHits4;
   Float_t         LooseMuonPhi1;
   Float_t         LooseMuonPhi2;
   Float_t         LooseMuonPhi3;
   Float_t         LooseMuonPhi4;
   Float_t         LooseMuonPt1;
   Float_t         LooseMuonPt2;
   Float_t         LooseMuonPt3;
   Float_t         LooseMuonPt4;
   Int_t           LooseMuonRPCHits1;
   Int_t           LooseMuonRPCHits2;
   Int_t           LooseMuonRPCHits3;
   Int_t           LooseMuonRPCHits4;
   Int_t           LooseMuonTGCHits1;
   Int_t           LooseMuonTGCHits2;
   Int_t           LooseMuonTGCHits3;
   Int_t           LooseMuonTGCHits4;
   UInt_t          LooseMuonTightness1;
   UInt_t          LooseMuonTightness2;
   UInt_t          LooseMuonTightness3;
   UInt_t          LooseMuonTightness4;
   UInt_t          NLooseMuon;
   Float_t         BJetLikelihood1;
   Float_t         BJetLikelihood2;
   Float_t         BJetLikelihood3;
   Float_t         BJetLikelihood4;
   Float_t         BJetLikelihood5;
   Float_t         BJetLikelihood6;
   Float_t         JetEmFrac1;
   Float_t         JetEmFrac2;
   Float_t         JetEmFrac3;
   Float_t         JetEmFrac4;
   Float_t         JetEmFrac5;
   Float_t         JetEmFrac6;
   Float_t         JetEta1;
   Float_t         JetEta2;
   Float_t         JetEta3;
   Float_t         JetEta4;
   Float_t         JetEta5;
   Float_t         JetEta6;
   Float_t         JetPhi1;
   Float_t         JetPhi2;
   Float_t         JetPhi3;
   Float_t         JetPhi4;
   Float_t         JetPhi5;
   Float_t         JetPhi6;
   Float_t         JetPt1;
   Float_t         JetPt2;
   Float_t         JetPt3;
   Float_t         JetPt4;
   Float_t         JetPt5;
   Float_t         JetPt6;
   Float_t         JetSumET;
   UInt_t          NBJet;
   UInt_t          NJet;
   UInt_t          BunchGroup;
   ULong64_t       EFPassedTrigMask0;
   ULong64_t       EFPassedTrigMask1;
   ULong64_t       EFPassedTrigMask10;
   ULong64_t       EFPassedTrigMask11;
   ULong64_t       EFPassedTrigMask12;
   ULong64_t       EFPassedTrigMask13;
   ULong64_t       EFPassedTrigMask14;
   ULong64_t       EFPassedTrigMask15;
   ULong64_t       EFPassedTrigMask16;
   ULong64_t       EFPassedTrigMask17;
   ULong64_t       EFPassedTrigMask18;
   ULong64_t       EFPassedTrigMask19;
   ULong64_t       EFPassedTrigMask2;
   ULong64_t       EFPassedTrigMask20;
   ULong64_t       EFPassedTrigMask21;
   ULong64_t       EFPassedTrigMask22;
   ULong64_t       EFPassedTrigMask23;
   ULong64_t       EFPassedTrigMask24;
   ULong64_t       EFPassedTrigMask25;
   ULong64_t       EFPassedTrigMask26;
   ULong64_t       EFPassedTrigMask27;
   ULong64_t       EFPassedTrigMask28;
   ULong64_t       EFPassedTrigMask29;
   ULong64_t       EFPassedTrigMask3;
   ULong64_t       EFPassedTrigMask30;
   ULong64_t       EFPassedTrigMask31;
   ULong64_t       EFPassedTrigMask4;
   ULong64_t       EFPassedTrigMask5;
   ULong64_t       EFPassedTrigMask6;
   ULong64_t       EFPassedTrigMask7;
   ULong64_t       EFPassedTrigMask8;
   ULong64_t       EFPassedTrigMask9;
   UInt_t          L1PassedTrigMaskTAP0;
   UInt_t          L1PassedTrigMaskTAP1;
   UInt_t          L1PassedTrigMaskTAP2;
   UInt_t          L1PassedTrigMaskTAP3;
   UInt_t          L1PassedTrigMaskTAP4;
   UInt_t          L1PassedTrigMaskTAP5;
   UInt_t          L1PassedTrigMaskTAP6;
   UInt_t          L1PassedTrigMaskTAP7;
   UInt_t          L1PassedTrigMaskTAV0;
   UInt_t          L1PassedTrigMaskTAV1;
   UInt_t          L1PassedTrigMaskTAV2;
   UInt_t          L1PassedTrigMaskTAV3;
   UInt_t          L1PassedTrigMaskTAV4;
   UInt_t          L1PassedTrigMaskTAV5;
   UInt_t          L1PassedTrigMaskTAV6;
   UInt_t          L1PassedTrigMaskTAV7;
   UInt_t          L1PassedTrigMaskTBP0;
   UInt_t          L1PassedTrigMaskTBP1;
   UInt_t          L1PassedTrigMaskTBP2;
   UInt_t          L1PassedTrigMaskTBP3;
   UInt_t          L1PassedTrigMaskTBP4;
   UInt_t          L1PassedTrigMaskTBP5;
   UInt_t          L1PassedTrigMaskTBP6;
   UInt_t          L1PassedTrigMaskTBP7;
   UInt_t          L2PassedTrigMask0;
   UInt_t          L2PassedTrigMask1;
   UInt_t          L2PassedTrigMask10;
   UInt_t          L2PassedTrigMask11;
   UInt_t          L2PassedTrigMask12;
   UInt_t          L2PassedTrigMask13;
   UInt_t          L2PassedTrigMask14;
   UInt_t          L2PassedTrigMask15;
   UInt_t          L2PassedTrigMask16;
   UInt_t          L2PassedTrigMask17;
   UInt_t          L2PassedTrigMask18;
   UInt_t          L2PassedTrigMask19;
   UInt_t          L2PassedTrigMask2;
   UInt_t          L2PassedTrigMask20;
   UInt_t          L2PassedTrigMask21;
   UInt_t          L2PassedTrigMask22;
   UInt_t          L2PassedTrigMask23;
   UInt_t          L2PassedTrigMask24;
   UInt_t          L2PassedTrigMask25;
   UInt_t          L2PassedTrigMask26;
   UInt_t          L2PassedTrigMask27;
   UInt_t          L2PassedTrigMask28;
   UInt_t          L2PassedTrigMask29;
   UInt_t          L2PassedTrigMask3;
   UInt_t          L2PassedTrigMask30;
   UInt_t          L2PassedTrigMask31;
   UInt_t          L2PassedTrigMask4;
   UInt_t          L2PassedTrigMask5;
   UInt_t          L2PassedTrigMask6;
   UInt_t          L2PassedTrigMask7;
   UInt_t          L2PassedTrigMask8;
   UInt_t          L2PassedTrigMask9;
   UInt_t          Level1TriggerType;
   UInt_t          SFOStreams;
   Float_t         TrackLead1D0;
   Float_t         TrackLead1Z0;
   Float_t         TrackLead1Phi0;
   Float_t         TrackLead1Theta;
   Float_t         TrackLead1QOverP;
   Float_t         TrackLead2D0;
   Float_t         TrackLead2Z0;
   Float_t         TrackLead2Phi0;
   Float_t         TrackLead2Theta;
   Float_t         TrackLead2QOverP;
   Float_t         CellEnergySum;
   Float_t         CellEnergySumEMB;
   Float_t         CellEnergySumEMEC;
   Float_t         CellEnergySumHEC;
   Float_t         CellEnergySumFCAL;
   Float_t         CellEnergySumTile;
   Float_t         ClusterEnergySum;
   Float_t         TopoClusterEt1;
   Float_t         TopoClusterEta1;
   Float_t         TopoClusterPhi1;
   Float_t         CellMissingET;
   Float_t         CellMissingETPhi;
   Float_t         MBTSTimeDiff;
   Float_t         LArECTimeDiff;
   Float_t         TRTEventPhase;
   Int_t           NPixelTracks;
   Int_t           NSCTTracks;
   Int_t           NTRTTracks;
   Int_t           NMooreSegs;
   Int_t           NMboySegs;
   Int_t           NInnerMboySegs;
   Int_t           NHitsInnerMboySegs;
   Int_t           NInnerMuSegs;
   Int_t           NHitsInnerMooreSegs;
   Int_t           NSCTSPs;
   Int_t           NPixelSPs;
   Int_t           NTRTDCs;
   Int_t           NTRTHtDCs;
   Int_t           NMDTHits;
   Int_t           NRPCHits;
   Int_t           NTGCHits;
   Int_t           NCSCHits;
   Int_t           NBCMHits;
   UInt_t          MBTSWord;
   UInt_t          BPhysWord;
   UInt_t          CombinedMuonWord;
   UInt_t          DAODStreams;
   UInt_t          DESDStreams;
   UInt_t          EgammaWord;
   UInt_t          ExoticWord;
   UInt_t          HeavyIonWord;
   UInt_t          HiggsWord;
   UInt_t          JetMissingETWord;
   UInt_t          JetTagWord;
   UInt_t          SMWord;
   UInt_t          SUSYWord;
   UInt_t          TauIdWord;
   UInt_t          TopWord;

   // List of branches
   TBranch        *b_StreamESD_ref;   //!
   TBranch        *b_StreamRAW_ref;   //!
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_EventTime;   //!
   TBranch        *b_EventTimeNanoSec;   //!
   TBranch        *b_LumiBlockN;   //!
   TBranch        *b_BunchId;   //!
   TBranch        *b_PixelFlags;   //!
   TBranch        *b_SCTFlags;   //!
   TBranch        *b_TRTFlags;   //!
   TBranch        *b_LArFlags;   //!
   TBranch        *b_TileFlags;   //!
   TBranch        *b_MuonFlags;   //!
   TBranch        *b_ForwardDetFlags;   //!
   TBranch        *b_CoreFlags;   //!
   TBranch        *b_IsSimulation;   //!
   TBranch        *b_IsCalibration;   //!
   TBranch        *b_IsTestBeam;   //!
   TBranch        *b_RandomNumber;   //!
   TBranch        *b_NTrk;   //!
   TBranch        *b_NVtx;   //!
   TBranch        *b_VtxX;   //!
   TBranch        *b_VtxY;   //!
   TBranch        *b_VtxZ;   //!
   TBranch        *b_VtxChiSq;   //!
   TBranch        *b_VtxNDOF;   //!
   TBranch        *b_NVtxTight;   //!
   TBranch        *b_LooseElectronEta1;   //!
   TBranch        *b_LooseElectronEta2;   //!
   TBranch        *b_LooseElectronEta3;   //!
   TBranch        *b_LooseElectronEta4;   //!
   TBranch        *b_LooseElectronIsForward1;   //!
   TBranch        *b_LooseElectronIsForward2;   //!
   TBranch        *b_LooseElectronIsForward3;   //!
   TBranch        *b_LooseElectronIsForward4;   //!
   TBranch        *b_LooseElectronPhi1;   //!
   TBranch        *b_LooseElectronPhi2;   //!
   TBranch        *b_LooseElectronPhi3;   //!
   TBranch        *b_LooseElectronPhi4;   //!
   TBranch        *b_LooseElectronPt1;   //!
   TBranch        *b_LooseElectronPt2;   //!
   TBranch        *b_LooseElectronPt3;   //!
   TBranch        *b_LooseElectronPt4;   //!
   TBranch        *b_LooseElectronTightness1;   //!
   TBranch        *b_LooseElectronTightness2;   //!
   TBranch        *b_LooseElectronTightness3;   //!
   TBranch        *b_LooseElectronTightness4;   //!
   TBranch        *b_NLooseElectron;   //!
   TBranch        *b_LoosePhotonEta1;   //!
   TBranch        *b_LoosePhotonEta2;   //!
   TBranch        *b_LoosePhotonPhi1;   //!
   TBranch        *b_LoosePhotonPhi2;   //!
   TBranch        *b_LoosePhotonPt1;   //!
   TBranch        *b_LoosePhotonPt2;   //!
   TBranch        *b_LoosePhotonTightness1;   //!
   TBranch        *b_LoosePhotonTightness2;   //!
   TBranch        *b_NConvertedLoosePhoton;   //!
   TBranch        *b_NLoosePhoton;   //!
   TBranch        *b_LooseMuonCSCHits1;   //!
   TBranch        *b_LooseMuonCSCHits2;   //!
   TBranch        *b_LooseMuonCSCHits3;   //!
   TBranch        *b_LooseMuonCSCHits4;   //!
   TBranch        *b_LooseMuonEta1;   //!
   TBranch        *b_LooseMuonEta2;   //!
   TBranch        *b_LooseMuonEta3;   //!
   TBranch        *b_LooseMuonEta4;   //!
   TBranch        *b_LooseMuonIsolEt1;   //!
   TBranch        *b_LooseMuonIsolEt2;   //!
   TBranch        *b_LooseMuonIsolEt3;   //!
   TBranch        *b_LooseMuonIsolEt4;   //!
   TBranch        *b_LooseMuonIsolN1;   //!
   TBranch        *b_LooseMuonIsolN2;   //!
   TBranch        *b_LooseMuonIsolN3;   //!
   TBranch        *b_LooseMuonIsolN4;   //!
   TBranch        *b_LooseMuonMDTHits1;   //!
   TBranch        *b_LooseMuonMDTHits2;   //!
   TBranch        *b_LooseMuonMDTHits3;   //!
   TBranch        *b_LooseMuonMDTHits4;   //!
   TBranch        *b_LooseMuonPhi1;   //!
   TBranch        *b_LooseMuonPhi2;   //!
   TBranch        *b_LooseMuonPhi3;   //!
   TBranch        *b_LooseMuonPhi4;   //!
   TBranch        *b_LooseMuonPt1;   //!
   TBranch        *b_LooseMuonPt2;   //!
   TBranch        *b_LooseMuonPt3;   //!
   TBranch        *b_LooseMuonPt4;   //!
   TBranch        *b_LooseMuonRPCHits1;   //!
   TBranch        *b_LooseMuonRPCHits2;   //!
   TBranch        *b_LooseMuonRPCHits3;   //!
   TBranch        *b_LooseMuonRPCHits4;   //!
   TBranch        *b_LooseMuonTGCHits1;   //!
   TBranch        *b_LooseMuonTGCHits2;   //!
   TBranch        *b_LooseMuonTGCHits3;   //!
   TBranch        *b_LooseMuonTGCHits4;   //!
   TBranch        *b_LooseMuonTightness1;   //!
   TBranch        *b_LooseMuonTightness2;   //!
   TBranch        *b_LooseMuonTightness3;   //!
   TBranch        *b_LooseMuonTightness4;   //!
   TBranch        *b_NLooseMuon;   //!
   TBranch        *b_BJetLikelihood1;   //!
   TBranch        *b_BJetLikelihood2;   //!
   TBranch        *b_BJetLikelihood3;   //!
   TBranch        *b_BJetLikelihood4;   //!
   TBranch        *b_BJetLikelihood5;   //!
   TBranch        *b_BJetLikelihood6;   //!
   TBranch        *b_JetEmFrac1;   //!
   TBranch        *b_JetEmFrac2;   //!
   TBranch        *b_JetEmFrac3;   //!
   TBranch        *b_JetEmFrac4;   //!
   TBranch        *b_JetEmFrac5;   //!
   TBranch        *b_JetEmFrac6;   //!
   TBranch        *b_JetEta1;   //!
   TBranch        *b_JetEta2;   //!
   TBranch        *b_JetEta3;   //!
   TBranch        *b_JetEta4;   //!
   TBranch        *b_JetEta5;   //!
   TBranch        *b_JetEta6;   //!
   TBranch        *b_JetPhi1;   //!
   TBranch        *b_JetPhi2;   //!
   TBranch        *b_JetPhi3;   //!
   TBranch        *b_JetPhi4;   //!
   TBranch        *b_JetPhi5;   //!
   TBranch        *b_JetPhi6;   //!
   TBranch        *b_JetPt1;   //!
   TBranch        *b_JetPt2;   //!
   TBranch        *b_JetPt3;   //!
   TBranch        *b_JetPt4;   //!
   TBranch        *b_JetPt5;   //!
   TBranch        *b_JetPt6;   //!
   TBranch        *b_JetSumET;   //!
   TBranch        *b_NBJet;   //!
   TBranch        *b_NJet;   //!
   TBranch        *b_BunchGroup;   //!
   TBranch        *b_EFPassedTrigMask0;   //!
   TBranch        *b_EFPassedTrigMask1;   //!
   TBranch        *b_EFPassedTrigMask10;   //!
   TBranch        *b_EFPassedTrigMask11;   //!
   TBranch        *b_EFPassedTrigMask12;   //!
   TBranch        *b_EFPassedTrigMask13;   //!
   TBranch        *b_EFPassedTrigMask14;   //!
   TBranch        *b_EFPassedTrigMask15;   //!
   TBranch        *b_EFPassedTrigMask16;   //!
   TBranch        *b_EFPassedTrigMask17;   //!
   TBranch        *b_EFPassedTrigMask18;   //!
   TBranch        *b_EFPassedTrigMask19;   //!
   TBranch        *b_EFPassedTrigMask2;   //!
   TBranch        *b_EFPassedTrigMask20;   //!
   TBranch        *b_EFPassedTrigMask21;   //!
   TBranch        *b_EFPassedTrigMask22;   //!
   TBranch        *b_EFPassedTrigMask23;   //!
   TBranch        *b_EFPassedTrigMask24;   //!
   TBranch        *b_EFPassedTrigMask25;   //!
   TBranch        *b_EFPassedTrigMask26;   //!
   TBranch        *b_EFPassedTrigMask27;   //!
   TBranch        *b_EFPassedTrigMask28;   //!
   TBranch        *b_EFPassedTrigMask29;   //!
   TBranch        *b_EFPassedTrigMask3;   //!
   TBranch        *b_EFPassedTrigMask30;   //!
   TBranch        *b_EFPassedTrigMask31;   //!
   TBranch        *b_EFPassedTrigMask4;   //!
   TBranch        *b_EFPassedTrigMask5;   //!
   TBranch        *b_EFPassedTrigMask6;   //!
   TBranch        *b_EFPassedTrigMask7;   //!
   TBranch        *b_EFPassedTrigMask8;   //!
   TBranch        *b_EFPassedTrigMask9;   //!
   TBranch        *b_L1PassedTrigMaskTAP0;   //!
   TBranch        *b_L1PassedTrigMaskTAP1;   //!
   TBranch        *b_L1PassedTrigMaskTAP2;   //!
   TBranch        *b_L1PassedTrigMaskTAP3;   //!
   TBranch        *b_L1PassedTrigMaskTAP4;   //!
   TBranch        *b_L1PassedTrigMaskTAP5;   //!
   TBranch        *b_L1PassedTrigMaskTAP6;   //!
   TBranch        *b_L1PassedTrigMaskTAP7;   //!
   TBranch        *b_L1PassedTrigMaskTAV0;   //!
   TBranch        *b_L1PassedTrigMaskTAV1;   //!
   TBranch        *b_L1PassedTrigMaskTAV2;   //!
   TBranch        *b_L1PassedTrigMaskTAV3;   //!
   TBranch        *b_L1PassedTrigMaskTAV4;   //!
   TBranch        *b_L1PassedTrigMaskTAV5;   //!
   TBranch        *b_L1PassedTrigMaskTAV6;   //!
   TBranch        *b_L1PassedTrigMaskTAV7;   //!
   TBranch        *b_L1PassedTrigMaskTBP0;   //!
   TBranch        *b_L1PassedTrigMaskTBP1;   //!
   TBranch        *b_L1PassedTrigMaskTBP2;   //!
   TBranch        *b_L1PassedTrigMaskTBP3;   //!
   TBranch        *b_L1PassedTrigMaskTBP4;   //!
   TBranch        *b_L1PassedTrigMaskTBP5;   //!
   TBranch        *b_L1PassedTrigMaskTBP6;   //!
   TBranch        *b_L1PassedTrigMaskTBP7;   //!
   TBranch        *b_L2PassedTrigMask0;   //!
   TBranch        *b_L2PassedTrigMask1;   //!
   TBranch        *b_L2PassedTrigMask10;   //!
   TBranch        *b_L2PassedTrigMask11;   //!
   TBranch        *b_L2PassedTrigMask12;   //!
   TBranch        *b_L2PassedTrigMask13;   //!
   TBranch        *b_L2PassedTrigMask14;   //!
   TBranch        *b_L2PassedTrigMask15;   //!
   TBranch        *b_L2PassedTrigMask16;   //!
   TBranch        *b_L2PassedTrigMask17;   //!
   TBranch        *b_L2PassedTrigMask18;   //!
   TBranch        *b_L2PassedTrigMask19;   //!
   TBranch        *b_L2PassedTrigMask2;   //!
   TBranch        *b_L2PassedTrigMask20;   //!
   TBranch        *b_L2PassedTrigMask21;   //!
   TBranch        *b_L2PassedTrigMask22;   //!
   TBranch        *b_L2PassedTrigMask23;   //!
   TBranch        *b_L2PassedTrigMask24;   //!
   TBranch        *b_L2PassedTrigMask25;   //!
   TBranch        *b_L2PassedTrigMask26;   //!
   TBranch        *b_L2PassedTrigMask27;   //!
   TBranch        *b_L2PassedTrigMask28;   //!
   TBranch        *b_L2PassedTrigMask29;   //!
   TBranch        *b_L2PassedTrigMask3;   //!
   TBranch        *b_L2PassedTrigMask30;   //!
   TBranch        *b_L2PassedTrigMask31;   //!
   TBranch        *b_L2PassedTrigMask4;   //!
   TBranch        *b_L2PassedTrigMask5;   //!
   TBranch        *b_L2PassedTrigMask6;   //!
   TBranch        *b_L2PassedTrigMask7;   //!
   TBranch        *b_L2PassedTrigMask8;   //!
   TBranch        *b_L2PassedTrigMask9;   //!
   TBranch        *b_Level1TriggerType;   //!
   TBranch        *b_SFOStreams;   //!
   TBranch        *b_TrackLead1D0;   //!
   TBranch        *b_TrackLead1Z0;   //!
   TBranch        *b_TrackLead1Phi0;   //!
   TBranch        *b_TrackLead1Theta;   //!
   TBranch        *b_TrackLead1QOverP;   //!
   TBranch        *b_TrackLead2D0;   //!
   TBranch        *b_TrackLead2Z0;   //!
   TBranch        *b_TrackLead2Phi0;   //!
   TBranch        *b_TrackLead2Theta;   //!
   TBranch        *b_TrackLead2QOverP;   //!
   TBranch        *b_CellEnergySum;   //!
   TBranch        *b_CellEnergySumEMB;   //!
   TBranch        *b_CellEnergySumEMEC;   //!
   TBranch        *b_CellEnergySumHEC;   //!
   TBranch        *b_CellEnergySumFCAL;   //!
   TBranch        *b_CellEnergySumTile;   //!
   TBranch        *b_ClusterEnergySum;   //!
   TBranch        *b_TopoClusterEt1;   //!
   TBranch        *b_TopoClusterEta1;   //!
   TBranch        *b_TopoClusterPhi1;   //!
   TBranch        *b_CellMissingET;   //!
   TBranch        *b_CellMissingETPhi;   //!
   TBranch        *b_MBTSTimeDiff;   //!
   TBranch        *b_LArECTimeDiff;   //!
   TBranch        *b_TRTEventPhase;   //!
   TBranch        *b_NPixelTracks;   //!
   TBranch        *b_NSCTTracks;   //!
   TBranch        *b_NTRTTracks;   //!
   TBranch        *b_NMooreSegs;   //!
   TBranch        *b_NMboySegs;   //!
   TBranch        *b_NInnerMboySegs;   //!
   TBranch        *b_NHitsInnerMboySegs;   //!
   TBranch        *b_NInnerMuSegs;   //!
   TBranch        *b_NHitsInnerMooreSegs;   //!
   TBranch        *b_NSCTSPs;   //!
   TBranch        *b_NPixelSPs;   //!
   TBranch        *b_NTRTDCs;   //!
   TBranch        *b_NTRTHtDCs;   //!
   TBranch        *b_NMDTHits;   //!
   TBranch        *b_NRPCHits;   //!
   TBranch        *b_NTGCHits;   //!
   TBranch        *b_NCSCHits;   //!
   TBranch        *b_NBCMHits;   //!
   TBranch        *b_MBTSWord;   //!
   TBranch        *b_BPhysWord;   //!
   TBranch        *b_CombinedMuonWord;   //!
   TBranch        *b_DAODStreams;   //!
   TBranch        *b_DESDStreams;   //!
   TBranch        *b_EgammaWord;   //!
   TBranch        *b_ExoticWord;   //!
   TBranch        *b_HeavyIonWord;   //!
   TBranch        *b_HiggsWord;   //!
   TBranch        *b_JetMissingETWord;   //!
   TBranch        *b_JetTagWord;   //!
   TBranch        *b_SMWord;   //!
   TBranch        *b_SUSYWord;   //!
   TBranch        *b_TauIdWord;   //!
   TBranch        *b_TopWord;   //!

   HITAGprinter(TTree *tree=0);
   virtual ~HITAGprinter();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef HITAGprinter_cxx
HITAGprinter::HITAGprinter(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/u/at/ahaas/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000005.pool.root.1");
      if (!f) {
         f = new TFile("/u/at/ahaas/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000005.pool.root.1");
         f->cd("Rint:/");
      }
      tree = (TTree*)gDirectory->Get("POOLCollectionTree");

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("POOLCollectionTree","");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000001.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000002.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000003.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000004.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000005.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000006.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000007.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000008.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000009.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000010.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000011.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000012.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000013.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000014.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000015.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000016.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000017.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000018.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000019.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000020.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000021.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000022.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000023.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000024.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000025.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000026.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000027.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000028.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000029.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000030.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000031.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000032.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000033.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000034.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000035.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000036.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000037.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000038.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000039.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000040.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000041.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000042.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000043.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000044.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000045.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000046.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000047.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000048.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000049.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000050.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000051.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000052.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000053.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000054.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000055.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000056.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000057.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000058.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000059.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000060.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000061.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000062.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000063.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000064.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000065.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000066.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000067.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000068.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000069.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000070.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000071.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000072.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000073.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000074.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000075.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000076.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000077.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000078.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000079.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000080.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000081.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000082.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000083.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000084.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000085.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000086.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000087.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000088.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000089.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000090.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000091.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000092.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000093.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000094.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000095.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000096.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000097.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000098.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000099.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000100.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000101.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000102.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000103.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000104.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000105.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000106.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000107.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000108.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000109.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000110.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000111.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000112.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000113.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000114.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000115.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000116.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000117.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000118.pool.root.1/POOLCollectionTree");
      chain->Add("~/nfs3/data10_hi.00170467.physics_bulk.merge.TAG.r2111_p455_tid288698_00/TAG.288698._000119.pool.root.1/POOLCollectionTree");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

HITAGprinter::~HITAGprinter()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t HITAGprinter::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t HITAGprinter::LoadTree(Long64_t entry)
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

void HITAGprinter::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("StreamESD_ref", StreamESD_ref, &b_StreamESD_ref);
   fChain->SetBranchAddress("StreamRAW_ref", StreamRAW_ref, &b_StreamRAW_ref);
   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("EventTime", &EventTime, &b_EventTime);
   fChain->SetBranchAddress("EventTimeNanoSec", &EventTimeNanoSec, &b_EventTimeNanoSec);
   fChain->SetBranchAddress("LumiBlockN", &LumiBlockN, &b_LumiBlockN);
   fChain->SetBranchAddress("BunchId", &BunchId, &b_BunchId);
   fChain->SetBranchAddress("PixelFlags", &PixelFlags, &b_PixelFlags);
   fChain->SetBranchAddress("SCTFlags", &SCTFlags, &b_SCTFlags);
   fChain->SetBranchAddress("TRTFlags", &TRTFlags, &b_TRTFlags);
   fChain->SetBranchAddress("LArFlags", &LArFlags, &b_LArFlags);
   fChain->SetBranchAddress("TileFlags", &TileFlags, &b_TileFlags);
   fChain->SetBranchAddress("MuonFlags", &MuonFlags, &b_MuonFlags);
   fChain->SetBranchAddress("ForwardDetFlags", &ForwardDetFlags, &b_ForwardDetFlags);
   fChain->SetBranchAddress("CoreFlags", &CoreFlags, &b_CoreFlags);
   fChain->SetBranchAddress("IsSimulation", &IsSimulation, &b_IsSimulation);
   fChain->SetBranchAddress("IsCalibration", &IsCalibration, &b_IsCalibration);
   fChain->SetBranchAddress("IsTestBeam", &IsTestBeam, &b_IsTestBeam);
   fChain->SetBranchAddress("RandomNumber", &RandomNumber, &b_RandomNumber);
   fChain->SetBranchAddress("NTrk", &NTrk, &b_NTrk);
   fChain->SetBranchAddress("NVtx", &NVtx, &b_NVtx);
   fChain->SetBranchAddress("VtxX", &VtxX, &b_VtxX);
   fChain->SetBranchAddress("VtxY", &VtxY, &b_VtxY);
   fChain->SetBranchAddress("VtxZ", &VtxZ, &b_VtxZ);
   fChain->SetBranchAddress("VtxChiSq", &VtxChiSq, &b_VtxChiSq);
   fChain->SetBranchAddress("VtxNDOF", &VtxNDOF, &b_VtxNDOF);
   fChain->SetBranchAddress("NVtxTight", &NVtxTight, &b_NVtxTight);
   fChain->SetBranchAddress("LooseElectronEta1", &LooseElectronEta1, &b_LooseElectronEta1);
   fChain->SetBranchAddress("LooseElectronEta2", &LooseElectronEta2, &b_LooseElectronEta2);
   fChain->SetBranchAddress("LooseElectronEta3", &LooseElectronEta3, &b_LooseElectronEta3);
   fChain->SetBranchAddress("LooseElectronEta4", &LooseElectronEta4, &b_LooseElectronEta4);
   fChain->SetBranchAddress("LooseElectronIsForward1", &LooseElectronIsForward1, &b_LooseElectronIsForward1);
   fChain->SetBranchAddress("LooseElectronIsForward2", &LooseElectronIsForward2, &b_LooseElectronIsForward2);
   fChain->SetBranchAddress("LooseElectronIsForward3", &LooseElectronIsForward3, &b_LooseElectronIsForward3);
   fChain->SetBranchAddress("LooseElectronIsForward4", &LooseElectronIsForward4, &b_LooseElectronIsForward4);
   fChain->SetBranchAddress("LooseElectronPhi1", &LooseElectronPhi1, &b_LooseElectronPhi1);
   fChain->SetBranchAddress("LooseElectronPhi2", &LooseElectronPhi2, &b_LooseElectronPhi2);
   fChain->SetBranchAddress("LooseElectronPhi3", &LooseElectronPhi3, &b_LooseElectronPhi3);
   fChain->SetBranchAddress("LooseElectronPhi4", &LooseElectronPhi4, &b_LooseElectronPhi4);
   fChain->SetBranchAddress("LooseElectronPt1", &LooseElectronPt1, &b_LooseElectronPt1);
   fChain->SetBranchAddress("LooseElectronPt2", &LooseElectronPt2, &b_LooseElectronPt2);
   fChain->SetBranchAddress("LooseElectronPt3", &LooseElectronPt3, &b_LooseElectronPt3);
   fChain->SetBranchAddress("LooseElectronPt4", &LooseElectronPt4, &b_LooseElectronPt4);
   fChain->SetBranchAddress("LooseElectronTightness1", &LooseElectronTightness1, &b_LooseElectronTightness1);
   fChain->SetBranchAddress("LooseElectronTightness2", &LooseElectronTightness2, &b_LooseElectronTightness2);
   fChain->SetBranchAddress("LooseElectronTightness3", &LooseElectronTightness3, &b_LooseElectronTightness3);
   fChain->SetBranchAddress("LooseElectronTightness4", &LooseElectronTightness4, &b_LooseElectronTightness4);
   fChain->SetBranchAddress("NLooseElectron", &NLooseElectron, &b_NLooseElectron);
   fChain->SetBranchAddress("LoosePhotonEta1", &LoosePhotonEta1, &b_LoosePhotonEta1);
   fChain->SetBranchAddress("LoosePhotonEta2", &LoosePhotonEta2, &b_LoosePhotonEta2);
   fChain->SetBranchAddress("LoosePhotonPhi1", &LoosePhotonPhi1, &b_LoosePhotonPhi1);
   fChain->SetBranchAddress("LoosePhotonPhi2", &LoosePhotonPhi2, &b_LoosePhotonPhi2);
   fChain->SetBranchAddress("LoosePhotonPt1", &LoosePhotonPt1, &b_LoosePhotonPt1);
   fChain->SetBranchAddress("LoosePhotonPt2", &LoosePhotonPt2, &b_LoosePhotonPt2);
   fChain->SetBranchAddress("LoosePhotonTightness1", &LoosePhotonTightness1, &b_LoosePhotonTightness1);
   fChain->SetBranchAddress("LoosePhotonTightness2", &LoosePhotonTightness2, &b_LoosePhotonTightness2);
   fChain->SetBranchAddress("NConvertedLoosePhoton", &NConvertedLoosePhoton, &b_NConvertedLoosePhoton);
   fChain->SetBranchAddress("NLoosePhoton", &NLoosePhoton, &b_NLoosePhoton);
   fChain->SetBranchAddress("LooseMuonCSCHits1", &LooseMuonCSCHits1, &b_LooseMuonCSCHits1);
   fChain->SetBranchAddress("LooseMuonCSCHits2", &LooseMuonCSCHits2, &b_LooseMuonCSCHits2);
   fChain->SetBranchAddress("LooseMuonCSCHits3", &LooseMuonCSCHits3, &b_LooseMuonCSCHits3);
   fChain->SetBranchAddress("LooseMuonCSCHits4", &LooseMuonCSCHits4, &b_LooseMuonCSCHits4);
   fChain->SetBranchAddress("LooseMuonEta1", &LooseMuonEta1, &b_LooseMuonEta1);
   fChain->SetBranchAddress("LooseMuonEta2", &LooseMuonEta2, &b_LooseMuonEta2);
   fChain->SetBranchAddress("LooseMuonEta3", &LooseMuonEta3, &b_LooseMuonEta3);
   fChain->SetBranchAddress("LooseMuonEta4", &LooseMuonEta4, &b_LooseMuonEta4);
   fChain->SetBranchAddress("LooseMuonIsolEt1", &LooseMuonIsolEt1, &b_LooseMuonIsolEt1);
   fChain->SetBranchAddress("LooseMuonIsolEt2", &LooseMuonIsolEt2, &b_LooseMuonIsolEt2);
   fChain->SetBranchAddress("LooseMuonIsolEt3", &LooseMuonIsolEt3, &b_LooseMuonIsolEt3);
   fChain->SetBranchAddress("LooseMuonIsolEt4", &LooseMuonIsolEt4, &b_LooseMuonIsolEt4);
   fChain->SetBranchAddress("LooseMuonIsolN1", &LooseMuonIsolN1, &b_LooseMuonIsolN1);
   fChain->SetBranchAddress("LooseMuonIsolN2", &LooseMuonIsolN2, &b_LooseMuonIsolN2);
   fChain->SetBranchAddress("LooseMuonIsolN3", &LooseMuonIsolN3, &b_LooseMuonIsolN3);
   fChain->SetBranchAddress("LooseMuonIsolN4", &LooseMuonIsolN4, &b_LooseMuonIsolN4);
   fChain->SetBranchAddress("LooseMuonMDTHits1", &LooseMuonMDTHits1, &b_LooseMuonMDTHits1);
   fChain->SetBranchAddress("LooseMuonMDTHits2", &LooseMuonMDTHits2, &b_LooseMuonMDTHits2);
   fChain->SetBranchAddress("LooseMuonMDTHits3", &LooseMuonMDTHits3, &b_LooseMuonMDTHits3);
   fChain->SetBranchAddress("LooseMuonMDTHits4", &LooseMuonMDTHits4, &b_LooseMuonMDTHits4);
   fChain->SetBranchAddress("LooseMuonPhi1", &LooseMuonPhi1, &b_LooseMuonPhi1);
   fChain->SetBranchAddress("LooseMuonPhi2", &LooseMuonPhi2, &b_LooseMuonPhi2);
   fChain->SetBranchAddress("LooseMuonPhi3", &LooseMuonPhi3, &b_LooseMuonPhi3);
   fChain->SetBranchAddress("LooseMuonPhi4", &LooseMuonPhi4, &b_LooseMuonPhi4);
   fChain->SetBranchAddress("LooseMuonPt1", &LooseMuonPt1, &b_LooseMuonPt1);
   fChain->SetBranchAddress("LooseMuonPt2", &LooseMuonPt2, &b_LooseMuonPt2);
   fChain->SetBranchAddress("LooseMuonPt3", &LooseMuonPt3, &b_LooseMuonPt3);
   fChain->SetBranchAddress("LooseMuonPt4", &LooseMuonPt4, &b_LooseMuonPt4);
   fChain->SetBranchAddress("LooseMuonRPCHits1", &LooseMuonRPCHits1, &b_LooseMuonRPCHits1);
   fChain->SetBranchAddress("LooseMuonRPCHits2", &LooseMuonRPCHits2, &b_LooseMuonRPCHits2);
   fChain->SetBranchAddress("LooseMuonRPCHits3", &LooseMuonRPCHits3, &b_LooseMuonRPCHits3);
   fChain->SetBranchAddress("LooseMuonRPCHits4", &LooseMuonRPCHits4, &b_LooseMuonRPCHits4);
   fChain->SetBranchAddress("LooseMuonTGCHits1", &LooseMuonTGCHits1, &b_LooseMuonTGCHits1);
   fChain->SetBranchAddress("LooseMuonTGCHits2", &LooseMuonTGCHits2, &b_LooseMuonTGCHits2);
   fChain->SetBranchAddress("LooseMuonTGCHits3", &LooseMuonTGCHits3, &b_LooseMuonTGCHits3);
   fChain->SetBranchAddress("LooseMuonTGCHits4", &LooseMuonTGCHits4, &b_LooseMuonTGCHits4);
   fChain->SetBranchAddress("LooseMuonTightness1", &LooseMuonTightness1, &b_LooseMuonTightness1);
   fChain->SetBranchAddress("LooseMuonTightness2", &LooseMuonTightness2, &b_LooseMuonTightness2);
   fChain->SetBranchAddress("LooseMuonTightness3", &LooseMuonTightness3, &b_LooseMuonTightness3);
   fChain->SetBranchAddress("LooseMuonTightness4", &LooseMuonTightness4, &b_LooseMuonTightness4);
   fChain->SetBranchAddress("NLooseMuon", &NLooseMuon, &b_NLooseMuon);
   fChain->SetBranchAddress("BJetLikelihood1", &BJetLikelihood1, &b_BJetLikelihood1);
   fChain->SetBranchAddress("BJetLikelihood2", &BJetLikelihood2, &b_BJetLikelihood2);
   fChain->SetBranchAddress("BJetLikelihood3", &BJetLikelihood3, &b_BJetLikelihood3);
   fChain->SetBranchAddress("BJetLikelihood4", &BJetLikelihood4, &b_BJetLikelihood4);
   fChain->SetBranchAddress("BJetLikelihood5", &BJetLikelihood5, &b_BJetLikelihood5);
   fChain->SetBranchAddress("BJetLikelihood6", &BJetLikelihood6, &b_BJetLikelihood6);
   fChain->SetBranchAddress("JetEmFrac1", &JetEmFrac1, &b_JetEmFrac1);
   fChain->SetBranchAddress("JetEmFrac2", &JetEmFrac2, &b_JetEmFrac2);
   fChain->SetBranchAddress("JetEmFrac3", &JetEmFrac3, &b_JetEmFrac3);
   fChain->SetBranchAddress("JetEmFrac4", &JetEmFrac4, &b_JetEmFrac4);
   fChain->SetBranchAddress("JetEmFrac5", &JetEmFrac5, &b_JetEmFrac5);
   fChain->SetBranchAddress("JetEmFrac6", &JetEmFrac6, &b_JetEmFrac6);
   fChain->SetBranchAddress("JetEta1", &JetEta1, &b_JetEta1);
   fChain->SetBranchAddress("JetEta2", &JetEta2, &b_JetEta2);
   fChain->SetBranchAddress("JetEta3", &JetEta3, &b_JetEta3);
   fChain->SetBranchAddress("JetEta4", &JetEta4, &b_JetEta4);
   fChain->SetBranchAddress("JetEta5", &JetEta5, &b_JetEta5);
   fChain->SetBranchAddress("JetEta6", &JetEta6, &b_JetEta6);
   fChain->SetBranchAddress("JetPhi1", &JetPhi1, &b_JetPhi1);
   fChain->SetBranchAddress("JetPhi2", &JetPhi2, &b_JetPhi2);
   fChain->SetBranchAddress("JetPhi3", &JetPhi3, &b_JetPhi3);
   fChain->SetBranchAddress("JetPhi4", &JetPhi4, &b_JetPhi4);
   fChain->SetBranchAddress("JetPhi5", &JetPhi5, &b_JetPhi5);
   fChain->SetBranchAddress("JetPhi6", &JetPhi6, &b_JetPhi6);
   fChain->SetBranchAddress("JetPt1", &JetPt1, &b_JetPt1);
   fChain->SetBranchAddress("JetPt2", &JetPt2, &b_JetPt2);
   fChain->SetBranchAddress("JetPt3", &JetPt3, &b_JetPt3);
   fChain->SetBranchAddress("JetPt4", &JetPt4, &b_JetPt4);
   fChain->SetBranchAddress("JetPt5", &JetPt5, &b_JetPt5);
   fChain->SetBranchAddress("JetPt6", &JetPt6, &b_JetPt6);
   fChain->SetBranchAddress("JetSumET", &JetSumET, &b_JetSumET);
   fChain->SetBranchAddress("NBJet", &NBJet, &b_NBJet);
   fChain->SetBranchAddress("NJet", &NJet, &b_NJet);
   fChain->SetBranchAddress("BunchGroup", &BunchGroup, &b_BunchGroup);
   fChain->SetBranchAddress("EFPassedTrigMask0", &EFPassedTrigMask0, &b_EFPassedTrigMask0);
   fChain->SetBranchAddress("EFPassedTrigMask1", &EFPassedTrigMask1, &b_EFPassedTrigMask1);
   fChain->SetBranchAddress("EFPassedTrigMask10", &EFPassedTrigMask10, &b_EFPassedTrigMask10);
   fChain->SetBranchAddress("EFPassedTrigMask11", &EFPassedTrigMask11, &b_EFPassedTrigMask11);
   fChain->SetBranchAddress("EFPassedTrigMask12", &EFPassedTrigMask12, &b_EFPassedTrigMask12);
   fChain->SetBranchAddress("EFPassedTrigMask13", &EFPassedTrigMask13, &b_EFPassedTrigMask13);
   fChain->SetBranchAddress("EFPassedTrigMask14", &EFPassedTrigMask14, &b_EFPassedTrigMask14);
   fChain->SetBranchAddress("EFPassedTrigMask15", &EFPassedTrigMask15, &b_EFPassedTrigMask15);
   fChain->SetBranchAddress("EFPassedTrigMask16", &EFPassedTrigMask16, &b_EFPassedTrigMask16);
   fChain->SetBranchAddress("EFPassedTrigMask17", &EFPassedTrigMask17, &b_EFPassedTrigMask17);
   fChain->SetBranchAddress("EFPassedTrigMask18", &EFPassedTrigMask18, &b_EFPassedTrigMask18);
   fChain->SetBranchAddress("EFPassedTrigMask19", &EFPassedTrigMask19, &b_EFPassedTrigMask19);
   fChain->SetBranchAddress("EFPassedTrigMask2", &EFPassedTrigMask2, &b_EFPassedTrigMask2);
   fChain->SetBranchAddress("EFPassedTrigMask20", &EFPassedTrigMask20, &b_EFPassedTrigMask20);
   fChain->SetBranchAddress("EFPassedTrigMask21", &EFPassedTrigMask21, &b_EFPassedTrigMask21);
   fChain->SetBranchAddress("EFPassedTrigMask22", &EFPassedTrigMask22, &b_EFPassedTrigMask22);
   fChain->SetBranchAddress("EFPassedTrigMask23", &EFPassedTrigMask23, &b_EFPassedTrigMask23);
   fChain->SetBranchAddress("EFPassedTrigMask24", &EFPassedTrigMask24, &b_EFPassedTrigMask24);
   fChain->SetBranchAddress("EFPassedTrigMask25", &EFPassedTrigMask25, &b_EFPassedTrigMask25);
   fChain->SetBranchAddress("EFPassedTrigMask26", &EFPassedTrigMask26, &b_EFPassedTrigMask26);
   fChain->SetBranchAddress("EFPassedTrigMask27", &EFPassedTrigMask27, &b_EFPassedTrigMask27);
   fChain->SetBranchAddress("EFPassedTrigMask28", &EFPassedTrigMask28, &b_EFPassedTrigMask28);
   fChain->SetBranchAddress("EFPassedTrigMask29", &EFPassedTrigMask29, &b_EFPassedTrigMask29);
   fChain->SetBranchAddress("EFPassedTrigMask3", &EFPassedTrigMask3, &b_EFPassedTrigMask3);
   fChain->SetBranchAddress("EFPassedTrigMask30", &EFPassedTrigMask30, &b_EFPassedTrigMask30);
   fChain->SetBranchAddress("EFPassedTrigMask31", &EFPassedTrigMask31, &b_EFPassedTrigMask31);
   fChain->SetBranchAddress("EFPassedTrigMask4", &EFPassedTrigMask4, &b_EFPassedTrigMask4);
   fChain->SetBranchAddress("EFPassedTrigMask5", &EFPassedTrigMask5, &b_EFPassedTrigMask5);
   fChain->SetBranchAddress("EFPassedTrigMask6", &EFPassedTrigMask6, &b_EFPassedTrigMask6);
   fChain->SetBranchAddress("EFPassedTrigMask7", &EFPassedTrigMask7, &b_EFPassedTrigMask7);
   fChain->SetBranchAddress("EFPassedTrigMask8", &EFPassedTrigMask8, &b_EFPassedTrigMask8);
   fChain->SetBranchAddress("EFPassedTrigMask9", &EFPassedTrigMask9, &b_EFPassedTrigMask9);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP0", &L1PassedTrigMaskTAP0, &b_L1PassedTrigMaskTAP0);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP1", &L1PassedTrigMaskTAP1, &b_L1PassedTrigMaskTAP1);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP2", &L1PassedTrigMaskTAP2, &b_L1PassedTrigMaskTAP2);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP3", &L1PassedTrigMaskTAP3, &b_L1PassedTrigMaskTAP3);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP4", &L1PassedTrigMaskTAP4, &b_L1PassedTrigMaskTAP4);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP5", &L1PassedTrigMaskTAP5, &b_L1PassedTrigMaskTAP5);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP6", &L1PassedTrigMaskTAP6, &b_L1PassedTrigMaskTAP6);
   fChain->SetBranchAddress("L1PassedTrigMaskTAP7", &L1PassedTrigMaskTAP7, &b_L1PassedTrigMaskTAP7);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV0", &L1PassedTrigMaskTAV0, &b_L1PassedTrigMaskTAV0);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV1", &L1PassedTrigMaskTAV1, &b_L1PassedTrigMaskTAV1);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV2", &L1PassedTrigMaskTAV2, &b_L1PassedTrigMaskTAV2);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV3", &L1PassedTrigMaskTAV3, &b_L1PassedTrigMaskTAV3);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV4", &L1PassedTrigMaskTAV4, &b_L1PassedTrigMaskTAV4);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV5", &L1PassedTrigMaskTAV5, &b_L1PassedTrigMaskTAV5);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV6", &L1PassedTrigMaskTAV6, &b_L1PassedTrigMaskTAV6);
   fChain->SetBranchAddress("L1PassedTrigMaskTAV7", &L1PassedTrigMaskTAV7, &b_L1PassedTrigMaskTAV7);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP0", &L1PassedTrigMaskTBP0, &b_L1PassedTrigMaskTBP0);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP1", &L1PassedTrigMaskTBP1, &b_L1PassedTrigMaskTBP1);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP2", &L1PassedTrigMaskTBP2, &b_L1PassedTrigMaskTBP2);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP3", &L1PassedTrigMaskTBP3, &b_L1PassedTrigMaskTBP3);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP4", &L1PassedTrigMaskTBP4, &b_L1PassedTrigMaskTBP4);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP5", &L1PassedTrigMaskTBP5, &b_L1PassedTrigMaskTBP5);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP6", &L1PassedTrigMaskTBP6, &b_L1PassedTrigMaskTBP6);
   fChain->SetBranchAddress("L1PassedTrigMaskTBP7", &L1PassedTrigMaskTBP7, &b_L1PassedTrigMaskTBP7);
   fChain->SetBranchAddress("L2PassedTrigMask0", &L2PassedTrigMask0, &b_L2PassedTrigMask0);
   fChain->SetBranchAddress("L2PassedTrigMask1", &L2PassedTrigMask1, &b_L2PassedTrigMask1);
   fChain->SetBranchAddress("L2PassedTrigMask10", &L2PassedTrigMask10, &b_L2PassedTrigMask10);
   fChain->SetBranchAddress("L2PassedTrigMask11", &L2PassedTrigMask11, &b_L2PassedTrigMask11);
   fChain->SetBranchAddress("L2PassedTrigMask12", &L2PassedTrigMask12, &b_L2PassedTrigMask12);
   fChain->SetBranchAddress("L2PassedTrigMask13", &L2PassedTrigMask13, &b_L2PassedTrigMask13);
   fChain->SetBranchAddress("L2PassedTrigMask14", &L2PassedTrigMask14, &b_L2PassedTrigMask14);
   fChain->SetBranchAddress("L2PassedTrigMask15", &L2PassedTrigMask15, &b_L2PassedTrigMask15);
   fChain->SetBranchAddress("L2PassedTrigMask16", &L2PassedTrigMask16, &b_L2PassedTrigMask16);
   fChain->SetBranchAddress("L2PassedTrigMask17", &L2PassedTrigMask17, &b_L2PassedTrigMask17);
   fChain->SetBranchAddress("L2PassedTrigMask18", &L2PassedTrigMask18, &b_L2PassedTrigMask18);
   fChain->SetBranchAddress("L2PassedTrigMask19", &L2PassedTrigMask19, &b_L2PassedTrigMask19);
   fChain->SetBranchAddress("L2PassedTrigMask2", &L2PassedTrigMask2, &b_L2PassedTrigMask2);
   fChain->SetBranchAddress("L2PassedTrigMask20", &L2PassedTrigMask20, &b_L2PassedTrigMask20);
   fChain->SetBranchAddress("L2PassedTrigMask21", &L2PassedTrigMask21, &b_L2PassedTrigMask21);
   fChain->SetBranchAddress("L2PassedTrigMask22", &L2PassedTrigMask22, &b_L2PassedTrigMask22);
   fChain->SetBranchAddress("L2PassedTrigMask23", &L2PassedTrigMask23, &b_L2PassedTrigMask23);
   fChain->SetBranchAddress("L2PassedTrigMask24", &L2PassedTrigMask24, &b_L2PassedTrigMask24);
   fChain->SetBranchAddress("L2PassedTrigMask25", &L2PassedTrigMask25, &b_L2PassedTrigMask25);
   fChain->SetBranchAddress("L2PassedTrigMask26", &L2PassedTrigMask26, &b_L2PassedTrigMask26);
   fChain->SetBranchAddress("L2PassedTrigMask27", &L2PassedTrigMask27, &b_L2PassedTrigMask27);
   fChain->SetBranchAddress("L2PassedTrigMask28", &L2PassedTrigMask28, &b_L2PassedTrigMask28);
   fChain->SetBranchAddress("L2PassedTrigMask29", &L2PassedTrigMask29, &b_L2PassedTrigMask29);
   fChain->SetBranchAddress("L2PassedTrigMask3", &L2PassedTrigMask3, &b_L2PassedTrigMask3);
   fChain->SetBranchAddress("L2PassedTrigMask30", &L2PassedTrigMask30, &b_L2PassedTrigMask30);
   fChain->SetBranchAddress("L2PassedTrigMask31", &L2PassedTrigMask31, &b_L2PassedTrigMask31);
   fChain->SetBranchAddress("L2PassedTrigMask4", &L2PassedTrigMask4, &b_L2PassedTrigMask4);
   fChain->SetBranchAddress("L2PassedTrigMask5", &L2PassedTrigMask5, &b_L2PassedTrigMask5);
   fChain->SetBranchAddress("L2PassedTrigMask6", &L2PassedTrigMask6, &b_L2PassedTrigMask6);
   fChain->SetBranchAddress("L2PassedTrigMask7", &L2PassedTrigMask7, &b_L2PassedTrigMask7);
   fChain->SetBranchAddress("L2PassedTrigMask8", &L2PassedTrigMask8, &b_L2PassedTrigMask8);
   fChain->SetBranchAddress("L2PassedTrigMask9", &L2PassedTrigMask9, &b_L2PassedTrigMask9);
   fChain->SetBranchAddress("Level1TriggerType", &Level1TriggerType, &b_Level1TriggerType);
   fChain->SetBranchAddress("SFOStreams", &SFOStreams, &b_SFOStreams);
   fChain->SetBranchAddress("TrackLead1D0", &TrackLead1D0, &b_TrackLead1D0);
   fChain->SetBranchAddress("TrackLead1Z0", &TrackLead1Z0, &b_TrackLead1Z0);
   fChain->SetBranchAddress("TrackLead1Phi0", &TrackLead1Phi0, &b_TrackLead1Phi0);
   fChain->SetBranchAddress("TrackLead1Theta", &TrackLead1Theta, &b_TrackLead1Theta);
   fChain->SetBranchAddress("TrackLead1QOverP", &TrackLead1QOverP, &b_TrackLead1QOverP);
   fChain->SetBranchAddress("TrackLead2D0", &TrackLead2D0, &b_TrackLead2D0);
   fChain->SetBranchAddress("TrackLead2Z0", &TrackLead2Z0, &b_TrackLead2Z0);
   fChain->SetBranchAddress("TrackLead2Phi0", &TrackLead2Phi0, &b_TrackLead2Phi0);
   fChain->SetBranchAddress("TrackLead2Theta", &TrackLead2Theta, &b_TrackLead2Theta);
   fChain->SetBranchAddress("TrackLead2QOverP", &TrackLead2QOverP, &b_TrackLead2QOverP);
   fChain->SetBranchAddress("CellEnergySum", &CellEnergySum, &b_CellEnergySum);
   fChain->SetBranchAddress("CellEnergySumEMB", &CellEnergySumEMB, &b_CellEnergySumEMB);
   fChain->SetBranchAddress("CellEnergySumEMEC", &CellEnergySumEMEC, &b_CellEnergySumEMEC);
   fChain->SetBranchAddress("CellEnergySumHEC", &CellEnergySumHEC, &b_CellEnergySumHEC);
   fChain->SetBranchAddress("CellEnergySumFCAL", &CellEnergySumFCAL, &b_CellEnergySumFCAL);
   fChain->SetBranchAddress("CellEnergySumTile", &CellEnergySumTile, &b_CellEnergySumTile);
   fChain->SetBranchAddress("ClusterEnergySum", &ClusterEnergySum, &b_ClusterEnergySum);
   fChain->SetBranchAddress("TopoClusterEt1", &TopoClusterEt1, &b_TopoClusterEt1);
   fChain->SetBranchAddress("TopoClusterEta1", &TopoClusterEta1, &b_TopoClusterEta1);
   fChain->SetBranchAddress("TopoClusterPhi1", &TopoClusterPhi1, &b_TopoClusterPhi1);
   fChain->SetBranchAddress("CellMissingET", &CellMissingET, &b_CellMissingET);
   fChain->SetBranchAddress("CellMissingETPhi", &CellMissingETPhi, &b_CellMissingETPhi);
   fChain->SetBranchAddress("MBTSTimeDiff", &MBTSTimeDiff, &b_MBTSTimeDiff);
   fChain->SetBranchAddress("LArECTimeDiff", &LArECTimeDiff, &b_LArECTimeDiff);
   fChain->SetBranchAddress("TRTEventPhase", &TRTEventPhase, &b_TRTEventPhase);
   fChain->SetBranchAddress("NPixelTracks", &NPixelTracks, &b_NPixelTracks);
   fChain->SetBranchAddress("NSCTTracks", &NSCTTracks, &b_NSCTTracks);
   fChain->SetBranchAddress("NTRTTracks", &NTRTTracks, &b_NTRTTracks);
   fChain->SetBranchAddress("NMooreSegs", &NMooreSegs, &b_NMooreSegs);
   fChain->SetBranchAddress("NMboySegs", &NMboySegs, &b_NMboySegs);
   fChain->SetBranchAddress("NInnerMboySegs", &NInnerMboySegs, &b_NInnerMboySegs);
   fChain->SetBranchAddress("NHitsInnerMboySegs", &NHitsInnerMboySegs, &b_NHitsInnerMboySegs);
   fChain->SetBranchAddress("NInnerMuSegs", &NInnerMuSegs, &b_NInnerMuSegs);
   fChain->SetBranchAddress("NHitsInnerMooreSegs", &NHitsInnerMooreSegs, &b_NHitsInnerMooreSegs);
   fChain->SetBranchAddress("NSCTSPs", &NSCTSPs, &b_NSCTSPs);
   fChain->SetBranchAddress("NPixelSPs", &NPixelSPs, &b_NPixelSPs);
   fChain->SetBranchAddress("NTRTDCs", &NTRTDCs, &b_NTRTDCs);
   fChain->SetBranchAddress("NTRTHtDCs", &NTRTHtDCs, &b_NTRTHtDCs);
   fChain->SetBranchAddress("NMDTHits", &NMDTHits, &b_NMDTHits);
   fChain->SetBranchAddress("NRPCHits", &NRPCHits, &b_NRPCHits);
   fChain->SetBranchAddress("NTGCHits", &NTGCHits, &b_NTGCHits);
   fChain->SetBranchAddress("NCSCHits", &NCSCHits, &b_NCSCHits);
   fChain->SetBranchAddress("NBCMHits", &NBCMHits, &b_NBCMHits);
   fChain->SetBranchAddress("MBTSWord", &MBTSWord, &b_MBTSWord);
   fChain->SetBranchAddress("BPhysWord", &BPhysWord, &b_BPhysWord);
   fChain->SetBranchAddress("CombinedMuonWord", &CombinedMuonWord, &b_CombinedMuonWord);
   fChain->SetBranchAddress("DAODStreams", &DAODStreams, &b_DAODStreams);
   fChain->SetBranchAddress("DESDStreams", &DESDStreams, &b_DESDStreams);
   fChain->SetBranchAddress("EgammaWord", &EgammaWord, &b_EgammaWord);
   fChain->SetBranchAddress("ExoticWord", &ExoticWord, &b_ExoticWord);
   fChain->SetBranchAddress("HeavyIonWord", &HeavyIonWord, &b_HeavyIonWord);
   fChain->SetBranchAddress("HiggsWord", &HiggsWord, &b_HiggsWord);
   fChain->SetBranchAddress("JetMissingETWord", &JetMissingETWord, &b_JetMissingETWord);
   fChain->SetBranchAddress("JetTagWord", &JetTagWord, &b_JetTagWord);
   fChain->SetBranchAddress("SMWord", &SMWord, &b_SMWord);
   fChain->SetBranchAddress("SUSYWord", &SUSYWord, &b_SUSYWord);
   fChain->SetBranchAddress("TauIdWord", &TauIdWord, &b_TauIdWord);
   fChain->SetBranchAddress("TopWord", &TopWord, &b_TopWord);
   Notify();
}

Bool_t HITAGprinter::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void HITAGprinter::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t HITAGprinter::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef HITAGprinter_cxx
