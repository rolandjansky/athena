# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file MuonD3PDMaker/python/muonD3PD.py
# @author srivas prasad <srivas.prasad@cern.ch>
# @date Nov, 2010
# @brief Construct a muon D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from MuonD3PDMaker.MuonD3PDMakerFlags                  import MuonD3PDFlags
from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject
from RecExConfig.RecFlags                              import rec
from AthenaCommon.AlgSequence                          import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def muonD3PD (file,
              tuplename = 'muon',
              seq = topSequence,
              D3PDSvc = 'D3PD::RootD3PDSvc',
              streamNameRoot = None,
              **kw):
    
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file,
                                      D3PDSvc = D3PDSvc,
                                      streamNameRoot = streamNameRoot)
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))

    if D3PDMakerFlags.DoTrigger():

        # Segregate trigger decision flags in a separate tree.
        trigalg = D3PDMakerCoreComps.MakerAlg(tuplename + 'TrigDec',
                                              seq,
                                              file = file,
                                              D3PDSvc = D3PDSvc,
                                              streamNameRoot = streamNameRoot)
        trigalg += EventInfoD3PDObject        (0)
        alg.trigDecisionTree = trigalg
    
        
    # Muon blocks
    alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                sgkey='Muons',
                                                prefix='mu_', allowMissing=True))

    if not MuonD3PDFlags.doNewChainOnly():
        
        alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                    sgkey='MuidMuonCollection',
                                                    prefix='mu_muid_', allowMissing=True))
        alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey='StacoMuonCollection',
                                                prefix='mu_staco_', allowMissing=True))
        alg += MuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                    sgkey='CaloMuonCollection',
                                                    prefix='mu_calo_', allowMissing=True))
        
    # Muon segments
    alg += MuonSegmentD3PDObject             (**_args (10, 'MuonSeg', kw,
                                                       sgkey='MuonSegments',
                                                       prefix='muonseg_',
                                                       include = ['MuonSegmentT0'],
                                                       exclude = ['MuonboySegmentT0'],
                                                       allowMissing=True
                                                       )
                                              )

    if not MuonD3PDFlags.doNewChainOnly():

        alg += MuonSegmentD3PDObject             (**_args (10, 'MooreSeg', kw,
                                                           sgkey='MooreSegments',
                                                           prefix='mooreseg_',
                                                           include = ['MooreSegmentT0'],
                                                           exclude = ['MuonboySegmentT0'],
                                                           allowMissing=True
                                                           )
                                                  )
        
        alg += MuonSegmentD3PDObject             (**_args (10, 'MboySeg', kw,
                                                           sgkey='ConvertedMBoySegments',
                                                           prefix='mboyseg_',
                                                           include = ['MuonboySegmentT0'],
                                                           exclude = ['MooreSegmentT0'],
                                                           allowMissing=True
                                                           )
                                                  )

        alg += MuonSegmentD3PDObject             (**_args (10, 'MuGirlSeg', kw,
                                                           sgkey='MuGirlSegments',
                                                           prefix='mgseg_',
                                                           include = ['MooreSegmentT0'],
                                                       exclude = ['MuonboySegmentT0'],
                                                           allowMissing=True
                                                           )
                                                  )

    from MuonD3PDMaker.MuonSpShowerD3PDObject            import MuonSpShowerD3PDObject
    alg += MuonSpShowerD3PDObject (**_args(10, 'MuonSpShower', kw, allowMissing=True))

    # Truth    
    if rec.doTruth():
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject
        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw, allowMissing=True))
        
        from MuonD3PDMaker.TrackRecordD3PDObject            import TrackRecordD3PDObject
        alg += TrackRecordD3PDObject (**_args(10, 'MuonEntryLayerFilter', kw))
        
    # Trigger
    if D3PDMakerFlags.DoTrigger():
        alg += MuonTriggerBitsD3PDObject      (**_args ( 2, 'MuonTriggerBits', kw))

    alg.MetadataTools += [LBMetadataConfig()]

    return alg
