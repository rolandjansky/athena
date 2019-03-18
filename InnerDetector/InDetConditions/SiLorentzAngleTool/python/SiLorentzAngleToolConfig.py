# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory

from AthenaCommon import CfgMgr

def getSCTLorentzAngleTool(name="SCTLorentzAngleTool", **kwargs):
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
    return CfgMgr.SiLorentzAngleTool(name, **kwargs)

def getPixelLorentzAngleTool(name="PixelLorentzAngleTool", **kwargs):
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData","PixelSiLorentzAngleCondData")
    from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
    return CfgMgr.SiLorentzAngleTool(name, **kwargs)
