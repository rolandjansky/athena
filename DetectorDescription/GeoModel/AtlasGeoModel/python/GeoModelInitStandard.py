# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]

# Load the detectors. These job option fragments look at DetFlags 
from AtlasGeoModel import InDetGM
from AtlasGeoModel import LArGM
from AtlasGeoModel import TileGM
from AtlasGeoModel import MuonGM
from AtlasGeoModel import ForDetGM
from AtlasGeoModel import MiscGM

