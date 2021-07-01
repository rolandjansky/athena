# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def PhaseIIPileUp(flags):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up"""
    flags.Beam.NumberOfCollisions = 200.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3PileUp
    LArConfigRun3PileUp(flags)
    flags.LAr.ROD.NumberOfCollisions = 200  # Run 4 default

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    # TODO


def PhaseIINoPileUp(flags):
    """Phase-II Upgrade / Run 4 flags for MC without pile-up"""
    flags.Beam.NumberOfCollisions = 0.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3NoPileUp
    LArConfigRun3NoPileUp(flags)

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False
