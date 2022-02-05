# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    kwargs.setdefault("ServiceBuilderTool", "")
    kwargs.setdefault("GeometryDBSvc", "InDetGeometryDBSvc")
    return CfgMgr.PixelDetectorTool(name, **kwargs)
