# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

if ( DetFlags.detdescr.Muon_on() ):
    from AthenaCommon import CfgGetter
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp

    from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
    AGDD2Geo = AGDDtoGeoSvc()

    if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders:
        ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
        AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]

    Run3NSW = CommonGeometryFlags.Run() in ["RUN3"]
    Run4NSW = CommonGeometryFlags.Run() in ["RUN4"] and not MuonGeometryFlags.hasCSC() # assumes RUN4 layouts will be symmetric
    if Run3NSW or Run4NSW:
        if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders:
            ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
            AGDD2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]

    theApp.CreateSvc += ["AGDDtoGeoSvc"]
    ServiceMgr += AGDD2Geo
