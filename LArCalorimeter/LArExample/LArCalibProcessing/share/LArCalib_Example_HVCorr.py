#################################################################
# 
# example jobOptions to read HV from Cool/DCS in athena
#  and compute cell level correction factors to store in conditions 
#  database
#
##################################################################

from time import strptime,time
from calendar import timegm

#set date to compute the correction

if "date" not in dir():
    date="2013-02-06:09:45:00"


if "TimeStamp" not in dir():
   try:
      ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
      TimeStamp=int(timegm(ts))*1000000000L
   except ValueError:
      print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"
      

from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi

rlb=TimeStampToRunLumi(TimeStamp,dbInstance="CONDBR2")
if rlb is None:
   print "WARNING: Failed to convert time",TimeStamp,"into a run/lumi number"
   RunNumber=999999
   LumiBlock=0
else:
   RunNumber=rlb[0]
   LumiBlock=rlb[1]


print "---> Working on run",RunNumber,"LB",LumiBlock,"Timestamp:",TimeStamp
timediff=int(time()-(TimeStamp/1000000000L))
if timediff<0:
    print "ERROR: Timestamp in the future???"
else:
    (days,remainder)=divmod(timediff,24*60*60)
    (hours,seconds)=divmod(remainder,60*60)
    print "---> Timestamp is %i days %i hours and %i minutes ago" % (days,hours,int(seconds/60))
    pass
                                                                

# name of output local sql file
OutputSQLiteFile = 'HVScaleCorr.db'

# name of output Pool file
PoolFileName = "dummy.pool.root"

# database folder
LArHVScaleCorrFolder = "/LAR/ElecCalibFlat/HVScaleCorr"

# output key
keyOutput = "LArHVScaleCorr"

# tag suffix
#LArCalibFolderOutputTag = "-UPD3-00"

# write IOV
WriteIOV      = True

# global tag to read other conditions if needed
if "GlobalTag" not in dir():
    GlobalTag     = 'LARCALIB-RUN2-00'

# begin run IOV
IOVBegin = 0

###################################################################

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(int(RunNumber))

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DatabaseInstance="CONDBR2"

# Get a handle to the default top-level algorithm sequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R2-2015-04-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.IOVDbSvc.GlobalTag = GlobalTag
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
#include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
#include( "TileIdCnv/TileIdCnv_jobOptions.py" )
#include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

#include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

#block to read the existing HVCorr
conddb.blockFolder(LArHVScaleCorrFolder);

from LArConditionsCommon import LArHVDB #Sets HV Calbling and DCS Database folders
#conddb.addOverride("/LAR/IdentifierOfl/HVLineToElectrodeMap","LARIdentifierOflHVLineToElectrodeMap-UPD3-00")
from LArCondUtils.LArCondUtilsConf import LArHVToolDB
theLArHVToolDB = LArHVToolDB("LArHVToolDB")
ToolSvc += theLArHVToolDB

from LArRecUtils.LArRecUtilsConf import LArHVCorrTool
theLArHVCorrTool = LArHVCorrTool("LArHVCorrTool")
theLArHVCorrTool.keyOutput = keyOutput
theLArHVCorrTool.folderName= LArHVScaleCorrFolder
theLArHVCorrTool.HVTool = theLArHVToolDB
ToolSvc += theLArHVCorrTool

from LArCalibUtils.LArCalibUtilsConf import LArHVCorrMaker
theLArHVCorrMaker = LArHVCorrMaker("LArHVCorrMaker")
topSequence += theLArHVCorrMaker

from LArCalibTools.LArCalibToolsConf import LArHVScaleCorr2Ntuple
theLArHVScaleCorr2Ntuple = LArHVScaleCorr2Ntuple("LArHVScaleCorr2Ntuple")
theLArHVScaleCorr2Ntuple.AddFEBTempInfo = False
topSequence += theLArHVScaleCorr2Ntuple

#from LArCalibTools.LArCalibToolsConf import LArWFParams2Ntuple
#LArWFParams2Ntuple = LArWFParams2Ntuple("LArWFParams2Ntuple")
#LArWFParams2Ntuple.DumpTdrift = True
#topSequence += LArWFParams2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='hvcorr_ntuple.root' OPT='NEW'" ]

# deal with DB output
OutputObjectSpec = "CondAttrListCollection#"+LArHVScaleCorrFolder
OutputObjectSpecTag = ''
OutputDB = "sqlite://;schema="+OutputSQLiteFile+";dbname=CONDBR2"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",PoolFileName,
                          [OutputObjectSpec],[OutputObjectSpecTag],WriteIOV)
theOutputConditionsAlg.Run1 = IOVBegin

svcMgr.IOVDbSvc.dbConnection  = OutputDB

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG 
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder=True
svcMgr.IOVRegistrationSvc.OverrideNames += ["HVScaleCorr",]
svcMgr.IOVRegistrationSvc.OverrideTypes += ["Blob16M",]

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = LumiBlock
svcMgr.EventSelector.InitialTimeStamp  = int(TimeStamp/1e9)
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO

theLArHVCorrMaker.OutputLevel = INFO
