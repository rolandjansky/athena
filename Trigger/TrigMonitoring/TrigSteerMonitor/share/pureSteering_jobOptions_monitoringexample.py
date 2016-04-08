
MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"

### Some more debug from StoreGate ###
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents


### for (python) messaging ###
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( 'PureSteeringJob' )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 50


# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Create the Menu ###
#######################
include("TrigSteering/pureSteering_menugordon.py")


### L2 TopAlgorithm from configurable ###
########################################
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
log.info("setting up TestingTrigSteer_L2 configurable:")
#hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2', hltFile='pureSteering_menu.xml', lvl1File='')
hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2', hltFile='pureSteering_menugordon.xml', lvl1File='')

#from TrigSteering.TrigSteeringConfig import TrigSteer_L2
#log.info("setting up TestingTrigSteer_L2 configurable:")
#hltSteer_L2 = TrigSteer_L2('hltSteer_L2', hltFile='pureSteering_menu.xml', lvl1File='')


#from TrigSteerMonitor.TrigSteerMonitorConf import TimeInfo
#timeMoni = TimeInfo()
#timeMoni.OutputLevel=VERBOSE
#hltSteer_L2.MonTools += [timeMoni]

# integration of monitoring classes
# starting with managedMonitorToolBase
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
trigMonMan = AthenaMonManager( "TriggerMonManager" )
trigMonMan.FileKey = "EXPERT"
trigMonMan.ManualDataTypeSetup = True
trigMonMan.DataType            = "collisions"
trigMonMan.Environment         = "online"

job += trigMonMan

# class SignatureMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoni
#signatureMoni = TrigSignatureMoni()
#signatureMoni.OutputLevel=VERBOSE
#signatureMoni.ManagerName = "TriggerMonManager"
#hltSteer_L2.MonTools += [signatureMoni]

# class RoIMoni
from TrigSteerMonitor.TrigSteerMonitorConf import TrigRoIMoni
roiMoni = TrigRoIMoni()
roiMoni.OutputLevel=VERBOSE
roiMoni.ManagerName = "TriggerMonManager"
hltSteer_L2.MonTools += [roiMoni]

# class ElementMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigElementMoni
#elementMoni = TrigElementMoni()
#elementMoni.OutputLevel=VERBOSE
#elementMoni.ManagerName = "TriggerMonManager"
#hltSteer_L2.MonTools += [elementMoni]

# class TEMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEMoni
#teMoni = TrigTEMoni()
#teMoni.OutputLevel=VERBOSE
#teMoni.ManagerName = "TriggerMonManager"
#hltSteer_L2.MonTools += [teMoni]

# class TEOfflineMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEOfflineMoni
#teOfflineMoni = TrigTEOfflineMoni()
#teOfflineMoni.OutputLevel=VERBOSE
#teOfflineMoni.ManagerName = "TriggerMonManager"
#hltSteer_L2.MonTools += [teOfflineMoni]

# class ChainMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
#chainMoni = TrigChainMoni()
#chainMoni.OutputLevel=VERBOSE
#chainMoni.ManagerName = "TriggerMonManager"
#hltSteer_L2.MonTools += [chainMoni]
job += hltSteer_L2



### EF TopAlgorithm from configurable ###
#########################################
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
log.info("setting up TestingTrigSteer_EF configurable:")
hltSteer_EF = TestingTrigSteer_EF('hltSteer_EF')

#from TrigSteering.TrigSteeringConfig import TrigSteer_EF
#log.info("setting up TestingTrigSteer_EF configurable:")
#hltSteer_EF = TrigSteer_EF('hltSteer_EF')


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
trigMonMan = AthenaMonManager( "TriggerMonManager" )
trigMonMan.FileKey = "EXPERT"
trigMonMan.ManualDataTypeSetup = True
trigMonMan.DataType            = "collisions"
trigMonMan.Environment         = "online"

job += trigMonMan

# class SignatureMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoni
#signatureMoni = TrigSignatureMoni()
#signatureMoni.OutputLevel=VERBOSE
#signatureMoni.ManagerName = "TriggerMonManager"
#hltSteer_EF.MonTools += [signatureMoni]


# class TEMoni
#from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEMoni
#teMoni = TrigTEMoni()
#teMoni.OutputLevel=VERBOSE
#teMoni.ManagerName = "TriggerMonManager"
#hltSteer_EF.MonTools += [teMoni]

job += hltSteer_EF

### print the job ###
#####################
log.info("print job:")
print job

### print services ###
######################
log.info("print theApp.services():")
print theApp.services()
log.info("print ServicesMgr:")
print ServiceMgr



########################
# THistSvc
########################

THistSvc = Service ("THistSvc")
exec 'THistSvc.Output += ["SHIFT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "shift-monitoring.root"
exec 'THistSvc.Output += ["EXPERT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %  "expert-monitoring.root"
exec 'THistSvc.Output += ["RUNSTAT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % "runstat-monitoring.root"
exec 'THistSvc.Output += ["DEBUG DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "debug-monitoring.root"
