from AthenaCommon.Logging import logging
log = logging.getLogger( 'PureSteeringJob' )

svcMgr.MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.defaultLimit = 0
svcMgr.StoreGateSvc.Dump = True  #true will dump data store contents


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
if "repeat" not in dir(): 
    repeat = 1
theApp.EvtMax = 7*repeat

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Add xAOD::EventInfo
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
job += xAODMaker__EventInfoCnvAlg()


### Create the Menu ###
#######################

if not "useRerunMenu" in dir(): useRerunMenu = False
if not "useErrorHandlingMenu" in dir(): useErrorHandlingMenu = False
if not "useTopoMenu" in dir(): useTopoMenu = False
if not "usePrescaleMenu" in dir(): usePrescaleMenu = False
if not "useMultiSeedingMenu" in dir(): useMultiSeedingMenu = False
if not "useMenuWithAcceptInput" in dir(): useMenuWithAcceptInput = False
if not "useBusyEventSetup" in dir(): useBusyEventSetup = False
if not "l1SeedingTest" in dir(): l1SeedingTest = False

# Default L1 RoIs if not set otherwise below

RoIs = """EM15i,EM25i;  EM15i,EM25i;  MU6,MU20; MU6; MU6; J50
EM15i,EM25i;  EM15i,EM25i;  MU6,MU20; MU6,MU20; J50,J65,J90,J200
EM15i,EM25i;  EM15i,EM25i;  EM15i,EM25i;  EM15i,EM25i;
MU06; MU06;  MU06; MU06; TM10; EM15i, EM25i; EM25i; EM15i; J50
TAU25; xE30, xE60; J50,J65,J90,J200
J50,J65,J90; J50,J65,J90,J200; J50,J65,J90
MU6, MU6
J50,J65,J90; J50,J65,J90,J200; J50,J65,J90
MU6
"""
    
if useTopoMenu:
    include("TrigSteering/pureTopoSteering_menu.py")

elif usePrescaleMenu:
    include("TrigSteering/pureSteering_menu_with_prescales.py")
    RoIs = """EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
EM15i,EM25i; EM15i
"""

elif useErrorHandlingMenu:
    include("TrigSteering/pureSteering_menu_with_errors.py")
    RoIs = """EM25i,EM25i;
EM15i,EM15i;
EM15i,EM25i;
EM15i,EM25i;
EM25i,EM25i;
EM15i,EM25i;
EM12i,EM3;
"""

elif useRerunMenu:
    include("TrigSteering/pureSteering_menu_with_rerun.py")
    RoIs ="""EM15i,EM25i;*L1_EM15i;L1_EM25i;L1_EM3
EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_EM3
EM15i,EM25i;*L1_EM15i;L1_EM3
EM15i,EM25i;*L1_EM15i!;L1_EM3
EM15i,EM25i;*L1_EM15i;L1_EM25i!;L1_EM3
EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_EM3
EM15i,EM25i;*L1_EM25i!;L1_EM3
"""

elif useMultiSeedingMenu:
    include("TrigSteering/pureSteering_menu_with_multi_seeding.py")
    RoIs ="""EM15i,EM25i;*L1_EM15i;L1_EM25i;L1_MU6
EM15i,EM25i;*L1_EM15i!;L1_EM25i!;L1_MU6
EM15i,EM25i;*L1_EM15i!;L1_EM25i;L1_MU6
EM15i,EM25i;*L1_MU6
EM15i,EM25i;*L1_EM15i
EM15i,EM25i;*L1_EM25i!;L1_MU6!
EM15i,EM25i;*L1_MU6!
"""

elif useMenuWithAcceptInput:
    include("TrigSteering/pureSteering_menu_with_acceptInput.py")    
    RoIs = """EM15i,EM25i
EM15i,EM25i
EM15i,EM25i
EM15i,EM25i
EM15i,EM25i
EM15i,EM25i
EM15i,EM25i
"""

elif l1SeedingTest:
    include("TrigSteering/pureSteering_l1Seeding_menu.py")
    RoIs = "MU6, MU20\n"*7
    # see for info about 
    # 106 is ctp ID of L1_MU06 120 is for L1_MU20 and 121 for L1_MU21
    #
    ctpbits = "106:1,0 120:1,1\n" 
    ctpbits += "106:1,1 120:0,0 121:1,1\n" # L1_MU21 active
    ctpbits += "106:1,0 120:1,1 121:0,0\n"  # L1_MU20 acts ( the other is inactive )
    ctpbits += "106:1,1 120:1,1 121:0,0\n"
    ctpbits += "106:1,1 120:1,1 121:0,0\n"
    ctpbits += "106:1,1 120:1,1 121:0,0\n"
    ctpbits += "106:1,1 120:1,0 121:0,0\n" # L1_MU20 activated but prescaled

    ctpfile=open("Lvl1CTPResults.txt", "w")
    ctpfile.write(ctpbits)
    ctpfile.close()

else:
    include("TrigSteering/pureSteering_menu.py")


# GEOMETRY AND MUON CABLING CONFIG
from AthenaCommon.DetFlags import DetFlags; 
DetFlags.detdescr.Muon_setOn();
from AtlasGeoModel import SetGeometryVersion;
from AtlasGeoModel import GeoModelInit
import MuonCnvExample.MuonCablingConfig

    
roifile=open("Lvl1Results.txt", "w")
for i in xrange(0,repeat):
    roifile.write(RoIs)
