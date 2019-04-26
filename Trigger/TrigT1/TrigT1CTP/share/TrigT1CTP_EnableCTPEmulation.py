
def enableCTPEmulation():

    from RecExConfig.RecFlags import rec
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags

    from AthenaCommon.GlobalFlags import globalflags 
    isData = (globalflags.DataSource == 'data')

    algseq = CfgMgr.AthSequencer("AthAlgSeq")

    # if running on bytestream data -- will not use for the time being
    #algseq += CfgMgr.TrigT1CTMonitoring__DeriveSimulationInputs(
    #    )

    algseq += CfgMgr.LVL1CTP__CTPEmulation (
        RDOOutputLocation = "CTP_RDO_L1Run3",
        IsData = isData
        )

    if not hasattr(svcMgr,"THistSvc"):
        svcMgr += CfgMgr.THistSvc()
    svcMgr.THistSvc.Output += ["L1Sim DATAFILE='l1simulation.root' OPT='RECREATE'"];
    print svcMgr.THistSvc

    if rec.doWriteAOD():
        StreamAOD.ItemList += [ "CTP_RDO#CTP_RDO",
                                "CTP_RDO#CTP_RDO_L1Run3"
                                ]
    if rec.doWriteESD():
        StreamESD.ItemList += [ "CTP_RDO#CTP_RDO",
                                "CTP_RDO#CTP_RDO_L1Run3"
                                ]
