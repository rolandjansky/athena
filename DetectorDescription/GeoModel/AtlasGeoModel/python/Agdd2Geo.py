# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags      import DetFlags

if ( DetFlags.detdescr.Muon_on() ):
    from AthenaCommon import CfgGetter
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp

    ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)

    from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
    AGDD2Geo = AGDDtoGeoSvc()
    AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]

    theApp.CreateSvc += ["AGDDtoGeoSvc"]
    ServiceMgr += AGDD2Geo
