theApp.Dlls += [ "TrigDecisionMaker" ]
if CBNTAthenaAware:
    CBNT_AthenaAware = Algorithm( "CBNT_AthenaAware" )
    CBNT_AthenaAware.Members+=[ "CBNTAA_TriggerDecision/CBNT_TriggerDecision" ]
    CBNT_TriggerDecision = Algorithm( "CBNT_TriggerDecision" )

    # flags to include for L1/L2/EF signatures in TriggerDecision or not
    CBNT_TriggerDecision.doL1 = True
    CBNT_TriggerDecision.doL2 = True
    CBNT_TriggerDecision.doEF = True
    
    # SG keys for config data
    CBNT_TriggerDecision.TrigConfigL2Key = "storeL2Location"
    CBNT_TriggerDecision.TrigConfigEFKey = "storeEFLocation"
    
    # base SG key for trigger decision object
    CBNT_TriggerDecision.TrigDecisionKey = "MyTriggerDecision"
    
#else:
#    CBNT_Athena = Algorithm( "CBNT_Athena" )
#    CBNT_Athena.Members += [ "CBNT_TrigT2Tau" ]


