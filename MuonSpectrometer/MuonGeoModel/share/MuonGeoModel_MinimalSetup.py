# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
GeoModelSvc.DetectorTools += [ MuonDetectorTool(HasSTgc=False, HasMM=False, UseConditionDb=0, UseAsciiConditionData=0) ]
GeoModelSvc.SupportedGeometry=21
GeoModelSvc.AtlasVersion='ATLAS-R2-2016-01-00-01'
GeoModelSvc.MuonVersionOverride=database_layout
theApp.CreateSvc += ["GeoModelSvc"]
ServiceMgr += GeoModelSvc
