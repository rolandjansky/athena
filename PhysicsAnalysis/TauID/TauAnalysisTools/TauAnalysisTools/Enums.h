/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ENUMS_H
#define TAUANALYSISTOOLS_ENUMS_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

namespace TauAnalysisTools
{
typedef enum e_JETID
{
  JETIDNONEUNCONFIGURED  = 0,
  JETIDNONE              = 1,
  JETIDBDTLOOSE          = 2,
  JETIDBDTMEDIUM         = 3,
  JETIDBDTTIGHT          = 4,
  JETIDBDTFAIL           = 5,
  JETIDBDTOTHER          = 6,
  JETIDLLHLOOSE          = 7,
  JETIDLLHMEDIUM         = 8,
  JETIDLLHTIGHT          = 9,
  JETIDLLHFAIL           = 10,
  JETIDBDTLOOSENOTMEDIUM = 11,
  JETIDBDTLOOSENOTTIGHT  = 12,
  JETIDBDTMEDIUMNOTTIGHT = 13,
  JETIDBDTNOTLOOSE       = 14,
  JETIDBDTVERYLOOSE      = 15,
  JETBDTBKGLOOSE         = 16,
  JETBDTBKGMEDIUM        = 17,
  JETBDTBKGTIGHT         = 18,
  JETIDRNNVERYLOOSE      = 19,
  JETIDRNNLOOSE          = 20,
  JETIDRNNMEDIUM         = 21,
  JETIDRNNTIGHT          = 22
} JETID;

typedef enum e_ELEID
{
  ELEIDNONEUNCONFIGURED = 0,
  ELEIDNONE      = 1,
  ELEIDBDTLOOSE  = 2,
  ELEIDBDTMEDIUM = 3,
  ELEIDBDTTIGHT  = 4,
  ELEIDOTHER     = 5
} ELEID;

typedef enum
{
  OLRNONEUNCONFIGURED = 0,
  OLRNONE     = 1,
  OLRLOOSEPP  = 2,
  OLRMEDIUMPP = 3,
  OLRTIGHTPP  = 4,
  OLROTHER    = 5,

  TAUELEOLR       = 6,
  ELELOOSELLHOLR  = 7,
  ELEMEDIUMLLHOLR = 8,
  ELETIGHTLLHOLR  = 9,

  ELEBDTIGHTPLUSVETO = 10,
  ELEBDTIGHT = 11,
  ELEBDTMEDIUMPLUSVETO = 12,
  ELEBDTMEDIUM = 13,
  ELEBDTLOOSEPLUSVETO = 14,
  ELEBDTLOOSE = 15
} OLR;

typedef enum
{
  TOTAL  = 1,
  QCD    = 2,
  WJETS  = 3,
  TES    = 4,
  REMAIN = 5
} CONTIDSYS;

enum EfficiencyCorrectionType
{
  // run 1 enums
  SFJetID          = 1,
  SFContJetID      = 2,
  SFEleID          = 3,

  // run 2 enums
  SFRecoHadTau     = 4,
  // SFRecoJet        = 5,
  // SFRecoElectron   = 6,
  // SFRecoMuon       = 7,

  SFJetIDHadTau    = 8,
  // SFJetIDJet       = 9,
  // SFJetIDElectron  = 10,
  // SFJetIDMuon      = 11,

  SFEleOLRHadTau   = 12,
  // SFEleOLRJet      = 13,
  SFEleOLRElectron = 14,
  // SFEleOLRMuon     = 15,

  SFEleIDHadTau    = 16,
  // SFEleIDJet       = 17,
  // SFEleIDElectron  = 18,
  // SFEleIDMuon      = 19,

  SFTriggerHadTau  = 20,
  // SFEleIDJet       = 21,
  // SFEleIDElectron  = 22,
  // SFEleIDMuon      = 23,
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

typedef enum e_DataPeriodBinning
{
  PeriodBinningUnknown  = 0,
  PeriodBinningAll        = 1,
  PeriodBinningD_EFGH23J  = 2,
  PeriodBinningD_EFH23J_G = 3
} DataPeriodBinning;

}

#endif // TAUANALYSISTOOLS_ENUMS_H
