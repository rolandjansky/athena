##################################################################
# 
# example jobOptions to read HV from Cool/DCS in athena and fill simple ntuple
#
##################################################################

import time

tuple=(2010, 04, 29, 16, 00, 0,-1,-1,-1)
itime = int(time.mktime(tuple))

TimeStamp=itime

# run number only relevant for HV mapping information..   Use latest mapping here
RunNumber = 999999

# global tag to read other conditions if needed
GlobalTag     = 'COMCOND-ES1P-002-00'


###################################################################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from RecExConfig.RecFlags import rec

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()

from AthenaCommon.GlobalFlags  import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
GlobalFlags.Luminosity.set_zero()

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
jobproperties.Global.DetDescrVersion='ATLAS-GEO-10-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

from IOVDbSvc.CondDB import conddb 
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")

# to read HV mapping from database file instead of Ascii file in LArTools
from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap");

from LArCondUtils.LArCondUtilsConf import LArHV2Ntuple
theLArHV2Ntuple = LArHV2Ntuple("LArHV2Ntuple")
topSequence += theLArHV2Ntuple

#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='dump_hv.root' OPT='RECREATE'"];



#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 100
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 100
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = TimeStamp
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
