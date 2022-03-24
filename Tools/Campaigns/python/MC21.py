# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from G4AtlasApps.SimEnums import SimulationFlavour, TruthStrategy
from AthenaConfiguration.Enums import ProductionStep
from LArConfiguration.LArConfigRun3 import LArConfigRun3PileUp, LArConfigRun3NoPileUp


def MC21a(flags):
    """MC21a flags for MC to match initial Run 3 data"""
    flags.Beam.NumberOfCollisions = 60.

    LArConfigRun3PileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # radiation damage
    flags.Digitization.DoPixelPlanarRadiationDamage = True

    # pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 52.
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 51.898
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.102
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run330000_MC21a_SingleBeamspot'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC21aSingleBeamspot(flags):
    """MC21a flags for MC to match initial Run 3 data (single beamspot version)"""
    MC21a(flags)

    # override only pile-up profile
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run330000_MC21a_SingleBeamspot'


def MC21NoPileUp(flags):
    """MC21a flags for MC to match initial Run 3 data"""
    flags.Beam.NumberOfCollisions = 0.

    LArConfigRun3NoPileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # radiation damage
    flags.Digitization.DoPixelPlanarRadiationDamage = True


def BeamspotSplitMC21a():
    """MC21a beamspot splitting configuration"""
    substeps = 3
    event_fractions = [0.3, 0.3, 0.4]

    return substeps, event_fractions


def MC21Simulation(flags):
    """MC21 flags for simulation"""
    flags.Sim.PhysicsList = 'FTFP_BERT_ATL'
    flags.Sim.TruthStrategy = TruthStrategy.MC15aPlus

    flags.Input.RunNumber = [330000]
    flags.Input.OverrideRunNumber = True
    flags.Input.LumiBlockNumber = [1] # dummy value

    flags.Sim.TRTRangeCut = 30.0
    flags.Sim.TightMuonStepping = True

    from SimuJobTransforms.SimulationHelpers import enableBeamPipeKill #, enableFrozenShowersFCalOnly
    enableBeamPipeKill(flags)
    if flags.Sim.ISF.Simulator in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS]:
        # Not tuned yet for G4 10.6
        # enableFrozenShowersFCalOnly(flags)
        pass
    from SimuJobTransforms.G4Optimizations import enableG4Optimizations
    enableG4Optimizations(flags)
