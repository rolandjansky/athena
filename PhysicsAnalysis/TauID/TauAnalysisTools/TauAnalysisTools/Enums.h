/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENUMS_H
#define ENUMS_H

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
  SFJetID     = 1,
  SFContJetID = 2,
  SFEleID     = 3
};

enum EfficiencyCorrectionSystematicType
{
  STAT = 1,
  SYST = 2
};

}

#endif // ENUMS_H
