# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Haifeng.Li@cern.ch

# For May reprocess data


import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
from JetD3PDMaker.JetD3PDObject               import JetD3PDObject
from TrackD3PDMaker.TrackD3PDObject           import TrackD3PDObject
from TrackD3PDMaker.VertexD3PDObject          import PrimaryVertexD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject   import *
from CaloD3PDMaker.MBTSD3PDObject             import MBTSD3PDObject
from egammaD3PDAnalysis.egammaUserDataConfig  import egammaUserDataConfig
from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
from EventCommonD3PDMaker.GenEventD3PDObject  import GenEventD3PDObject
from TruthD3PDAnalysis.truthParticleConfig    import truthParticleConfig
from TruthD3PDMaker.TruthParticleD3PDObject   import TruthParticleD3PDObject
from RecExConfig.RecFlags                     import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def JetD3PD (file,
             level = 4,
             tuplename = 'jet',
             seq = topSequence,
             D3PDSvc = 'D3PD::RootD3PDSvc'):

    egammaUserDataConfig (seq)
    if rec.doTruth():
        truthParticleConfig (seq)

    JetIncludes = ['AssocTrackCont']

    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)
    #-- General
    alg += EventInfoD3PDObject (level)
    alg += ElectronD3PDObject (0)
    alg += PhotonD3PDObject (0)
    alg += MuonD3PDObject (0)
    alg += PrimaryVertexD3PDObject (4, allowMissing = True,
                                    sgkey = D3PDMakerFlags.VertexSGKey(),
                                    prefix = 'vxp_')
    #-- JET
    alg += JetD3PDObject (level, sgkey='AntiKt4H1TopoJets',  include = JetIncludes, prefix='AntiKt4H1Topo_' )
    alg += JetD3PDObject (level, sgkey='AntiKt6H1TopoJets',  include = JetIncludes, prefix='AntiKt6H1Topo_'           )
    
    #alg += JetD3PDObject (level, sgkey='Cone4H1TopoJets',    include = JetIncludes, prefix='Cone4H1Topo_'   )
 
    #-- MET 
    alg += RefFinalMETD3PDObject (level)
    alg += MuonMETD3PDObject (level)
    alg += MuonBoyMETD3PDObject (level)
    alg += FinalMETD3PDObject (level)
    alg += CryoMETD3PDObject (level)
    alg += CryoConeMETD3PDObject (level)
    alg += RefEleMETD3PDObject (level)
    alg += RefJetMETD3PDObject (level)
    alg += RefGammaMETD3PDObject (level)
    alg += RefTauMETD3PDObject (level)
    alg += CellOutMETD3PDObject (level)
    alg += BaseMETD3PDObject (level)
    alg += Base0METD3PDObject (level)
    alg += CalibMETD3PDObject (level)
    alg += MuonBoySpectroMETD3PDObject (level)
    alg += MuonBoyTrackMETD3PDObject (level)
    alg += TopoMETD3PDObject (level)
    alg += CorrTopoMETD3PDObject (level)
    alg += LocHadTopoMETD3PDObject (level)
    alg += TopoObjMETD3PDObject (level)

    #-- Trigger
    if D3PDMakerFlags.DoTrigger():
        alg +=  jetMETTriggerBitsD3PDObject(level)

    #-- Truth     
    if rec.doTruth() :
        alg += GenEventD3PDObject (1)
        #alg += TruthParticleD3PDObject (level)
        #alg += JetD3PDObject (level, sgkey='Cone4TruthJets'  , prefix='Cone4Truth_'    )
        alg += JetD3PDObject (3, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_' )
        #alg += JetD3PDObject (3, sgkey='AntiKt6TruthJets', prefix='AntiKt6Truth_'  )
        

    #-- Meta Data    
    alg.MetadataTools += [LBMetadataConfig()]    
    return alg
