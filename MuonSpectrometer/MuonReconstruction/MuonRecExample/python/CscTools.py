# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for CSC reconstruction"""

###############################################################
#
# Configuration for Moore
#
#==============================================================
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

import copy

from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

from AthenaCommon.ConfiguredFactory import getProperty

from MuonRecUtils import logMuon,ConfiguredBase,ExtraFlags
from MuonRecExample.MuonAlignFlags import muonAlignFlags

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip


def CscAlignmentTool(name="CscAlignmentTool",extraFlags=None,**kwargs):
    etaposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0,   -0.0902347,   -0.0984321,    -0.141175, #sector -16
        0.0,    -0.166412,    -0.150399,     -0.18592, #sector -15
        0.0,   -0.0544449,     0.101448,   -0.0433321, #sector -14
        0.0,    -0.047769,    -0.156247,   -0.0854826, #sector -13
        0.0,   -0.0970101,    -0.227578,    -0.301079, #sector -12
        0.0,    -0.152912,    -0.365218,    -0.314042, #sector -11
        0.0,    0.0338696,    -0.264766,    -0.224567, #sector -10
        0.0,    -0.107372,     -0.16099,     -0.13508, #sector -9
        0.0,   -0.0663979,   -0.0312645,    -0.130711, #sector -8
        0.0,    -0.249974,    -0.269372,    -0.353648, #sector -7
        0.0,    -0.194175,    0.0238348,    -0.188433, #sector -6
        0.0,    0.0977346,    0.0492461,     0.150434, #sector -5
        0.0,    0.0444637,   -0.0871214,   -0.0627541, #sector -4
        0.0,    0.0347245,   -0.0226186,   -0.0826478, #sector -3
        0.0,  -0.00408879,   -0.0638005,    -0.206868, #sector -2
        0.0,    0.0331328,    0.0402103,     0.118611, #sector -1
        0.0,   -0.0634537,   -0.0516743,    0.0045364, #sector 1
        0.0,   -0.0957718,    -0.235246,    -0.439811, #sector 2
        0.0,    -0.163504,   -0.0129115,   0.00140143, #sector 3
        0.0,    -0.182903,   -0.0503858,    -0.207799, #sector 4
        0.0,   -0.0970927,            0,     0.124744, #sector 5
        0.0,   -0.0169602,   -0.0527447,    0.0319154, #sector 6
        0.0,    -0.176499,    -0.196542,   -0.0846979, #sector 7
        0.0,    0.0589994,    0.0131431,    0.0461769, #sector 8
        0.0,   -0.0237507,   -0.0307316,     0.144429, #sector 9
        0.0,   -0.0144483,   -0.0328234,  -0.00553684, #sector 10
        0.0,     0.113188,    -0.176182,    0.0635706, #sector 11
        0.0,    -0.109776,    -0.727705,    -0.756824, #sector 12
        0.0,   -0.0553061,    -0.056162,   -0.0336955, #sector 13
        0.0,    -0.070077,    -0.127203,    -0.140082, #sector 14
        0.0,   -0.0844368,   -0.0735331,    -0.016667, #sector 15
        0.0,    0.0426443,    -0.153659,    -0.129711, #sector 16
    ]
    
    phiposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0, 0.0, 0.0, 0.0, #-16
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-6
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #-4
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-2
        0.0, 0.0, 0.0, 0.0, #-1  
        0.0, 0.0, 0.0, 0.0, # sector =1 and above...
        0.0, 0.0, 0.0, 0.0, #2
        0.0, 0.0, 0.0, 0.0, #3
        0.0, 0.0, 0.0, 0.0, #4
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #6
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0  #16
    ]
    kwargs.setdefault("phiposAlignConsts", phiposAlignConsts)
    kwargs.setdefault("etaposAlignConsts", etaposAlignConsts)
    kwargs.setdefault("useAlignment", (globalflags.DataSource() == 'data' and not muonAlignFlags.UseIlines() ) )

    return CfgMgr.CscAlignmentTool(name,**kwargs)
# end of factory function CscAlignmentTool

def SimpleCscClusterFitter(name="SimpleCscClusterFitter",extraFlags=None,**kwargs):
    kwargs.setdefault("CscAlignmentTool", getPublicTool("CscAlignmentTool") )
    return CfgMgr.SimpleCscClusterFitter(name,**kwargs)

def CscPeakThresholdClusterBuilderTool(name="CscPeakThresholdClusterBuilderTool",extraFlags=None,**kwargs):
    kwargs.setdefault("default_fitter",  getPublicTool("SimpleCscClusterFitter") )
    kwargs.setdefault("precision_fitter",  getPublicTool("QratCscClusterFitter") )
    kwargs.setdefault("split_fitter",  getPublicTool("CscSplitClusterFitter") )
    kwargs.setdefault("strip_fitter",  getPublicTool("CalibCscStripFitter") )
    return CfgMgr.CscPeakThresholdClusterBuilderTool(name,**kwargs)

