# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PhotonD3PDMaker.PhotonD3PD import _args

def AddMuonsToD3PD(alg,
                   stacoMuonContainer="StacoMuonCollection",
                   muidMuonContainer="MuidMuonCollection",
                   caloMuonContainer="CaloMuonCollection",
                   **kw):
    #-------------------------------------------------------------------------------------
    # Add some additional stuff to the muons beyond what's defined by default at level 0
    import MuonD3PDMaker
    from MuonD3PDMaker.MuonD3PDObject                    import MuonD3PDObject
    from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
    from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject
    if not MuonD3PDObject.allBlocknames().has_key("MuonScatteringAngleSignificance"):
        from AthenaCommon.AppMgr import ToolSvc
        muonScatteringSigTool=None
        if hasattr(ToolSvc, "MuonScatteringSigTool"):
            muonScatteringSigTool=ToolSvc.MuonScatteringSigTool
        from JetTagD3PDMaker import MuonScatteringAngleSignificanceFillerTool
        MuonD3PDObject.defineBlock (100, "MuonScatteringAngleSignificance",
                                    MuonScatteringAngleSignificanceFillerTool,
                                    ScatteringSigTool=muonScatteringSigTool) 
    from RecExConfig.ObjKeyStore                  import cfgKeyStore
    if (stacoMuonContainer != 'None' and cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" )):
        alg += MuonD3PDObject      (**_args (10, 'StacoMuon', kw,
                                    sgkey=stacoMuonContainer, prefix='mu_staco_',
                                    include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                    allowMissing = True ))
    else:
        alg += MuonD3PDObject      (**_args (10, 'Muon'             , kw,
                                             sgkey="Muons", prefix='mu_muon_',
                                             include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                             exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                             allowMissing = True ))

    if (muidMuonContainer != 'None' and cfgKeyStore.isInInput( "Analysis::MuonContainer", "MuidMuonCollection" )):
        alg += MuonD3PDObject      (**_args (10, 'MuidMuon', kw,
                                    sgkey=muidMuonContainer, prefix='mu_muid_',
                                    include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                    allowMissing = True ))

    if (caloMuonContainer != 'None'): 
        alg += MuonD3PDObject      (**_args (10, 'CaloMuon', kw, 
                                             sgkey=caloMuonContainer, prefix='mu_calo_', 
                                             include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                             exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                             allowMissing = True)) 
    #-------------------------------------------------------------------------------------
    
