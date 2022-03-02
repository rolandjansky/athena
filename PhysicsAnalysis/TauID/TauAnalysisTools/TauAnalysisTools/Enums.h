/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ENUMS_H
#define TAUANALYSISTOOLS_ENUMS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/TauID/TauAnalysisTools/doc/README-TauSelectionTool.rst
*/

namespace TauAnalysisTools
{
typedef enum e_JETID
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
} JETID;

typedef enum e_ELEID
{
  ELEIDNONEUNCONFIGURED = 0,
  ELEIDNONE         = 1,
  ELEIDRNNLOOSE     = 2,
  ELEIDRNNMEDIUM    = 3,
  ELEIDRNNTIGHT     = 4,
} ELEID;

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

typedef enum e_TruthMatchedParticleType
{
  Unknown          = 0,
  TruthHadronicTau = 1,
  TruthLeptonicTau = 2,
  TruthMuon        = 3,
  TruthElectron    = 4,
  TruthJet         = 5,
  TruthHadronicDiTau = 6
} TruthMatchedParticleType;

// FIXME: do we want to keep this?
typedef enum e_DataPeriod
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
} DataPeriod;

// FIXME: do we want to keep this?
typedef enum e_DataPeriodBinning
{
  PeriodBinningUnknown  = 0,
  PeriodBinningAll        = 1,
} DataPeriodBinning;

struct ROOT6_NamespaceAutoloadHook{};

}

#endif // TAUANALYSISTOOLS_ENUMS_H
