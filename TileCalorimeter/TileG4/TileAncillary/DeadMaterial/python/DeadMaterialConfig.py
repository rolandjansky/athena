# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getDeadMaterialTool(name="DeadMaterial", **kwargs):
    kwargs.setdefault("DetectorName", "DeadMaterial")
    kwargs.setdefault("ZLength", 150.)
    kwargs.setdefault("YLength", 1935.)
    kwargs.setdefault("XLength", 150.)
    return CfgMgr.DeadMaterialTool(name, **kwargs)
