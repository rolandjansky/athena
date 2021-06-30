# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def PhaseII(flags):
    """Phase-II Upgrade / Run 4 flags for MC """
    flags.Beam.NumberOfCollisions = 200. if flags.Digitization.PileUp else 0.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3
    LArConfigRun3(flags)
    flags.LAr.ROD.NumberOfCollisions = 200 if flags.Digitization.PileUp else 0  # Run 4 default

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    # TODO
