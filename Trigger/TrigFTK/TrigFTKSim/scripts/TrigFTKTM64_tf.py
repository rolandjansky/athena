#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 579246 2014-01-20 17:48:57Z gvolpi $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigFTKSim.FTKSimOptions import *
 
ntowers = 64
subregions = 1

ListOfDefaultPositionalKeys=['--AMI', '--CachePath', '--CachedBank', '--DBBankLevel', '--DoRoadFile', '--FTKDoGrid', '--FTKForceAllInput', '--FTKSetupTag', '--FTKUnmergedInputPath', '--HWNDiff', '--HitWarrior', '--IBLMode', '--MakeCache', '--PixelClusteringMode', '--RoadFilesDir', '--SSFAllowExtraMiss', '--SSFMultiConnection', '--SSFNConnections', '--SSFTRDefn', '--SSFTRMaxEta', '--SSFTRMinEta', '--SaveRoads', '--SctClustering', '--SecondStageFit', '--SetAMSize', '--TRACKFITTER_MODE', '--TSPMinCoverage', '--TSPSimulationLevel', '--UseTSPBank', '--badmap_path', '--badmap_path_for_hit', '--bankpatterns', '--execOnly', '--fit711constantspath', '--fitconstantspath', '--ignoreErrors', '--inputNTUP_FTKIPFile', '--inputNTUP_FTKTMP_0_0File', '--inputNTUP_FTKTMP_0_1File', '--inputNTUP_FTKTMP_0_2File', '--inputNTUP_FTKTMP_0_3File', '--inputNTUP_FTKTMP_10_0File', '--inputNTUP_FTKTMP_10_1File', '--inputNTUP_FTKTMP_10_2File', '--inputNTUP_FTKTMP_10_3File', '--inputNTUP_FTKTMP_11_0File', '--inputNTUP_FTKTMP_11_1File', '--inputNTUP_FTKTMP_11_2File', '--inputNTUP_FTKTMP_11_3File', '--inputNTUP_FTKTMP_12_0File', '--inputNTUP_FTKTMP_12_1File', '--inputNTUP_FTKTMP_12_2File', '--inputNTUP_FTKTMP_12_3File', '--inputNTUP_FTKTMP_13_0File', '--inputNTUP_FTKTMP_13_1File', '--inputNTUP_FTKTMP_13_2File', '--inputNTUP_FTKTMP_13_3File', '--inputNTUP_FTKTMP_14_0File', '--inputNTUP_FTKTMP_14_1File', '--inputNTUP_FTKTMP_14_2File', '--inputNTUP_FTKTMP_14_3File', '--inputNTUP_FTKTMP_15_0File', '--inputNTUP_FTKTMP_15_1File', '--inputNTUP_FTKTMP_15_2File', '--inputNTUP_FTKTMP_15_3File', '--inputNTUP_FTKTMP_16_0File', '--inputNTUP_FTKTMP_16_1File', '--inputNTUP_FTKTMP_16_2File', '--inputNTUP_FTKTMP_16_3File', '--inputNTUP_FTKTMP_17_0File', '--inputNTUP_FTKTMP_17_1File', '--inputNTUP_FTKTMP_17_2File', '--inputNTUP_FTKTMP_17_3File', '--inputNTUP_FTKTMP_18_0File', '--inputNTUP_FTKTMP_18_1File', '--inputNTUP_FTKTMP_18_2File', '--inputNTUP_FTKTMP_18_3File', '--inputNTUP_FTKTMP_19_0File', '--inputNTUP_FTKTMP_19_1File', '--inputNTUP_FTKTMP_19_2File', '--inputNTUP_FTKTMP_19_3File', '--inputNTUP_FTKTMP_1_0File', '--inputNTUP_FTKTMP_1_1File', '--inputNTUP_FTKTMP_1_2File', '--inputNTUP_FTKTMP_1_3File', '--inputNTUP_FTKTMP_20_0File', '--inputNTUP_FTKTMP_20_1File', '--inputNTUP_FTKTMP_20_2File', '--inputNTUP_FTKTMP_20_3File', '--inputNTUP_FTKTMP_21_0File', '--inputNTUP_FTKTMP_21_1File', '--inputNTUP_FTKTMP_21_2File', '--inputNTUP_FTKTMP_21_3File', '--inputNTUP_FTKTMP_22_0File', '--inputNTUP_FTKTMP_22_1File', '--inputNTUP_FTKTMP_22_2File', '--inputNTUP_FTKTMP_22_3File', '--inputNTUP_FTKTMP_23_0File', '--inputNTUP_FTKTMP_23_1File', '--inputNTUP_FTKTMP_23_2File', '--inputNTUP_FTKTMP_23_3File', '--inputNTUP_FTKTMP_24_0File', '--inputNTUP_FTKTMP_24_1File', '--inputNTUP_FTKTMP_24_2File', '--inputNTUP_FTKTMP_24_3File', '--inputNTUP_FTKTMP_25_0File', '--inputNTUP_FTKTMP_25_1File', '--inputNTUP_FTKTMP_25_2File', '--inputNTUP_FTKTMP_25_3File', '--inputNTUP_FTKTMP_26_0File', '--inputNTUP_FTKTMP_26_1File', '--inputNTUP_FTKTMP_26_2File', '--inputNTUP_FTKTMP_26_3File', '--inputNTUP_FTKTMP_27_0File', '--inputNTUP_FTKTMP_27_1File', '--inputNTUP_FTKTMP_27_2File', '--inputNTUP_FTKTMP_27_3File', '--inputNTUP_FTKTMP_28_0File', '--inputNTUP_FTKTMP_28_1File', '--inputNTUP_FTKTMP_28_2File', '--inputNTUP_FTKTMP_28_3File', '--inputNTUP_FTKTMP_29_0File', '--inputNTUP_FTKTMP_29_1File', '--inputNTUP_FTKTMP_29_2File', '--inputNTUP_FTKTMP_29_3File', '--inputNTUP_FTKTMP_2_0File', '--inputNTUP_FTKTMP_2_1File', '--inputNTUP_FTKTMP_2_2File', '--inputNTUP_FTKTMP_2_3File', '--inputNTUP_FTKTMP_30_0File', '--inputNTUP_FTKTMP_30_1File', '--inputNTUP_FTKTMP_30_2File', '--inputNTUP_FTKTMP_30_3File', '--inputNTUP_FTKTMP_31_0File', '--inputNTUP_FTKTMP_31_1File', '--inputNTUP_FTKTMP_31_2File', '--inputNTUP_FTKTMP_31_3File', '--inputNTUP_FTKTMP_32_0File', '--inputNTUP_FTKTMP_32_1File', '--inputNTUP_FTKTMP_32_2File', '--inputNTUP_FTKTMP_32_3File', '--inputNTUP_FTKTMP_33_0File', '--inputNTUP_FTKTMP_33_1File', '--inputNTUP_FTKTMP_33_2File', '--inputNTUP_FTKTMP_33_3File', '--inputNTUP_FTKTMP_34_0File', '--inputNTUP_FTKTMP_34_1File', '--inputNTUP_FTKTMP_34_2File', '--inputNTUP_FTKTMP_34_3File', '--inputNTUP_FTKTMP_35_0File', '--inputNTUP_FTKTMP_35_1File', '--inputNTUP_FTKTMP_35_2File', '--inputNTUP_FTKTMP_35_3File', '--inputNTUP_FTKTMP_36_0File', '--inputNTUP_FTKTMP_36_1File', '--inputNTUP_FTKTMP_36_2File', '--inputNTUP_FTKTMP_36_3File', '--inputNTUP_FTKTMP_37_0File', '--inputNTUP_FTKTMP_37_1File', '--inputNTUP_FTKTMP_37_2File', '--inputNTUP_FTKTMP_37_3File', '--inputNTUP_FTKTMP_38_0File', '--inputNTUP_FTKTMP_38_1File', '--inputNTUP_FTKTMP_38_2File', '--inputNTUP_FTKTMP_38_3File', '--inputNTUP_FTKTMP_39_0File', '--inputNTUP_FTKTMP_39_1File', '--inputNTUP_FTKTMP_39_2File', '--inputNTUP_FTKTMP_39_3File', '--inputNTUP_FTKTMP_3_0File', '--inputNTUP_FTKTMP_3_1File', '--inputNTUP_FTKTMP_3_2File', '--inputNTUP_FTKTMP_3_3File', '--inputNTUP_FTKTMP_40_0File', '--inputNTUP_FTKTMP_40_1File', '--inputNTUP_FTKTMP_40_2File', '--inputNTUP_FTKTMP_40_3File', '--inputNTUP_FTKTMP_41_0File', '--inputNTUP_FTKTMP_41_1File', '--inputNTUP_FTKTMP_41_2File', '--inputNTUP_FTKTMP_41_3File', '--inputNTUP_FTKTMP_42_0File', '--inputNTUP_FTKTMP_42_1File', '--inputNTUP_FTKTMP_42_2File', '--inputNTUP_FTKTMP_42_3File', '--inputNTUP_FTKTMP_43_0File', '--inputNTUP_FTKTMP_43_1File', '--inputNTUP_FTKTMP_43_2File', '--inputNTUP_FTKTMP_43_3File', '--inputNTUP_FTKTMP_44_0File', '--inputNTUP_FTKTMP_44_1File', '--inputNTUP_FTKTMP_44_2File', '--inputNTUP_FTKTMP_44_3File', '--inputNTUP_FTKTMP_45_0File', '--inputNTUP_FTKTMP_45_1File', '--inputNTUP_FTKTMP_45_2File', '--inputNTUP_FTKTMP_45_3File', '--inputNTUP_FTKTMP_46_0File', '--inputNTUP_FTKTMP_46_1File', '--inputNTUP_FTKTMP_46_2File', '--inputNTUP_FTKTMP_46_3File', '--inputNTUP_FTKTMP_47_0File', '--inputNTUP_FTKTMP_47_1File', '--inputNTUP_FTKTMP_47_2File', '--inputNTUP_FTKTMP_47_3File', '--inputNTUP_FTKTMP_48_0File', '--inputNTUP_FTKTMP_48_1File', '--inputNTUP_FTKTMP_48_2File', '--inputNTUP_FTKTMP_48_3File', '--inputNTUP_FTKTMP_49_0File', '--inputNTUP_FTKTMP_49_1File', '--inputNTUP_FTKTMP_49_2File', '--inputNTUP_FTKTMP_49_3File', '--inputNTUP_FTKTMP_4_0File', '--inputNTUP_FTKTMP_4_1File', '--inputNTUP_FTKTMP_4_2File', '--inputNTUP_FTKTMP_4_3File', '--inputNTUP_FTKTMP_50_0File', '--inputNTUP_FTKTMP_50_1File', '--inputNTUP_FTKTMP_50_2File', '--inputNTUP_FTKTMP_50_3File', '--inputNTUP_FTKTMP_51_0File', '--inputNTUP_FTKTMP_51_1File', '--inputNTUP_FTKTMP_51_2File', '--inputNTUP_FTKTMP_51_3File', '--inputNTUP_FTKTMP_52_0File', '--inputNTUP_FTKTMP_52_1File', '--inputNTUP_FTKTMP_52_2File', '--inputNTUP_FTKTMP_52_3File', '--inputNTUP_FTKTMP_53_0File', '--inputNTUP_FTKTMP_53_1File', '--inputNTUP_FTKTMP_53_2File', '--inputNTUP_FTKTMP_53_3File', '--inputNTUP_FTKTMP_54_0File', '--inputNTUP_FTKTMP_54_1File', '--inputNTUP_FTKTMP_54_2File', '--inputNTUP_FTKTMP_54_3File', '--inputNTUP_FTKTMP_55_0File', '--inputNTUP_FTKTMP_55_1File', '--inputNTUP_FTKTMP_55_2File', '--inputNTUP_FTKTMP_55_3File', '--inputNTUP_FTKTMP_56_0File', '--inputNTUP_FTKTMP_56_1File', '--inputNTUP_FTKTMP_56_2File', '--inputNTUP_FTKTMP_56_3File', '--inputNTUP_FTKTMP_57_0File', '--inputNTUP_FTKTMP_57_1File', '--inputNTUP_FTKTMP_57_2File', '--inputNTUP_FTKTMP_57_3File', '--inputNTUP_FTKTMP_58_0File', '--inputNTUP_FTKTMP_58_1File', '--inputNTUP_FTKTMP_58_2File', '--inputNTUP_FTKTMP_58_3File', '--inputNTUP_FTKTMP_59_0File', '--inputNTUP_FTKTMP_59_1File', '--inputNTUP_FTKTMP_59_2File', '--inputNTUP_FTKTMP_59_3File', '--inputNTUP_FTKTMP_5_0File', '--inputNTUP_FTKTMP_5_1File', '--inputNTUP_FTKTMP_5_2File', '--inputNTUP_FTKTMP_5_3File', '--inputNTUP_FTKTMP_60_0File', '--inputNTUP_FTKTMP_60_1File', '--inputNTUP_FTKTMP_60_2File', '--inputNTUP_FTKTMP_60_3File', '--inputNTUP_FTKTMP_61_0File', '--inputNTUP_FTKTMP_61_1File', '--inputNTUP_FTKTMP_61_2File', '--inputNTUP_FTKTMP_61_3File', '--inputNTUP_FTKTMP_62_0File', '--inputNTUP_FTKTMP_62_1File', '--inputNTUP_FTKTMP_62_2File', '--inputNTUP_FTKTMP_62_3File', '--inputNTUP_FTKTMP_63_0File', '--inputNTUP_FTKTMP_63_1File', '--inputNTUP_FTKTMP_63_2File', '--inputNTUP_FTKTMP_63_3File', '--inputNTUP_FTKTMP_6_0File', '--inputNTUP_FTKTMP_6_1File', '--inputNTUP_FTKTMP_6_2File', '--inputNTUP_FTKTMP_6_3File', '--inputNTUP_FTKTMP_7_0File', '--inputNTUP_FTKTMP_7_1File', '--inputNTUP_FTKTMP_7_2File', '--inputNTUP_FTKTMP_7_3File', '--inputNTUP_FTKTMP_8_0File', '--inputNTUP_FTKTMP_8_1File', '--inputNTUP_FTKTMP_8_2File', '--inputNTUP_FTKTMP_8_3File', '--inputNTUP_FTKTMP_9_0File', '--inputNTUP_FTKTMP_9_1File', '--inputNTUP_FTKTMP_9_2File', '--inputNTUP_FTKTMP_9_3File', '--inputTXT_FTKIPFile', '--loadHWConf_path', '--maxEvents', '--omitFileValidation', '--outputNTUP_FTKFile', '--outputNTUP_FTKTMP_0File', '--outputNTUP_FTKTMP_0_0File', '--outputNTUP_FTKTMP_0_1File', '--outputNTUP_FTKTMP_0_2File', '--outputNTUP_FTKTMP_0_3File', '--outputNTUP_FTKTMP_10File', '--outputNTUP_FTKTMP_10_0File', '--outputNTUP_FTKTMP_10_1File', '--outputNTUP_FTKTMP_10_2File', '--outputNTUP_FTKTMP_10_3File', '--outputNTUP_FTKTMP_11File', '--outputNTUP_FTKTMP_11_0File', '--outputNTUP_FTKTMP_11_1File', '--outputNTUP_FTKTMP_11_2File', '--outputNTUP_FTKTMP_11_3File', '--outputNTUP_FTKTMP_12File', '--outputNTUP_FTKTMP_12_0File', '--outputNTUP_FTKTMP_12_1File', '--outputNTUP_FTKTMP_12_2File', '--outputNTUP_FTKTMP_12_3File', '--outputNTUP_FTKTMP_13File', '--outputNTUP_FTKTMP_13_0File', '--outputNTUP_FTKTMP_13_1File', '--outputNTUP_FTKTMP_13_2File', '--outputNTUP_FTKTMP_13_3File', '--outputNTUP_FTKTMP_14File', '--outputNTUP_FTKTMP_14_0File', '--outputNTUP_FTKTMP_14_1File', '--outputNTUP_FTKTMP_14_2File', '--outputNTUP_FTKTMP_14_3File', '--outputNTUP_FTKTMP_15File', '--outputNTUP_FTKTMP_15_0File', '--outputNTUP_FTKTMP_15_1File', '--outputNTUP_FTKTMP_15_2File', '--outputNTUP_FTKTMP_15_3File', '--outputNTUP_FTKTMP_16File', '--outputNTUP_FTKTMP_16_0File', '--outputNTUP_FTKTMP_16_1File', '--outputNTUP_FTKTMP_16_2File', '--outputNTUP_FTKTMP_16_3File', '--outputNTUP_FTKTMP_17File', '--outputNTUP_FTKTMP_17_0File', '--outputNTUP_FTKTMP_17_1File', '--outputNTUP_FTKTMP_17_2File', '--outputNTUP_FTKTMP_17_3File', '--outputNTUP_FTKTMP_18File', '--outputNTUP_FTKTMP_18_0File', '--outputNTUP_FTKTMP_18_1File', '--outputNTUP_FTKTMP_18_2File', '--outputNTUP_FTKTMP_18_3File', '--outputNTUP_FTKTMP_19File', '--outputNTUP_FTKTMP_19_0File', '--outputNTUP_FTKTMP_19_1File', '--outputNTUP_FTKTMP_19_2File', '--outputNTUP_FTKTMP_19_3File', '--outputNTUP_FTKTMP_1File', '--outputNTUP_FTKTMP_1_0File', '--outputNTUP_FTKTMP_1_1File', '--outputNTUP_FTKTMP_1_2File', '--outputNTUP_FTKTMP_1_3File', '--outputNTUP_FTKTMP_20File', '--outputNTUP_FTKTMP_20_0File', '--outputNTUP_FTKTMP_20_1File', '--outputNTUP_FTKTMP_20_2File', '--outputNTUP_FTKTMP_20_3File', '--outputNTUP_FTKTMP_21File', '--outputNTUP_FTKTMP_21_0File', '--outputNTUP_FTKTMP_21_1File', '--outputNTUP_FTKTMP_21_2File', '--outputNTUP_FTKTMP_21_3File', '--outputNTUP_FTKTMP_22File', '--outputNTUP_FTKTMP_22_0File', '--outputNTUP_FTKTMP_22_1File', '--outputNTUP_FTKTMP_22_2File', '--outputNTUP_FTKTMP_22_3File', '--outputNTUP_FTKTMP_23File', '--outputNTUP_FTKTMP_23_0File', '--outputNTUP_FTKTMP_23_1File', '--outputNTUP_FTKTMP_23_2File', '--outputNTUP_FTKTMP_23_3File', '--outputNTUP_FTKTMP_24File', '--outputNTUP_FTKTMP_24_0File', '--outputNTUP_FTKTMP_24_1File', '--outputNTUP_FTKTMP_24_2File', '--outputNTUP_FTKTMP_24_3File', '--outputNTUP_FTKTMP_25File', '--outputNTUP_FTKTMP_25_0File', '--outputNTUP_FTKTMP_25_1File', '--outputNTUP_FTKTMP_25_2File', '--outputNTUP_FTKTMP_25_3File', '--outputNTUP_FTKTMP_26File', '--outputNTUP_FTKTMP_26_0File', '--outputNTUP_FTKTMP_26_1File', '--outputNTUP_FTKTMP_26_2File', '--outputNTUP_FTKTMP_26_3File', '--outputNTUP_FTKTMP_27File', '--outputNTUP_FTKTMP_27_0File', '--outputNTUP_FTKTMP_27_1File', '--outputNTUP_FTKTMP_27_2File', '--outputNTUP_FTKTMP_27_3File', '--outputNTUP_FTKTMP_28File', '--outputNTUP_FTKTMP_28_0File', '--outputNTUP_FTKTMP_28_1File', '--outputNTUP_FTKTMP_28_2File', '--outputNTUP_FTKTMP_28_3File', '--outputNTUP_FTKTMP_29File', '--outputNTUP_FTKTMP_29_0File', '--outputNTUP_FTKTMP_29_1File', '--outputNTUP_FTKTMP_29_2File', '--outputNTUP_FTKTMP_29_3File', '--outputNTUP_FTKTMP_2File', '--outputNTUP_FTKTMP_2_0File', '--outputNTUP_FTKTMP_2_1File', '--outputNTUP_FTKTMP_2_2File', '--outputNTUP_FTKTMP_2_3File', '--outputNTUP_FTKTMP_30File', '--outputNTUP_FTKTMP_30_0File', '--outputNTUP_FTKTMP_30_1File', '--outputNTUP_FTKTMP_30_2File', '--outputNTUP_FTKTMP_30_3File', '--outputNTUP_FTKTMP_31File', '--outputNTUP_FTKTMP_31_0File', '--outputNTUP_FTKTMP_31_1File', '--outputNTUP_FTKTMP_31_2File', '--outputNTUP_FTKTMP_31_3File', '--outputNTUP_FTKTMP_32File', '--outputNTUP_FTKTMP_32_0File', '--outputNTUP_FTKTMP_32_1File', '--outputNTUP_FTKTMP_32_2File', '--outputNTUP_FTKTMP_32_3File', '--outputNTUP_FTKTMP_33File', '--outputNTUP_FTKTMP_33_0File', '--outputNTUP_FTKTMP_33_1File', '--outputNTUP_FTKTMP_33_2File', '--outputNTUP_FTKTMP_33_3File', '--outputNTUP_FTKTMP_34File', '--outputNTUP_FTKTMP_34_0File', '--outputNTUP_FTKTMP_34_1File', '--outputNTUP_FTKTMP_34_2File', '--outputNTUP_FTKTMP_34_3File', '--outputNTUP_FTKTMP_35File', '--outputNTUP_FTKTMP_35_0File', '--outputNTUP_FTKTMP_35_1File', '--outputNTUP_FTKTMP_35_2File', '--outputNTUP_FTKTMP_35_3File', '--outputNTUP_FTKTMP_36File', '--outputNTUP_FTKTMP_36_0File', '--outputNTUP_FTKTMP_36_1File', '--outputNTUP_FTKTMP_36_2File', '--outputNTUP_FTKTMP_36_3File', '--outputNTUP_FTKTMP_37File', '--outputNTUP_FTKTMP_37_0File', '--outputNTUP_FTKTMP_37_1File', '--outputNTUP_FTKTMP_37_2File', '--outputNTUP_FTKTMP_37_3File', '--outputNTUP_FTKTMP_38File', '--outputNTUP_FTKTMP_38_0File', '--outputNTUP_FTKTMP_38_1File', '--outputNTUP_FTKTMP_38_2File', '--outputNTUP_FTKTMP_38_3File', '--outputNTUP_FTKTMP_39File', '--outputNTUP_FTKTMP_39_0File', '--outputNTUP_FTKTMP_39_1File', '--outputNTUP_FTKTMP_39_2File', '--outputNTUP_FTKTMP_39_3File', '--outputNTUP_FTKTMP_3File', '--outputNTUP_FTKTMP_3_0File', '--outputNTUP_FTKTMP_3_1File', '--outputNTUP_FTKTMP_3_2File', '--outputNTUP_FTKTMP_3_3File', '--outputNTUP_FTKTMP_40File', '--outputNTUP_FTKTMP_40_0File', '--outputNTUP_FTKTMP_40_1File', '--outputNTUP_FTKTMP_40_2File', '--outputNTUP_FTKTMP_40_3File', '--outputNTUP_FTKTMP_41File', '--outputNTUP_FTKTMP_41_0File', '--outputNTUP_FTKTMP_41_1File', '--outputNTUP_FTKTMP_41_2File', '--outputNTUP_FTKTMP_41_3File', '--outputNTUP_FTKTMP_42File', '--outputNTUP_FTKTMP_42_0File', '--outputNTUP_FTKTMP_42_1File', '--outputNTUP_FTKTMP_42_2File', '--outputNTUP_FTKTMP_42_3File', '--outputNTUP_FTKTMP_43File', '--outputNTUP_FTKTMP_43_0File', '--outputNTUP_FTKTMP_43_1File', '--outputNTUP_FTKTMP_43_2File', '--outputNTUP_FTKTMP_43_3File', '--outputNTUP_FTKTMP_44File', '--outputNTUP_FTKTMP_44_0File', '--outputNTUP_FTKTMP_44_1File', '--outputNTUP_FTKTMP_44_2File', '--outputNTUP_FTKTMP_44_3File', '--outputNTUP_FTKTMP_45File', '--outputNTUP_FTKTMP_45_0File', '--outputNTUP_FTKTMP_45_1File', '--outputNTUP_FTKTMP_45_2File', '--outputNTUP_FTKTMP_45_3File', '--outputNTUP_FTKTMP_46File', '--outputNTUP_FTKTMP_46_0File', '--outputNTUP_FTKTMP_46_1File', '--outputNTUP_FTKTMP_46_2File', '--outputNTUP_FTKTMP_46_3File', '--outputNTUP_FTKTMP_47File', '--outputNTUP_FTKTMP_47_0File', '--outputNTUP_FTKTMP_47_1File', '--outputNTUP_FTKTMP_47_2File', '--outputNTUP_FTKTMP_47_3File', '--outputNTUP_FTKTMP_48File', '--outputNTUP_FTKTMP_48_0File', '--outputNTUP_FTKTMP_48_1File', '--outputNTUP_FTKTMP_48_2File', '--outputNTUP_FTKTMP_48_3File', '--outputNTUP_FTKTMP_49File', '--outputNTUP_FTKTMP_49_0File', '--outputNTUP_FTKTMP_49_1File', '--outputNTUP_FTKTMP_49_2File', '--outputNTUP_FTKTMP_49_3File', '--outputNTUP_FTKTMP_4File', '--outputNTUP_FTKTMP_4_0File', '--outputNTUP_FTKTMP_4_1File', '--outputNTUP_FTKTMP_4_2File', '--outputNTUP_FTKTMP_4_3File', '--outputNTUP_FTKTMP_50File', '--outputNTUP_FTKTMP_50_0File', '--outputNTUP_FTKTMP_50_1File', '--outputNTUP_FTKTMP_50_2File', '--outputNTUP_FTKTMP_50_3File', '--outputNTUP_FTKTMP_51File', '--outputNTUP_FTKTMP_51_0File', '--outputNTUP_FTKTMP_51_1File', '--outputNTUP_FTKTMP_51_2File', '--outputNTUP_FTKTMP_51_3File', '--outputNTUP_FTKTMP_52File', '--outputNTUP_FTKTMP_52_0File', '--outputNTUP_FTKTMP_52_1File', '--outputNTUP_FTKTMP_52_2File', '--outputNTUP_FTKTMP_52_3File', '--outputNTUP_FTKTMP_53File', '--outputNTUP_FTKTMP_53_0File', '--outputNTUP_FTKTMP_53_1File', '--outputNTUP_FTKTMP_53_2File', '--outputNTUP_FTKTMP_53_3File', '--outputNTUP_FTKTMP_54File', '--outputNTUP_FTKTMP_54_0File', '--outputNTUP_FTKTMP_54_1File', '--outputNTUP_FTKTMP_54_2File', '--outputNTUP_FTKTMP_54_3File', '--outputNTUP_FTKTMP_55File', '--outputNTUP_FTKTMP_55_0File', '--outputNTUP_FTKTMP_55_1File', '--outputNTUP_FTKTMP_55_2File', '--outputNTUP_FTKTMP_55_3File', '--outputNTUP_FTKTMP_56File', '--outputNTUP_FTKTMP_56_0File', '--outputNTUP_FTKTMP_56_1File', '--outputNTUP_FTKTMP_56_2File', '--outputNTUP_FTKTMP_56_3File', '--outputNTUP_FTKTMP_57File', '--outputNTUP_FTKTMP_57_0File', '--outputNTUP_FTKTMP_57_1File', '--outputNTUP_FTKTMP_57_2File', '--outputNTUP_FTKTMP_57_3File', '--outputNTUP_FTKTMP_58File', '--outputNTUP_FTKTMP_58_0File', '--outputNTUP_FTKTMP_58_1File', '--outputNTUP_FTKTMP_58_2File', '--outputNTUP_FTKTMP_58_3File', '--outputNTUP_FTKTMP_59File', '--outputNTUP_FTKTMP_59_0File', '--outputNTUP_FTKTMP_59_1File', '--outputNTUP_FTKTMP_59_2File', '--outputNTUP_FTKTMP_59_3File', '--outputNTUP_FTKTMP_5File', '--outputNTUP_FTKTMP_5_0File', '--outputNTUP_FTKTMP_5_1File', '--outputNTUP_FTKTMP_5_2File', '--outputNTUP_FTKTMP_5_3File', '--outputNTUP_FTKTMP_60File', '--outputNTUP_FTKTMP_60_0File', '--outputNTUP_FTKTMP_60_1File', '--outputNTUP_FTKTMP_60_2File', '--outputNTUP_FTKTMP_60_3File', '--outputNTUP_FTKTMP_61File', '--outputNTUP_FTKTMP_61_0File', '--outputNTUP_FTKTMP_61_1File', '--outputNTUP_FTKTMP_61_2File', '--outputNTUP_FTKTMP_61_3File', '--outputNTUP_FTKTMP_62File', '--outputNTUP_FTKTMP_62_0File', '--outputNTUP_FTKTMP_62_1File', '--outputNTUP_FTKTMP_62_2File', '--outputNTUP_FTKTMP_62_3File', '--outputNTUP_FTKTMP_63File', '--outputNTUP_FTKTMP_63_0File', '--outputNTUP_FTKTMP_63_1File', '--outputNTUP_FTKTMP_63_2File', '--outputNTUP_FTKTMP_63_3File', '--outputNTUP_FTKTMP_6File', '--outputNTUP_FTKTMP_6_0File', '--outputNTUP_FTKTMP_6_1File', '--outputNTUP_FTKTMP_6_2File', '--outputNTUP_FTKTMP_6_3File', '--outputNTUP_FTKTMP_7File', '--outputNTUP_FTKTMP_7_0File', '--outputNTUP_FTKTMP_7_1File', '--outputNTUP_FTKTMP_7_2File', '--outputNTUP_FTKTMP_7_3File', '--outputNTUP_FTKTMP_8File', '--outputNTUP_FTKTMP_8_0File', '--outputNTUP_FTKTMP_8_1File', '--outputNTUP_FTKTMP_8_2File', '--outputNTUP_FTKTMP_8_3File', '--outputNTUP_FTKTMP_9File', '--outputNTUP_FTKTMP_9_0File', '--outputNTUP_FTKTMP_9_1File', '--outputNTUP_FTKTMP_9_2File', '--outputNTUP_FTKTMP_9_3File', '--patternbankspath', '--pmap_path', '--pmapcomplete_path', '--pmapunused_path', '--reportName', '--rmap_path', '--sectorspath', '--showGraph', '--showPath', '--showSteps', '--ssmap_path', '--ssmaptsp_path', '--ssmapunused_path', '--uploadtoami', '--validation', '--saveTruthTree']


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])
        
    trf = getTransform() 
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


