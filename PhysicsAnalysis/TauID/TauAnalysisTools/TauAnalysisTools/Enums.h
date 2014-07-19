/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENUMS_H
#define ENUMS_H

namespace TauAnalysisTools{
  typedef enum e_JETID {
    JETIDNONE,
    JETIDBDTLOOSE,
    JETIDBDTMEDIUM,
    JETIDBDTTIGHT,
    JETIDBDTFAIL,
    JETIDBDTOTHER,
    JETIDLLHLOOSE,
    JETIDLLHMEDIUM,
    JETIDLLHTIGHT,
    JETIDLLHFAIL
  } JETID;

  typedef enum e_ELEID{
    EVETONONE,
    EVETOBDTLOOSE,
    EVETOBDTMEDIUM,
    EVETOBDTTIGHT,
    EVETOOTHER
  } EVETO;

  typedef enum {
    OLRNONE,
    OLRLOOSEPP,
    OLRMEDIUMPP,
    OLRTIGHTPP,
    OLROTHER
  } OLR;

  typedef enum{
    TOTAL,
    QCD,
    WJETS,
    TES,
    REMAIN
  } CONTIDSYS;

  enum EfficiencyCorrectionType {
    SFJetID     = 1,
    SFContJetID = 2,
    SFEleVeto   = 3    
  };
}

#endif // ENUMS_H
