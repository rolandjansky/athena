# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool
from InDetRecExample import TrackingCommon
from AthenaCommon.AppMgr import ServiceMgr
def MuTagMatchingTool( name='MuTagMatchingTool', **kwargs ):
    kwargs.setdefault("AssumeLocalErrors", True )
    kwargs.setdefault("PhiCut", 30. )
    kwargs.setdefault("GlobalPhiCut", 1.)
    kwargs.setdefault("ThetaCut", 5. )
    kwargs.setdefault("GlobalThetaCut", 0.5 )
    kwargs.setdefault("ThetaAngleCut", 5. )
    kwargs.setdefault("DoDistanceCut", True )
    kwargs.setdefault("CombinedPullCut", 3.0 )
    if TrackingCommon.use_tracking_geometry_cond_alg:
        cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
        kwargs.setdefault('TrackingGeometryReadKey', cond_alg.TrackingGeometryWriteKey)
    else:
        kwargs.setdefault('TrackingGeometrySvc', ServiceMgr.AtlasTrackingGeometrySvc )
    
    return CfgMgr.MuTagMatchingTool(name,**kwargs)

def MuTagAmbiguitySolverTool(name='MuTagAmbiguitySolverTool', **kwargs ):
    kwargs.setdefault("RejectOuterEndcap",True)
    kwargs.setdefault("RejectMatchPhi",True)
    return CfgMgr.MuTagAmbiguitySolverTool(name,**kwargs)


def MuonSegmentTagTool( name="MuonSegmentTagTool", **kwargs ):
    kwargs.setdefault("ParticleCaloExtensionTool", getPublicTool("MuonParticleCaloExtensionTool"))
    kwargs.setdefault("MuTagMatchingTool", getPublicTool("MuTagMatchingTool") )
    kwargs.setdefault("MuTagAmbiguitySolverTool", getPublicTool("MuTagAmbiguitySolverTool") )
    return CfgMgr.MuonCombined__MuonSegmentTagTool(name,**kwargs)

