# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file  MuonD3PDMaker/python/TruthMuonD3PDObject.py
# $author  Srivas Prasad <srivas.prasad@cern.ch>
# @date    March 2010
# @brief   dump true muons - modeled on Tau code


import MuonD3PDMaker
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject         import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags        import D3PDMakerFlags
from AthenaCommon.AlgSequence              import AlgSequence
from RecExConfig.ObjKeyStore               import cfgKeyStore
import TruthD3PDMaker


TruthMuonD3PDObject = make_SGDataVector_D3PDObject ('DataVector<xAOD::TruthParticle_v1>',
                                                    'TruthMuons',
                                                    'muonTruth_',
                                                    'TruthMuonD3PDObject')
def _truthMuonAlgHook (c, prefix, sgkey,
                       TruthContainer = 'TruthParticles',
                       **kw):
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('DataVector<xAOD::TruthParticle_v1>', sgkey):
        return

    # Is the algorithm already in the sequence?
    algname = prefix + 'TruthMuonsToSG'
    if hasattr (preseq, algname):
        return

    from TruthD3PDMaker.MCTruthClassifierConfig \
         import D3PDMCTruthClassifier
    preseq += MuonD3PDMaker.TruthMuonsToSG \
              (algname,
               TruthMuonContainer = sgkey,
               TruthContainer = TruthContainer,
               Classifier = D3PDMCTruthClassifier)
    return
TruthMuonD3PDObject.defineHook (_truthMuonAlgHook)


#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------
TruthMuonD3PDObject.defineBlock (0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteEt = False,
                                 WritePt = True,
                                 WriteEtaPhi = True )
TruthMuonD3PDObject.defineBlock (0, 'Info',
                                 TruthD3PDMaker.TruthParticleFillerTool,
                                 PDGIDVariable = 'PDGID')
from TruthD3PDMaker.MCTruthClassifierConfig \
     import D3PDMCTruthClassifier
TruthMuonD3PDObject.defineBlock (0, 'Classification',
                                 TruthD3PDMaker.TruthParticleClassificationFillerTool,
                                 Classifier = D3PDMCTruthClassifier)
#delete TruthMuonFiller?

TruthMuonD3PDObject.defineBlock(99, "TruthHits",
                                MuonD3PDMaker.MuonTruthHitsFillerTool )

