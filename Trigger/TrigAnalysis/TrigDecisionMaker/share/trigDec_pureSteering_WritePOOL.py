MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"

########################
# Framework
########################

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10

########################
# Generate config XML files
########################

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



########################
# L2 Steering:
########################

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
log.info("setting up TestingTrigSteer_L2 configurable:")
hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2')


## Switch off all monitoring !!!
hltSteer_L2.pruneSteeringMonTools([])

topSequence += hltSteer_L2
hltSteer_L2.Navigation.ClassesToPayload += [ 'TrigRoiDescriptor' ]

########################
# EF Steering:
########################

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
log.info("setting up TestingTrigSteer_EF configurable:")
hltSteer_EF = TestingTrigSteer_EF('hltSteer_EF')

## Switch off all monitoring !!!
hltSteer_EF.pruneSteeringMonTools([])

topSequence += hltSteer_EF
hltSteer_EF.Navigation.ClassesToPayload += [ 'TrigRoiDescriptor' ]

########################
# POOL
########################

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecisionToFile
#WriteTrigDecisionToFile("TrigDec.root", "Conditions.root")
trigDecStream = WriteTrigDecisionToFile("TrigDec.root")
trigDecStream.TrigDecMaker.OutputLevel = VERBOSE



########################
# Serialization
########################
AthenaSealSvc = Service( "AthenaSealSvc" )
# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
include( "FourMom/FourMomDict_joboptions.py" )
include( "Navigation/NavigationDict_joboptions.py" )
include( "EventKernel/EventKernelDict_joboptions.py" )
include( "TrigCaloEvent/TrigCaloEventDict_joboptions.py" )
include( "TrigParticle/TrigParticleDict_joboptions.py" )
include ("TrigSteeringEvent/TrigSteeringEventDict_joboptions.py")

theApp.CreateSvc += [ "AthenaSealSvc" ]
AthenaSealSvc.CheckDictAtInit = False
#AthenaSealSvc.initialize()
theApp.Dlls += [ "TrigSerializeResult" ]
AthenaSealSvc.OutputLevel = VERBOSE





############################
# Set output level threshold
############################

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.PoolSvc.OutputLevel = DEBUG
svcMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG


### print the job ###
#####################
log.info("print job:")
print topSequence


topSequence.hltSteer_L2.OutputLevel=VERBOSE
