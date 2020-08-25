if not 'skipEvents' in dir():
   skipEvents=0

if not 'maxEvents' in dir():
   maxEvents=10

if not 'outputFile' in dir():
   outputFile="HIST.root"

if not 'isOnline' in dir():
    isOnline=False
    pass


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#############################################################33
#include("LArConditionsCommon/LArMinimalSetup.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DataSource.set_Value_and_Lock('data')
#globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
# Not sure that this is correct
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(["/eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input/q431/data16_13TeV.00297447.physics_Main.daq.RAW._lb0555._SFO-1._0001.data"])

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.AutoConfiguration import GetProjectName,ConfigureTriggerStream
from RecExConfig.AutoConfiguration import GetProjectName,ConfigureGeo
rec.projectName=GetProjectName()

ConfigureGeo()

rec.doExpressProcessing.set_Value_and_Lock(True)

rec.doWriteESD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(False)

rec.doPerfMon.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)

rec.doTile.set_Value_and_Lock(True)

rec.doTrigger.set_Value_and_Lock(True)

# switch off failint L1Topo stuff
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.writeL1TopoValData.set_Value_and_Lock(False)

ConfigureTriggerStream()


from AthenaCommon.JobProperties import jobproperties
defaultGeoVersion="ATLAS-R2-2015-02-00-00"
jobproperties.Global.DetDescrVersion = defaultGeoVersion
   
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOn()
DetFlags.digitize.all_setOff()
DetFlags.Print()


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

#svcMgr.GeoModelSvc.AtlasVersion=globalflags.DetDescrVersion()

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LoadElecCalib.set_Value_and_Lock(True)

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

# Event selector
#import AthenaPoolCnvSvc.ReadAthenaPool
from ByteStreamCnvSvc import ReadByteStream
###############################################################


from LArConditionsCommon.LArCondFlags import larCondFlags
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

print "Resolving 'CURRENT' express conditions tag ..."
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
resolver=resolveAlias()
currentGlobalES=resolver.getCurrentES().replace("*","ST")
print "Found ",currentGlobalES
svcMgr.IOVDbSvc.GlobalTag=currentGlobalES
#"COMCOND-BLKP-005-05"

#Specify the input file(s)
svcMgr.EventSelector.Input=athenaCommonFlags.BSRDOInput()

svcMgr.EventSelector.SkipEvents=skipEvents

theApp.EvtMax=maxEvents

# Specify the object you want to read from ByteStream
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels_fB"]
theByteStreamAddressProviderSvc.TypeNames += ["TileRawChannelContainer/TileRawChannelCnt"]
theByteStreamAddressProviderSvc.TypeNames += ["TileRawChannelContainer/MuRcvRawChCnt"]
theByteStreamAddressProviderSvc.TypeNames += ["TileDigitsContainer/MuRcvDigitsCnt"] 
theByteStreamAddressProviderSvc.TypeNames += ["ROIB::RoIBResult/RoIBResult"] 


from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

include("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
febSummaryMaker.CheckAllFEB=False

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=True
larRODFlags.keepDSPRaw = True
from CaloRec.CaloCellFlags import jobproperties;
jobproperties.CaloCellFlags.doDeadCellCorr.set_Value_and_Lock(True)
jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)
   
include("TileConditions/TileConditions_jobOptions.py")

from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
CaloClusterTopoGetter()
 
from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
CaloClusterSWCmbGetter()

from LArCellRec.LArNoisyROSummaryGetter import LArNoisyROSummaryGetter
LArNoisyROSummaryGetter()
#topSequence.LArNoisyROAlg.OutputLevel=DEBUG
#topSequence.LArNoisyROAlg.Tool.OutputLevel=DEBUG
topSequence.LArNoisyROAlg.Tool.MNBLooseCut=5
topSequence.LArNoisyROAlg.Tool.MNBTightCut=17

include("LArCellRec/LArTimeVetoAlg_jobOptions.py")
include("LArCellRec/LArCollisionTime_jobOptions.py")

####################
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.readBS.set_Value_and_Lock(True)
tf.doLVL1.set_Value_and_Lock(False)
tf.doLVL2.set_Value_and_Lock(False)
tf.doEF.set_Value_and_Lock(False)
tf.doL1Topo.set_Value_and_Lock(False)
tf.readLVL1Calo.set_Value_and_Lock(True)
tf.enableMonitoring.set_Value_and_Lock(False)
tf.doID.set_Value_and_Lock(False)
tf.doMuon.set_Value_and_Lock(False)
tf.doBcm.set_Value_and_Lock(False)
tf.doTrt.set_Value_and_Lock(False)
tf.configForStartup="HLTonline"
tf.configForStartup.lock()

include("TriggerJobOpts/BStoESD_Tier0_HLTConfig_jobOptions.py")

#This tool is apparently instanciated by TrkDetElementSurface/ DetElementSurfaceCnv_p1.h
from TrkEventCnvTools import TrkEventCnvToolsConf
EventCnvSuperTool = TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
EventCnvSuperTool.DoMuons = False
EventCnvSuperTool.DoID = False
ToolSvc += EventCnvSuperTool

# --- BunchCrossing Tool configuration ---
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theBCTool = BunchCrossingTool()
ToolSvc += theBCTool

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool


# variable for testing on ESD
try:
    LArNoisyROMonForceTrigger
except NameError:
    LArNoisyROMonForceTrigger = False

from LArCellRec.LArNoisyROFlags import larNoisyROFlags

###############
# Monitoring should go here
from AthenaMonitoring.DQMonFlags import DQMonFlags



theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

# Not sure, if this is needed: (Margherita: it looks like it's needed, and the code does not work if this is placed below, moved here)
svcMgr.THistSvc.Output += ["CombinedMonitoring DATAFILE='"+outputFile+"' OPT='RECREATE'"]



from AthenaMonitoring import AthMonitorCfgHelperOld

include("LArMonitoring/LArAffectedRegionsAlg_jobOptions.py")
include("LArMonitoring/LArCollisionTimeMonAlg_jobOptions.py")

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100


theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
 
#svcMgr.ChronoStatSvc.ChronoDestinationCout = True

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True
#svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1

#svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

#svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

theApp.AuditAlgorithms = True

#Hack to load the right dict to write vector< vector < X > > to TTRee
#import AthenaPython.PyAthena as PyAthena
#PyAthena.load_library('AtlasSTLAddReflexDict')


#svcMgr.ToolSvc.OutputLevel=DEBUG

svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
#svcMgr.MessageSvc.defaultLimit=1000000
#svcMgr.MessageSvc.OutputLevel=DEBUG

#svcMgr.StoreGateSvc.Dump=True

#svcMgr.ToolSvc.LArCellHVCorrDefault.OutputLevel=WARNING

