# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps            
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject
from JetD3PDMaker.JetTileD3PDObject           import JetTileD3PDObject
#from MissingETD3PDMaker.MissingETD3PDObject   import *
from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
from EventCommonD3PDMaker.GenEventD3PDObject  import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject   import TruthParticleD3PDObject
from RecExConfig.RecFlags                     import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

inputSGkey='AntiKt4TopoEMNewJets'

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


# TD: changed the tuplename here:
def JetTileD3PD (file,
             level = 4,
             tuplename = 'MyTuple',
             seq = topSequence,
             D3PDSvc = 'D3PD::RootD3PDSvc',
             streamNameRoot = None,
             **kw):


    alg = D3PDMakerCoreComps.MakerAlg(tuplename,
                    seq,
                    file = file,
                    D3PDSvc = D3PDSvc,
                    streamNameRoot = streamNameRoot)
    
    JetIncludes = ['AssocTrackCont']

    alg += EventInfoD3PDObject        (**_args (level, 'EventInfo', kw ))
         
    alg += JetTileD3PDObject (**_args(level,inputSGkey, kw,  sgkey=inputSGkey,   prefix='AntiKt4Topo_' ) )

    from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))
    addBunchStructureMetadata( alg )

    
    from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
    from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata

    alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
    addTrigConfMetadata( alg )

    if D3PDMakerFlags.DoTrigger():
        alg +=  jetMETTriggerBitsD3PDObject(level)
        
 
    from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
    TrackD3PDFlags.storeVertexTrackIndexAssociation.set_Value_and_Lock(False)
    TrackD3PDFlags.storeVertexTrackAssociation.set_Value_and_Lock(False)
    TrackD3PDFlags.storeVertexFitQuality.set_Value_and_Lock(False)

    from TrackD3PDMaker.VertexD3PDObject    import PrimaryVertexD3PDObject
    alg += PrimaryVertexD3PDObject (**_args (0, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))    


    if rec.doTruth():
        alg += GenEventD3PDObject     (**_args (1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args (1, 'TruthParticle', kw))
        alg += JetTileD3PDObject      (**_args (1, 'AntiKt4TruthJets', kw, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_' ) )
        
        
    alg.MetadataTools += [LBMetadataConfig()]    
    return alg