roifile.write("\n")
roifile.close()





###    Setup  TrigConfigSvc      ###
####################################
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
log.info("setting up TrigConfigSvc:")
svc = SetupTrigConfigSvc()
svc.hltXmlFile = 'pureSteering_menu.xml'
svc.l1XmlFile  = 'l1menu.xml'

try:
    svc.SetStates( 'xml' )
except:
    log.warning( 'failed to set state of TrigConfigSvc ...')
try:
    svc.InitialiseSvc()
except:
    log.warning( 'failed to activate TrigConfigSvc ...')

svcMgr.TrigConfigSvc.OutputLevel=DEBUG
svcMgr.HLTConfigSvc.OutputLevel=DEBUG
svcMgr.LVL1ConfigSvc.OutputLevel=DEBUG

runMergedSteering=True
if "runL2EFSteering" in dir():
    runMergedSteering=False

if runMergedSteering:
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer
    log.info("setting up HLT TestingTrigSteer configurable:")
    hltSteer = TestingTrigSteer('TrigSteer_HLT')
    hltSteer.calculatePrescaledChains     = useRerunMenu
    hltSteer.enableCoherentPrescaling     = True
    hltSteer.enableRobRequestPreparation  = True
    hltSteer.ExecutionOrderStrategy.order =['th:J50', 'th:J90', 'th:J200', 'name:.*mu.*', 'name:RobRequest.*']
    hltSteer.doL1TopoSimulation=False



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

    if l1SeedingTest:
        from TrigSteering.TestingTrigSteeringConfig import TestingLvl1Converter
        lvl1Converter = TestingLvl1Converter()
        hltSteer += lvl1Converter        
        hltSteer.LvlConverterTool = lvl1Converter
        hltSteer.LvlConverterTool.useL1Calo = False
        hltSteer.LvlConverterTool.useL1Muon = False
        hltSteer.LvlConverterTool.useL1JetEnergy = False
        hltSteer.LvlConverterTool.OutputLevel = DEBUG
        from TrigFake.TrigFakeConf import FakeRoIB
        fakeRoIB = FakeRoIB()
        fakeRoIB.OutputLevel = DEBUG
        fakeRoIB.InputFile="Lvl1CTPResults.txt"
        job += fakeRoIB

    job += hltSteer


else:
    ### L2 TopAlgorithm from configurable ###
    #########################################
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
    log.info("setting up TestingTrigSteer_L2 configurable:")
    hltSteer_L2= TestingTrigSteer_L2('TrigSteer_L2')
    hltSteer_L2.calculatePrescaledChains=False
    hltSteer_L2.doOperationalInfo=4
    hltSteer_L2.enableCoherentPrescaling=True
    hltSteer_L2.enableRobRequestPreparation=True
    hltSteer_L2.ExecutionOrderStrategy.OutputLevel=DEBUG
    hltSteer_L2.OutputLevel=DEBUG
    hltSteer_L2.doL1TopoSimulation=False
    hltSteer_L2.ExecutionOrderStrategy.order=['th:J50', 'th:J90', 'th:J200', 'name:.*mu.*', 'name:RobRequest.*']


    if useRerunMenu:
        hltSteer_L2.calculatePrescaledChains=True

    if useBusyEventSetup:
        hltSteer_L2.LvlConverterTool.overallRoIsLimit=1
        hltSteer_L2.ResultBuilder.ErrorStreamTags = ["ABORT_EVENT BUSY UNKNOWN: hltbusy physics"]

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

    if (useErrorHandlingMenu):
        hltSteer_L2.ResultBuilder.ErrorStreamTags = ["ABORT_CHAIN ALGO_ERROR GAUDI_EXCEPTION: hltexceptions physics", "ABORT_EVENT ALGO_ERROR TIMEOUT: hlttimeout debug"]
        hltSteer_L2.softEventTimeout = 1 * Units.s

    job += hltSteer_L2


    ### EF TopAlgorithm from configurable ###
    #########################################
    from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
    log.info("setting up TestingTrigSteer_EF configurable:")
    hltSteer_EF = TestingTrigSteer_EF('TrigSteer_EF')
    hltSteer_EF.doL1TopoSimulation=False
    hltSteer_EF.Navigation.OutputLevel=WARNING
    if (useErrorHandlingMenu):
        hltSteer_EF.softEventTimeout = 400 * Units.ms

    #from TrigSteering.TrigSteeringConfig import TrigSteer_EF
    #log.info("setting up TestingTrigSteer_EF configurable:")
    #hltSteer_EF = TrigSteer_EF('hltSteer_EF')

    hltSteer_EF.pruneSteeringMonTools( ['Validation'] )
    #hltSteer_EF.pruneSteeringMonTools( [] )



    hltSteer_EF.Navigation.OutputLevel=WARNING
    hltSteer_EF.calculatePrescaledChains=False
    if useRerunMenu:
        pass
    hltSteer_EF.OutputLevel=INFO
    #hltSteer_EF.ResultBuilder.DefaultStreamTagForErrors = ""

    hltSteer_EF.doL1TopoSimulation=False

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



