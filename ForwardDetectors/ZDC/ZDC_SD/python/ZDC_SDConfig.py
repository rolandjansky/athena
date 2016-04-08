# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getZDC_StripSD(name="ZDC_StripSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["ZDC::Strip_Logical"])
    kwargs.setdefault("OutputCollectionNames", ["ZDC_SimStripHit_Collection"])
    return CfgMgr.ZDC_StripSDTool(name, **kwargs)
def getZDC_PixelSD(name="ZDC_PixelSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["ZDC::Pixel_Logical"])
    kwargs.setdefault("OutputCollectionNames", ["ZDC_SimPixelHit_Collection"])
    return CfgMgr.ZDC_PixelSDTool(name, **kwargs)
