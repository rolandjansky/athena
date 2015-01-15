  
MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"
MessageSvc.OutputLevel = DEBUG
MessageSvc.defaultLimit=10000
### Some more debug from StoreGate ###
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents


### for (python) messaging ###
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( 'PureSteeringJob' )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
if "repeat" not in dir(): 
    repeat = 1
theApp.EvtMax = 7*repeat


# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Create the Menu ###
#######################

  
MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"
MessageSvc.OutputLevel = DEBUG
MessageSvc.defaultLimit=10000
### Some more debug from StoreGate ###
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents


### for (python) messaging ###
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( 'PureSteeringJob' )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
if "repeat" not in dir(): 
    repeat = 1
theApp.EvtMax = 7*repeat


# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Create the Menu ###
#######################

#INGRID WAS HERE.... Testing L1Topo

#include("/afs/cern.ch/work/i/ideigaar/TriggerHLTSim/Trigger/TrigSteer/TrigSteering/share/pureTopoSteering_menu.py")

#include("TrigSteering/pureSteering_menu.py")
#include("./pureSteering_menu.py")
# generate default L1 file

#RoIs = """EM15i,EM25i;  EM15i,EM25i;  MU6,MU20; MU6; MU6; J50
#EM15i,EM25i;  EM15i,EM25i;  MU6,MU20; MU6,MU20; J50,J65,J90,J200
#EM15i,EM25i;  EM15i,EM25i;  EM15i,EM25i;  EM15i,EM25i;
#MU06; MU06;  MU06; MU06; TM10; EM15i, EM25i; EM25i; EM15i; J50
#TAU25; xE30, xE60; J50,J65,J90,J200
#J50,J65,J90; J50,J65,J90,J200; J50,J65,J90
#MU6, MU6
#J50,J65,J90; J50,J65,J90,J200; J50,J65,J90
#MU6
#"""
#if "usePrescaleMenu" not in dir():
#    usePrescaleMenu=False
#if (usePrescaleMenu):
#    include("TrigSteering/pureSteering_menu_with_prescales.py")
    # generate input file
    
#    RoIs = """EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#EM15i,EM25i; EM15i
#"""


#if "useErrorHandlingMenu" not in dir():
#    useErrorHandlingMenu=False
#if (useErrorHandlingMenu):
#    include("TrigSteering/pureSteering_menu_with_errors.py")
#    # generate input file
    
#    RoIs = """EM25i,EM25i;
#EM15i,EM15i;
#EM15i,EM25i;
#EM15i,EM25i;
#EM25i,EM25i;
#EM15i,EM25i;
#EM12i,EM3;
#"""


#if "useRerunMenu" not in dir():
#    useRerunMenu=False
#if useRerunMenu:
#    include("TrigSteering/pureSteering_menu_with_rerun.py")
    
#    RoIs ="""EM15i,EM25i;*L1_EM15i;L1_EM25i;L1_EM3
#EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_EM3
#EM15i,EM25i;*L1_EM15i;L1_EM3
#EM15i,EM25i;*L1_EM15i!;L1_EM3
#EM15i,EM25i;*L1_EM15i;L1_EM25i!;L1_EM3
#EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_EM3
#EM15i,EM25i;*L1_EM25i!;L1_EM3
#"""

#if "useMultiSeedingMenu" not in dir():
#    useMultiSeedingMenu=False
#if useMultiSeedingMenu:
#    include("TrigSteering/pureSteering_menu_with_multi_seeding.py")
    
#    RoIs ="""EM15i,EM25i;*L1_EM15i;L1_EM25i;L1_MU6
#EM15i,EM25i;*L1_EM15i!;L1_EM25i!;L1_MU6
#EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_MU6
#EM15i,EM25i;*L1_MU6
#EM15i,EM25i;*L1_EM15i
#EM15i,EM25i;*L1_EM25i!;L1_MU6!
#EM15i,EM25i;*L1_MU6!
#"""

#if "useMenuWithAcceptInput" not in dir():
#    useMenuWithAcceptInput=False
#if (useMenuWithAcceptInput):
#    include("TrigSteering/pureSteering_menu_with_acceptInput.py")
    # generate input file
    
#    RoIs = """EM15i,EM25i
#EM15i,EM25i
#EM15i,EM25i
#EM15i,EM25i
#EM15i,EM25i
#EM15i,EM25i
#EM15i,EM25i
#"""

    
#roifile=open("Lvl1Results.txt", "w")
#for i in xrange(0,repeat):
#    roifile.write(RoIs)
#roifile.write("\n")
#roifile.close()


#if "useBusyEventSetup" not in dir():
#    useBusyEventSetup=False


###    Setup  TrigConfigSvc      ###
####################################
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
log.info("setting up TrigConfigSvc:")
svc = SetupTrigConfigSvc()
svc.hltXmlFile = 'hlttest.xml'
svc.l1topoXmlFile = 'TopoTestMenu.xml'
svc.l1XmlFile  = 'l1test.xml'

try:
    svc.SetStates( 'xml' )
except:
    log.warning( 'failed to set state of TrigConfigSvc ...')
try:
    svc.InitialiseSvc()
except:
    log.warning( 'failed to activate TrigConfigSvc ...')

ServiceMgr.TrigConfigSvc.OutputLevel=VERBOSE
ServiceMgr.HLTConfigSvc.OutputLevel=VERBOSE
ServiceMgr.LVL1ConfigSvc.OutputLevel=VERBOSE

