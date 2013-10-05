#
# Test joboptions for the loading of the DetectorStore
#
# For MC EventSelector:
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

# Set handler for traceback
gbl.AthenaServices.SetFatalHandler(438)

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp


#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
# For general flags
#DetDescrVersion = "ATLAS-CSC-01-02-00"
#DetDescrVersion = "ATLAS-CSC-02-00-00"
DetDescrVersion = "ATLAS-GEO-08-03-00"


include( "RecExCommon/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
#DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.Tile_setOff()
      
# Set version (must do so before AllDet_detDescr include)
# DetDescrVersion = "Rome-Initial"

# set up all detector description description 
include ("RecExCommon/AllDet_detDescr.py")

# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")
include( "IOVDbSvc/IOVRecExCommon.py" )

globalflags.ConditionsTag = 'OFLCOND-CSC-00-00-00'

if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
#--------------------------------------------------------------
from IdDictTest.IdDictTestConf import IdDictTest
topSequence += IdDictTest( "IdDictTest" )

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.Format      = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.debugLimit  = 100000
IdDictTest = Algorithm( "IdDictTest" )
IdDictTest.OutputLevel        = DEBUG
IdDictTest.muons              = False

# Provide dictionary file
#DetDescrCnvSvc.IdDictName = "ATLAS_IDS.xml"
svcMgr.DetDescrCnvSvc.OutputLevel   = DEBUG
svcMgr.DetDescrCnvSvc.IdDictFromRDB = False
svcMgr.DetDescrCnvSvc.IdDictFromRDB = True

svcMgr.DetDescrCnvSvc.IdDictGlobalTag = "destaged_layout"
# svcMgr.DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
svcMgr.DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_sLHC-MiniFcal-00.xml"
svcMgr.DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_L1Onl.xml"
svcMgr.DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
# Neighbours not working yet!:
# svcMgr.DetDescrCnvSvc.DoInitNeighbours = False

# Number of events to be processed (default is 10)
theApp.EvtMax = 2
