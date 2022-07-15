# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from SimulationConfig.SimEnums import SimulationFlavour, TruthStrategy
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
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 84.5
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 84.335
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.165
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_Fill7314_BCMSPattern_Flat'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run410000_MC21a_MultiBeamspot'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC21aSingleBeamspot(flags):
    """MC21a flags for MC to match initial Run 3 data (single beamspot version)"""
    MC21a(flags)

    # override only pile-up profile
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run410000_MC21a_SingleBeamspot'


def MC21NoPileUp(flags):
    """MC21a flags for MC to match initial Run 3 data"""
    flags.Beam.NumberOfCollisions = 0.
    flags.Input.ConditionsRunNumber = 410000

    LArConfigRun3NoPileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # radiation damage
    flags.Digitization.DoPixelPlanarRadiationDamage = True


def BeamspotSplitMC21a():
    """MC21a beamspot splitting configuration"""
    substeps = 4
    event_fractions = [0.14, 0.14, 0.14, 0.58]

    return substeps, event_fractions


def MC21SimulationBase(flags):
    """MC21 base flags for simulation"""
    flags.Sim.PhysicsList = 'FTFP_BERT_ATL'
    flags.Sim.TruthStrategy = TruthStrategy.MC15aPlus

    flags.Sim.TRTRangeCut = 30.0
    flags.Sim.TightMuonStepping = True

    from SimuJobTransforms.SimulationHelpers import enableBeamPipeKill, enableFrozenShowersFCalOnly
    enableBeamPipeKill(flags)
    if flags.Sim.ISF.Simulator in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS]:
        enableFrozenShowersFCalOnly(flags)

    from SimuJobTransforms.G4Optimizations import enableG4Optimizations
    enableG4Optimizations(flags)


def MC21Simulation(flags):
    """MC21 flags for simulation"""
    MC21SimulationBase(flags)

    flags.Input.RunNumber = [410000]
    flags.Input.OverrideRunNumber = True
    flags.Input.LumiBlockNumber = [1] # dummy value


def MC21SimulationMultiBeamSpot(flags):
    """MC21 flags for simulation"""
    MC21SimulationBase(flags)

    flags.Input.OverrideRunNumber = True

    from RunDependentSimComps.PileUpUtils import generateRunAndLumiProfile
    generateRunAndLumiProfile(flags,
                              profile= 'RunDependentSimData.PileUpProfile_run410000_MC21a_MultiBeamspot')
