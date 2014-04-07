#----
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.AtlasUnixStandardJob

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion = "ATLAS-GEO-20-00-01"
    print "MuonGeoModelTest/MuonHitRelocation Setting now DetDescrVersion to ", DetDescrVersion
else:
    print "MuonGeoModelTest/MuonHitRelocation DetDescrVersion already set to ", DetDescrVersion
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

if not 'ConditionsTag' in dir():
    ConditionsTag = "OFLCOND-MC12b-SIM-00"
    print "MuonGeoModelTest/MuonHitRelocation Setting now ConditionsTag to ", ConditionsTag
else:
    print "MuonGeoModelTest/MuonHitRelocation ConditionsTag already set to ", ConditionsTag
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.ConditionsTag = ConditionsTag

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

if not 'inputHits' in dir():
    inputHits="geantinoHits.pool.root"
    print "inputHits not known yet - setting to ", inputHits 
else:
    print "inputHits already known and equal to ", inputHits 

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ inputHits ]

theApp.EvtMax = 1000000000

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 9999999

theApp.HistogramPersistency = "ROOT"
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='RelocatedMuonHits.root' OPT='NEW'" ]

print "Reading alignment constants from DB"
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')
from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                      "/MUONALIGN/TGC/SIDEA",
                                      "/MUONALIGN/TGC/SIDEC"]
ToolSvc += MuonAlignmentDbTool
MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
MGM_AlignmentDbTool.OutputLevel=DEBUG

#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.07.00"
GeoModelSvc.IgnoreTagDifference =True

print GeoModelSvc

#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.StationSelection = 0
MuonDetectorTool.SelectedStations = [ "EIL", "B" ]
MuonDetectorTool.OutputLevel=VERBOSE
MuonDetectorTool.UseConditionDb = 0

print MuonDetectorTool

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonHitRelocation
MuonHitRelocation = MuonHitRelocation()
MuonHitRelocation.OutputLevel = INFO

print MuonHitRelocation

######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += MuonHitRelocation

print " "
print "List of all Dlls"
print theApp.Dlls
print " "
print "List of all Ext services"
print theApp.ExtSvc
print " "
print "List of all top algorithms"
print theApp.TopAlg

#print "Print here Top Sequence" 
#print topSequence
#print "Print here Service Manager" 
#print ServiceMgr


