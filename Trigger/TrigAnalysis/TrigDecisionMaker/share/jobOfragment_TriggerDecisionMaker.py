#--------------------------------------------------------------
# TrigDecisionMaker Algorithm
#--------------------------------------------------------------

# the package's DLL 
theApp.Dlls += [ "TrigDecisionMaker" ]

# the Algorithm
theApp.TopAlg += ["TriggerDecisionMaker"]
TriggerDecisionMaker = Algorithm( "TriggerDecisionMaker" )

# flags to include for L1/L2/EF signatures in TriggerDecision or not
TriggerDecisionMaker.doL1 = True
TriggerDecisionMaker.doL2 = True
TriggerDecisionMaker.doEF = True

# SG keys for config data
TriggerDecisionMaker.TrigConfigL2Key = "storeL2Location"
TriggerDecisionMaker.TrigConfigEFKey = "storeEFLocation"

# SG keys for output trigger decision object and signatures to ignore
TriggerDecisionMaker.TrigDecisionKey = "MyTriggerDecision"
TriggerDecisionMaker.IgnoreItemNames = [ "EM01",
                                         "L2_e10","EF_e10","L2_e10L2_e10",
                                         "L2_e10TRTxK","EF_e10TRTxK",
                                         "L2_g10","EF_g10",
                                         "L2_tauNoCut","EF_tauNoCut",
                                         "L2_jet20", "EF_jet20",
                                         "L2_jet20a", "L2_jet20b", "L2_jet20c",
                                         "L2_jet20d", "L2_jet20kt",
                                         "EF_jet20aEt", "EF_jet20bEt",
                                         "EF_jet20cEt", "EF_jet20dEt" ]


# debug level
TriggerDecisionMaker.OutputLevel = 3
