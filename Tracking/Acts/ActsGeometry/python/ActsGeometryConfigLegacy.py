# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getActsTrackingGeometryTool(name="ActsTrackingGeometryTool", **kwargs):
    kwargs.setdefault("TrackingGeometrySvc", "ActsTrackingGeometrySvc")
    return CfgMgr.ActsTrackingGeometryTool(name, **kwargs)


def getActsExtrapolationTool(name="ActsExtrapolationTool", **kwargs):
    return CfgMgr.ActsExtrapolationTool(name, **kwargs)


def getActsTrackingGeometrySvc(name="AtlasTrackingGeometrySvc", **kwargs):
    # Configuration for ACTS Fatras running
    kwargs.setdefault("UseMaterialMap", True)
    kwargs.setdefault("MaterialMapInputFile", "ACTS/material-maps-ATLAS-R2-2016-00-00-00_v1.json")

    from AthenaCommon.DetFlags import DetFlags
    subDetectors = []

    if DetFlags.bpipe_on():
        kwargs.setdefault("BuildBeamPipe", True)

    if DetFlags.pixel_on():
        subDetectors += ["Pixel"]

    if DetFlags.SCT_on():
        subDetectors += ["SCT"]

    if DetFlags.TRT_on():
        # Not production ready yet, but want
        # it on for technical tests
        subDetectors += ["TRT"]

    if DetFlags.Calo_on():
        # Commented out because Calo is not production ready yet and we don't
        # want to turn it on even if the global flag is set
        #subDetectors += ["Calo"]
        #kwargs.setdefault("CaloVolumeBuilder", CfgMgr.ActsCaloTrackingVolumeBuilder() )
        pass

    kwargs.setdefault("BuildSubDetectors", subDetectors)

    return CfgMgr.ActsTrackingGeometrySvc("ActsTrackingGeometrySvc", **kwargs)
