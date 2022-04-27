/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ENUMS_H
#define TAUANALYSISTOOLS_ENUMS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
*/

namespace TauAnalysisTools
{
  enum JetID
  {
    JETIDNONEUNCONFIGURED  = 0,
    JETIDNONE              = 1,
    JETIDBDTVERYLOOSE      = 2,
    JETIDBDTLOOSE          = 3,
    JETIDBDTMEDIUM         = 4,
    JETIDBDTTIGHT          = 5,
    JETIDRNNVERYLOOSE      = 6,
    JETIDRNNLOOSE          = 7,
    JETIDRNNMEDIUM         = 8,
    JETIDRNNTIGHT          = 9
  };

  enum EleID
  {
    ELEIDNONEUNCONFIGURED = 0,
    ELEIDNONE         = 1,
    ELEIDRNNLOOSE     = 2,
    ELEIDRNNMEDIUM    = 3,
    ELEIDRNNTIGHT     = 4,
  };

  enum SelectionCuts
  {
    // FIXME: harmonise names for RNN score cuts: "CutEleRNNScore" vs "CutJetRNNScoreSigTrans"
    NoCut           = 0,    // 000000000000
    CutPt           = 1,    // 000000000001
    CutAbsEta       = 1<<1, // 000000000010
    CutPhi          = 1<<2, // 000000000100
    CutNTrack       = 1<<3, // 000000001000
    CutAbsCharge    = 1<<4, // 000000010000
    CutJetIDWP      = 1<<5, // 000000100000
    CutEleRNNScore  = 1<<6, // 000001000000
    CutEleIDWP      = 1<<7, // 000010000000
    CutMuonOLR      = 1<<8,        // 000100000000
    CutJetRNNScoreSigTrans = 1<<9  // 001000000000
  };

  enum EfficiencyCorrectionType
  {
    SFRecoHadTau     = 0,
    // SFRecoJet        = 1,
    // SFRecoElectron   = 2,
    // SFRecoMuon       = 3,

    SFJetIDHadTau    = 4,
    // SFJetIDJet       = 5,
    // SFJetIDElectron  = 6,
    // SFJetIDMuon      = 7,

    SFEleIDHadTau    = 8,
    // SFEleIDJet       = 9,
    SFEleIDElectron  = 10,
    // SFEleIDMuon      = 11,

    SFTriggerHadTau  = 12,
    // SFEleIDJet       = 13,
    // SFEleIDElectron  = 14,
    // SFEleIDMuon      = 15,

    SFDecayModeHadTau = 16,
  };

  enum EfficiencyCorrectionSystematicType
  {
    STAT = 1,
    SYST = 2
  };

  enum TruthMatchedParticleType
  {
    Unknown          = 0,
    TruthHadronicTau = 1,
    TruthLeptonicTau = 2,
    TruthMuon        = 3,
    TruthElectron    = 4,
    TruthJet         = 5,
    TruthHadronicDiTau = 6
  };

  // FIXME: do we want to keep this?
  enum DataPeriod
  {
    PeriodUnknown  = 0,
    PeriodD  = 1,
    PeriodE  = 2,
    PeriodF  = 3,
    PeriodG  = 4,
    PeriodH2 = 5,
    PeriodH3 = 6,
    PeriodI  = 7,
    PeriodJ  = 8
  };

  // FIXME: do we want to keep this?
  enum DataPeriodBinning
  {
    PeriodBinningUnknown  = 0,
    PeriodBinningAll        = 1,
  };

  struct ROOT6_NamespaceAutoloadHook{};

}

#endif // TAUANALYSISTOOLS_ENUMS_H
