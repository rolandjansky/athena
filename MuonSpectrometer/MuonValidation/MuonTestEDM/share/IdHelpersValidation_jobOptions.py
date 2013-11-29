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


# Set output level threshold
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"


#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
DetDescrCnvSvc.OutputLevel   = DEBUG
DetDescrCnvSvc.IdDictFromRDB = False
DetDescrCnvSvc.IdDictGlobalTag = "destaged_layout"
DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_L1Onl.xml"
DetDescrCnvSvc.ForwardIDFileName = "IdDictForwardDetectors_2010.xml"

######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

# Add top algorithms to be run
from MuonTestEDM.MuonTestEDMConf import Muon__IdHelpersValidation
topSequence += Muon__IdHelpersValidation("IdHelperTest",
                                         FilePrefix = "/tmp/pfleisch/mig3/",
                                         OutputLevel=2,
                                         CheckMdtIds=True,
                                         CheckCscIds=True,
                                         CheckRpcIds=True,
                                         CheckTgcIds=True,
                                         CheckMmIds=True,
                                         CheckStgcIds=True)

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


