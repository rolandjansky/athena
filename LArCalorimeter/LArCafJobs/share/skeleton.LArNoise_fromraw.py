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
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(runArgs.inputBSFile)

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

rec.doTile.set_Value_and_Lock(False)

rec.doTrigger.set_Value_and_Lock(True)

# switch off failint L1Topo stuff
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.writeL1TopoValData.set_Value_and_Lock(False)

ConfigureTriggerStream()


from AthenaCommon.JobProperties import jobproperties
if hasattr(runArgs,'geometryVersion'):
    jobproperties.Global.DetDescrVersion = runArgs.geometryVersion
#    print "I found the geometryVersion of",defaultGeoVersion
else:
    defaultGeoVersion="ATLAS-R2-2015-02-00-00"
    print "No geometryVersion given, use default value of",defaultGeoVersion
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

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
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

#Specify the input file(s)
svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.BSRDOInput()

if hasattr(runArgs,"skipEvents"):
    svcMgr.EventSelector.SkipEvents=runArgs.skipEvents

if hasattr(runArgs,"maxEvents"):
    theApp.EvtMax=runArgs.maxEvents

# Specify the object you want to read from ByteStream
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels_fB"]

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

include("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
febSummaryMaker.CheckAllFEB=False

#include("CaloRec/CaloRec_jobOptions.py")
#from AthenaCommon.Include import excludeTracePattern
#excludeTracePattern.append("*/CaloClusterCorrection/common.py")
#include ("TileRec/TileDefaults_jobOptions.py")

#from LArROD.LArRODConf import LArRawChannelBuilderDriver
#theLArRawChannelBuilder=LArRawChannelBuilderDriver("LArRawChannelBuilder")
#topSequence += theLArRawChannelBuilder
## The first tool filters out bad channels
#from LArROD.LArRODConf import LArRawChannelBuilderToolBadChannelTool
#theLArRawChannelBuilderToolBadChannel=LArRawChannelBuilderToolBadChannelTool()
#from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
#theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
#theLArRCBMasker.DoMasking=True
#theLArRCBMasker.ProblemsToMask=[
#    "deadReadout","deadPhys","almostDead","short",
#    "lowNoiseHG","highNoiseHG","unstableNoiseHG",
#    "lowNoiseMG","highNoiseMG","unstableNoiseMG",
#    "lowNoiseLG","highNoiseLG","unstableNoiseLG"
#    ]
#ToolSvc+=theLArRCBMasker
#from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
#ToolSvc+=LArBadChanTool()
#theLArRawChannelBuilderToolBadChannel.BadChannelMask=theLArRCBMasker
#theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolBadChannel,]#[theLArRawChannelBuilderToolBadChannel.getFullName()]
#ToolSvc+=theLArRawChannelBuilderToolBadChannel
##
#from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter
#theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()
#
#### # look for max in this range
#theLArRawChannelBuilderToolOFCIter.minSample = 2 
#theLArRawChannelBuilderToolOFCIter.maxSample = 12
#theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=4 # ADCmax at least 3 sigma above noise for iteration
#theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
#theLArRawChannelBuilderToolOFCIter.defaultPhase=0  # starting delay, also the fixed delay for ADC below min.
#theLArRawChannelBuilderToolOFCIter.ECut=250. 
#theLArRawChannelBuilderToolOFCIter.StoreTiming=True
#theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolOFCIter,]#.getFullName()]
#ToolSvc += theLArRawChannelBuilderToolOFCIter
#
#
#from LArROD.LArRODConf import LArRawChannelBuilderPedestalDataBase
#theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
#theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"
#theLArRawChannelBuilder.PedestalTools  = [theLArRawChannelBuilderPedestalDataBase,]#.getFullName()]
#ToolSvc += theLArRawChannelBuilderPedestalDataBase
#
#from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
#theLArADC2MeVTool=LArADC2MeVToolDefault("LArADC2MeVTool")
#ToolSvc+=theLArADC2MeVTool
#
#from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
#theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
#theLArRawChannelBuilder.ADCtoEnergyTools  = [theLArRawChannelBuilderADC2EDataBase,]#.getFullName()]
#theLArRawChannelBuilderADC2EDataBase.ADC2MeVTool = theLArADC2MeVTool
#ToolSvc += theLArRawChannelBuilderADC2EDataBase
#
#from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
#theLArOFPeakRecoTool=LArOFPeakRecoTool()
#theLArOFPeakRecoTool.UseShape=True 
#ToolSvc += theLArOFPeakRecoTool

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=True
larRODFlags.keepDSPRaw = True
from CaloRec.CaloCellFlags import jobproperties;
jobproperties.CaloCellFlags.doDeadCellCorr.set_Value_and_Lock(True)

from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

from LArCellRec.LArNoisyROSummaryGetter import LArNoisyROSummaryGetter
LArNoisyROSummaryGetter()
topSequence.LArNoisyROAlg.OutputLevel=DEBUG
topSequence.LArNoisyROAlg.Tool.OutputLevel=DEBUG

if hasattr(runArgs,"outputNTUP_LARNOISEFile") or hasattr(runArgs,"outputNTUP_HECNOISEFile"):
   include("LArCellRec/LArTimeVetoAlg_jobOptions.py")

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

   include("TrigTier0/BStoESD_Tier0_HLTConfig_jobOptions.py")

   ## trigger configuration
   #from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
   #cfg = TriggerConfigGetter()

   #from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
   #lvl1 = Lvl1ResultBuilderGetter()

   #from TriggerJobOpts.HLTTriggerResultGetter import ByteStreamUnpackGetter
   #bsu=ByteStreamUnpackGetter()

   # TDT
   #from AthenaCommon.AppMgr import ToolSvc
   #from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
   #tdt = Trig__TrigDecisionTool("TrigDecisionTool")
   #ToolSvc += tdt

   #from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
   #trigDecWriter = WriteTrigDecision()


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


#
#####################

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
ToolSvc+=theLArBadChannelTool

#svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
   #include("LArCellRec//LArCollisionTime_jobOptions.py")
   from LArCafJobs.LArCafJobsConf import LArNoiseBursts
   topSequence += LArNoiseBursts( "LArNoiseBursts" )
   topSequence.LArNoiseBursts.ICaloNoiseTool = theCaloNoiseTool
   topSequence.LArNoiseBursts.BCTool = theBCTool
   topSequence.LArNoiseBursts.SigmaCut = 3.0
   topSequence.LArNoiseBursts.NumberOfBunchesInFront = 30
   topSequence.LArNoiseBursts.OutputLevel=DEBUG

   conddb.addFolder("TDAQ","/TDAQ/RunCtrl/DataTakingMode")
   #conddb.addFolder("TRIGGER","/TRIGGER/HLT/PrescaleKey")


if hasattr(runArgs,"outputHIST_LARNOISEFile"):
   # creation of noise tree as in standard monitoring
   #########
   from LArMonTools.LArMonToolsConf import LArNoisyROMon
   from LArMonTools.LArMonFlags import LArMonFlags
   
   # variable for testing on ESD
   try:
       LArNoisyROMonForceTrigger
   except NameError:
       LArNoisyROMonForceTrigger = False
   
   from LArCellRec.LArNoisyROFlags import larNoisyROFlags

   theLArNoisyROMon = LArNoisyROMon(name="LArNoisyROMon")
   theLArNoisyROMon.IsOnline = False
   theLArNoisyROMon.inputKey = "LArNoisyROSummary"
   theLArNoisyROMon.ProcessNEvents= LArMonFlags.EventBlockSize()
   theLArNoisyROMon.NoisyFEBDefStr =  '(>'+str(larNoisyROFlags.BadChanPerFEB())+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+')' #LArNoisyROCutHelper('BadChanPerFEB')
   theLArNoisyROMon.BadFEBCut = larNoisyROFlags.BadFEBCut() #LArNoisyROFEBCutHelper()
   theLArNoisyROMon.doTrigger = False
   theLArNoisyROMon.doHisto = False
   theLArNoisyROMon.OutputLevel = DEBUG
   
   ToolSvc += theLArNoisyROMon
   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   larRawMon=False
   larESDMon=True
   from AthenaMonitoring.DQMonFlags import DQMonFlags
   LArMonFlags.doLArDigitMon = False
   LArMonFlags.EventBlockSize = 0
   
   LArMon = AthenaMonManager(name="LArMonManager",
                             FileKey             = "LArMon",
                             Environment         = DQMonFlags.monManEnvironment(),
                             ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                             DataType            = DQMonFlags.monManDataType())
   topSequence += LArMon
   
   LArMon.AthenaMonTools += [theLArNoisyROMon]
   
   #########


if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
    from LArCafJobs.LArCafJobsConf import LArHECNoise
    topSequence += LArHECNoise('LArHECNoise')
    #little hack while autoConfiguration=everything is still not the default...
    if hasattr(runArgs,"inputESDFile") and not hasattr(runArgs,"inputFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

if hasattr(runArgs,"outputNTUP_LARNOISEFile"):
   svcMgr.THistSvc.Output += ["TTREE DATAFILE='"+runArgs.outputNTUP_LARNOISEFile+"' OPT='RECREATE'"]
if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
   svcMgr.THistSvc.Output += ["HEC DATAFILE='"+runArgs.outputNTUP_HECNOISEFile+"' OPT='RECREATE'"]
if hasattr(runArgs,"outputHIST_LARNOISEFile"):
   svcMgr.THistSvc.Output += ["LArMon DATAFILE='"+runArgs.outputHIST_LARNOISEFile+"' OPT='RECREATE'"]

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
