# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of the ElectronLikelihoodMenuDefs
# 

import PyCintex
try :
    PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
except :
    pass

from ROOT import LikeEnum, std

# Next 3 lines are needed due to a difference how pyROOT and pyCintex handle stl vectors
std.vector('float')
std.vector('double')
std.vector('int')

# Import a needed helper
from PATCore.HelperUtils import *

def ElectronLikelihoodVeryTightConfig2012(theTool) :
    '''
    This is for VeryTight LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.VeryTight
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodTightConfig2012(theTool) :
    '''
    This is for Tight LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.Tight
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodMediumConfig2012(theTool) :
    '''
    This is for Medium LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.Medium
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodLooseConfig2012(theTool) :
    '''
    This is for Loose LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.Loose
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodVeryLooseConfig2012(theTool) :
    '''
    This is for VeryLoose LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.VeryLoose
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodLooseRelaxedConfig2012(theTool) :
    '''
    This is for LooseRelaxed LH
    '''
    theTool = GetTool(theTool)
    theTool.OperatingPoint = LikeEnum.LooseRelaxed
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root"

def ElectronLikelihoodCUSTOMMediumConfig(theTool) :
    '''
    This is for the custom implementation of the MediumLLH definitions.
    '''

    theTool = GetTool(theTool)

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_DeltaPoverP,el_deltaphiRescaled,el_trackd0pvunbiased,el_d0significance"

    # Operating point.
    theTool.OperatingPoint = LikeEnum.CustomOperatingPoint

    # PdfFileNameOB
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

    # Operating point.
    theTool.OperatingPoint = LikeEnum.CustomOperatingPoint

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

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    # No deltapoverp, no deltaphires, no d0significance
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_trackd0pvunbiased"

    # Operating point.
    theTool.OperatingPoint = LikeEnum.CustomOperatingPoint

    # PdfFileName
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/dc14b_20141031/DC14OnlinePDFs.root"

    # do cut on conversion
    theTool.doCutConversion = 0

    # do remove f3 at high Et
    theTool.doRemoveF3AtHighEt = 1

    # cut on pixel-layer hits. Must be 10 long
    theTool.CutPi += [2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
    
    # cut on precision hits. Must be 10 long
    theTool.CutSi += [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]

    # cut on precision hits. Must be 10 long
    theTool.CutBL += [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    #
    # Details: 
    # No GSF variables; Tuned to fully binned offline 2012 Loosepp + 1% signal efficiency
    # No Pileup Correction due to lack of nvtx online, and no mu distribution at this time!
    #
    theTool.CutLikelihood += [ 0.406,  0.406,  0.439,  0.325,  0.427,  0.151,  0.310,  0.391,  0.373, -0.059,
                               0.406,  0.406,  0.439,  0.325,  0.427,  0.151,  0.310,  0.391,  0.373, -0.059,
                               0.406,  0.406,  0.418,  0.325,  0.394,  0.085,  0.235,  0.298,  0.274, -0.368,
                               0.409,  0.409,  0.433,  0.358,  0.421,  0.097,  0.331,  0.355,  0.304, -0.032,
                               0.427,  0.427,  0.451,  0.361,  0.442,  0.082,  0.322,  0.328,  0.262, -0.953,
                               0.457,  0.457,  0.469,  0.415,  0.412,  0.088,  0.319,  0.274,  0.331, -0.389,
                               0.457,  0.457,  0.478,  0.421,  0.430,  0.079,  0.262,  0.235,  0.292, -1.124,
                               0.505,  0.505,  0.472,  0.484,  0.415,  0.007,  0.280,  0.178,  0.349, -0.875,
                               0.505,  0.505,  0.475,  0.502,  0.421, -0.017,  0.223, -0.077,  0.331, -0.755,
                               ]
    
    theTool.CutLikelihoodPileupCorrection += [  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                ]
    
def ElectronLikelihoodMediumTriggerConfig2015(theTool) :
    '''
    This is for the custom implementation of the Medium, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as Offline Mediumpp + 1%)
    '''
    
    theTool = GetTool(theTool)

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    # No deltapoverp, no deltaphires, no d0significance
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_trackd0pvunbiased"

    # Operating point.
    theTool.OperatingPoint = LikeEnum.CustomOperatingPoint

    # PdfFileName
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/dc14b_20141031/DC14OnlinePDFs.root"

    # do cut on conversion
    theTool.doCutConversion = 0

    # do remove f3 at high Et
    theTool.doRemoveF3AtHighEt = 1

    # cut on pixel-layer hits. Must be 10 long
    theTool.CutPi += [2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
    
    # cut on precision hits. Must be 10 long
    theTool.CutSi += [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]

    # cut on precision hits. Must be 10 long
    theTool.CutBL += [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

    #
    # Details: 
    # No GSF variables; Tuned to fully binned offline 2012 Mediumpp + 1% signal efficiency
    # No Pileup Correction due to lack of nvtx online, and no mu distribution at this time!
    #
    theTool.CutLikelihood += [ 0.451,  0.451,  0.511,  0.463,  0.502,  0.193,  0.412,  0.505,  0.454,  0.139,
                               0.451,  0.451,  0.511,  0.463,  0.502,  0.193,  0.412,  0.505,  0.454,  0.139,
                               0.463,  0.463,  0.493,  0.448,  0.466,  0.157,  0.385,  0.484,  0.391,  0.019,
                               0.544,  0.544,  0.532,  0.520,  0.523,  0.175,  0.451,  0.523,  0.502,  0.067,
                               0.562,  0.562,  0.553,  0.535,  0.529,  0.160,  0.451,  0.535,  0.514, -0.527,
                               0.565,  0.565,  0.550,  0.523,  0.508,  0.169,  0.442,  0.508,  0.532, -0.122,
                               0.568,  0.568,  0.565,  0.538,  0.520,  0.175,  0.427,  0.520,  0.535, -1.124,
                               0.574,  0.574,  0.550,  0.559,  0.505,  0.127,  0.403,  0.487,  0.529, -0.824,
                               0.592,  0.592,  0.577,  0.586,  0.523,  0.133,  0.400,  0.502,  0.538, -0.755,
                               ]
    
    theTool.CutLikelihoodPileupCorrection += [  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                ]
    
def ElectronLikelihoodTightTriggerConfig2015(theTool) :
    '''
    This is for the custom implementation of the Tight, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    
    theTool = GetTool(theTool)

    # Variable names in the likelihood. Use these exact names! Comma-separated.
    # No deltapoverp, no deltaphires, no d0significance
    theTool.VariableNames = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_trackd0pvunbiased"

    # Operating point.
    theTool.OperatingPoint = LikeEnum.CustomOperatingPoint

    # PdfFileName
    theTool.inputPDFFileName = "ElectronPhotonSelectorTools/dc14b_20141031/DC14OnlinePDFs.root"

    # do cut on conversion
    theTool.doCutConversion = 0

    # do remove f3 at high Et
    theTool.doRemoveF3AtHighEt = 1

    # cut on pixel-layer hits. Must be 10 long
    theTool.CutPi += [2, 2, 2, 2, 2, 2, 2, 2, 2, 2]
    
    # cut on precision hits. Must be 10 long
    theTool.CutSi += [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]

    # cut on precision hits. Must be 10 long
    theTool.CutBL += [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

    #
    # Details: 
    # No GSF variables; Tuned to fully binned offline 2012 Tightpp + 1% signal efficiency
    # No Pileup Correction due to lack of nvtx online, and no mu distribution at this time!
    #
    theTool.CutLikelihood += [ 0.541,  0.541,  0.562,  0.538,  0.553,  0.250,  0.532,  0.613,  0.544,  0.295,
                               0.541,  0.541,  0.562,  0.538,  0.553,  0.250,  0.532,  0.613,  0.544,  0.295,
                               0.538,  0.538,  0.550,  0.511,  0.532,  0.223,  0.511,  0.574,  0.502,  0.292,
                               0.601,  0.601,  0.592,  0.577,  0.589,  0.241,  0.562,  0.622,  0.565,  0.331,
                               0.628,  0.628,  0.616,  0.601,  0.610,  0.238,  0.583,  0.649,  0.589,  0.367,
                               0.652,  0.652,  0.631,  0.613,  0.628,  0.256,  0.595,  0.640,  0.631,  0.391,
                               0.670,  0.670,  0.649,  0.640,  0.652,  0.262,  0.613,  0.670,  0.652,  0.424,
                               0.727,  0.727,  0.661,  0.664,  0.655,  0.241,  0.604,  0.655,  0.655,  0.418,
                               0.754,  0.754,  0.688,  0.697,  0.682,  0.247,  0.625,  0.691,  0.688,  0.466,
                               ]
    
    theTool.CutLikelihoodPileupCorrection += [  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,
                                                ]
    
def ElectronLikelihoodLooseOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Loose offline likelihood for DC14 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Loosepp + 1%)
    '''
    
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronLikelihoodLooseOfflineConfig2015.conf"

    
def ElectronLikelihoodMediumOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Medium offline likelihood for DC14 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Mediumpp + 1%)
    '''
    
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronLikelihoodMediumOfflineConfig2015.conf"

    
def ElectronLikelihoodTightOfflineConfig2015(theTool) :
    '''
    This is for the custom implementation of the Tight offline likelihood for DC14 / Run 2.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronLikelihoodTightOfflineConfig2015.conf"

    
