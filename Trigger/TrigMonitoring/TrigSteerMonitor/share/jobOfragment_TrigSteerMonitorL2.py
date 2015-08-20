
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    trigMonMan = AthenaMonManager( "TriggerMonManager" )
    trigMonMan.FileKey = "EXPERT"
    trigMonMan.ManualDataTypeSetup = True
    trigMonMan.DataType            = "collisions"
    trigMonMan.Environment         = "online"


    job += trigMonMan


    from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
    chainMoni = TrigChainMoni()
    chainMoni.OutputLevel=VERBOSE
    chainMoni.ManagerName = "TriggerMonManager"
    TrigSteer_L2.MonTools += [chainMoni]

    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoni
    signMoni = TrigSignatureMoni()
    signMoni.OutputLevel=VERBOSE
    signMoni.ManagerName = "TriggerMonManager"
    TrigSteer_L2.MonTools += [signMoni]

    from TrigSteerMonitor.TrigSteerMonitorConf import TrigTEMoni
    teMoni = TrigTEMoni()
    teMoni.OutputLevel=VERBOSE
    teMoni.ManagerName = "TriggerMonManager"
    TrigSteer_L2.MonTools += [teMoni]

    pruneMonitoringTools(TrigSteer_L2)
    job += TrigSteer_L2


