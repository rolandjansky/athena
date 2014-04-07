# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-21-00-01"
    #DetDescrVersion="ATLAS-GEOTEST"
    print 'MuonGeoModelTest/NSWGeoSetup DetDescrVersion now set to ', DetDescrVersion
else:
    print 'MuonGeoModelTest/NSWGeoSetup DetDescrVersion is already ', DetDescrVersion
                  

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if DetDescrVersion=="ATLAS-GEOTEST" or DetDescrVersion=="ATLAS-GEO-21-00-01":
    # do nothing
    print 'NSWGeoSetup: no need to override tag on the MuonSpectrometer Node with ATLAS tag ', DetDescrVersion 
else:
    GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.06.03-NSW.01.64"
print GeoModelSvc

#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.StationSelection  = 2
MuonDetectorTool.SelectedStations  = [ "EIL1" ]
MuonDetectorTool.SelectedStations  += [ "EIL2" ]
MuonDetectorTool.SelectedStations  += [ "EIS*" ]
MuonDetectorTool.SelectedStations  += [ "EIL10" ]
MuonDetectorTool.SelectedStations  += [ "EIL11" ]
MuonDetectorTool.SelectedStations  += [ "CSS*" ]
MuonDetectorTool.SelectedStations  += [ "CSL*" ]
MuonDetectorTool.SelectedStations  += [ "T4E*" ]
MuonDetectorTool.SelectedStations  += [ "T4F*" ]
from AGDD2Geo.AGDD2GeoConf import AGDD2GeoSvc
Agdd2GeoSvc = AGDD2GeoSvc()
if not hasattr(Agdd2GeoSvc, 'OverrideConfiguration') or not Agdd2GeoSvc.OverrideConfiguration :
    Agdd2GeoSvc.Locked = True
Agdd2GeoSvc.PrintSections = False
# The following 4 lines were relevant only before porting the NSW description in Oracle   
#Agdd2GeoSvc.XMLFiles += ["stations.v1.63.xml"]
#Agdd2GeoSvc.Sections += ["NewSmallWheel"] ##### needed for v1.63
#Agdd2GeoSvc.DisableSections = False
Agdd2GeoSvc.PrintLevel = 1
theApp.CreateSvc += ["AGDD2GeoSvc"]
ServiceMgr += Agdd2GeoSvc
