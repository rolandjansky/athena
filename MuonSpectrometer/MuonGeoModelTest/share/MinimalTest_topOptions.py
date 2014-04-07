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


############################ general initialization of the geometry 
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-20-00-01"
    print 'MuonGeoModelTest/MinimalTest_topOptions DetDescrVersion now set to ', DetDescrVersion
else:
    print 'MuonGeoModelTest/MinimalTest_topOptions DetDescrVersion is already ', DetDescrVersion
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


##***************************************************** HERE override MuonSpectrometer tag
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.06.03-NSW.01.64"

############################ general Muon GetectorDescription version setup
from MuonGeoModelTest import MuonGeoSetup


    
if not 'DictFromFile' in dir():
    DictFromFile=False
if DictFromFile:
    # this must happen after the Geometry initialization
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_S.01.xml"
    #DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.02.xml" 
    DetDescrCnvSvc.OutputLevel    = INFO
    DetDescrCnvSvc.IdDictFromRDB     = False
    DetDescrCnvSvc.IdDictGlobalTag   = "destaged_layout"
    DetDescrCnvSvc.LArIDFileName     = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
    DetDescrCnvSvc.CaloIDFileName    = "IdDictCalorimeter_L1Onl.xml"
    DetDescrCnvSvc.ForwardIDFileName = "IdDictForwardDetectors_2010.xml"


###############################################################
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

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = DEBUG
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 99999999


print GeoModelSvc
###print MuonDetectorTool

    
#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMCheck
MuonGMCheck = MuonGMCheck()
MuonGMCheck.EventLoopMode = GMCheckLoop
MuonGMCheck.check_mm   = 0
MuonGMCheck.check_stgc = 0
MuonGMCheck.check_mdt = 1
MuonGMCheck.check_rpc = 1
MuonGMCheck.check_tgc = 1
MuonGMCheck.check_csc = 1 # as an example
#MuonGMCheck.check_surfaces = 1
MuonGMCheck.OutputLevel=DEBUG

print MuonGMCheck


######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

topSequence += MuonGMCheck

## Add top algorithms to be run
#from IdHelpersValidation.IdHelpersValidationConf import Muon__IdHelpersValidation
#topSequence += Muon__IdHelpersValidation("Philipp",FilePrefix = "/tmp/stefania/new/",OutputLevel=2)



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


