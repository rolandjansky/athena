# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory

from AthenaCommon import CfgMgr

def getSCTLorentzAngleTool(name="SCTLorentzAngleTool", **kwargs):
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("DetEleCollKey", "SCT_DetectorElementCollection")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    return CfgMgr.SiLorentzAngleTool(name, **kwargs)

def getPixelLorentzAngleTool(name="PixelLorentzAngleTool", **kwargs):
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("DetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("SiLorentzAngleCondData", "PixelSiLorentzAngleCondData")
    return CfgMgr.SiLorentzAngleTool(name, **kwargs)
