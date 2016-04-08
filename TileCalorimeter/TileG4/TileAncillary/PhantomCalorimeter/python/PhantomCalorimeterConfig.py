# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getPhantomBarrelSD(name="PhantomBarrelSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['PhantomBarrel::ScintillatorLayer'])
    kwargs.setdefault("OutputCollectionNames", ["TileTBHits"])
    return CfgMgr.PhantomBarrelSDTool(name, **kwargs)
def getPhantomBarrelTool(name="PhantomBarrel", **kwargs):
    kwargs.setdefault("ZLength", 400., "");
    kwargs.setdefault("RMin", 1100., "");
    kwargs.setdefault("RMax", 1670., "");
    return CfgMgr.PhantomBarrelTool(name, **kwargs)
