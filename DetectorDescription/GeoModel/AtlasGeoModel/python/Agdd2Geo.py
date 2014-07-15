# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags      import DetFlags

if ( DetFlags.detdescr.Muon_on() ):
    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaCommon.AppMgr import theApp
    from AGDD2Geo.AGDD2GeoConf import AGDD2GeoSvc
    agdd2GeoSvc = AGDD2GeoSvc()
    agdd2GeoSvc.Locked = True
    theApp.CreateSvc += ["AGDD2GeoSvc"]
    ServiceMgr += agdd2GeoSvc
