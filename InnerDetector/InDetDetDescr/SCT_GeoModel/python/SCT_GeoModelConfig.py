# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSCT_DetectorTool(name="SCT_DetectorTool", **kwargs):
    kwargs.setdefault("DetectorName",     "SCT");
    kwargs.setdefault("InitialLayout",    False);
    kwargs.setdefault("Alignable",        True);
    kwargs.setdefault("RDBAccessSvc",     "RDBAccessSvc");
    kwargs.setdefault("GeometryDBSvc",    "InDetGeometryDBSvc");
    kwargs.setdefault("GeoDbTagSvc",      "GeoDbTagSvc");
    from AthenaCommon.DetFlags      import DetFlags
    return CfgMgr.SCT_DetectorTool(name, **kwargs)
