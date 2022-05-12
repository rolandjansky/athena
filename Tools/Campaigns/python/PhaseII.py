# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from G4AtlasApps.SimEnums import TruthStrategy


def PhaseIIPileUpBase(flags, collisions=200):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up"""
    flags.Beam.NumberOfCollisions = collisions

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3PileUp
    LArConfigRun3PileUp(flags)
    flags.LAr.ROD.NumberOfCollisions = collisions  # Run 4 default

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False


def PhaseIIPileUp60(flags):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up, mu=60"""
    PhaseIIPileUpBase(flags, collisions=60)

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 70.
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 69.863
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.137
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.CustomProfile={
        'run': 242006,
        'startmu': 50.0,
        'endmu': 70.0,
        'stepmu': 1.0,
        'startlb': 1,
        'timestamp': 1412006000
    }


def PhaseIIPileUp140(flags):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up, mu=140"""
    PhaseIIPileUpBase(flags, collisions=140)

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 150.
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 149.707
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.293
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.CustomProfile={
        'run': 242014,
        'startmu': 130.0,
        'endmu': 150.0,
        'stepmu': 1.0,
        'startlb': 1,
        'timestamp': 1412014000
    }


def PhaseIIPileUp200(flags):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up, mu=200"""
    PhaseIIPileUpBase(flags, collisions=200)

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 210.
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 209.590
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.410
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.CustomProfile={
        'run': 242020,
        'startmu': 190.0,
        'endmu': 210.0,
        'stepmu': 1.0,
        'startlb': 1,
        'timestamp': 1412020000
    }


def PhaseIINoPileUp(flags):
    """Phase-II Upgrade / Run 4 flags for MC without pile-up"""
    flags.Beam.NumberOfCollisions = 0.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3NoPileUp
    LArConfigRun3NoPileUp(flags)

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False


def PhaseIISimulation(flags):
    """Phase-II Upgrade / Run 4 flags for simulation"""
    flags.Sim.PhysicsList = 'FTFP_BERT_ATL'
    flags.Sim.TruthStrategy = TruthStrategy.MC15aPlus

    flags.Input.RunNumber = [242000]
    flags.Input.OverrideRunNumber = True
    flags.Input.LumiBlockNumber = [1] # dummy value

    flags.Sim.TightMuonStepping = True

    from SimuJobTransforms.SimulationHelpers import enableBeamPipeKill, enableFrozenShowersFCalOnly
    enableBeamPipeKill(flags)
    enableFrozenShowersFCalOnly(flags)
    from SimuJobTransforms.G4Optimizations import enableG4Optimizations
    enableG4Optimizations(flags)
