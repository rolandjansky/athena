# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PhotonD3PDMaker.PhotonD3PDMakerFlags import PhotonD3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PD import _args

def AddTriggerToD3PD(alg, **kw):
    # trigger objects info: to be used for matching offline photons to trigger objects and know the trigger decision
    if PhotonD3PDMakerFlags.DumpTriggerObjectsInfo():
        from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
        alg += TrigDecisionD3PDObject (**_args (2, 'TrigDecision', kw))
        from TrigEgammaD3PDMaker.TrigEgammaD3PD              import TrigEgammaD3PDObjects
        TrigEgammaD3PDObjects (alg, **_args (1, 'TrigEgamma', kw))
        from TrigMuonD3PDMaker.TrigMuonD3PD           import TrigMuonD3PDObjects
        TrigMuonD3PDObjects   (alg, 1,
                               TrigMuonEFIsolation_allowMissing = True) 
        
    if PhotonD3PDMakerFlags.DumpLotsOfTriggerInfo():
        from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
        from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject
        from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import METD3PDTriggerBitsObject
        
        alg += EnergySumROID3PDObject(**_args ( 1, 'EnergySumROI', kw,
                                                prefix = "trig_L1_esum_",
                                                allowMissing = True))
        
        # The LVL2 information:
        # does this need to be at a higher level?
        alg += TrigMETD3PDObject(**_args( 1, 'TrigMETL2', kw,
                                          prefix = "trig_L2_met_",
                                          sgkey = "HLT_T2MissingET" ))
        
        # The EF information:
        alg += TrigMETD3PDObject(**_args ( 1, 'TrigMETEF', kw,
                                           prefix = "trig_EF_met_",
                                           sgkey = "HLT_TrigEFMissingET" ))
        
        
        alg += METD3PDTriggerBitsObject       (**_args ( 0, 'METTriggerBits', kw))
        
        from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
        alg += BGCodeD3PDObject (**_args (1, 'BGCode', kw))
        
        from RecExConfig.ObjKeyStore                  import cfgKeyStore
        if cfgKeyStore.isInInputFile( "TrigMissingETContainer", "HLT_L2MissingET_FEB" ):
            alg += TrigMETD3PDObject(**_args( 1, 'TrigMETL2_FEB', kw,
                                              prefix = "trig_L2_feb_met_",
                                              sgkey = "HLT_L2MissingET_FEB" ))
            
