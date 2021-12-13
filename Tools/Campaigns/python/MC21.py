# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.Enums import ProductionStep
from LArConfiguration.LArConfigRun3 import LArConfigRun3PileUp, LArConfigRun3NoPileUp


def MC21a(flags):
    """MC21a flags for MC to match initial Run 3 data"""
    flags.Beam.NumberOfCollisions = 60.

    LArConfigRun3PileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    # TODO: using MC20e pile-up profile for now
    flags.Digitization.PU.NumberOfLowPtMinBias = 99.33
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.17
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run310000_MC20e'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC21NoPileUp(flags):
    """MC21a flags for MC to match initial Run 3 data"""
    flags.Beam.NumberOfCollisions = 0.

    LArConfigRun3NoPileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False


def BeamspotSplitMC21a():
    """MC21a beamspot splitting configuration"""
    substeps = 3
    event_fractions = [0.3, 0.3, 0.4]

    return substeps, event_fractions


def MC21Simulation(flags):
    """MC21 flags for simulation"""
    flags.Sim.PhysicsList = 'FTFP_BERT_ATL'
    flags.Sim.TruthStrategy = 'MC15aPlus'

    flags.Input.RunNumber = [330000]
    flags.Input.OverrideRunNumber = True
    flags.Input.LumiBlockNumber = [1] # dummy value

    flags.Digitization.TRTRangeCut = 30.0
    flags.Sim.TightMuonStepping = True

    from SimuJobTransforms.SimulationHelpers import enableBeamPipeKill #, enableFrozenShowersFCalOnly
    enableBeamPipeKill(flags)
    if 'FullG4' in flags.Sim.ISF.Simulator:
        # Not tuned yet for G4 10.6
        # enableFrozenShowersFCalOnly(flags)
        pass
    from SimuJobTransforms.G4Optimizations import enableG4Optimizations
    enableG4Optimizations(flags)
