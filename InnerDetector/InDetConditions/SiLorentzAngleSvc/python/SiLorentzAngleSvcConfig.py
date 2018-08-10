# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory

from AthenaCommon import CfgMgr

def getSCTLorentzAngleTool(name="SCTLorentzAngleTool", **kwargs):
    kwargs.setdefault("DetectorName", "SCT")
    from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleTool
    return CfgMgr.SiLorentzAngleTool(name, **kwargs)
