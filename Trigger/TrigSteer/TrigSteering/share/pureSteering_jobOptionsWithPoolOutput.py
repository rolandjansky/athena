  
MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"
MessageSvc.OutputLevel = INFO
### Some more debug from StoreGate ###
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents


### for (python) messaging ###
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( 'PureSteeringJob' )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 7


# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Create the Menu ###
#######################
include("TrigSteering/pureSteering_menu.py")




###    Setup  TrigConfigSvc      ###
####################################
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
log.info("setting up TrigConfigSvc:")
svc = SetupTrigConfigSvc()
svc.hltXmlFile = 'pureSteering_menu.xml'
svc.l1XmlFile  = 'l1.xml'

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



### L2 TopAlgorithm from configurable ###
#########################################
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
log.info("setting up TestingTrigSteer_L2 configurable:")
hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2')

#from TrigSteering.TrigSteeringConfig import TrigSteer_L2
#log.info("setting up TestingTrigSteer_L2 configurable:")
#hltSteer_L2 = TrigSteer_L2('hltSteer_L2')

#from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEMoni
#teMoni = TrigTEMoni()
#teMoni.OutputLevel=VERBOSE
#hltSteer_L2.MonTools += [teMoni]

#from TrigSteering.TrigSteeringConfig import pruneSteeringMonTools
job += hltSteer_L2
hltSteer_L2.pruneSteeringMonTools( ["Validation", "Time"] )

### EF TopAlgorithm from configurable ###
#########################################
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
log.info("setting up TestingTrigSteer_EF configurable:")
hltSteer_EF = TestingTrigSteer_EF('hltSteer_EF')

#from TrigSteering.TrigSteeringConfig import TrigSteer_EF
#log.info("setting up TestingTrigSteer_EF configurable:")
#hltSteer_EF = TrigSteer_EF('hltSteer_EF')

job += hltSteer_EF
hltSteer_EF.pruneSteeringMonTools( ["Validation", "Time"])
#pruneSteeringMonTools(hltSteer_EF, ["Validation", "Time"])



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

# at the end of the list partial buildin info dumper
from TrigSteeringTest.TrigSteeringTestConf import PartialEBDumper
peb = PartialEBDumper()
peb.OutputLevel = DEBUG
job += peb


####### from here goes AthenPool
import AthenaPoolCnvSvc.WriteAthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalogue1.xml"

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()
#svcMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;

from OutputStreamAthenaPool.OutputStreamAthenaPool import createOutputStream
stream = createOutputStream( "Stream1" )

#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#stream = AthenaPoolOutputStream( "Stream1" )

stream.OutputFile = "Result.root"
stream.ItemList += [ "HLT::HLTResult#HLTResult_L2", "HLT::HLTResult#HLTResult_EF" ]
stream.ItemList += [ "TrigRoiDescriptorCollection#HLT_initialRoI" ]

from StoreGate.StoreGateConf import StoreGateSvc
stream.Store = StoreGateSvc( "StoreGateSvc" )

######### to here




### print the job ###
#####################
#log.info("print job:")
#print job

### print services ###
######################
#log.info("print theApp.services():")
#print theApp.services()
#log.info("print ServicesMgr:")
#print ServiceMgr


