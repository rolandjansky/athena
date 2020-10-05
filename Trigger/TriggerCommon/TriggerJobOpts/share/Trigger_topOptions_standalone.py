# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'Trigger_topOptions_standalone.py' )

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


if globalflags.InputFormat() == 'bytestream':
    TriggerFlags.doLVL1=False

# Common alias for online EventLoopMgr used below
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if hasattr(svcMgr,"HltEventLoopMgr"):
    onlEventLoopMgr = svcMgr.HltEventLoopMgr
else:
    onlEventLoopMgr = None

#-------------------------------------------------------------
# Setting DetFlags
#-------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
if TriggerFlags.doLVL1():
    DetFlags.detdescr.all_setOn()
if TriggerFlags.doID():
    DetFlags.detdescr.ID_setOn()
    DetFlags.makeRIO.ID_setOn()
else:
    DetFlags.ID_setOff()
if TriggerFlags.doMuon():
    DetFlags.detdescr.Muon_setOn()
    DetFlags.makeRIO.all_setOn()
if TriggerFlags.doCalo():
    DetFlags.detdescr.Calo_setOn()
    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.LoadElecCalib.set_Value_and_Lock(False)
else:
    DetFlags.Calo_setOff()

# Always enable magnetic field
if hasattr(DetFlags,'BField_on'):
    DetFlags.BField_setOn()

# Setup IOVDbSvc
from IOVDbSvc.CondDB import conddb  # noqa: F401 configuration by import, old Run-2 job options
svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

# ----------------------------------------------------------------
# Setting detector geometry 
# ----------------------------------------------------------------
from AthenaCommon.Include import include
include ("RecExCond/AllDet_detDescr.py")

#we have to reset these flags since the muons do a sync of flags in above fragment
if TriggerFlags.MuonSlice.doEFRoIDrivenAccess():
    cscOn=DetFlags.makeRIO.CSC_on()
    DetFlags.makeRIO.Muon_setOff()
    if cscOn:  #doesn't support RoI driven access yet
        DetFlags.makeRIO.CSC_setOn()


# ----------------------------------------------------------------
# Setup magnetic field
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq.AtlasFieldMapCondAlg.LoadMapOnStart = True
condSeq.AtlasFieldMapCondAlg.UseMapsFromCOOL = False  # not possible with LoadMapOnStart (ATLASRECTS-5604)
condSeq.AtlasFieldMapCondAlg.SoleMinCurrent = 160     # Standby current is 150A
condSeq.AtlasFieldMapCondAlg.ToroMinCurrent = 210     # Standby current is 200A
condSeq.AtlasFieldCacheCondAlg.UseDCS = False         # no DCS
condSeq.AtlasFieldCacheCondAlg.LockMapCurrents = True # no field scaling

# Read currents from IS if available
if onlEventLoopMgr:
    onlEventLoopMgr.setMagFieldFromPtree = True
            
# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
if globalflags.InputFormat()=='pool':
    import AthenaPoolCnvSvc.ReadAthenaPool  # noqa: F401 configuration by import, old Run-2 job options
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
    svcMgr.PoolSvc.AttemptCatalogPatch=True 
     
# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if globalflags.InputFormat()=='bytestream':

    # This is only needed running athena (as opposed to athenaMT/PT)
    if not hasattr(svcMgr,"ByteStreamCnvSvc"):
        from ByteStreamCnvSvc import ReadByteStream  # noqa: F401 configuration by import, old Run-2 job options
        # Define the input
        svcMgr.EventSelector.Input = athenaCommonFlags.BSRDOInput()
        from AthenaCommon.AppMgr import theApp
        theApp.ExtSvc += [ "ByteStreamCnvSvc"]


# ----------------------------------------------------------------
# Detector services not guaranteed to be loaded elsewhere
# ----------------------------------------------------------------

if TriggerFlags.doID():

    from InDetTrigRecExample.InDetTrigConfigConditions import \
         PixelConditionsSetup, SCT_ConditionsSetup, TRT_ConditionsSetup

    PixelConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
    PixelConditionsSetup.lock()
    PixelConditionsSetup.createTool()
    SCT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='InDetTrig')
    SCT_ConditionsSetup.lock()
    SCT_ConditionsSetup.createTool()
    TRT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
    TRT_ConditionsSetup.lock()
    TRT_ConditionsSetup.createSvc()

    include( "InDetRecExample/InDetRecCabling.py" )


