if 'inputfolder' not in dir():
   inputfolder="/LAR/Configuration/DSPThreshold/Thresholds"

if 'inputtag' not in dir():
   inputtag=""

if 'inputdb' not in dir():
   inputdb="COOLONL_LAR/COMP200"
   
   
##include ("LArConditionsCommon/LArMinimalSetup.py")
##svcMgr.IOVDbSvc.GlobalTag=LArCalib_Flags.globalFlagDB
##svcMgr.IOVDbSvc.DBInstance=""   
   
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
#import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
from time import time
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(["root://eosatlas//eos/atlas/atlascerngroupdisk/det-larg/ShapeTest/00297041/data16_13TeV.00297041.physics_CosmicCalo.merge.RAW._lb0100._SFO-2._0001.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-larg/ShapeTest/00297041/data16_13TeV.00297041.physics_CosmicCalo.merge.RAW._lb0410._SFO-1._0001.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-larg/ShapeTest/00297041/data16_13TeV.00297041.physics_CosmicCalo.merge.RAW._lb0577._SFO-1._0001.1","root://eosatlas//eos/atlas/atlascerngroupdisk/det-larg/ShapeTest/00297041/data16_13TeV.00297041.physics_CosmicCalo.merge.RAW._lb0582._SFO-1._0001.1"])

from ByteStreamCnvSvc import ReadByteStream
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()
svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.BSRDOInput()

#Get identifier mapping (needed by LArConditionsContainer)
                           
svcMgr.IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2016-12"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

theApp.EvtMax = -1

from RecExConfig.AutoConfiguration import GetProjectName,ConfigureTriggerStream
ConfigureTriggerStream()
print rec.triggerStream()


## TDT
from AthenaCommon.AppMgr import ToolSvc
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/FREE"   ]

from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.readBS=True
tf.doID=False

# trigger configuration
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
lvl1 = Lvl1ResultBuilderGetter()

from TriggerJobOpts.HLTTriggerResultGetter import ByteStreamUnpackGetter
bsu=ByteStreamUnpackGetter()

# TDT
from AthenaCommon.AppMgr import ToolSvc
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt


from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
theLumiTool = LuminosityToolDefault()
ToolSvc += theLumiTool

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()


#This tool is apparently instanciated by TrkDetElementSurface/ DetElementSurfaceCnv_p1.h
from TrkEventCnvTools import TrkEventCnvToolsConf
EventCnvSuperTool = TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
EventCnvSuperTool.DoMuons = False
EventCnvSuperTool.DoID = False
ToolSvc += EventCnvSuperTool



from LArCalibTools.LArCalibToolsConf import LArPulseShape
theLArPulseShape=LArPulseShape()
topSequence+=theLArPulseShape

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='pulse_shape.root' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
svcMgr.MessageSvc.OutputLevel = INFO

svcMgr.IOVDbSvc.DBInstance=""

svcMgr.MessageSvc.debugLimit = 5000000

svcMgr.IOVDbSvc.OutputLevel=DEBUG

print theApp

