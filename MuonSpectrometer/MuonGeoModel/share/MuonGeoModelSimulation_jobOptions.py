############## 
from AtlasGeoModel.MuonGM import GeoModelSvc
############## special options for simuation ###########################################
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
# allow fine clash fixing in simulation 
MuonDetectorTool.EnableFineClashFixing = 1 # default is 0 for reco.
# turn off all tracking related calculations to reduce memory allocation 
MuonDetectorTool.CachingFlag       = 0 # default is 1 for reco 
MuonDetectorTool.FillCacheInitTime = 0 # default is 1 for reco 
MuonDetectorTool.UseConditionDb    = 0 # default might become 1 for reco 
#
print MuonDetectorTool
