# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.Enums import ProductionStep
from LArConfiguration.LArConfigRun2 import LArConfigRun2


def MC20a(flags):
    """MC20a flags for MC to match 2015 and 2016 data"""
    flags.Beam.NumberOfCollisions = 20. if flags.Digitization.PileUp else 0.
    flags.Digitization.InputBeamSigmaZ = 42

    LArConfigRun2(flags)
    flags.Digitization.HighGainEMECIW = True

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 44.42397
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.07603015
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2015'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run284500_MC20a'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC20d(flags):
    """MC20d flags for MC to match 2017 data"""
    flags.Beam.NumberOfCollisions = 20. if flags.Digitization.PileUp else 0.
    flags.Digitization.InputBeamSigmaZ = 42

    LArConfigRun2(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 80.36246
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.1375377
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run300000_MC20d'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC20e(flags):
    """MC20e flags for MC to match 2018 data"""
    flags.Beam.NumberOfCollisions = 20. if flags.Digitization.PileUp else 0.
    flags.Digitization.InputBeamSigmaZ = 42

    LArConfigRun2(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 99.33
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.17
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run310000_MC20e'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1
