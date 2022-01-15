# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
import AthenaCommon.SystemOfUnits as Units
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from InDetConfig.TrackingPassFlags import createTrackingPassFlags

# for the time when the two config systems coexist we reuse -flags 
from TrigInDetConfig.ConfigSettings import (
    _ConfigSettings_electron, _ConfigSettings_muon, _ConfigSettings_muonLRT, _ConfigSettings_fullScan, _ConfigSettings_muonIso, _ConfigSettings_tau, _ConfigSettings_tauCore, _ConfigSettings_tauIso
)


def __flagsFromConfigSettings(settings):
    flags = createTrackingPassFlags()
    for setting, value in settings.__dict__.items():
        setting = setting.lstrip("_")
        if value is None:
            flags.addFlag(setting, lambda pf: None)
        else:
            flags.addFlag(setting, value)

    flags.addFlag("trkTracks_FTF", f'HLT_IDTrkTrack_{flags.suffix}_FTF')
    flags.addFlag("tracks_FTF", f'HLT_IDTrack_{flags.suffix}_FTF')
    flags.addFlag("trkTracks_IDTrig", f'HLT_IDTrkTrack_{flags.suffix}_IDTrig')
    flags.addFlag("tracks_IDTrig", f"HLT_IDTrack_{flags.suffix}_IDTrig")
    flags.addFlag("refitROT", False) # should likely be moved to ConfigSettingsBase
    flags.addFlag("trtExtensionType", "xf") # should likely be moved to ConfigSettingsBase
    flags.input_name = flags.name
    flags.minPT = flags.pTmin # hack to sync pT threshold used in offline and trigger
    return flags

def __electronFlags():
    return __flagsFromConfigSettings(_ConfigSettings_electron())

def __muonFlags():
    return __flagsFromConfigSettings(_ConfigSettings_muon())

def __muonIsoFlags():
    return __flagsFromConfigSettings(_ConfigSettings_muonIso())

def _muonLRTFlags():
    return __flagsFromConfigSettings(_ConfigSettings_muonLRT())

def __tauFlags():
    return __flagsFromConfigSettings(_ConfigSettings_tau())

def __tauCoreFlags():
    return __flagsFromConfigSettings(_ConfigSettings_tauCore())

def __tauIsoFlags():
    return __flagsFromConfigSettings(_ConfigSettings_tauIso())

def __tauIsoBDTFlags():
    return __flagsFromConfigSettings(_ConfigSettings_tauIso())

def __jetFlags():
    return __flagsFromConfigSettings(_ConfigSettings_fullScan())

def createTrigTrackingPassFlags():
    flags = AthConfigFlags()
    flags.addFlagsCategory('Trigger.InDetTracking.Electron', __electronFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.Muon', __muonFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.MuonIso', __muonIsoFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.MuonFS', __muonFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.MuonLRT', _muonLRTFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.Tau', __tauFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.TauCore', __tauCoreFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.TauIso', __tauIsoFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.TauIsoBDT', __tauIsoBDTFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.jet', __jetFlags, prefix=True)
    return flags


import unittest

class FlagsCopiedTest(unittest.TestCase):
    def setUp(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.doID
        flags.Trigger.InDetTracking.Muon
        flags.Trigger.InDetTracking.Electron.minPT = 2.0 * Units.GeV
        flags.loadAllDynamicFlags()
        self.newflags = flags.cloneAndReplace('InDet.Tracking.ActivePass', 'Trigger.InDetTracking.Electron')

        self.newflags.dump(".*InDet")

    def runTest(self):
        self.assertEqual(self.newflags.InDet.Tracking.ActivePass.minPT, 2.0 * Units.GeV, msg="Flags are not copied")



class UnsetFlagsTest(FlagsCopiedTest):
    def runTest(self):
        self.assertEqual(self.newflags.InDet.Tracking.ActivePass.vertex_jet, None)


if __name__ == "__main__":
    unittest.main()
