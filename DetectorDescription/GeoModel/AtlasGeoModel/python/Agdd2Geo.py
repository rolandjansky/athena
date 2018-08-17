# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags

if ( DetFlags.detdescr.Muon_on() ):
    from AthenaCommon import CfgGetter
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp

    from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
    AGDD2Geo = AGDDtoGeoSvc()

    if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders:
        ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
        AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]

    if CommonGeometryFlags.Run() == "RUN3" :
        if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders:
            ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
            AGDD2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]

    theApp.CreateSvc += ["AGDDtoGeoSvc"]
    ServiceMgr += AGDD2Geo
