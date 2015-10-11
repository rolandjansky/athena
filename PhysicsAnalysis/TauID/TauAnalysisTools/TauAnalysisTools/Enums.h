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
  JETIDBDTNOTLOOSE       = 14
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
  OLRNONE     = 1,
  OLRLOOSEPP  = 2,
  OLRMEDIUMPP = 3,
  OLRTIGHTPP  = 4,
  OLROTHER    = 5
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
  TruthJet         = 5
} TruthMatchedParticleType;

}

#endif // TAUANALYSISTOOLS_ENUMS_H
