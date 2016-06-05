# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the ElectronLikelihoodMenuDefs
# 

import cppyy
try :
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import LikeEnum, std

# Next 3 lines are needed due to a difference how pyROOT and pyCintex handle stl vectors
std.vector('float')
std.vector('double')
std.vector('int')

# Import a needed helper
from PATCore.HelperUtils import *


def ElectronLikelihoodCUSTOMMediumConfig(theTool) :
    '''
    This is for the custom implementation of the MediumLLH definitions.
    '''

    theTool = GetTool(theTool)

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_DeltaPoverP,el_deltaphiRescaled,el_trackd0pvunbiased,el_d0significance"

    # PdfFileName
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

    # do cut on conversion
    theTool.doCutConversion = 0

    # do remove f3 at high Et
    theTool.doRemoveF3AtHighEt = 0

    # cut on pixel-layer hits. Must be 10 long
    theTool.CutPi += [2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
    
    # cut on precision hits. Must be 10 long
    theTool.CutSi += [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]

    # cut on precision hits. Must be 10 long
    theTool.CutBL += [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

    # the likelihood cut itself
    theTool.CutLikelihood += [  0.301,  0.301,  0.345,  0.368,  0.266,  0.148,  0.319,  0.312,  0.267,  0.123, # 7-10
                            0.401,  0.401,  0.410,  0.407,  0.396,  0.250,  0.395,  0.426,  0.346,  0.102, # 10-15
                            0.439,  0.439,  0.466,  0.429,  0.422,  0.255,  0.403,  0.439,  0.341,  0.091, # 15-20
                            0.499,  0.499,  0.480,  0.475,  0.443,  0.258,  0.458,  0.459,  0.448,  0.158, # 20-25
                            0.514,  0.514,  0.501,  0.498,  0.466,  0.265,  0.463,  0.475,  0.463,  0.265, # 25-30
                            0.513,  0.513,  0.494,  0.500,  0.457,  0.270,  0.455,  0.471,  0.481,  0.289, # 30-35
                            0.535,  0.535,  0.528,  0.533,  0.486,  0.290,  0.467,  0.491,  0.505,  0.361, # 35-40
                            0.514,  0.514,  0.517,  0.520,  0.454,  0.230,  0.387,  0.464,  0.481,  0.372, # 40-45
                            0.566,  0.566,  0.581,  0.585,  0.526,  0.243,  0.441,  0.529,  0.524,  0.487, # 45<
                            ]

    # the nvtx correction part to the likelihood cut
    theTool.CutLikelihoodPileupCorrection += [ -0.00216, -0.00216, -0.00344, -0.00504, -0.00600, -0.00176, -0.00488, -0.00896, -0.00816, -0.00448,  # 7-10
                                               -0.00184, -0.00184,  0.00000, -0.00168, -0.00160, -0.00096, -0.00288, -0.00472, -0.00496, -0.00528,  # 10-15
                                               -0.00096, -0.00096, -0.00120, -0.00128, -0.00160, -0.00008, -0.00168, -0.00208, -0.00344, -0.00448,  # 15-20
                                               -0.00120, -0.00120, -0.00160, -0.00120, -0.00088, -0.00040, -0.00072, -0.00128, -0.00128, -0.00112,  # 20-25
                                               -0.00080, -0.00080, -0.00080, -0.00088, -0.00088, -0.00016, -0.00048, -0.00120, -0.00120, -0.00184,  # 25-30
                                               -0.00104, -0.00104, -0.00088, -0.00104, -0.00096, -0.00064, -0.00040, -0.00096, -0.00120, -0.00136,  # 30-35
                                               -0.00080, -0.00080, -0.00072, -0.00096, -0.00096, -0.00040, -0.00048, -0.00080, -0.00080, -0.00080,  # 35-40
                                               -0.00104, -0.00104, -0.00112, -0.00088, -0.00088, -0.00064, -0.00064, -0.00104, -0.00096, -0.00056,  # 40-45
                                               -0.00088, -0.00088, -0.00096, -0.00088, -0.00088, -0.00064, -0.00080, -0.00080, -0.00088, -0.00080,  # 45<
                                               ]


def ElectronLikelihoodCUSTOMMediumNoGSFConfig(theTool) :
    '''
    This is for the custom implementation of the Medium, No GSF-variable (trigger-friendly)  definitions.
    '''
    
    theTool = GetTool(theTool)

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    # No deltapoverp, no deltaphires
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_trackd0pvunbiased,el_d0significance"


    # PdfFileName
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

    # do cut on conversion
    theTool.doCutConversion = 0

    # do remove f3 at high Et
    theTool.doRemoveF3AtHighEt = 0

    # cut on pixel-layer hits. Must be 10 long
    theTool.CutPi += [2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
    
    # cut on precision hits. Must be 10 long
    theTool.CutSi += [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]

    # cut on precision hits. Must be 10 long
    theTool.CutBL += [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

    #
    # Details: 
    # No GSF variables; Tuned to Mediumpp (not trigger mediumpp) - same signal efficiency
    # Pileup Correction is in MU!
    # Below 20 GeV the matching with mediumpp is not so great. Efficiencies too low.
    # Took 0.6 x the mu correction suggested
    # Bumped up the efficiency by 0.5%
    #
    theTool.CutLikelihood += [ 0.306,  0.306,  0.392,  0.367,  0.358,  0.167,  0.396,  0.403,  0.420,  0.168,
                               0.345,  0.345,  0.357,  0.411,  0.356,  0.192,  0.382,  0.454,  0.389,  0.196,
                               0.412,  0.412,  0.399,  0.375,  0.372,  0.194,  0.354,  0.432,  0.349,  0.162,
                               0.448,  0.448,  0.446,  0.419,  0.403,  0.184,  0.392,  0.411,  0.401,  0.114,
                               0.438,  0.438,  0.430,  0.426,  0.405,  0.181,  0.385,  0.414,  0.418,  0.226,
                               0.447,  0.447,  0.437,  0.434,  0.387,  0.194,  0.385,  0.407,  0.434,  0.251,
                               0.455,  0.455,  0.464,  0.455,  0.408,  0.206,  0.384,  0.432,  0.440,  0.310,
                               0.458,  0.458,  0.476,  0.467,  0.395,  0.158,  0.335,  0.422,  0.430,  0.302,
                               0.490,  0.490,  0.506,  0.511,  0.450,  0.175,  0.372,  0.471,  0.479,  0.416,
                               ]
    
    theTool.CutLikelihoodPileupCorrection += [  -0.00360, -0.00360, -0.00588, -0.00528, -0.00864, -0.00300, -0.00840, -0.00936, -0.01128, -0.00432,
                                                -0.00192, -0.00192, -0.00108, -0.00432, -0.00240, -0.00144, -0.00408, -0.00648, -0.00600, -0.00648,
                                                -0.00324, -0.00324, -0.00132, -0.00216, -0.00228, -0.00108, -0.00228, -0.00372, -0.00408, -0.00516,
                                                -0.00204, -0.00204, -0.00228, -0.00180, -0.00216, -0.00024, -0.00096, -0.00144, -0.00120, -0.00012,
                                                -0.00120, -0.00120, -0.00108, -0.00132, -0.00168,  0.00000, -0.00060, -0.00096, -0.00132, -0.00108,
                                                -0.00144, -0.00144, -0.00144, -0.00144, -0.00120, -0.00048, -0.00060, -0.00048, -0.00120, -0.00120,
                                                -0.00120, -0.00120, -0.00144, -0.00132, -0.00120, -0.00036, -0.00036, -0.00084, -0.00048, -0.00096,
                                                -0.00156, -0.00156, -0.00192, -0.00156, -0.00108, -0.00036, -0.00084, -0.00096, -0.00084, -0.00012,
                                                -0.00144, -0.00144, -0.00144, -0.00144, -0.00120, -0.00072, -0.00108, -0.00084, -0.00132, -0.00048,
                                                ]
    
def ElectronLikelihoodLooseTriggerConfig2015(theTool) :
    '''
    This is for the custom implementation of the Loose, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as offline Loosepp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodLooseTriggerConfig2015.conf"

    
def ElectronLikelihoodMediumTriggerConfig2015(theTool) :
    '''
    This is for the custom implementation of the Medium, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as Offline Mediumpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodMediumTriggerConfig2015.conf"

  
    
def ElectronLikelihoodTightTriggerConfig2015(theTool) :
    '''
    This is for the custom implementation of the Tight, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodTightTriggerConfig2015.conf"


def ElectronLikelihoodVeryLooseOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the VeryLoose offline likelihood for MC15 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodVeryLooseOfflineConfig2016_Smooth.conf"
    
    
def ElectronLikelihoodLooseOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Loose offline likelihood for MC15 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Loosepp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_Smooth.conf"

    
def ElectronLikelihoodMediumOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Medium offline likelihood for MC15 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Mediumpp + 1%)
    '''    
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf"

    
def ElectronLikelihoodTightOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Tight offline likelihood for MC15 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodTightOfflineConfig2016_Smooth.conf"
