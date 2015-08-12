

#theApp.Dlls += [ "GaudiAud" ]
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors += [ "ChronoAuditor" ]
#theAuditorSvc.Auditors += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel=DEBUG
MessageSvc.defaultLimit = 2000000
theApp.EvtMax = 3



########################
# Trigger libraries
########################


#theApp.Dlls += [ "TrigConfigSvc" ]
#theApp.Dlls += [ "TrigSteering" ]

#theApp.Dlls += [ "TrigSteeringTest" ]
#theApp.Dlls += [ "TrigTimeAlgs" ]
#theApp.Dlls += [ "TrigSteerMonitor" ]

########################
# Generate config XML files
########################

include("TrigSteering/pureNewSteering_menu.py")



########################
# L2 Steering:
########################
from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

from TrigSteering.TestingTrigSteeringConfig import TestingReruningTrigSteer_L2
TrigSteer_L2 = TestingReruningTrigSteer_L2('TrigSteer_L2', hltFile="pureNewSteering_menu.xml")
TopAlg += TrigSteer_L2

########################
# EF Steering:
########################
from TrigSteering.TestingTrigSteeringConfig import TestingReruningAfterL2TrigSteer_EF
TrigSteer_EF = TestingReruningAfterL2TrigSteer_EF('TrigSteer_EF', hltFile="pureNewSteering_menu.xml")
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
# BS reading
########################

include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2" ]
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_EF" ]

Service("ByteStreamInputSvc").InputFiles = ['RawEvent.re'] 

########################
# THistSvc
########################

THistSvc = Service ("THistSvc")
exec 'THistSvc.Output += ["SHIFT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "shift-monitoring.root"
exec 'THistSvc.Output += ["EXPERT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %  "expert-monitoring.root"
exec 'THistSvc.Output += ["RUNSTAT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % "runstat-monitoring.root"
exec 'THistSvc.Output += ["DEBUG DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "debug-monitoring.root"


print TopAlg
