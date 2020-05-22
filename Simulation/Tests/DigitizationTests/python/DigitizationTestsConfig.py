# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr


def McEventCollectionTestTool(name="McEventCollectionTestTool", **kwargs):
    return CfgMgr.McEventCollectionTestTool(name, **kwargs)


def EventInfoTestTool(name="EventInfoTestTool", **kwargs):
    return CfgMgr.EventInfoTestTool(name, **kwargs)


def PixelRDOsTestTool(name="PixelRDOsTestTool", **kwargs):
    return CfgMgr.PixelRDOsTestTool(name, **kwargs)


def SCT_RDOsTestTool(name="SCT_RDOsTestTool", **kwargs):
    return CfgMgr.SCT_RDOsTestTool(name, **kwargs)


def TRT_RDOsTestTool(name="TRT_RDOsTestTool", **kwargs):
    return CfgMgr.TRT_RDOsTestTool(name, **kwargs)
