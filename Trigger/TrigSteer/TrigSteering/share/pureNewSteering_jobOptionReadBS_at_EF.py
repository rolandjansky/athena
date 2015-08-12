


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel=DEBUG
MessageSvc.defaultLimit = 2000000
theApp.EvtMax = 10



########################
# Trigger libraries
########################

#include( "TrigSteering/jobOfragment_TrigSteering_New.py" )


theApp.Dlls += [ "TrigSteeringTest" ]


from TriggerMenuPython.GenerateMenu import GenerateMenu

########################
# Generate config XML files
########################

include("TrigSteering/pureSteering_menu.py")

####################################
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




from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

########################
# EF Steering:
########################

# no customization
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
TrigSteer_EF = TestingTrigSteer_EF('TrigSteer_EF')
TopAlg += TrigSteer_EF




########################
# Serialization
########################

AthenaSealSvc = Service( "AthenaSealSvc" )
# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
# include( "FourMom/FourMomDict_joboptions.py" )
# include( "Navigation/NavigationDict_joboptions.py" )
# include( "EventKernel/EventKernelDict_joboptions.py" )
# include( "TrigCaloEvent/TrigCaloEventDict_joboptions.py" )
#include( "TrigParticle/TrigParticleDict_joboptions.py" )
include ("TrigSteeringEvent/TrigSteeringEventDict_joboptions.py")

theApp.CreateSvc += [ "AthenaSealSvc" ]
AthenaSealSvc.CheckDictAtInit = False
#AthenaSealSvc.initialize()
theApp.Dlls += [ "TrigSerializeResult" ]
AthenaSealSvc.OutputLevel = VERBOSE

########################
# BS reading copied from: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Event/ByteStreamCnvSvc/share/BSEventStorageEventSelector_jobOptions.py?view=markup&sortby=date
########################

# Services
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")
svcMgr.ByteStreamInputSvc.FullFileName = [ "daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data" ]

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
svcMgr += ROBDataProviderSvc()

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"; 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2" ]

# proxy provider
from SGComps.SGCompsConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
ProxyProviderSvc = svcMgr.ProxyProviderSvc
ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]


 

########################
# THistSvc
########################

THistSvc = Service ("THistSvc")
exec 'THistSvc.Output += ["SHIFT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "shift-monitoring.root"
exec 'THistSvc.Output += ["EXPERT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %  "expert-monitoring.root"
exec 'THistSvc.Output += ["RUNSTAT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % "runstat-monitoring.root"
exec 'THistSvc.Output += ["DEBUG DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "debug-monitoring.root"




