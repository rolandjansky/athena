# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def enableCTPEmulation(athSequence):

    from RecExConfig.RecFlags import rec
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    import AthenaCommon.CfgMgr as CfgMgr
    from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags

    from AthenaCommon.GlobalFlags import globalflags 
    isData = (globalflags.DataSource == 'data')

    if not hasattr(svcMgr,"THistSvc"):
        svcMgr += CfgMgr.THistSvc()
    (hStream , hFile) = simflags.OutputHistFile().split('#')  # e.g. EXPERT#l1Simulation.root
    if not hStream in [entry.split()[0] for entry in svcMgr.THistSvc.Output]:
        svcMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % tuple(simflags.OutputHistFile().split('#')) ]

    from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CTPEmulation
    ctpEmulation = LVL1CTP__CTPEmulation( name="CTPEmulation",
                                          HistogramStream=hStream,
                                          RDOOutputLocation = "CTP_RDO_L1Run3",
                                          IsData = isData )

    from AthenaCommon.Constants import DEBUG
    if simflags.EnableDebugOutput():
        ctpEmulation.OutputLevel = DEBUG
    athSequence += ctpEmulation

    # adding output
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    from AthenaCommon.AlgSequence import AlgSequence
    if rec.doRDOTrigger():
        MSMgr.GetStream( "StreamRDO" ).AddItem( getOutputData() )

    if rec.doWriteESD():
        MSMgr.GetStream( "StreamESD" ).AddItem( getOutputData() )

    if rec.doWriteAOD():
        MSMgr.GetStream( "StreamAOD" ).AddItem( getOutputData() )


def getOutputData():
    output = [ "CTP_RDO#CTP_RDO",
               "CTP_RDO#CTP_RDO_L1Run3" ]
    return output
