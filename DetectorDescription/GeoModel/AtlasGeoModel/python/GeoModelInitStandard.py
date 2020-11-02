# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]

# Load the detectors. These job option fragments look at DetFlags 
from AtlasGeoModel import InDetGM   # noqa: F401
from AtlasGeoModel import LArGM     # noqa: F401
from AtlasGeoModel import TileGM    # noqa: F401
from AtlasGeoModel import MuonGM    # noqa: F401
from AtlasGeoModel import ForDetGM  # noqa: F401
from AtlasGeoModel import MiscGM    # noqa: F401
