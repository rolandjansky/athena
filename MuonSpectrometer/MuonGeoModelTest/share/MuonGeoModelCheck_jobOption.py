#----
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
import AthenaCommon.AtlasUnixStandardJob
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr
## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
#import AthenaPoolCnvSvc.AthenaPool
#ServiceMgr.EventSelector.RunNumber            = 122189
#ServiceMgr.EventSelector.InitialTimeStamp     = 1342888195



# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-20-00-01"
    print 'MuonGeoModelTest/runAmdcMGM DetDescrVersion now set to ', DetDescrVersion
else:
    print 'MuonGeoModelTest/runAmdcMGM DetDescrVersion is already ', DetDescrVersion
                    

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

# 0 to run MuonGM checks init time only
# 1 to run MuonGM checks at each event
GMCheckLoop = 0

if GMCheckLoop:
    #import AthenaPoolCnvSvc.ReadAthenaPool
    #ServiceMgr.EventSelector.InputCollections = [ "/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root" ]
    theApp.EvtMax = 10
else:
    #theApp.EvtSel = "NONE"
    theApp.EvtMax = 1

# Modify A line parameters
# GeneralControlAlines = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation
# I2 = 0/1 zerofy or not the z translation
# I3 = 0/1 zerofy or not the t translation
# I4 = 0/1 zerofy or not the s rotation
# I5 = 0/1 zerofy or not the z rotation
# I6 = 0/1 zerofy or not the t rotation
GeneralControlAlines = 111111

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO


#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"

print GeoModelSvc

#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.StationSelection = 0
MuonDetectorTool.SelectedStations = [ "BOG" ]
MuonDetectorTool.SelectedStJzz = [3]
MuonDetectorTool.SelectedStJff = [6]
MuonDetectorTool.ControlAlines    = GeneralControlAlines
MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000; # avoid building the dead matter
MuonDetectorTool.FillCacheInitTime=1
MuonDetectorTool.DumpMemoryBreakDown=True

print MuonDetectorTool

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMCheck
MuonGMCheck = MuonGMCheck()
MuonGMCheck.EventLoopMode = GMCheckLoop
MuonGMCheck.check_mdt = 1
MuonGMCheck.check_rpc = 1
MuonGMCheck.check_tgc = 1
MuonGMCheck.check_csc = 1
#MuonGMCheck.check_surfaces = 1

print MuonGMCheck


######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

topSequence += MuonGMCheck

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


