# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import AthenaCommon.SystemOfUnits as Units
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from InDetConfig.TrackingCutsFlags import createTrackingFlags


def __sliceFlags():
    flags = createTrackingFlags()
    flags.addFlag("seedRadBinWidth", 2)
    flags.addFlag("d0SeedMax", 4.0)
    flags.addFlag("d0SeedPPSMax", 1.7)
    flags.addFlag("doCloneRemoval", True)
    flags.addFlag("doResMon", False)
    flags.addFlag("checkRedundantSeeds", False)
    return flags

def __electronFlags():
    flags = __sliceFlags()
    flags.minPT = Units.GeV * 1.0
    flags.checkRedundantSeeds=True
    return flags

def __muonFlags():
    flags = __sliceFlags()
    flags.minPT=Units.GeV * 1.0
    flags.d0SeedMax = 10.0
    flags.checkRedundantSeeds=True
    flags.doResMon = True
    return flags

def _lrtFlags():
    flags = __sliceFlags()
    flags.maxHoles = 2
    flags.minClusters = 8
    flags.minSiNotShared = 6
    flags.maxShared = 1
    flags.maxPixelHoles = 1
    flags.maxSctHoles = 1
    flags.maxDoubleHoles = 0
    flags.maxPrimaryImpact = 300.0 * Units.mm # highlumi
    flags.maxZImpact = 500.0 * Units.mm
    flags.roadWidth = 5.
    flags.seedFilterLevel = 1
    flags.nHolesMax = flags.maxHoles #fix them together as in the code being ported from. Should be safer if values are adjusted
    flags.nHolesGapMax = 1
    flags.Xi2max = 9.
    flags.Xi2maxNoAdd =25.0
    flags.nWeightedClustersMin = 8
    flags.doZBoundary = True
    flags.maxdImpactSSSSeeds = 300.0

    return flags

def createTrigTrackingFlags():
    flags = AthConfigFlags()
    flags.addFlagsCategory('Trigger.InDetTracking.Electron', __electronFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.Muon', __muonFlags, prefix=True)
    flags.addFlagsCategory('Trigger.InDetTracking.LRT', _lrtFlags, prefix=True)
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
