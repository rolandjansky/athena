from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#############################################################33
#include("LArConditionsCommon/LArMinimalSetup.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.InputFormat.set_Value_and_Lock('pool')
globalflags.DataSource.set_Value_and_Lock('data')
#globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
# Not sure that this is correct
# In LArCAF, BSRDOInput was used but here; ESDInput does not work...
athenaCommonFlags.FilesInput.set_Value_and_Lock(runArgs.inputESDFile)

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.AutoConfiguration import GetProjectName,ConfigureGeo
rec.projectName=GetProjectName()

ConfigureGeo()

rec.readRDO.set_Value_and_Lock(False)
rec.readTAG.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.readAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(True)
recAlgs.doTrigger.set_Value_and_Lock(False)

rec.doPerfMon.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteAOD=False




from AthenaCommon.JobProperties import jobproperties
#if hasattr(runArgs,'geometryVersion'):
#    jobproperties.Global.DetDescrVersion = runArgs.geometryVersion
#    print "I found the geometryVersion of",defaultGeoVersion
#else:
#    defaultGeoVersion="ATLAS-GEO-18-00-01"
#    print "No geometryVersion given, use default value of",defaultGeoVersion
#    jobproperties.Global.DetDescrVersion = defaultGeoVersion
   
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()


#from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.GeoModelSvc.AtlasVersion=globalflags.DetDescrVersion()

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
   from LArConditionsCommon.LArCondFlags import larCondFlags
   larCondFlags.LoadElecCalib.set_Value_and_Lock(True)

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

###############################################################


from LArConditionsCommon.LArCondFlags import larCondFlags
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

if not hasattr(runArgs,"conditionsTag") or runArgs.conditionsTag=="CURRENT":
    print "Resolving 'CURRENT' express conditions tag ..."
    sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
    from AtlCoolBKLib import resolveAlias
    resolver=resolveAlias()
    currentGlobalES=resolver.getCurrentES().replace("*","ST")
    print "Found ",currentGlobalES
    svcMgr.IOVDbSvc.GlobalTag=currentGlobalES
else:
    svcMgr.IOVDbSvc.GlobalTag=runArgs.conditionsTag
#"COMCOND-BLKP-005-05"

if hasattr(runArgs,"skipEvents"):
    svcMgr.EventSelector.SkipEvents=runArgs.skipEvents

if hasattr(runArgs,"maxEvents"):
    theApp.EvtMax=runArgs.maxEvents


#
#####################
from RecExConfig.InputFilePeeker import inputFileSummary
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])

from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.readBS=False
tf.doID=False


from TriggerJobOpts.TriggerGetter import TriggerGetter
triggerGetter = TriggerGetter()

# trigger configuration
#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg =  TriggerConfigGetter("ReadPool")

#from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
#lvl1 = Lvl1ResultBuilderGetter()

# TDT
#from AthenaCommon.AppMgr import ToolSvc
#from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#tdt = Trig__TrigDecisionTool("TrigDecisionTool")
#ToolSvc += tdt

#from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
#trigDecWriter = WriteTrigDecision()

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
ToolSvc+=theLArBadChannelTool

# --- BunchCrossing Tool configuration ---
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theBCTool = BunchCrossingTool()
ToolSvc += theBCTool

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()


include("LArCellRec//LArCollisionTime_jobOptions.py")


from LArCafJobs.LArCafJobsConf import LArNoiseBursts
topSequence += LArNoiseBursts( "LArNoiseBursts" )
topSequence.LArNoiseBursts.ICaloNoiseTool = theCaloNoiseTool
topSequence.LArNoiseBursts.BCTool = theBCTool
topSequence.LArNoiseBursts.SigmaCut = 3.0
topSequence.LArNoiseBursts.NumberOfBunchesInFront = 30

conddb.addFolder("TDAQ","/TDAQ/RunCtrl/DataTakingMode")

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
    svcMgr.THistSvc.Output += ["TTREE DATAFILE='"+runArgs.outputNTUP_LARNOISEFile+"' OPT='RECREATE'"]

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
    from LArCafJobs.LArCafJobsConf import LArHECNoise
    topSequence += LArHECNoise('LArHECNoise')
    svcMgr.THistSvc.Output += ["HEC DATAFILE='"+runArgs.outputNTUP_HECNOISEFile+"' OPT='RECREATE'"]
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

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
svcMgr.MessageSvc.defaultLimit=1000000
#svcMgr.MessageSvc.OutputLevel=DEBUG

#svcMgr.StoreGateSvc.Dump=True
