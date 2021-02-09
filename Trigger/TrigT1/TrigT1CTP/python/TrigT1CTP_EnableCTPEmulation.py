# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def enableCTPEmulation(athSequence, setupOutput=False):

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

    from TriggerMenu.l1menu.CaloDef import getRun3EFexParameterSelection
    ctpEmulation.eFEXREta  = getRun3EFexParameterSelection('reta')
    ctpEmulation.eFEXRHad  = getRun3EFexParameterSelection('rhad')
    ctpEmulation.eFEXWStot = getRun3EFexParameterSelection('wstot')
    ctpEmulation.eFEXTauIso = getRun3EFexParameterSelection('eTauIso')
    ctpEmulation.jFEXTauIso = getRun3EFexParameterSelection('jTauIso')

    from AthenaCommon.Constants import DEBUG
    if simflags.EnableDebugOutput():
        ctpEmulation.OutputLevel = DEBUG
    athSequence += ctpEmulation

    if setupOutput:
        _setupOutput()

def _setupOutput():
    from RecExConfig.RecFlags import rec
    output = [ "CTP_RDO#CTP_RDO",
               "CTP_RDO#CTP_RDO_L1Run3" ]
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    if rec.doRDOTrigger():
        MSMgr.GetStream( "StreamRDO" ).AddItem( output )
    
    if rec.doWriteESD():
        MSMgr.GetStream( "StreamESD" ).AddItem( output )
    
    if rec.doWriteAOD():
        MSMgr.GetStream( "StreamAOD" ).AddItem( output )
