# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/atlfast1D3PD.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Feb, 2010
# @brief Construct an atlfast1 D3PD.
#


import D3PDMakerCoreComps

from EventCommonD3PDMaker.EventInfoD3PDObject        import EventInfoD3PDObject
from TruthD3PDMaker.Atlfast1ElectronD3PDObject       import Atlfast1ElectronD3PDObject
from TruthD3PDMaker.Atlfast1PhotonD3PDObject         import Atlfast1PhotonD3PDObject
from MuonD3PDMaker.MuonD3PDObject                    import MuonD3PDObject
from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject
from TruthD3PDMaker.TruthJetD3PDObject               import TruthJetD3PDObject
from TruthD3PDMaker.Atlfast1MissingETD3PDObject      import Atlfast1MissingETD3PDObject
from TruthD3PDMaker.Atlfast1MissingETD3PDObject      import TruthMETD3PDObject

from TruthD3PDMaker.GenEventD3PDObject               import GenEventD3PDObject
from TruthD3PDAnalysis.truthParticleConfig           import truthParticleConfig
from TruthD3PDMaker.TruthParticleD3PDObject          import TruthParticleD3PDObject
from TruthD3PDMaker.PartonJetConfig                  import PartonJetConfig
from RecExConfig.RecFlags                            import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def atlfast1D3PD (file,
                  tuplename = 'atlfast1',
                  seq = topSequence,
                  D3PDSvc = 'D3PD::RootD3PDSvc'):

    #--------------------------------------------------------------------------
    # Configuration
    #--------------------------------------------------------------------------
    if rec.doTruth():
        truthParticleConfig (seq)
        #TruthJetFilterConfig (seq, excludeWZdecays = False)
        # PartonJetConfig is used to build parton-level jets
        # PartonJetConfig requires JetSimTools-00-01-22 or higher
        PartonJetConfig (doPythia = True, doHerwig = False)

    #--------------------------------------------------------------------------
    # Make the D3PD
    #--------------------------------------------------------------------------
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)
    alg += EventInfoD3PDObject (10)
    alg += Atlfast1ElectronD3PDObject (10)
    alg += Atlfast1PhotonD3PDObject (10)
    alg += MuonD3PDObject (0)
    alg += JetD3PDObject (0)
    alg += Atlfast1MissingETD3PDObject (0)

    #alg += TauD3PDObject (0)

    if rec.doTruth():
        alg += GenEventD3PDObject (1)
        alg += TruthParticleD3PDObject (1)
        alg += TruthMETD3PDObject (level=10)
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt4TruthJets', prefix='AntiKt4TruthJets_')
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt4TruthPartonJets', prefix='AntiKt4TruthPartonJets_')

    return alg
