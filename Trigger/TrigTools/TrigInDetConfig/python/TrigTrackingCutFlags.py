# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import AthenaCommon.SystemOfUnits as Units
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from InDetConfig.TrackingCutsFlags import createTrackingFlags


def __sliceFlags():
    flags = createTrackingFlags()
    flags.minPT = Units.GeV * 0.5 # TODO revise values
    flags.addFlag("seedRadBinWidth", 2)
    flags.addFlag("d0SeedMax", 4.0)
    flags.addFlag("d0SeedPPSMax", 1.7)
    flags.addFlag("doCloneRemoval", True)
    flags.addFlag("doResMon", False)
    flags.addFlag("checkRedundantSeeds", False)
    return flags

def __electronFlags():
    flags = __sliceFlags()
    flags.checkRedundantSeeds=True
    return flags

def createTrigTrackingFlags():
    flags = AthConfigFlags() 
    flags.addFlagsCategory('Trigger.InDetTracking.Electron', __electronFlags, prefix=True)
    return flags

if __name__ == "__main__":
    from AthenaCommon.Logging import logging
    _msg = logging.getLogger('AthConfigFlags')    
    _msg.setLevel(2)
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Trigger.doID
    flags.dump(".*Trig")
    
    flags.Trigger.InDetTracking.Electron.minPT = 2.0 * Units.GeV
    newflags = flags.cloneAndReplace('InDet.Tracking', 'Trigger.InDetTracking.Electron')
    assert newflags.InDet.Tracking.minPT == 2.0 * Units.GeV
    flags.dump()