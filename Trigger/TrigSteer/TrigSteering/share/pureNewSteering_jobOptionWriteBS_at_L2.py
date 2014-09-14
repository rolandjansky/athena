

#theApp.Dlls += [ "GaudiAud" ]
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors += [ "ChronoAuditor" ]
#theAuditorSvc.Auditors += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel=DEBUG
MessageSvc.defaultLimit = 2000000
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

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
TrigSteer_L2 = TestingTrigSteer_L2('TrigSteer_L2')
TopAlg += TrigSteer_L2

from TrigSerializeResult.TrigSerializeResultConf import TrigTSerializer
TrigSteer_L2.Navigation.Serializer = TrigTSerializer("SerializerL2")
TrigSteer_L2.Navigation.ClassesToPayload = [ "TrigRoiDescriptor", "TrigOperationalInfo" ]


from TrigSteering.TestingTrigSteeringConfig import TestingLvl1FromFile
TrigSteer_L2.LvlConverterTool = TestingLvl1FromFile('Lvl1FromFile')


# customize L2 for test purposes
#bring Lvl1test file
import commands
commands.getoutput("get_joboptions Lvl1Results.txt")



##### EF unsomment below to have also EF processing

# from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
# TrigSteer_EF = TestingTrigSteer_EF('TrigSteer_EF', hltFile="pureNewSteering_menu.xml")
# TopAlg += TrigSteer_EF
# TrigSteer_EF.Navigation.ClassesToPayload = [ "TrigRoiDescriptor" ]




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

########################
# BS production
########################


# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc, ByteStreamCnvSvc, ByteStreamCnvSvc
svcMgr += ByteStreamEventStorageOutputSvc()
# Properties
svcMgr += ByteStreamCnvSvc("ByteStreamCnvSvc")
svcMgr.ByteStreamCnvSvc.ByteStreamOutputSvc = "ByteStreamEventStorageOutputSvc"


from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
        "StreamBS",
        EvtConversionSvc = "ByteStreamCnvSvc",
        OutputFile = "ByteStreamEventStorageOutputSvc"
        )
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"

svcMgr.ByteStreamEventStorageOutputSvc.OutputDirectory = "./" 
svcMgr.ByteStreamEventStorageOutputSvc.AppName = "Athena"
svcMgr.ByteStreamEventStorageOutputSvc.FileTag = "csc13"

StreamBS = AthenaOutputStream("StreamBS", EvtConversionSvc = "ByteStreamCnvSvc")
StreamBS.ForceRead=True

StreamBS.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
StreamBS.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]



########################
# THistSvc
########################

THistSvc = Service ("THistSvc")
exec 'THistSvc.Output += ["SHIFT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "shift-monitoring.root"
exec 'THistSvc.Output += ["EXPERT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %  "expert-monitoring.root"
exec 'THistSvc.Output += ["RUNSTAT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % "runstat-monitoring.root"


print TopAlg
