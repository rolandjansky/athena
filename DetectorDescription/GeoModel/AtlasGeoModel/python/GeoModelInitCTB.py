# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]

from AtlasGeoModel import InDetGMCTB  # noqa: F401
from AtlasGeoModel import TileGMCTB   # noqa: F401
from AtlasGeoModel import LArGM       # noqa: F401