def CscThresholdClusterBuilderTool(name="CscThresholdClusterBuilderTool",extraFlags=None,**kwargs):
    kwargs.setdefault("default_fitter",  getPublicTool("SimpleCscClusterFitter") )
    kwargs.setdefault("precision_fitter",  getPublicTool("QratCscClusterFitter") )
    kwargs.setdefault("split_fitter",  getPublicTool("CscSplitClusterFitter") )
    kwargs.setdefault("strip_fitter",  getPublicTool("CalibCscStripFitter") )
    kwargs.setdefault("cscCalibTool", getPublicTool("CscCalibTool") )
    return CfgMgr.CscThresholdClusterBuilderTool(name,**kwargs)

def CscClusterUtilTool(name = "CscClusterUtilTool",extraFlags=None,**kwargs):
    kwargs.setdefault("strip_fitter", getPublicTool("CalibCscStripFitter") )
    kwargs.setdefault("precision_fitter", getPublicTool("QratCscClusterFitter") )
    return CfgMgr.CscClusterUtilTool(name,**kwargs)

def CalibCscStripFitter(name = "CalibCscStripFitter",extraFlags=None,**kwargs):
    kwargs.setdefault("cscCalibTool", getPublicTool("CscCalibTool") )
    return CfgMgr.CalibCscStripFitter(name,**kwargs)

def CscSplitClusterFitter(name="CscSplitClusterFitter",extraFlags=None,**kwargs):
    kwargs.setdefault("precision_fitter", getPublicTool("QratCscClusterFitter") )
    kwargs.setdefault("default_fitter",  getPublicTool("SimpleCscClusterFitter") )    
    return CfgMgr.CscSplitClusterFitter(name,**kwargs)

def CscThresholdClusterBuilder( name = "CscThresholdClusterBuilder",extraFlags=None,**kwargs):
    kwargs.setdefault("cluster_builder", getPublicTool("CscThresholdClusterBuilderTool") )
    return CfgMgr.CscThresholdClusterBuilder(name,**kwargs)

def QratCscClusterFitter( name="QratCscClusterFitter",extraFlags=None,**kwargs):
    qratcor_css_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0699381, 0.178291, 0.271303, 0.345611, 
        0.400738, 0.452451, 0.493772, 0.526792, 0.553845, 0.580111, 0.603337, 0.624749, 
        0.646065, 0.663924, 0.681897, 0.701345, 0.716067, 0.730324, 0.743168, 0.760598, 
        0.773341, 0.786406, 0.798358, 0.811339, 0.820949, 0.832676, 0.843917, 0.85412, 
        0.863914, 0.873229, 0.88251, 0.892856, 0.903296, 0.910067, 0.917892, 0.928028, 
        0.93461, 0.943679, 0.951617, 0.958326, 0.964428, 0.97341, 0.978204, 0.987075, 
        0.994789, 1.000000
        ]
    qratcor_csl_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0290177, 0.0834867, 0.188683, 0.269967, 
        0.334887, 0.393036, 0.440317, 0.481884, 0.51725, 0.545107, 0.56959, 0.592327, 
        0.614087, 0.635344, 0.655252, 0.672766, 0.690929, 0.707293, 0.722742, 0.738323, 
        0.753562, 0.76749, 0.780983, 0.79354, 0.806521, 0.818085, 0.829987, 0.841443, 
        0.853723, 0.863393, 0.87431, 0.883139, 0.892962, 0.902803, 0.911276, 0.921417, 
        0.929387, 0.938106, 0.947072, 0.954406, 0.961692, 0.97055, 0.978517, 0.985133, 
        0.992257, 1.000000
        ]
    kwargs.setdefault("qratcor_csl_eta",qratcor_csl_eta)
    kwargs.setdefault("qratcor_css_eta",qratcor_css_eta)
    kwargs.setdefault("CscAlignmentTool", getPublicTool("CscAlignmentTool") )
    return CfgMgr.QratCscClusterFitter(name,**kwargs)


def CscSegmentUtilTool(name= "CscSegmentUtilTool",extraFlags=None,**kwargs):
    kwargs.setdefault("precision_fitter",  getPublicTool("QratCscClusterFitter") )
    kwargs.setdefault("rot_creator",  getPublicTool("CscClusterOnTrackCreator") )
    return CfgMgr.CscSegmentUtilTool(name,**kwargs)

def Csc2dSegmentMaker(name= "Csc2dSegmentMaker",extraFlags=None,**kwargs):
    kwargs.setdefault("segmentTool",  getPublicTool("CscSegmentUtilTool") )
    return CfgMgr.Csc2dSegmentMaker(name,**kwargs)

def Csc4dSegmentMaker(name= "Csc4dSegmentMaker",extraFlags=None,**kwargs):
    kwargs.setdefault("segmentTool",  getPublicTool("CscSegmentUtilTool") )
    return CfgMgr.Csc4dSegmentMaker(name,**kwargs)