if TriggerFlags.doCalo():
    if globalflags.DataSource()=='data':
        include("TrigT2CaloCommon/CaloCellMasking.py")

if TriggerFlags.doMuon():
    # load services needed for converters
    import MuonCnvExample.MuonCablingConfig  # noqa: F401 configuration by import, old Run-2 job options
    import MuonRecExample.MuonReadCalib  # noqa: F401 configuration by import, old Run-2 job options
    if (TriggerFlags.doEF() or TriggerFlags.doHLT()) and 'forceMuonDataPrep' in dir():
        if (TriggerFlags.MuonSlice.doEFRoIDrivenAccess()):
            include("MuonRdoToPrepData/CscRdoToCscPrepData_jobOptions.py")
        else:
            include( "MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py" )
        if DetFlags.makeRIO.CSC_on():
            include( "CscClusterization/CscThresholdClusterizationOptions.py" )

    if globalflags.InputFormat()=='pool':
        include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

    include ("MuonRecExample/MuonRecLoadTools.py")

# this is to limit messags when running with -s
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/TriggerMenu/menu/TriggerPythonConfig.py")
excludeTracePattern.append("*/TriggerMenu/menu/HltConfig.py")


# trigger menu files generation
log.info("generating menu")
from TriggerMenu.menu.GenerateMenu import GenerateMenu
import time

g = GenerateMenu()
t0 = time.time()
g.generate()
log.info('Trigger menu generation took %0.fs', time.time()-t0)

log = logging.getLogger( 'Trigger_topOptions_standalone.py' )  # generate modifies the log


# setup Lvl1
# initialize LVL1ConfigSvc
log.info("configuring LVL1 (L1Topo and CTP)")
from TriggerJobOpts.Lvl1TriggerOnlineGetter import Lvl1SimulationGetter
lvl1 = Lvl1SimulationGetter()

if globalflags.InputFormat()=='pool':
    log.info("setting up transient BS")
    TriggerFlags.doTransientByteStream = True
    include( "TriggerJobOpts/jobOfragment_TransBS_standalone.py" )

# setup HLT
# initialize HLT config svc
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
log.info('Creating the trigger configuration service wrapper')
svc = SetupTrigConfigSvc()
if TriggerFlags.readL1TopoConfigFromXML():
    svc.l1topoXmlFile = TriggerFlags.inputL1TopoConfigFile()   # given XML
else:
    svc.l1topoXmlFile = TriggerFlags.outputL1TopoConfigFile()  # generated in python

if TriggerFlags.readLVL1configFromXML():
    svc.l1XmlFile  = TriggerFlags.inputLVL1configFile()   # given XML
else:
    svc.l1XmlFile  = TriggerFlags.outputLVL1configFile()  # generated in python

if TriggerFlags.readHLTconfigFromXML():
    svc.hltXmlFile  = TriggerFlags.inputHLTconfigFile()   # given XML
else:
    svc.hltXmlFile  = TriggerFlags.outputHLTconfigFile()  # generated in python

try:
    svc.SetStates( ["xml"] )
except Exception as ex:
    log.error('failed to set state of TrigConfigSvc, %s', str(ex))

try:
    svc.InitialiseSvc()
except Exception as ex:
    log.error('failed to activate TrigConfigSvc, %s', str(ex))

# Enable PSK/LB monitoring
if 'doMonitoring' in svcMgr.HLTConfigSvc.getProperties():
    svcMgr.HLTConfigSvc.doMonitoring = True

log.info("configuring hlt")
from TriggerJobOpts.HLTTriggerGetter import HLTSimulationGetter
hlt = HLTSimulationGetter(g)


if 'stopAfterMenuGeneration' in globals() and globals()['stopAfterMenuGeneration'] is True:
    theApp.exit()


# setup writing of temporary cool db for a possible reco-step afterwards
log.info("Will create temporary cool file, sources are '%s' and '%s'", svc.l1XmlFile, svc.hltXmlFile )
from TrigConfigSvc.TrigConf2COOL import theConfCOOLWriter
theConfCOOLWriter.lvl1menu = svc.l1XmlFile
theConfCOOLWriter.hltmenu  = svc.hltXmlFile
theConfCOOLWriter.isWritingNeeded = True
import re
TrigCoolDbConnection = re.match(".*;schema=(.*);dbname=.*",theConfCOOLWriter.dbConnection).group(1)
log.info("Start writing with TrigCoolDbConnection %s", TrigCoolDbConnection )
theConfCOOLWriter.writeConf2COOL()
f = open("MenuCoolDbLocation.txt","w")
f.write(TrigCoolDbConnection)
f.close()
del f

