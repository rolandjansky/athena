# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getAlgorithm,getPublicToolClone,getService,getServiceClone
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

def MuTagMatchingTool( name='MuTagMatchingTool', **kwargs ):
    kwargs.setdefault("AssumeLocalErrors", True )
    kwargs.setdefault("PhiCut", 30. )
    kwargs.setdefault("GlobalPhiCut", 1.)
    kwargs.setdefault("ThetaCut", 5. )
    kwargs.setdefault("GlobalThetaCut", 0.5 )
    kwargs.setdefault("ThetaAngleCut", 5. )
    kwargs.setdefault("DoDistanceCut", True )
    kwargs.setdefault("CombinedPullCut", 3.0 )
    return CfgMgr.MuTagMatchingTool(name,**kwargs)

def MuTagAmbiguitySolverTool(name='MuTagAmbiguitySolverTool', **kwargs ):
    kwargs.setdefault("RejectOuterEndcap",True)
    kwargs.setdefault("RejectMatchPhi",True)
    return CfgMgr.MuTagAmbiguitySolverTool(name,**kwargs)


def MuonSegmentTagTool( name="MuonSegmentTagTool", **kwargs ):
    kwargs.setdefault("MuTagMatchingTool", getPublicTool("MuTagMatchingTool") )
    kwargs.setdefault("MuTagAmbiguitySolverTool", getPublicTool("MuTagAmbiguitySolverTool") )
    return CfgMgr.MuonCombined__MuonSegmentTagTool(name,**kwargs)

