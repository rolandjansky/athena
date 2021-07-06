# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/TruthJetFilterConfig.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Build truth container to be used for parton-jet building
#

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from McParticleAlgs.JobOptCfg                 import createMcAodBuilder
from RecExConfig.RecFlags                     import rec
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from AthenaCommon                             import CfgMgr
from TruthD3PDMaker.TruthD3PDMakerConf        import D3PD__TruthJetFilterTool


def TruthJetFilterConfig (seq,
                          sgkey = 'Filtered'+D3PDMakerFlags.TruthParticlesSGKey(),
                          prefix = '',
                          excludeWZdecays = True,
                          photonCone   = -1.,
                          doPileup     = D3PDMakerFlags.TruthDoPileup(),
                          writePartons = D3PDMakerFlags.TruthWritePartons(),
                          writeHadrons = D3PDMakerFlags.TruthWriteHadrons(),
                          writeGeant   = D3PDMakerFlags.TruthWriteGeant(),
                          excludeLeptonsFromTau = False):

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
            filterTool = D3PD__TruthJetFilterTool
              (filtname,
               DoPileup     = doPileup,
               WritePartons = writePartons,
               WriteHadrons = writeHadrons,
               WriteGeant   = writeGeant,
               ExcludeWZdecays = excludeWZdecays,
               PhotonCone   = photonCone,
               ExcludeLeptonsFromTau = excludeLeptonsFromTau,
               ),
            cnvTool = CfgMgr.TruthParticleCnvTool('D3PDTruthJetCnvTool'),
            )

    cfgKeyStore.addTransient ('TruthParticleContainer', sgkey)
    return
