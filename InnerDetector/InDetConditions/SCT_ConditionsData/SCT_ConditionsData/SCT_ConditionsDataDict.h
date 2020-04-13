/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Usages in Python:

  import ROOT, cppyy
  cppyy.loadDictionary('SCT_ConditionsDataDict')
  bsErrors = ROOT.SCT_ByteStreamErrors
  bsErrors.NUM_ERROR_TYPES

  from ROOT import SCT_ByteStreamErrors
  for i in range(SCT_ByteStreamErrors.NUM_ERROR_TYPES):
    print(SCT_ByteStreamErrors.ErrorTypeDescription[i])
 */

#ifndef SCT_ConditionsDataDict_h
#define SCT_ConditionsDataDict_h

#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"

#endif // SCT_ConditionsDataDict_h
