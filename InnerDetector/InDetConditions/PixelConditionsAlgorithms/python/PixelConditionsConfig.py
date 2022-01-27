"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders,addFoldersSplitOnline

def PixelConfigCondAlgCfg(flags, name="PixelConfigCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelConfigCondAlg"""
    acc = ComponentAccumulator()
    runNum = flags.Input.RunNumber[0]

    # FIXME commented properties are not currently accepted by PixelConfigCondAlg
    CondArgs = {}
    if flags.Beam.Type == "cosmics":
        CondArgs.update(
            BarrelTimeJitter=[25.0,25.0,25.0,25.0],
            EndcapTimeJitter=[25.0,25.0,25.0],
            DBMTimeJitter=[25.0,25.0,25.0],
            BarrelNumberOfBCID=[8,8,8,8],
            EndcapNumberOfBCID=[8,8,8],
            DBMNumberOfBCID=[8,8,8],
            BarrelTimeOffset=[100.0,100.0,100.0,100.0],
            EndcapTimeOffset=[100.0,100.0,100.0],
            DBMTimeOffset=[100.0,100.0,100.0]
        )
    else:
        CondArgs.update(
            BarrelTimeJitter=[0.0,0.0,0.0,0.0],
            EndcapTimeJitter=[0.0,0.0,0.0],
            DBMTimeJitter=[0.0,0.0,0.0],
            BarrelNumberOfBCID=[1,1,1,1],
            EndcapNumberOfBCID=[1,1,1],
            DBMNumberOfBCID=[1,1,1],
            BarrelTimeOffset=[5.0,5.0,5.0,5.0],
            EndcapTimeOffset=[5.0,5.0,5.0],
            DBMTimeOffset=[5.0,5.0,5.0]
        )

    if flags.Input.isMC or flags.Overlay.DataOverlay:
        # Digitization parameters
        CondArgs.update(
            BunchSpace=25.0,
            FEI4BarrelHitDiscConfig=[2],
            ChargeScaleFEI4=1.0,
            UseFEI4SpecialScalingFunction=True
        )
        #====================================================================================
        # Run-dependent SIMULATION(digitization) parameters:
        #====================================================================================
        # RUN2 2015/2016 (mc16a)
        # The pixel conditions are matched with 2016 data (mc16a) at L=17.3fb-1 (run#303638).
        CondArgs.update(
            BarrelToTThreshold2016       = [     -1,      5,      5,      5],
            FEI3BarrelLatency2016        = [      0,    151,    256,    256],
            FEI3BarrelHitDuplication2016 = [  False,  False,  False,  False],
            FEI3BarrelSmallHitToT2016    = [     -1,     -1,     -1,     -1],
            FEI3BarrelTimingSimTune2016  = [     -1,   2015,   2015,   2015],
            BarrelCrossTalk2016          = [   0.30,   0.12,   0.12,   0.12],
            BarrelNoiseOccupancy2016     = [   5e-8,   5e-8,   5e-8,   5e-8],
            BarrelDisableProbability2016 = [   9e-3,   9e-3,   9e-3,   9e-3],
            BarrelLorentzAngleCorr2016   = [    1.0,    1.0,    1.0,    1.0],
            DefaultBarrelBiasVoltage2016 = [   80.0,  350.0,  200.0,  150.0],
            BarrelFluence2016            = [0.80e14,1.61e14,0.71e14,0.48e14],
            BarrelFluenceMap2016 = ["PixelDigitization/maps_IBL_PL_80V_fl0_8e14.root",
                                    "PixelDigitization/maps_PIX_350V_fl1_61e14.root",
                                    "PixelDigitization/maps_PIX_200V_fl0_71e14.root",
                                    "PixelDigitization/maps_PIX_150V_fl0_48e14.root"],
            EndcapToTThreshold2016       = [    5,    5,    5],
            FEI3EndcapLatency2016        = [  256,  256,  256],
            FEI3EndcapHitDuplication2016 = [False,False,False],
            FEI3EndcapSmallHitToT2016    = [   -1,   -1,   -1],
            FEI3EndcapTimingSimTune2016  = [ 2015, 2015, 2015],
            EndcapCrossTalk2016          = [ 0.06, 0.06, 0.06],
            EndcapNoiseOccupancy2016     = [ 5e-8, 5e-8, 5e-8],
            EndcapDisableProbability2016 = [ 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorr2016   = [  1.0,  1.0,  1.0],
            DBMToTThreshold2016       = [   -1,   -1,   -1],
            DBMCrossTalk2016          = [ 0.06, 0.06, 0.06],
            DBMNoiseOccupancy2016     = [ 5e-8, 5e-8, 5e-8],
            DBMDisableProbability2016 = [ 9e-3, 9e-3, 9e-3],
            IBLNoiseShape2016    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 1.0],
            BLayerNoiseShape2016 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
            PixelNoiseShape2016  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
            # Layer-2 noise shape                     [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2129, 0.4016, 0.5477, 0.6599, 0.7435, 0.8160, 0.8779, 0.9340, 0.9798, 1.0]
            # So far, Gaudi::Property does not support 2D vector.
            #EndcapNoiseShape=[[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1748, 0.3409, 0.4760, 0.5850, 0.6754, 0.7538, 0.8264, 0.8962, 0.9655, 1.0],
            #                  [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1852, 0.3528, 0.4881, 0.5961, 0.6855, 0.7640, 0.8374, 0.9068, 0.9749, 1.0],
            #                  [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1735, 0.3380, 0.4733, 0.5829, 0.6730, 0.7516, 0.8234, 0.8916, 0.9595, 1.0]]
        )
        #====================================================================================
        # RUN2 2017 (mc16d)
        # The pixel conditions are matched with 2017 data (mc16d) at L=69.0fb-1 (run#336506).
        CondArgs.update(
            BarrelToTThreshold2017       = [     -1,      5,      5,      5],
            FEI3BarrelLatency2017        = [      0,    151,    256,    256],
            FEI3BarrelHitDuplication2017 = [  False,  False,  False,  False],
            FEI3BarrelSmallHitToT2017    = [     -1,     -1,     -1,     -1],
            FEI3BarrelTimingSimTune2017  = [     -1,   2018,   2018,   2018],
            BarrelCrossTalk2017          = [   0.30,   0.12,   0.12,   0.12],
            BarrelNoiseOccupancy2017     = [   5e-8,   5e-8,   5e-8,   5e-8],
            BarrelDisableProbability2017 = [   9e-3,   9e-3,   9e-3,   9e-3],
            BarrelLorentzAngleCorr2017   = [    1.0,    1.0,    1.0,    1.0],
            DefaultBarrelBiasVoltage2017 = [  350.0,  350.0,  200.0,  150.0],
            BarrelFluence2017            = [3.18e14,3.42e14,1.50e14,1.01e14],
            BarrelFluenceMap2017 = ["PixelDigitization/maps_IBL_PL_350V_fl3_18e14.root",
                                    "PixelDigitization/maps_PIX_350V_fl3_42e14.root",
                                    "PixelDigitization/maps_PIX_200V_fl1_5e14.root",
                                    "PixelDigitization/maps_PIX_150V_fl1_01e14.root"],
            EndcapToTThreshold2017       = [    5,    5,    5],
            FEI3EndcapLatency2017        = [  256,  256,  256],
            FEI3EndcapHitDuplication2017 = [False,False,False],
            FEI3EndcapSmallHitToT2017    = [   -1,   -1,   -1],
            FEI3EndcapTimingSimTune2017  = [ 2018, 2018, 2018],
            EndcapCrossTalk2017          = [ 0.06, 0.06, 0.06],
            EndcapNoiseOccupancy2017     = [ 5e-8, 5e-8, 5e-8],
            EndcapDisableProbability2017 = [ 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorr2017   = [  1.0,  1.0,  1.0],
            DBMToTThreshold2017       = [   -1,   -1,   -1],
            DBMCrossTalk2017          = [ 0.06, 0.06, 0.06],
            DBMNoiseOccupancy2017     = [ 5e-8, 5e-8, 5e-8],
            DBMDisableProbability2017 = [ 9e-3, 9e-3, 9e-3],
            IBLNoiseShape2017    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 1.0],
            BLayerNoiseShape2017 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
            PixelNoiseShape2017  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
        )
        #====================================================================================
        # RUN2 2018 (mc16e)
        # The pixel conditions are matched with 2018 data (mc16e) at L=119.4fb-1 (run#357193).
        CondArgs.update(
            BarrelToTThreshold2018       = [     -1,      3,      5,      5],
            FEI3BarrelLatency2018        = [      0,    151,    256,    256],
            FEI3BarrelHitDuplication2018 = [  False,  False,  False,  False],
            FEI3BarrelSmallHitToT2018    = [     -1,     -1,     -1,     -1],
            FEI3BarrelTimingSimTune2018  = [     -1,   2018,   2018,   2018],
            BarrelCrossTalk2018          = [   0.30,   0.12,   0.12,   0.12],
            BarrelNoiseOccupancy2018     = [   5e-8,   5e-8,   5e-8,   5e-8],
            BarrelDisableProbability2018 = [   9e-3,   9e-3,   9e-3,   9e-3],
            BarrelLorentzAngleCorr2018   = [    1.0,    1.0,    1.0,    1.0],
            DefaultBarrelBiasVoltage2018 = [  400.0,  400.0,  250.0,  250.0],
            BarrelFluence2018            = [5.50e14,5.19e14,2.28e14,1.53e14],
            BarrelFluenceMap2018 = ["PixelDigitization/maps_IBL_PL_400V_fl5_5e14.root",
                                    "PixelDigitization/maps_PIX_400V_fl5_19e14.root",
                                    "PixelDigitization/maps_PIX_250V_fl2_28e14.root",
                                    "PixelDigitization/maps_PIX_250V_fl1_53e14.root"],
            EndcapToTThreshold2018       = [    5,    5,    5],
            FEI3EndcapLatency2018        = [  256,  256,  256],
            FEI3EndcapHitDuplication2018 = [False,False,False],
            FEI3EndcapSmallHitToT2018    = [   -1,   -1,   -1],
            FEI3EndcapTimingSimTune2018  = [ 2018, 2018, 2018],
            EndcapCrossTalk2018          = [ 0.06, 0.06, 0.06],
            EndcapNoiseOccupancy2018     = [ 5e-8, 5e-8, 5e-8],
            EndcapDisableProbability2018 = [ 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorr2018   = [  1.0,  1.0,  1.0],
            DBMToTThreshold2018       = [   -1,   -1,   -1],
            DBMCrossTalk2018          = [ 0.06, 0.06, 0.06],
            DBMNoiseOccupancy2018     = [ 5e-8, 5e-8, 5e-8],
            DBMDisableProbability2018 = [ 9e-3, 9e-3, 9e-3],
            IBLNoiseShape2018    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 1.0],
            BLayerNoiseShape2018 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
            PixelNoiseShape2018  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
        )
        #====================================================================================
        # RUN3 2022
        # The pixel conditions are matched with 2022 data (mc16e) at L=119.4fb-1 (run#357193).
        CondArgs.update(
            BarrelToTThreshold2022       = [     -1,      3,      5,      5],
            FEI3BarrelLatency2022        = [      0,    151,    256,    256],
            FEI3BarrelHitDuplication2022 = [  False,  False,  False,  False],
            FEI3BarrelSmallHitToT2022    = [     -1,     -1,     -1,     -1],
            FEI3BarrelTimingSimTune2022  = [     -1,   2018,   2018,   2018],
            BarrelCrossTalk2022          = [   0.30,   0.12,   0.12,   0.12],
            BarrelNoiseOccupancy2022     = [   5e-8,   5e-8,   5e-8,   5e-8],
            BarrelDisableProbability2022 = [   9e-3,   9e-3,   9e-3,   9e-3],
            BarrelLorentzAngleCorr2022   = [    1.0,    1.0,    1.0,    1.0],
            DefaultBarrelBiasVoltage2022 = [  400.0,  400.0,  250.0,  250.0],
            BarrelFluence2022            = [5.50e14,5.19e14,2.28e14,1.53e14],
            BarrelFluenceMap2022 = ["PixelDigitization/maps_IBL_PL_450V_fl7_2e14.root",
                                    "PixelDigitization/maps_PIX_450V_fl6_8e14.root",
                                    "PixelDigitization/maps_PIX_300V_fl3e14.root",
                                    "PixelDigitization/maps_PIX_300V_fl2e14.root"],
            Barrel3DFluenceMap2022 = ["PixelDigitization/maps_IBL_3D_60V_fl7_54e14_LHCb.root"],
            EndcapToTThreshold2022       = [    5,    5,    5],
            FEI3EndcapLatency2022        = [  256,  256,  256],
            FEI3EndcapHitDuplication2022 = [False,False,False],
            FEI3EndcapSmallHitToT2022    = [   -1,   -1,   -1],
            FEI3EndcapTimingSimTune2022  = [ 2018, 2018, 2018],
            EndcapCrossTalk2022          = [ 0.06, 0.06, 0.06],
            EndcapNoiseOccupancy2022     = [ 5e-8, 5e-8, 5e-8],
            EndcapDisableProbability2022 = [ 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorr2022   = [  1.0,  1.0,  1.0],
            DBMToTThreshold2022       = [   -1,   -1,   -1],
            DBMCrossTalk2022          = [ 0.06, 0.06, 0.06],
            DBMNoiseOccupancy2022     = [ 5e-8, 5e-8, 5e-8],
            DBMDisableProbability2022 = [ 9e-3, 9e-3, 9e-3],
            IBLNoiseShape2022    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 0.9918, 1.0],
            BLayerNoiseShape2022 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
            PixelNoiseShape2022  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
        )
        #====================================================================================
        # RUN1
        CondArgs.update(
            BarrelToTThresholdRUN1       = [      3,      3,      3],
            FEI3BarrelLatencyRUN1        = [    256,    256,    256],
            FEI3BarrelHitDuplicationRUN1 = [   True,   True,   True],
            FEI3BarrelSmallHitToTRUN1    = [      7,      7,      7],
            FEI3BarrelTimingSimTuneRUN1  = [   2009,   2009,   2009],
            BarrelCrossTalkRUN1          = [   0.06,   0.06,   0.06],
            BarrelNoiseOccupancyRUN1     = [   5e-8,   5e-8,   5e-8],
            BarrelDisableProbabilityRUN1 = [   9e-3,   9e-3,   9e-3],
            BarrelLorentzAngleCorrRUN1   = [    1.0,    1.0,    1.0],
            DefaultBarrelBiasVoltageRUN1 = [  150.0,  150.0,  150.0],
            BarrelFluenceRUN1            = [1.01e14,0.44e14,0.30e14],
            BarrelFluenceMapRUN1 = ["PixelDigitization/maps_PIX_150V_fl1_01e14.root",
                                    "PixelDigitization/maps_PIX_150V_fl0_44e14.root",
                                    "PixelDigitization/maps_PIX_150V_fl0_3e14.root"],
            EndcapToTThresholdRUN1       = [    3,    3,    3],
            FEI3EndcapLatencyRUN1        = [  256,  256,  256],
            FEI3EndcapHitDuplicationRUN1 = [ True, True, True],
            FEI3EndcapSmallHitToTRUN1    = [    7,    7,    7],
            FEI3EndcapTimingSimTuneRUN1  = [ 2009, 2009, 2009],
            EndcapCrossTalkRUN1          = [ 0.06, 0.06, 0.06],
            EndcapNoiseOccupancyRUN1     = [ 5e-8, 5e-8, 5e-8],
            EndcapDisableProbabilityRUN1 = [ 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorrRUN1   = [  1.0,  1.0,  1.0],
            BLayerNoiseShapeRUN1 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
            PixelNoiseShapeRUN1  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
        )
        #====================================================================================
        # ITK
        CondArgs.update(
            BarrelToTThresholdITK       = [     3,     3,     3,     3,     3],
            BarrelCrossTalkITK          = [  0.06,  0.06,  0.06,  0.06,  0.06],
            BarrelNoiseOccupancyITK     = [  5e-8,  5e-8,  5e-8,  5e-8,  5e-8],
            BarrelDisableProbabilityITK = [  9e-3,  9e-3,  9e-3,  9e-3,  9e-3],
            BarrelLorentzAngleCorrITK   = [   1.0,   1.0,   1.0,   1.0,   1.0],
            DefaultBarrelBiasVoltageITK = [ 150.0, 150.0, 150.0, 150.0, 150.0],
            BarrelFluenceITK            = [0.0e14,0.0e14,0.0e14,0.0e14,0.0e14],
            BarrelFluenceMapITK = ["PixelDigitization/maps_IBL_PL_80V_fl0e14.root",
                                   "PixelDigitization/maps_IBL_PL_80V_fl0e14.root",
                                   "PixelDigitization/maps_IBL_PL_80V_fl0e14.root",
                                   "PixelDigitization/maps_IBL_PL_80V_fl0e14.root",
                                   "PixelDigitization/maps_IBL_PL_80V_fl0e14.root"],
            EndcapToTThresholdITK       = [    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3],
            EndcapCrossTalkITK          = [ 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06],
            EndcapNoiseOccupancyITK     = [ 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8],
            EndcapDisableProbabilityITK = [ 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3],
            EndcapLorentzAngleCorrITK   = [  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0],
            InnermostNoiseShapeITK     = [0.0, 1.0],
            NextInnermostNoiseShapeITK = [0.0, 1.0],
            PixelNoiseShapeITK         = [0.0, 1.0]
        )
    else:
        # for data, make sure no Lorentz angle correction
        CondArgs.update(
            BarrelLorentzAngleCorr2016 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2016 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorr2017 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2017 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorr2018 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2018 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorrRUN1 = [  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorrRUN1 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorrITK  = [  1.0,  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorrITK  = [  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0]
        )

    # Charge calibration parameters
    CondArgs.update(
        DefaultCalibrationParameterA=70.2,
        DefaultCalibrationParameterE=-3561.25,
        DefaultCalibrationParameterC=26000.0
    )
    # DCS parameters
    CondArgs.update(
        DefaultBiasVoltage=150.0,
        DefaultTemperature=-7.0
    )
    # Cabling parameters
    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    if flags.Input.isMC:
        # ITk:
        if flags.GeoModel.Run not in ['RUN1', 'RUN2', 'RUN3']: # RUN4 and beyond
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
            if flags.GeoModel.Type == "BrlIncl4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
            elif flags.GeoModel.Type == "IBrlExt4.0ref":
                IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt3.2_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif flags.GeoModel.Run == "RUN2" or flags.GeoModel.Run == "RUN3":
            # Planar IBL
            if flags.GeoModel.IBLLayout == "planar":
                if flags.GeoModel.Run == "RUN2":
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif flags.GeoModel.IBLLayout == "3D":
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        else:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
    else:
        if runNum < 222222:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
        else:
            # Even though we are reading from COOL, set the correct fallback map.
            if (runNum >= 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
            elif (runNum >= 314940 and runNum < 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
            elif (runNum >= 289350 and runNum < 314940): # 2016
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
            elif (runNum >= 222222 and runNum < 289350): # 2015
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
            else:
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"

    CondArgs.update(
        CablingMapToFile=False,
        CablingMapFileName=IdMappingDat
    )

    # set default kwargs
    CondArgs.update(kwargs)
    acc.addCondAlgo(CompFactory.PixelConfigCondAlg(name, **CondArgs))
    return acc


def PixelAlignCondAlgCfg(flags, name="PixelAlignCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelAlignCondAlg"""
    from PixelGeoModel.PixelGeoModelConfig import PixelGeoModelCfg
    acc = PixelGeoModelCfg(flags)

    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))

    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)
    kwargs.setdefault("ReadKeyStatic", "/Indet/Align")
    kwargs.setdefault("ReadKeyDynamicL1", "/Indet/AlignL1/ID")
    kwargs.setdefault("ReadKeyDynamicL2", "/Indet/AlignL2/PIX")
    kwargs.setdefault("ReadKeyDynamicL3", "/Indet/AlignL3")
    kwargs.setdefault("ReadKeyIBLDist", "/Indet/IBLDist")
    kwargs.setdefault("WriteKey", "PixelAlignmentStore")

    acc.addCondAlgo(CompFactory.PixelAlignCondAlg(name, **kwargs))
    return acc

def PixelCablingCondAlgCfg(flags, name="PixelCablingCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))

    if not flags.Input.isMC and not flags.Overlay.DataOverlay:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList"))
        kwargs.setdefault("ReadKey", "/PIXEL/CablingMap")
        if flags.Input.RunNumber[0]<222222:
            kwargs.setdefault("ReadKey", "")
    else:
        kwargs.setdefault("ReadKey", "")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("PixelReadoutSpeedData", "PixelReadoutSpeedData")
    kwargs.setdefault("WriteKey", "PixelCablingCondData")
    acc.addCondAlgo(CompFactory.PixelCablingCondAlg(name, **kwargs))
    return acc

def PixelChargeCalibCondAlgCfg(flags, name="PixelChargeCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection"))
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg(flags))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/PixCalib")
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeCalibCondAlg(name, **kwargs))
    return acc

def PixelChargeLUTCalibCondAlgCfg(flags, name="PixelChargeLUTCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeLUTCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/ChargeCalibration", "/PIXEL/ChargeCalibration", className="CondAttrListCollection"))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/ChargeCalibration")
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeLUTCalibCondAlg(name, **kwargs))
    return acc

