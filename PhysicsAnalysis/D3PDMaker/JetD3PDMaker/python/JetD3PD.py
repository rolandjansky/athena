# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Haifeng Li <Haifeng.Li@cern.ch>

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
from JetD3PDMaker.JetD3PDObject               import JetD3PDObject
from TrackD3PDMaker.xAODVertexD3PDObject      import PrimaryxAODVertexD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
from TruthD3PDMaker.GenEventD3PDObject        import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject   import TruthParticleD3PDObject
from RecExConfig.RecFlags                     import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def JetD3PD (file,
             level = 4,
             tuplename = 'jet',
             seq = topSequence,
             D3PDSvc = 'D3PD::RootD3PDSvc',
             streamNameRoot = None,
             **kw):

    
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file,
                                      D3PDSvc = D3PDSvc,
                                      streamNameRoot = streamNameRoot)

    alg += EventInfoD3PDObject        (**_args (level, 'EventInfo', kw ))
    alg += ElectronD3PDObject         (**_args (level, 'Electron', kw))
    alg += PhotonD3PDObject           (**_args (level, 'Photon', kw))
    alg += MuonD3PDObject             (**_args (level, 'Muon', kw))
    #alg += MissingETD3PDObject        (**_args (level, 'MissingET', kw))
   
    

    #at the moment only the first one holds also b tagging information
    alg += JetD3PDObject (**_args(level,'AntiKt4H1TopoJets', kw,  sgkey='AntiKt4H1TopoJets',   prefix='AntiKt4H1Topo_' ) )
    alg += JetD3PDObject (**_args(level,'AntiKt6H1TopoJets', kw,  sgkey='AntiKt6H1TopoJets',   prefix='AntiKt6H1Topo_' ) )

    
    if D3PDMakerFlags.DoTrigger():
        alg +=  jetMETTriggerBitsD3PDObject(level)
        
 
    alg += PrimaryxAODVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                                 allowMissing = True,
                                                 sgkey = D3PDMakerFlags.VertexSGKey(),
                                                 prefix = 'vxp_'))    


    if rec.doTruth():
        alg += GenEventD3PDObject     (**_args (1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args (1, 'TruthParticle', kw))
        alg += JetD3PDObject          (**_args (1, 'AntiKt4TruthJets', kw, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_' ) )
        
        #alg += JetD3PDObject (level, sgkey='AntiKt6TruthJets', prefix='AntiKt6Truth_'  )
        
        
        
    alg.MetadataTools += [LBMetadataConfig()]    
    return alg
