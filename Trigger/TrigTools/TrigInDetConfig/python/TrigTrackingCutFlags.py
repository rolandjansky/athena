# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import AthenaCommon.SystemOfUnits as Units
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from InDetConfig.TrackingCutsFlags import createTrackingFlags

# for the time when the two config systems coexist we reuse flags 
from TrigInDetConfig.ConfigSettings import _ConfigSettings_electron, _ConfigSettings_muon, _ConfigSettings_muonLRT
from TrigInDetConfig.ConfigSettingsBase import _ConfigSettingsBase

def genError(setting):
    def _shouldNotBeUsed(f):
        raise RuntimeError(f"Flag {setting} is unset and since there is not good default it can not be used")
    return _shouldNotBeUsed

def copyValues(flags, configClass):
    settings = configClass
    for setting, value in settings.__dict__.items():
        setting = setting.lstrip("_")
        if value is None:
            flags._set(setting, genError(setting) )
        else:
            flags._set(setting, value)

def __sliceFlags():
    flags = createTrackingFlags()
    for setting, value in _ConfigSettingsBase().__dict__.items():
        setting = setting.lstrip("_")
        if value is None:
            flags.addFlag(setting, genError(setting))
        else:
            flags.addFlag(setting, value)
    return flags

def __electronFlags():
    flags = __sliceFlags()
    copyValues(flags, _ConfigSettings_electron())
    flags.minPT = flags.pTmin # hack to sync pT threshold used in offline and trigger
    return flags

def __muonFlags():
    flags = __sliceFlags()
    copyValues(flags, _ConfigSettings_muon())
    flags.minPT = flags.pTmin
    return flags

def _muonLRTFlags():
    flags = __sliceFlags()
    copyValues(flags, _ConfigSettings_muonLRT())
    flags.minPT = flags.pTmin
    return flags

def createTrigTrackingFlags():
    flags = AthConfigFlags()
    flags.addFlagsCategory('Trigger.InDetTracking.Electron', __electronFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.Muon', __muonFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.MuonLRT', _muonLRTFlags, prefix=True)
    return flags


import unittest

class FlagsCopiedTest(unittest.TestCase):
    def setUp(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.doID
        flags.Trigger.InDetTracking.Muon
        flags.Trigger.InDetTracking.Electron.minPT = 2.0 * Units.GeV
        self.newflags = flags.cloneAndReplace('InDet.Tracking', 'Trigger.InDetTracking.Electron')
        self.newflags.dump(".*InDet")

    def runTest(self):
        self.assertEqual(self.newflags.InDet.Tracking.minPT, 2.0 * Units.GeV, msg="Flags are not copied")


class UnsetFlagsRiseTest(FlagsCopiedTest):
    def runTest(self):
        with self.assertRaises(RuntimeError):
            self.newflags.InDet.Tracking.vertex_jet


if __name__ == "__main__":
    unittest.main()