if "runMergedSteering" not in dir():
    runMergedSteering=False

if runMergedSteering:
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer
    log.info("setting up HLT TestingTrigSteer configurable:")
    hltSteer = TestingTrigSteer('TrigSteer_HLT')
    hltSteer.calculatePrescaledChains     = useRerunMenu
    hltSteer.enableCoherentPrescaling     = True
    hltSteer.enableRobRequestPreparation  = True
    hltSteer.ExecutionOrderStrategy.order =['th:J50', 'th:J90', 'th:J200', 'name:.*mu.*', 'name:RobRequest.*']

    if useBusyEventSetup:
        hltSteer.LvlConverterTool.overallRoIsLimit=1
        hltSteer.ResultBuilder.ErrorStreamTags = ["ABORT_EVENT BUSY UNKNOWN: hltbusy physics"]

    hltSteer.pruneSteeringMonTools( ['Validation'] )

    if "ord" not in dir():
        ord=0
    hltSteer.sortChains=ord

    if useErrorHandlingMenu:
        hltSteer.ResultBuilder.ErrorStreamTags = ["ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION: hltexceptions physics", "ABORT_EVENT ALGO_ERROR TIMEOUT: hlttimeout debug"]
        hltSteer.softEventTimeout = 1 * Units.s

    job += hltSteer

else:
    ### L2 TopAlgorithm from configurable ###
    #########################################
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
    log.info("setting up TestingTrigSteer_L2 configurable:")
    hltSteer_L2= TestingTrigSteer_L2('TrigSteer_L2')
    hltSteer_L2.calculatePrescaledChains=False
    hltSteer_L2.doOperationalInfo=DEBUG
    hltSteer_L2.enableCoherentPrescaling=True
    hltSteer_L2.enableRobRequestPreparation=True
    hltSteer_L2.ExecutionOrderStrategy.OutputLevel=VERBOSE
    hltSteer_L2.ExecutionOrderStrategy.order=['th:J50', 'th:J90', 'th:J200', 'name:.*mu.*', 'name:RobRequest.*']


#    if useRerunMenu:
#        hltSteer_L2.calculatePrescaledChains=True

#    if useBusyEventSetup:
#        hltSteer_L2.LvlConverterTool.overallRoIsLimit=1
#        hltSteer_L2.ResultBuilder.ErrorStreamTags = ["ABORT_EVENT BUSY UNKNOWN: hltbusy physics"]

    #from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEMoni
    #teMoni = TrigTEMoni()
    #teMoni.OutputLevel=VERBOSE
    #hltSteer_L2.MonTools += [teMoni]

    #from TrigSteering.TrigSteeringConfig import pruneSteeringMonTools
    hltSteer_L2.OutputLevel=DEBUG
    hltSteer_L2.pruneSteeringMonTools( ['Validation'] )
    #hltSteer_L2.pruneSteeringMonTools( ["Online", "Time"] )
    #hltSteer_L2.pruneSteeringMonTools( [] )
    #hltSteer_L2.ResultBuilder.DefaultStreamTagForErrors = ""

    hltSteer_L2.Navigation.OutputLevel=DEBUG
    if "ord" not in dir():
        ord=0
    hltSteer_L2.sortChains=ord

#    if (useErrorHandlingMenu):
#        hltSteer_L2.ResultBuilder.ErrorStreamTags = ["ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION: hltexceptions physics", "ABORT_EVENT ALGO_ERROR TIMEOUT: hlttimeout debug"]
#        hltSteer_L2.softEventTimeout = 1 * Units.s

    job += hltSteer_L2


    ### EF TopAlgorithm from configurable ###
    #########################################
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
    log.info("setting up TestingTrigSteer_EF configurable:")
    hltSteer_EF = TestingTrigSteer_EF('TrigSteer_EF')
    hltSteer_EF.Navigation.OutputLevel=WARNING
#    if (useErrorHandlingMenu):
#        hltSteer_EF.softEventTimeout = 400 * Units.ms

    #from TrigSteering.TrigSteeringConfig import TrigSteer_EF
    #log.info("setting up TestingTrigSteer_EF configurable:")
    #hltSteer_EF = TrigSteer_EF('hltSteer_EF')

    hltSteer_EF.pruneSteeringMonTools( ['Validation'] )
    #hltSteer_EF.pruneSteeringMonTools( [] )



    hltSteer_EF.Navigation.OutputLevel=WARNING
    hltSteer_EF.calculatePrescaledChains=False
#    if useRerunMenu:
#        pass
    hltSteer_EF.OutputLevel=INFO
    #hltSteer_EF.ResultBuilder.DefaultStreamTagForErrors = ""

    hltSteer_EF.doL1TopoSimulation=False

    #from TrigSteering.TrigSteeringConf import HLT__HeavyIonStreamingStrategy
    #hltSteer_EF.ResultBuilder.StreamingStrategy = HLT__HeavyIonStreamingStrategy( LowPriority='express', Override='exp70')

    job += hltSteer_EF




########################
# THistSvc
########################

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["SHIFT DATAFILE='shift-monioring.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["DEBUG DATAFILE='debug-monitoring.root' OPT='RECREATE'"]

# fix back the outut level


### print the job ###
#####################
log.info("print job:")
print job

### print services ###
######################
#log.info("print theApp.services():")
#print theApp.services()
#log.info("print ServicesMgr:")
#print ServiceMgr


