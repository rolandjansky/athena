# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSCT_DetectorTool(name="SCT_DetectorTool", **kwargs):
    kwargs.setdefault("DetectorName",     "SCT")
    kwargs.setdefault("Alignable",        True)
    kwargs.setdefault("RDBAccessSvc",     "RDBAccessSvc")
    kwargs.setdefault("GeometryDBSvc",    "InDetGeometryDBSvc")
    kwargs.setdefault("GeoDbTagSvc",      "GeoDbTagSvc")
    return CfgMgr.SCT_DetectorTool(name, **kwargs)
