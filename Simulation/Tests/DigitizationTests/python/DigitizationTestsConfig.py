# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def McEventCollectionTestTool(name="McEventCollectionTestTool", **kwargs):
    return CfgMgr.McEventCollectionTestTool(name, **kwargs)
def PileUpEventInfoTestTool(name="PileUpEventInfoTestTool", **kwargs):
    return CfgMgr.PileUpEventInfoTestTool(name, **kwargs)
def PixelRDOsTestTool(name="PixelRDOsTestTool", **kwargs):
    return CfgMgr.PixelRDOsTestTool(name, **kwargs)
def SCT_RDOsTestTool(name="SCT_RDOsTestTool", **kwargs):
    return CfgMgr.SCT_RDOsTestTool(name, **kwargs)
def TRT_RDOsTestTool(name="TRT_RDOsTestTool", **kwargs):
    return CfgMgr.TRT_RDOsTestTool(name, **kwargs)

#For HepMcParticleLink InDetSimData checks
def PixelSDOMapTestTool(name="PixelSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","PixelSDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/Pixel/PixelSDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def PixelPUSDOMapTestTool(name="PixelPUSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","Pixel_PU_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/Pixel/Pixel_PU_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def SCTSDOMapTestTool(name="SCTSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","SCT_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/SCT/SCT_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def SCTPUSDOMapTestTool(name="SCTPUSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","SCT_PU_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/SCT/SCT_PU_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def TRTSDOMapTestTool(name="TRTSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","TRT_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/TRT/TRT_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def TRTPUSDOMapTestTool(name="TRTPUSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","TRT_PU_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/TRT/TRT_PU_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def BCMSDOMapTestTool(name="BCMSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","BCM_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/BCM/BCM_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
def BCMPUSDOMapTestTool(name="BCMPUSDOMapTestTool", **kwargs):
    kwargs.setdefault("SDO_Map_Name","BCM_PU_SDO_Map")
    kwargs.setdefault("OutputAdditionalPath","InDet/BCM/BCM_PU_SDO_Map")
    return CfgMgr.InDetSDOMapTestTool(name, **kwargs)
