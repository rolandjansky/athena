#--------------------------------------------------------------
# TrigDecisionMaker Algorithm
#--------------------------------------------------------------

# the package's DLL 
theApp.Dlls += [ "TrigDecisionMaker" ]

# the Algorithm
theApp.TopAlg += ["TrigDec::TrigDecisionMaker"]
theApp.TopAlg += ["TrigDec::TrigAODConfigMaker"]

TrigDecisionMaker = Algorithm( "TrigDec::TrigDecisionMaker" )

# flags to include for L1/L2/EF signatures in TriggerDecision or not
TrigDecisionMaker.doL1 = True
TrigDecisionMaker.doL2 = True
TrigDecisionMaker.doEF = True
TrigDecisionMaker.doHLT = True

# SG keys for config data
TrigDecisionMaker.l2ResultKey = "HLTResult_L2"
TrigDecisionMaker.efResultKey = "HLTResult_EF"
TrigDecisionMaker.efResultKey = "HLTResult_HLT"

# SG keys for output trigger decision object and signatures to ignore
TrigDecisionMaker.TrigDecisionKey = "TrigDecision"

# debug level
TrigDecisionMaker.OutputLevel = VERBOSE

TrigAODConfigMaker = Algorithm( "TrigDec::TrigAODConfigMaker" )

# SG keys for config data
TrigAODConfigMaker.HLTConfigDataKey = "AODConfig"
TrigAODConfigMaker.Lvl1ConfigDataKey = "AODConfig"
TrigAODConfigMaker.Lvl1PrescaleConfigDataKey = "AODConfig"

# debug level
TrigAODConfigMaker.OutputLevel = VERBOSE
