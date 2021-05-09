# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def MC16a(flags):
    """MC16a flags for MC to match 2015 and 2016 data"""
    flags.Beam.NumberOfCollisions = 20.

    flags.LAr.ROD.DoOFCPileupOptimization = True
    flags.LAr.ROD.FirstSample = 0  # default
    flags.LAr.ROD.nSamples = 4
    flags.LAr.ROD.NumberOfCollisions = 20
    flags.LAr.ROD.UseHighestGainAutoCorr = True

    flags.Digitization.HighGainEMECIW = True

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    if flags.Digitization.PileUp:
        from Digitization.PileUpUtils import setupPileUpFlags
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_2015', 'RunDependentSimData.PileUpProfile_run284500_MC16a')


def MC16d(flags):
    """MC16d flags for MC to match 2017 data"""
    flags.Beam.NumberOfCollisions = 20.

    flags.LAr.ROD.DoOFCPileupOptimization = True
    flags.LAr.ROD.FirstSample = 0  # default
    flags.LAr.ROD.nSamples = 4
    flags.LAr.ROD.NumberOfCollisions = 20
    flags.LAr.ROD.UseHighestGainAutoCorr = True

    flags.Digitization.HighGainEMECIW = False

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    if flags.Digitization.PileUp:
        from Digitization.PileUpUtils import setupPileUpFlags
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_2017', 'RunDependentSimData.PileUpProfile_run300000_MC16d')


def MC16e(flags):
    """MC16e flags for MC to match 2018 data"""
    flags.Beam.NumberOfCollisions = 20.

    flags.LAr.ROD.DoOFCPileupOptimization = True
    flags.LAr.ROD.FirstSample = 0  # default
    flags.LAr.ROD.nSamples = 4
    flags.LAr.ROD.NumberOfCollisions = 20
    flags.LAr.ROD.UseHighestGainAutoCorr = True

    flags.Digitization.HighGainEMECIW = False

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    if flags.Digitization.PileUp:
        from Digitization.PileUpUtils import setupPileUpFlags
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_2017', 'RunDependentSimData.PileUpProfile_run310000_MC16e')
