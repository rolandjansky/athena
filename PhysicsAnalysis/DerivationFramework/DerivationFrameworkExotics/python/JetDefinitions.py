# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetRec.JetRecStandard import jtm
from JetRec.JetRecFlags import jetFlags
from AthenaCommon.GlobalFlags import globalflags

jtm.modifiersMap["calib_notruth"] = jtm.modifiersMap["calib"]
if globalflags.DataSource()=='geant4':
  jtm.modifiersMap["calib_notruth"].remove('truthassoc')

jtm.modifiersMap["exotJetMods"] = jtm.modifiersMap["dfgroomed"]
if globalflags.DataSource()=='geant4':
  jtm.modifiersMap["exotJetMods"] += [jtm.truthpartondr]

print "EXOT derivations use the calib_notruth modifier for jets defined as: ", jtm.modifiersMap["calib_notruth"]
print "EXOT derivations use the exotJetMods modifier for jets defined as: ", jtm.modifiersMap["exotJetMods"]

