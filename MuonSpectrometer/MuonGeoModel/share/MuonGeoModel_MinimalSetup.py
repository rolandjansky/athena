from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
GeoModelSvc.DetectorTools += [ MuonDetectorTool() ]
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].BuildFromNova = 0
# GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].TheMuonAlignmentTool = ""
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseConditionDb = 0
GeoModelSvc.DetectorTools[ "MuonDetectorTool" ].UseAsciiConditionData = 0
GeoModelSvc.SupportedGeometry=21
GeoModelSvc.AtlasVersion='ATLAS-R2-2016-01-00-01'
GeoModelSvc.MuonVersionOverride=database_layout
theApp.CreateSvc += ["GeoModelSvc"]
ServiceMgr += GeoModelSvc
