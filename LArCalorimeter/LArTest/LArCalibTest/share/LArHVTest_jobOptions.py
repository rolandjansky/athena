#No input file -> use MC event selector
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-03-01-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.detdescr.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArConditionsCommon import LArHVDB
#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

theApp.EvtMax = 2

svcMgr.EventSelector.RunNumber         = 359191
#svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.InitialTimeStamp  = 1535324994
#svcMgr.EventSelector.TimeStampInterval = 5

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

## theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
## theByteStreamInputSvc.FullFileName=["/home/wlampl/LArOFIter/ramp/inputs/daq.Ramp.0029146.No.Streaming.LB0000.EB-EMBA._0001.data"]
   
## theByteStreamInputSvc.MaxBadEvents=0

svcMgr.IOVDbSvc.GlobalTag='CONDBR2-BLKPA-2018-05'

from LArCalibTest.LArCalibTestConf import LArHVTest
theTest=LArHVTest()
theTest.OutputLevel=DEBUG
   
topSequence += theTest


svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()


###########################################################################


svcMgr.MessageSvc.debugLimit=99999999