# -------------------------------------------------------------
# Message formatting and OutputLevel
# -------------------------------------------------------------
msgSvc = svcMgr.MessageSvc
msgSvc.Format = "% F%48W%S%7W%R%T %0W%M"

if TriggerFlags.Online.doValidation():
    TriggerFlags.enableMonitoring = TriggerFlags.enableMonitoring.get_Value()+['Log']
else:
    msgSvc.Format = "%t  " + msgSvc.Format   # add time stamp
    if hasattr(msgSvc,'useErsError'):   # ERS forwarding with TrigMessageSvc
        msgSvc.useErsError = ['*']

# ----------------------------------------------------------------
# Setting individual OutputLevel 
# ----------------------------------------------------------------
trigSteerConf = []
if TriggerFlags.doHLT():
    trigSteerConf += [topSequence.TrigSteer_HLT]

HLTOutputLevel = globals()['HLTOutputLevel'] if 'HLTOutputLevel' in globals() else msgSvc.OutputLevel
for s in trigSteerConf:
    s.OutputLevel = HLTOutputLevel
    for algo in s.getChildren():
        algo.OutputLevel = HLTOutputLevel

if onlEventLoopMgr:
    onlEventLoopMgr.OutputLevel = HLTOutputLevel

from AthenaCommon import Constants

if hasattr(topSequence,'CTPSimulation'):
    topSequence.CTPSimulation.OutputLevel = Constants.INFO

# Special settings of random number service for online
svcMgr.AtRanluxGenSvc.OutputLevel = Constants.WARNING
svcMgr.AtRanluxGenSvc.SaveToFile = False

# Re-seed the RNG on every event
if hasattr(svcMgr.ToolSvc,'HLT::RandomScaler'):
    rng = getattr(svcMgr.ToolSvc,'HLT::RandomScaler')
    rng.useEventSeed = True
    rng.config(seed=0, luxury=2)

# ----------------------------------------------------------------
# Setup timer histograms 
# ----------------------------------------------------------------
from TrigTimeAlgs.TrigTimeAlgsConf import TrigTimerSvc
svcMgr += TrigTimerSvc()
svcMgr.TrigTimerSvc.IncludeName=".*(TrigSteer_HLT:.+)|ALGO::OPITimer|.+:TotalTime"

if TriggerFlags.doHLT():

    # Enable timing except for hypos
    for alg in topSequence.TrigSteer_HLT.getChildren():
        if 'forceAccept' not in alg.properties():
            alg.doTiming = True

    steertime = topSequence.TrigSteer_HLT.MonTools['HLTSteeringTime']
    steertime.NumberOfHistBins = 200
    steertime.TimerHistLimits = [0,10000]
    steertime.Key = "TrigSteer_HLT:.+|.+:TotalTime"
    steertime.GroupedTimers = {  # captures in (...) will be removed from bin labels
        "TrigSteer_HLT_Chains": "(TrigSteer_HLT:Chain_).+",
        "TrigSteer_HLT_Sequences": "(TrigSteer_HLT:sequence_).+",
        "TrigSteer_HLT_Algorithms": "(?!TrigSteer_HLT:.+).+(:TotalTime)"  # all TotalTime timers except from steering
    }
    
    # In addition create per-LB TProfile histograms for all the grouped timers
    from TrigSteering.TrigSteeringConfig import SteeringTimingTool
    lbtime = SteeringTimingTool("HLTSteeringTimeLB")
    lbtime.GroupedTimers = steertime.GroupedTimers.copy()
    # only select per-LB timers to avoid duplicate histograms with the other tool
    lbtime.Key = "TrigSteer_HLT:Chain_.+|TrigSteer_HLT:sequence_.+|(?!TrigSteer_HLT:.+).+(:TotalTime)"
    lbtime.DoPerLBGroups = True
    lbtime.LBNHistoryGroup = 2
    lbtime.LBNHistoryDepth = lbtime.LBNHistoryGroup * 5
    topSequence.TrigSteer_HLT.MonTools += [lbtime]

# Must make sure that no OutStream's have been declared
theApp.OutStream = []

# Enable Aditors (i.e. AlgContextAuditor to get algorithm stack in CoreDumpSvc)
theApp.AuditAlgorithms = True 

# Some cleanup
del onlEventLoopMgr


