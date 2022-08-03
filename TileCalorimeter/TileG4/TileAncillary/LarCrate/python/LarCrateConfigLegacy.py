# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getLArCrateTool(name="LArCrate", **kwargs):
    kwargs.setdefault("DetectorName", "LArCrate")
    kwargs.setdefault("ZLength", 512.)
    kwargs.setdefault("YLength", 800.)
    kwargs.setdefault("XLength", 430.)
    return CfgMgr.LArCrateTool(name, **kwargs)