def PixelDCSCondHVAlgCfg(flags, name="PixelDCSCondHVAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondHVAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.update( ReadKey="/PIXEL/HLT/DCS/HV")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        kwargs.update( ReadKey="/PIXEL/DCS/HV")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))

    kwargs.setdefault("WriteKey", "PixelDCSHVCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondHVAlg(name, **kwargs))
    return acc

def PixelDCSCondStateAlgCfg(flags, name="PixelDCSCondStateAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStateAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.InDet.usePixelDCS:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATE", "DCS_OFL", className="CondAttrListCollection"))
        kwargs.setdefault("ReadKeyState", "/PIXEL/DCS/FSMSTATE")
    else:
        kwargs.setdefault("ReadKeyState", "")
    kwargs.setdefault("WriteKeyState", "PixelDCSStateCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStateAlg(name, **kwargs))
    return acc

def PixelDCSCondStatusAlgCfg(flags, name="PixelDCSCondStatusAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStatusAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.InDet.usePixelDCS:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATUS", "DCS_OFL", className="CondAttrListCollection"))
        kwargs.setdefault("ReadKeyStatus", "/PIXEL/DCS/FSMSTATUS")
    else:
        kwargs.setdefault("ReadKeyStatus", "")
    kwargs.setdefault("WriteKeyStatus", "PixelDCSStatusCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStatusAlg(name, **kwargs))
    return acc

def PixelDCSCondTempAlgCfg(flags, name="PixelDCSCondTempAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondTempAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.setdefault("ReadKey", "/PIXEL/HLT/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        kwargs.setdefault("ReadKey", "/PIXEL/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteKey", "PixelDCSTempCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondTempAlg(name, **kwargs))
    return acc

def PixelDeadMapCondAlgCfg(flags, name="PixelDeadMapCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDeadMapCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run == "RUN1":
        kwargs.setdefault("ReadKey", "")
    else:
        kwargs.setdefault("ReadKey", "/PIXEL/PixelModuleFeMask")
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelModuleFeMask", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection"))
    kwargs.setdefault("WriteKey", "PixelDeadMapCondData")
    acc.addCondAlgo(CompFactory.PixelDeadMapCondAlg(name, **kwargs))
    return acc

def PixelDetectorElementCondAlgCfg(flags, name="PixelDetectorElementCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDetectorElementCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelAlignCondAlgCfg(flags))
    
    kwargs.setdefault("PixelAlignmentStore", "PixelAlignmentStore")
    kwargs.setdefault("WriteKey", "PixelDetectorElementCollection")

    # FIXME
    # add artifical dependencies to SCT, TRT and Muon
    # conditions algs to ensure that the IOV
    # is identical to the IOV of the tracking geometry
    if flags.Detector.GeometryMuon and flags.Muon.enableAlignment:
        from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg
        acc.merge(MuonDetectorCondAlgCfg(flags))
        kwargs.setdefault("MuonManagerKey", "MuonDetectorManager")
    if flags.Detector.GeometryTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        acc.merge(TRT_ReadoutGeometryCfg(flags))
        kwargs.setdefault("TRT_DetEltContKey", "TRT_DetElementContainer")
    if not flags.GeoModel.Align.LegacyConditionsAccess and flags.Detector.GeometrySCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_AlignmentCfg
        acc.merge(SCT_AlignmentCfg(flags))
        kwargs.setdefault("SCTAlignmentStore", "SCTAlignmentStore")
    # end of hack

    acc.addCondAlgo(CompFactory.PixelDetectorElementCondAlg(name, **kwargs))    
    return acc

def PixelDistortionAlgCfg(flags, name="PixelDistortionAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDistortionAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/PixelDist","/Indet/PixelDist",className="DetCondCFloat"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/Indet/PixelDist")
    kwargs.setdefault("WriteKey", "PixelDistortionData")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.addCondAlgo(CompFactory.PixelDistortionAlg(name, **kwargs))
    return acc

def PixelHitDiscCnfgAlgCfg(flags, name="PixelHitDiscCnfgAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelHitDiscCnfgAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    # not for Run-1 data/MC
    if flags.GeoModel.IBLLayout in ("noIBL", "UNDEFINED"):
        return acc
    if (flags.IOVDb.DatabaseInstance=="CONDBR2"):
        acc.merge(addFolders(flags, "/PIXEL/HitDiscCnfg", "PIXEL", className="AthenaAttributeList"))
    elif (flags.Input.isMC and flags.GeoModel.Run=="RUN2") or (flags.Input.isMC and flags.GeoModel.Run=="RUN3"):
        acc.merge(addFoldersSplitOnline(flags,"PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/HitDiscCnfg")
    kwargs.setdefault("WriteKey", "PixelHitDiscCnfgData")
    acc.addCondAlgo(CompFactory.PixelHitDiscCnfgAlg(name, **kwargs))
    return acc

def PixelOfflineCalibCondAlgCfg(flags, name="PixelOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelOfflineCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixReco", "/PIXEL/PixReco", className="DetCondCFloat"))
    kwargs.setdefault("InputSource", 1 if flags.Common.isOnline else 2)
    kwargs.setdefault("PixelClusterErrorDataFile", "PixelClusterErrorData.txt")
    kwargs.setdefault("PixelClusterOnTrackErrorDataFile", "PixelClusterOnTrackErrorData.txt")
    kwargs.setdefault("PixelChargeInterpolationDataFile", "PixelChargeInterpolationData.txt")
    kwargs.setdefault("DumpConstants", 0)
    kwargs.setdefault("ReadKey", "/PIXEL/PixReco")
    kwargs.setdefault("WriteKey", "PixelOfflineCalibData")
    acc.addCondAlgo(CompFactory.PixelOfflineCalibCondAlg(name, **kwargs))
    return acc

def PixelReadoutSpeedAlgCfg(flags, name="PixelReadoutSpeedAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelReadoutSpeedAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/ReadoutSpeed", "PIXEL", className="AthenaAttributeList"))
    else:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList"))
    kwargs.setdefault("ReadKey", "/PIXEL/ReadoutSpeed")
    kwargs.setdefault("WriteKey", "PixelReadoutSpeedData")
    acc.addCondAlgo(CompFactory.PixelReadoutSpeedAlg(name, **kwargs))
    return acc

def PixelRadSimFluenceMapAlgCfg(flags, name="PixelRadSimFluenceMapAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelRadSimFluenceMapAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteRadiationFluenceMapKey", "PixelRadiationDamageFluenceMapData")
    acc.addCondAlgo(CompFactory.PixelRadSimFluenceMapAlg(name, **kwargs))
    return acc
