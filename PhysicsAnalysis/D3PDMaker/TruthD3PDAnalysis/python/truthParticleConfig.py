# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDAnalysis/python/TruthParticleConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Dec, 2009
# @brief Configure algorithms to build filtered TruthParticleContainer
#        to write to D3PD.
#


import TruthD3PDAnalysis
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from McParticleAlgs.JobOptCfg                 import createMcAodBuilder
from RecExConfig.RecFlags                     import rec
from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from AthenaCommon                             import CfgMgr


def truthParticleConfig (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                         sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
                         prefix = '',
                         doPileup      = D3PDMakerFlags.TruthDoPileup(),
                         writePartons  = D3PDMakerFlags.TruthWritePartons(),
                         writeHadrons  = D3PDMakerFlags.TruthWriteHadrons(),
                         writeGeant    = D3PDMakerFlags.TruthWriteGeant(),
                         writeBHadrons = D3PDMakerFlags.TruthWriteBHadrons(),
                         writeBosons   = D3PDMakerFlags.TruthWriteBosons(),
                         writeBSM      = D3PDMakerFlags.TruthWriteBSM(),
                         writeTauHad   = D3PDMakerFlags.TruthWriteTauHad(),
                         writeEverything     = D3PDMakerFlags.TruthWriteEverything(),
                         writeFirstN         = D3PDMakerFlags.TruthWriteFirstN(),
                         writeBosonProducts  = D3PDMakerFlags.TruthWriteBosonProducts(),
                         writeBSMProducts    = D3PDMakerFlags.TruthWriteBSMProducts(),
                         writeTopAndDecays   = D3PDMakerFlags.TruthWriteTopAndDecays(),
                         writeAllLeptons     = D3PDMakerFlags.TruthWriteAllLeptons(),
                         writeStatus3        = D3PDMakerFlags.TruthWriteStatus3(),
                         geantPhotonPtThresh = D3PDMakerFlags.TruthGeantPhotonPtThresh(),
                         partonPtThresh      = D3PDMakerFlags.TruthPartonPtThresh(),):

    if not rec.doTruth():
        return

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('TruthParticleContainer', sgkey):
        return

    algname = prefix + sgkey + 'Builder'
    if hasattr (seq, algname):
        return

    filtname = prefix + sgkey + 'Filter'

    seq += createMcAodBuilder\
           (algname,
            inMcEvtCollection = D3PDMakerFlags.TruthSGKey(),
            outMcEvtCollection = sgkey + '_GEN_D3PD',
            outTruthParticles = sgkey,
            filterTool = TruthD3PDAnalysis.TruthParticleFilterTool
              (filtname,
               DoPileup     = doPileup,
               WritePartons = writePartons,
               WriteHadrons = writeHadrons,
               WriteBHadrons = writeBHadrons,
               WriteGeant   = writeGeant,
               WriteBSM     = writeBSM,
               WriteBosons  = writeBosons,
               WriteTauHad  = writeTauHad,
               WriteEverything = writeEverything,
               WriteFirstN     = writeFirstN,
               WriteBosonProducts = writeBosonProducts,
               WriteBSMProducts = writeBSMProducts,
               WriteTopAndDecays = writeTopAndDecays,
               WriteAllLeptons = writeAllLeptons,
               WriteStatus3 = writeStatus3,
               GeantPhotonPtThresh = geantPhotonPtThresh,
               PartonPtThresh = partonPtThresh,
               ),
            cnvTool = CfgMgr.TruthParticleCnvTool('D3PDTruthParticleCnvTool'),
            )

    cfgKeyStore.addTransient ('TruthParticleContainer', sgkey)
    return

    
    
