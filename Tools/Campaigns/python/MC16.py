# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from G4AtlasApps.SimEnums import SimulationFlavour, TruthStrategy
from LArConfiguration.LArConfigRun2 import LArConfigRun2PileUp, LArConfigRun2NoPileUp


def MC16a(flags):
    """MC16a flags for MC to match 2015 and 2016 data"""
    flags.Beam.NumberOfCollisions = 20.

    LArConfigRun2PileUp(flags)
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

    LArConfigRun2PileUp(flags)

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

    LArConfigRun2PileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 99.2404608
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.2595392
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run310000_MC16e'


def MC16NoPileUp(flags):
    """MC16 flags for MC without pile-up"""
    flags.Beam.NumberOfCollisions = 0.

    LArConfigRun2NoPileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False


def MC16Simulation(flags):
    """MC16 flags for simulation"""
    flags.Sim.PhysicsList = 'FTFP_BERT_ATL'
    flags.Sim.TruthStrategy = TruthStrategy.MC15aPlus

    flags.Input.RunNumber = [284500]
    flags.Input.OverrideRunNumber = True
    flags.Input.LumiBlockNumber = [1] # dummy value

    flags.Sim.TRTRangeCut = 30.0
    flags.Sim.TightMuonStepping = True

    from SimuJobTransforms.SimulationHelpers import enableBeamPipeKill, enableFrozenShowersFCalOnly
    enableBeamPipeKill(flags)
    if flags.Sim.ISF.Simulator in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS]:
        enableFrozenShowersFCalOnly(flags)
