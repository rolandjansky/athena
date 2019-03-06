# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from HIJetRec.HIJetRecFlags import HIJetFlags
import AthenaCommon.SystemOfUnits as Units
from RecExConfig.RecFlags import rec
from JetRec.JetRecFlags import jetFlags

def SetHIPMode() :
    if not rec.doHIP() : return
    print 'Setting HIJetRec in HIPMode'
    HIJetFlags.HarmonicsForSubtraction.set_Value_and_Lock([])
    HIJetFlags.ExtraFlowMoments.set_Value_and_Lock(False)
    HIJetFlags.SeedPtMin.set_Value_and_Lock(8*Units.GeV)
    HIJetFlags.RecoOutputPtMin.set_Value_and_Lock(8*Units.GeV)
    HIJetFlags.TrackJetPtMin.set_Value_and_Lock(4*Units.GeV)
    HIJetFlags.ApplyTowerEtaPhiCorrection.set_Value_and_Lock(False)

def SetDebugMode():
    print 'Setting HIJetRec in DEBUG mode'
    HIJetFlags.ExtraFlowMoments.set_Value_and_Lock(True)
    HIJetFlags.RecoOutputPtMin.set_Value_and_Lock(8*Units.GeV)
    HIJetFlags.WriteSeeds.set_Value_and_Lock(True)
    jetFlags.debug.set_Value_and_Lock(2)
    jetFlags.timeJetRecTool.set_Value_and_Lock(2)
    jetFlags.timeJetToolRunner.set_Value_and_Lock(2)

