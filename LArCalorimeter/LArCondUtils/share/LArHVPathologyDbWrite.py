#
# Job Options for writing LAr Cell HV Pathologies to COOL using local SQLite database
#

from time import strptime,time
from calendar import timegm

if "InputFile" not in dir():
    InputFile="HVPathologies.txt"

if "OutputFile" not in dir():
    OutputFile="larhvpathology.db"

if "tagName" not in dir():
    tagName="LARHVPathologiesOflPathologies-RUN2-UPD1-00"

if "RunNumber" not in dir():
    RunNumber = 999999

if "LBNumber" not in dir():
    LBNumber = 0

if "GloablTag" not in dir():
    GlobalTag = 'CONDBR2-BLKPA-2017-03'

if "date" not in dir():
    date="2015-09-29:12:00:00"

if "TimeStamp" not in dir():
   try:
      ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
      TimeStamp=int(timegm(ts))*1000000000L
   except ValueError:
      print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"


## basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
#from AthenaCommon.AppMgr import theApp

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
#DetFlags.Tile_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'bytestream'


from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R2-2015-04-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

TileUseDCS=False
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
#include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include("TileConditions/TileConditions_jobOptions.py" )
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

#from LArConditionsCommon import LArHVDB

from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")


#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

from LArCondUtils.LArCondUtilsConf import LArHVPathologyDbAlg
LArHVPathologyDbAlg = LArHVPathologyDbAlg()
# Choose the following to write out cond objects 
LArHVPathologyDbAlg.WriteCondObjs = True
# Other properties of LArHVPathologyDbAlg
LArHVPathologyDbAlg.InpFile       =  InputFile
LArHVPathologyDbAlg.Folder        = "/LAR/HVPathologiesOfl/Pathologies"
LArHVPathologyDbAlg.TagName       = tagName
LArHVPathologyDbAlg.Mode          = 1
topSequence += LArHVPathologyDbAlg

# Here mytest.db is the name of SQLite file created by this job
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+OutputFile+";dbname=CONDBR2"

svcMgr.MessageSvc.OutputLevel = 4
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.MessageSvc.infoLimit   = 100000

topSequence.LArHVPathologyDbAlg.OutputLevel = 2
svcMgr.IOVDbSvc.OutputLevel     = 3


#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
regSvc.RecreateFolders = False
regSvc.BeginRun = RunNumber
regSvc.BeginLB = LBNumber

#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = int(TimeStamp/1e9)
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
#import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------

# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = TRUE

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------
#theApp.OutStream = []



