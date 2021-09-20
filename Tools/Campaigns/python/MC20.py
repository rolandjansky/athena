# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.Enums import ProductionStep
from LArConfiguration.LArConfigRun2 import LArConfigRun2PileUp, LArConfigRun2NoPileUp


def MC20a(flags):
    """MC20a flags for MC to match 2015 and 2016 data"""
    flags.Beam.NumberOfCollisions = 20.
    flags.Digitization.InputBeamSigmaZ = 42

    flags.Digitization.UseUpdatedTGCConditions = True

    LArConfigRun2PileUp(flags)
    flags.Digitization.HighGainEMECIW = True

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 56.5
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 56.390
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.110
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2015'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run284500_MC20a'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC20d(flags):
    """MC20d flags for MC to match 2017 data"""
    flags.Beam.NumberOfCollisions = 20.
    flags.Digitization.InputBeamSigmaZ = 42

    flags.Digitization.UseUpdatedTGCConditions = True

    LArConfigRun2PileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 90.5
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 90.323
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.177
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run300000_MC20d'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC20e(flags):
    """MC20e flags for MC to match 2018 data"""
    flags.Beam.NumberOfCollisions = 20.
    flags.Digitization.InputBeamSigmaZ = 42

    flags.Digitization.UseUpdatedTGCConditions = True

    LArConfigRun2PileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    ## pile-up
    # These numbers are based upon a relative XS scaling of the high-pt slice
    # of 64%, which leads to a relative high-pt / low-pt sampling of
    # 0.001953314389 / 0.9980466856. Those numbers are then multiplied by 90.5
    # to follow pile-up profile. Only a relevant number of significant digits
    # are kept.
    flags.Digitization.PU.NumberOfLowPtMinBias = 90.323
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.177
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.ProfileConfig = 'RunDependentSimData.PileUpProfile_run310000_MC20e'

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        # ensure better randomisation of high-pt minbias events
        flags.Digitization.PU.HighPtMinBiasInputColOffset = -1


def MC20NoPileUp(flags):
    """MC20e flags for MC without pile-up"""
    flags.Beam.NumberOfCollisions = 0
    flags.Digitization.InputBeamSigmaZ = 42

    flags.Digitization.UseUpdatedTGCConditions = True

    LArConfigRun2NoPileUp(flags)

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False
