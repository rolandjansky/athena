# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerConfig/python/egammaD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Construct an egamma D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
#from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
#from MuonD3PDMaker.TruthMuonD3PDObject        import TruthMuonD3PDObject
#from MuonD3PDMaker.MuonTriggerBitsD3PDObject  import MuonTriggerBitsD3PDObject
#from MissingETD3PDMaker.MissingETD3PDObject   import MissingETD3PDObject
#from MissingETD3PDMaker.MissingETGoodnessD3PDObject import MissingETGoodnessD3PDObject
#from JetD3PDMaker.JetD3PDObject               import JetD3PDObject  
#from D3PDMakerConfig.METD3PDObject            import METD3PDObject
from CaloD3PDMaker.MBTSD3PDObject             import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject         import MBTSTimeD3PDObject
from CaloD3PDMaker.MBTSTriggerBitsD3PDObject  import MBTSTriggerBitsD3PDObject
#from CaloD3PDMaker.ZDCTriggerBitsD3PDObject   import ZDCTriggerBitsD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject import CollisionDecisionD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
from EventCommonD3PDMaker.GenEventD3PDObject  import GenEventD3PDObject
from EventCommonD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
from TrackD3PDMaker.VertexD3PDObject          import PrimaryVertexD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD       import TrigEgammaD3PDObjects
from RecExConfig.RecFlags                     import rec


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
            pass
        pass
    return kw


def egammaValD3PD (file,
                   level = 10,
                   tuplename = 'egamma',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc',
                   streamNameRoot = None,
                   **kw):

    # Add D3PD to the algorithm name to prevent clashes with the
    # existing `egamma' algorithm in reco.
    alg = D3PDMakerCoreComps.MakerAlg(tuplename + 'D3PD',
                                      file = file,
                                      D3PDSvc = D3PDSvc,
                                      streamNameRoot = streamNameRoot)
    detail=0
    alg += EventInfoD3PDObject        (**_args (level, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (detail, 'LArCollisionTime', kw)) 
    alg += ElectronD3PDObject         (**_args (level, 'Electron', kw)) # doesn't work because of PAUPhotonShowerBuilder
    alg += PhotonD3PDObject           (**_args (level, 'Photon', kw))  # idem
    #alg += MuonD3PDObject             (**_args (detail, 'Muon', kw))
    #alg += JetD3PDObject              (**_args (detail, 'Jet', kw))
    #alg += METD3PDObject              (**_args (level, 'MET', kw))
    #alg += MissingETD3PDObject        (**_args (level, 'MissingET', kw))
    #alg += MissingETGoodnessD3PDObject(**_args ( 2, 'METGoodness', kw))
    #alg += MBTSD3PDObject             (**_args (detail, 'MBTS', kw))
    alg += MBTSTimeD3PDObject         (**_args (detail, 'MBTSTime', kw))
    alg += MBTSTriggerBitsD3PDObject  (**_args (detail, 'MBTSTriggerBits', kw))
    #alg += ZDCTriggerBitsD3PDObject   (**_args (level, 'ZDCTriggerBits', kw))
    #alg += CollisionDecisionD3PDObject(**_args (detail, 'CollisionDecision', kw))
    if D3PDMakerFlags.DoTrigger():
        TrigEgammaD3PDObjects (alg, **_args (1, 'TrigEgamma', kw))
        from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
        from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject

        alg += EnergySumROID3PDObject(**_args (level, 'EnergySumROI', kw,
                                               prefix = "trig_L1_esum_",
                                               allowMissing = True))

        # The LVL2 information:
        alg += TrigMETD3PDObject(**_args( level, 'TrigMETL2', kw,
                                          prefix = "trig_L2_met_",
                                          sgkey = "HLT_T2MissingET" ))

        # The EF information:
        alg += TrigMETD3PDObject(**_args ( level, 'TrigMETEF', kw,
                                           prefix = "trig_EF_met_",
                                           sgkey = "HLT_TrigEFMissingET" ))

        
        from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))
        pass
    # May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))

    if rec.doTruth():
        #alg += TruthMuonD3PDObject    (**_args (level, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args (1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args (level, 'TruthParticle', kw))

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg
