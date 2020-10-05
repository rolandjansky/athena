# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
GeoModelSvc.DetectorTools += [ MuonDetectorTool(HasSTgc=False, HasMM=False) ]
GeoModelSvc.SupportedGeometry=21
GeoModelSvc.AtlasVersion='ATLAS-R2-2016-01-00-01'
try:
    if database_layout: GeoModelSvc.MuonVersionOverride=database_layout
except NameError:
    pass
theApp.CreateSvc += ["GeoModelSvc"]
ServiceMgr += GeoModelSvc
