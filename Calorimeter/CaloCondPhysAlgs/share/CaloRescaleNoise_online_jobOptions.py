###############################################################
#
# Job options file for CaloRescaleNoise
#
#==============================================================


# TimeStamp gives the time to use to access the new HV setting

from time import strptime,time
from calendar import timegm

if "date" not in dir():
    date="2010-07-29:13:00:00"

if "TimeStamp" not in dir():
   try:
      ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
      TimeStamp=int(timegm(ts))*1000000000L
   except ValueError:
      print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"

print " TimeStamp to use for HV reading ", TimeStamp

# put here the run number of the period for which the HV correction was the old correction, but after the UPD1 IoV of the mapping change (in case of HV mapping change)

if "RunNumberOld" in dir():
   LumiBlock=0

if "RunNumberOld" not in dir():
    from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi
    rlb=TimeStampToRunLumi(TimeStamp,dbInstance="CONDBR2")
    if rlb is None:
        print "WARNING: Failed to convert time",TimeStamp,"into a run/lumi number" 
        RunNumberOld = 999999
        LumiBlock    = 0
    else:
        RunNumberOld = rlb[0]
        LumiBlock = rlb[1]


if "GlobalTag" not in dir():
    GlobalTag =  'COMCOND-BLKPA-2014-02'

if "Geometry" not in dir():
    Geometry = 'ATLAS-R2-2015-02-00-00'

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(int(RunNumberOld))


from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
from AthenaCommon.Resilience import treatException,protectedInclude
protectedInclude( "PerfMonComps/PerfMonSvc_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

# this setting is just to get directly pileup noise as b and write back the same in the database...
from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.)


import AthenaCommon.AtlasUnixGeneratorJob

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
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

from IOVDbSvc.CondDB import conddb
conddb.addOverride("/LAR/NoiseOfl/CellNoise","LARNoiseOflCellNoise-RUN2-UPD1-00")

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
theCaloNoiseTool.RescaleForHV=False #Turn automatic rescaling off
ToolSvc += theCaloNoiseTool

from LArConditionsCommon import LArHVDB

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloRescaleNoise
theCaloRescaleNoise = CaloRescaleNoise("CaloRescaleNoise")
theCaloRescaleNoise.noiseTool = theCaloNoiseTool

topSequence += theCaloRescaleNoise

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumberOld
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = LumiBlock
svcMgr.EventSelector.InitialTimeStamp  = int(TimeStamp/1e9)
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1


#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise_data.root' OPT='RECREATE'"];


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
