# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    kwargs.setdefault("ServiceBuilderTool",    "")

    return CfgMgr.PixelDetectorTool(name, **kwargs)
