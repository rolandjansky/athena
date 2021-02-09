# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def MC20a(flags):
    """MC20a flags for MC to match 2015 and 2016 data"""
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
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_MC20_2015', 'RunDependentSimData.PileUpProfile_run284500_MC20a')


def MC20d(flags):
    """MC20d flags for MC to match 2017 data"""
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
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_MC20_2017', 'RunDependentSimData.PileUpProfile_run300000_MC20d')


def MC20e(flags):
    """MC20e flags for MC to match 2018 data"""
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
        setupPileUpFlags(flags, 'RunDependentSimData.BunchTrains_MC20_2017', 'RunDependentSimData.PileUpProfile_run310000_MC20e')
