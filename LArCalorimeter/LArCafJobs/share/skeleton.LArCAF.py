
#  'conditionsTag', 'geometryVersion',
#  'maxEvents', 'skipEvents', 'RunNumber', 

#if not 'CafJobInputs' in dir():
#    CafJobInputs=[["data11_7TeV.00189288.calibration_LArCells.daq.RAW._lb0000._SFO-10._0001.data"]]
#    print "No input file given, use ",CafJobInputs[0]


#if not 'CafJobOutputs' in dir():
#    CafJobOutputs=["LArTiming.root"]

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#############################################################33
#include("LArConditionsCommon/LArMinimalSetup.py")



from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
#globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(runArgs.inputBSFile)

from RecExConfig.RecFlags import rec
from RecExConfig.AutoConfiguration import GetProjectName,ConfigureTriggerStream
rec.projectName=GetProjectName()
rec.doWriteESD=False
rec.doWriteAOD=False

ConfigureTriggerStream()
#print rec.triggerStream()

from AthenaCommon.JobProperties import jobproperties
if hasattr(runArgs,'geometryVersion'):
    jobproperties.Global.DetDescrVersion = runArgs.geometryVersion
else:
    defaultGeoVersion="ATLAS-R2-2015-02-00-00"
    print "No geometryVersion given, use default value of",defaultGeoVersion
    jobproperties.Global.DetDescrVersion = defaultGeoVersion
    

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()

if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)
 
 ## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)


#from RecExConfig.RecFlags import rec

#if hasattr(runArgs,"InputBSFile"):
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(runArgs.inputBSFile)
#athenaCommonFlags.FilesInput.set_Value_and_Lock(runArgs.inputBSFile)
    #rec.readRDO.set_Value_and_Lock(true)

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

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

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

include("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
febSummaryMaker.CheckAllFEB=False

from LArROD.LArRODConf import LArRawChannelBuilderDriver
theLArRawChannelBuilder=LArRawChannelBuilderDriver("LArRawChannelBuilder")
topSequence += theLArRawChannelBuilder
# The first tool filters out bad channels
from LArROD.LArRODConf import LArRawChannelBuilderToolBadChannelTool
theLArRawChannelBuilderToolBadChannel=LArRawChannelBuilderToolBadChannelTool()
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
    "deadReadout","deadPhys","almostDead","short",
    "lowNoiseHG","highNoiseHG","unstableNoiseHG",
    "lowNoiseMG","highNoiseMG","unstableNoiseMG",
    "lowNoiseLG","highNoiseLG","unstableNoiseLG"
    ]
ToolSvc+=theLArRCBMasker
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
ToolSvc+=LArBadChanTool()
theLArRawChannelBuilderToolBadChannel.BadChannelMask=theLArRCBMasker
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolBadChannel,]#[theLArRawChannelBuilderToolBadChannel.getFullName()]
ToolSvc+=theLArRawChannelBuilderToolBadChannel

from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter
theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()

## # look for max in this range
theLArRawChannelBuilderToolOFCIter.minSample = 2 
theLArRawChannelBuilderToolOFCIter.maxSample = 12
theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=3 # ADCmax at least 3 sigma above noise for iteration
theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
theLArRawChannelBuilderToolOFCIter.defaultPhase=12  # starting delay, also the fixed delay for ADC below min.
theLArRawChannelBuilderToolOFCIter.StoreTiming=True
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolOFCIter,]#.getFullName()]
ToolSvc += theLArRawChannelBuilderToolOFCIter


from LArROD.LArRODConf import LArRawChannelBuilderPedestalDataBase
theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"
theLArRawChannelBuilder.PedestalTools  = [theLArRawChannelBuilderPedestalDataBase,]#.getFullName()]
ToolSvc += theLArRawChannelBuilderPedestalDataBase

from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theLArADC2MeVTool=LArADC2MeVToolDefault("LArADC2MeVTool")
ToolSvc+=theLArADC2MeVTool

from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
theLArRawChannelBuilder.ADCtoEnergyTools  = [theLArRawChannelBuilderADC2EDataBase,]#.getFullName()]
theLArRawChannelBuilderADC2EDataBase.ADC2MeVTool = theLArADC2MeVTool
ToolSvc += theLArRawChannelBuilderADC2EDataBase

from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
theLArOFPeakRecoTool=LArOFPeakRecoTool()
theLArOFPeakRecoTool.UseShape=True 
ToolSvc += theLArOFPeakRecoTool


include("LArCellRec/LArTimeVetoAlg_jobOptions.py")


####################
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

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()


#This tool is apparently instanciated by TrkDetElementSurface/ DetElementSurfaceCnv_p1.h
from TrkEventCnvTools import TrkEventCnvToolsConf
EventCnvSuperTool = TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
EventCnvSuperTool.DoMuons = False
EventCnvSuperTool.DoID = False
ToolSvc += EventCnvSuperTool

###################



from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult",
    "MuCTPI_RDO/MUCTPI_RDO",
    "CTP_RDO/CTP_RDO",
    'HLT::HLTResult/HLTResult_L2',
    'HLT::HLTResult/HLTResult_EF',
    ]

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

if hasattr(runArgs,"outputNTUP_SAMPLESMONFile"):
    from LArCafJobs.LArCafJobsConfig import DefaultShapeDumper
    DefaultShapeDumper('LArShapeDumper', 'FREE', noiseSignifCut = 5, doShape = True, doTrigger = True, caloType = 'EMHECFCAL')
    topSequence.LArShapeDumper.CaloNoiseTool=theCaloNoiseTool
    topSequence.LArShapeDumper.TrigDecisionTool=tdt
    topSequence.LArShapeDumper.FileName=runArgs.outputNTUP_SAMPLESMONFile
    topSequence.LArShapeDumper.OutputLevel=DEBUG
    topSequence.LArShapeDumper.BunchCrossingTool=BunchCrossingTool()

    if ("Empty" in rec.triggerStream()):
       print "LArCellsEmpty: Process only empty bunch crossings"
       topSequence.LArShapeDumper.onlyEmptyBC=True 

    svcMgr.THistSvc.Output += ["AANT DATAFILE='"+runArgs.outputNTUP_SAMPLESMONFile+"' OPT='RECREATE'"]

if hasattr(runArgs,"outputNTUP_HECNOISEFile"):
    from LArCafJobs.LArCafJobsConf import LArHECNoise
    topSequence += LArHECNoise('LArHECNoise')
    #topSequence.LArHECNoise.OutputLevel=DEBUG
    svcMgr.THistSvc.Output += ["HEC DATAFILE='"+runArgs.outputNTUP_HECNOISEFile+"' OPT='RECREATE'"]


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




svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
#svcMgr.MessageSvc.useColors = False
#svcMgr.MessageSvc.OutputLevel=DEBUG

#svcMgr.StoreGateSvc.Dump=True
#svcMgr.ToolSvc.OutputLevel=DEBUG

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)
 
## Post-exec
if hasattr(runArgs,"postExec"):
    print "transform post-exec"
    for cmd in runArgs.postExec:
        print cmd
        exec(cmd)
    

