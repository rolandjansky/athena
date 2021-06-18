# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from LArConfiguration.LArConfigRun2 import LArConfigRun2


def MC16a(flags):
    """MC16a flags for MC to match 2015 and 2016 data"""
    flags.Beam.NumberOfCollisions = 20.

    LArConfigRun2(flags)
    flags.Digitization.HighGainEMECIW = True

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 44.3839246425
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.116075313
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2015'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run284500_MC16a'


def MC16d(flags):
    """MC16d flags for MC to match 2017 data"""
    flags.Beam.NumberOfCollisions = 20.

    LArConfigRun2(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 80.290021063135
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.2099789464
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run300000_MC16d'


def MC16e(flags):
    """MC16e flags for MC to match 2018 data"""
    flags.Beam.NumberOfCollisions = 20.

    LArConfigRun2(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 99.2404608
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.2595392
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run310000_MC16e'
