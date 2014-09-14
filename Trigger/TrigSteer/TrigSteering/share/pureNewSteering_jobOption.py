

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors += [ "ChronoAuditor" ]
theAuditorSvc.Auditors += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel=DEBUG
MessageSvc.defaultLimit = 2000000
theApp.EvtMax = 3
MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"


########################
# Trigger libraries
########################

theApp.Dlls += [ "TrigTimeAlgs" ]
theApp.Dlls += [ "TrigSteerMonitor" ]

########################
# Generate config XML files
########################

include("TrigSteering/pureNewSteering_menu.py")


from TrigSteering.TrigSteeringConfig import pruneSteeringMonTools
########################
# L2 Steering:
########################
from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
TrigSteer_L2 = TestingTrigSteer_L2('TrigSteer_L2', hltFile="pureNewSteering_menu.xml")
TopAlg += TrigSteer_L2
pruneSteeringMonTools(TrigSteer_L2, ["Validation", "Time"])


from TrigSteering.TestingTrigSteeringConfig import TestingLvl1FromFile
TrigSteer_L2.LvlConverterTool = TestingLvl1FromFile('Lvl1FromFile')


# customize L2 for test purposes
#bring Lvl1test file
import commands
commands.getoutput("get_joboptions Lvl1Results.txt")
TrigSteer_L2.Navigation.ClassesToPayload = [ "TrigRoiDescriptor" ]




########################
# EF Steering:
########################

# no customization
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_EF
TrigSteer_EF = TestingTrigSteer_EF('TrigSteer_EF', hltFile="pureNewSteering_menu.xml")
TopAlg += TrigSteer_EF
pruneSteeringMonTools(TrigSteer_L2, ["Validation", "Time"])
#TrigSteer_EF.ResultBuilder.ErrorStreamTags = ["OK:OK"] #stupid but shows how to set stream tags for errorus events

########################
# Serialization
########################
AthenaSealSvc = Service( "AthenaSealSvc" )
# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
include ("TrigSteeringEvent/TrigSteeringEventDict_joboptions.py")

theApp.CreateSvc += [ "AthenaSealSvc" ]
AthenaSealSvc.CheckDictAtInit = False
#AthenaSealSvc.initialize()
theApp.Dlls += [ "TrigSerializeResult" ]
AthenaSealSvc.OutputLevel = 7

#HistorySvc = Service( "HistorySvc" )
#HistorySvc.Dump     = TRUE
#HistorySvc.Activate = TRUE
#HistorySvc.OutputFile = "test.txt"
StatusCodeSvc = Service( "StatusCodeSvc" )
StatusCodeSvc.SuppressCheck=True


########################
# THistSvc
########################

THistSvc = Service ("THistSvc")
exec 'THistSvc.Output += ["SHIFT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "shift-monitoring.root"
exec 'THistSvc.Output += ["EXPERT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %  "expert-monitoring.root"
exec 'THistSvc.Output += ["RUNSTAT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % "runstat-monitoring.root"
exec 'THistSvc.Output += ["DEBUG DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' %   "debug-monitoring.root"


# -----
# Till:
# The following lines cause a crash in the end, when ToolSvc tries to finalise the histograms .... ?
# -----



# monitor timing
#Egamma_L2.AthenaMonTools += ""
# from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
# Egamma2.doTiming = True
# tool = TrigTimeHistToolConfig("Time")
# tool.OutputLevel = 1
# tool.THistSvc_OutStream="/EXPERT"
# tool.histoPathBase="/EXPERT"
# Egamma2.AthenaMonTools += [ tool ]

from TrigMonitorBase.TrigGenericMonitoringToolConfig import TrigGenericMonitoringToolConfig, defineHistogram

tool = TrigGenericMonitoringToolConfig("SomeTool")
tool.Histograms += [ defineHistogram("TotalTime", title="Total Timing of this algo", opt="kCanRebin") ]
Egamma2.doTiming = True
Egamma2.AthenaMonTools += [ tool ]



#TrigSteer_L2 += Algorithm( "Egamma_L2" )
#TrigSteer_L2.Egamma_L2.doTiming = True
#TrigSteer_L2.Egamma_L2.AthenaMonTools +=  [ "TrigTimeHistTool/TimeHist" ]

print TopAlg
