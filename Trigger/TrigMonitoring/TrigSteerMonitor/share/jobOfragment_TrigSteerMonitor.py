

    from TrigSteerMonitor.TrigSteerMonitorConf import TrigSteerMoni
    steerMoni = TrigSteerMoni()
    steerMoni.OutputLevel=VERBOSE
    steerMoni.ManagerName = "TriggerMonManager"
    TrigSteer_L2.MonTools += [steerMoni]

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



