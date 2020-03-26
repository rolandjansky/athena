/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Usages in Python:

  import ROOT, cppyy
  cppyy.loadDictionary("SCT_MonitoringDict")
  sctMon = ROOT.SCT_Monitoring
  sctMon.N_REGIONS

  from ROOT import SCT_Monitoring as sctMon
  for i in range(sctMon.N_REGIONS):
    print(sctMon.subDetName[i])
*/

#ifndef SCT_MonitoringDict_h
#define SCT_MonitoringDict_h

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#endif // SCT_MonitoringDict_h