## Get the base transform with all arguments added
def getTransform():
    
    executorSet = set()
    for tower in range(ntowers) :
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}'.format(tower), 
              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',
              inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP_{0}'.format(tower)],
              extraRunargs = {'bankregion': [tower],'banksubregion': [0]},
              # Need to ensure that the correct subregion is used
              runtimeRunargs = {'bankregion': [tower], 'subregions': [0],
               'patternbankpath': '[runArgs.patternbankspath[{0}]]'.format(tower),
               'fitconstantspath': '[runArgs.fitconstantspath[{0}]]'.format(tower),
               'fit711constantspath': '[runArgs.fit711constantspath[{0}]]'.format(tower),
               'sectorpath': '[runArgs.sectorspath[{0}]]'.format(tower),
               'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}File'.format(tower)}))

    # add final merge for all the tower, generating a NTUP_FTK file
    executorSet.add(athenaExecutor(name = "FTKSimulationMergeFinal",
          skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',
          inData = [tuple([ 'NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers) ])+('NTUP_FTKIP',)],
          outData = ['NTUP_FTK'],
          extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers)]},
          runtimeRunargs = {'MergeFromTowers': False,
                            'FirstRegion': 0,
                            'TruthTrackTreeName' : "'truthtracks'",
                            'EvtInfoTreeName' : "'evtinfo'",
                            'SaveTruthTree' : '1'}))

    trf = transform(executor = executorSet, description = 'FTK simulation for {0} towers, with no subregions, and final merge.'.format(ntowers))

#    addAthenaArguments(trf.parser)
    addTrigFTKSimOptions(trf.parser,nsubregions=subregions)
    addTrigFTKSimRFOptions(trf.parser)
    addTrigFTKSimTFOptions(trf.parser)
    addFTKSimulationArgs(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    
    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkip', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format', group='TrigFTKSim', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'), 
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    
    # The following for testing only
    for tower in range(ntowers) :
      parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(tower), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                          help='Tower {0} merged FTK file (for test only)'.format(tower), group='TrigFTKMerge')
      parser.add_argument('--inputNTUP_FTKTMP_{0}File'.format(tower), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftk', treeNames='ftkdata'),
                          help='FTK NTUP file from tower {0}  (for testing only)'.format(tower), group='TrigFTKMerge')
        
    parser.add_argument('--outputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKMerge') 
    parser.add_argument('--FTKUnmergedInputPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')
    

if __name__ == '__main__':
    main()
