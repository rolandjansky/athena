# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file PhotonD3PDMaker/python/PhotonTruthParticleConfig.py
# @author Mike Hance <hance@hep.upenn.edu>
# @date August 2010
# @brief Configure algorithms to build filtered PhotonTruthParticleContainer
#        to write to D3PD.
#


import PhotonD3PDMaker
import TruthD3PDAnalysis
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PDMakerFlags     import PhotonD3PDMakerFlags
from McParticleAlgs.JobOptCfg                 import createMcAodBuilder
from RecExConfig.RecFlags                     import rec
from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from AthenaCommon                             import CfgMgr


def PhotonTruthParticleConfig (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                               sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
                               prefix = ''):
    
    if not rec.doTruth():
        return

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('TruthParticleContainer', sgkey):
        return

    algname = prefix + sgkey + 'Builder'
    if hasattr (seq, algname):
        return

    filtname = prefix + sgkey + 'Filter'

    if PhotonD3PDMakerFlags.SaveAllTruthParticles():

      seq += createMcAodBuilder\
             (algname,
              inMcEvtCollection = D3PDMakerFlags.TruthSGKey(),
              outMcEvtCollection = sgkey + '_GEN_D3PD',
              outTruthParticles = sgkey,
              filterTool = TruthD3PDAnalysis.TruthParticleFilterTool
                                                 (filtname,
                                                  DoPileup     = D3PDMakerFlags.TruthDoPileup(),
                                                  WritePartons = D3PDMakerFlags.TruthWritePartons(),
                                                  WriteHadrons = D3PDMakerFlags.TruthWriteHadrons(),
                                                  WriteGeant   = D3PDMakerFlags.TruthWriteGeant(),
                                                  GeantPhotonPtThresh = D3PDMakerFlags.TruthGeantPhotonPtThresh(),
                                                 ),
              cnvTool = CfgMgr.TruthParticleCnvTool('D3PDTruthParticleCnvTool'),
              )

    else:
      seq += createMcAodBuilder\
             (algname,
              inMcEvtCollection = D3PDMakerFlags.TruthSGKey(),
              outMcEvtCollection = sgkey + '_GEN_D3PD',
              outTruthParticles = sgkey,
              filterTool = PhotonD3PDMaker.PhotonTruthParticleFilterTool
                                               (filtname,
                                                DoPileup     = D3PDMakerFlags.TruthDoPileup(),
                                                WritePartons = D3PDMakerFlags.TruthWritePartons(),
                                                WriteHadrons = D3PDMakerFlags.TruthWriteHadrons(),
                                                WriteGeant   = D3PDMakerFlags.TruthWriteGeant(),
                                                ParticleFilters = PhotonD3PDMakerFlags.TruthParticleFilters()),
              cnvTool = CfgMgr.TruthParticleCnvTool('D3PDTruthParticleCnvTool'),
              )

    
    cfgKeyStore.addTransient ('TruthParticleContainer', sgkey)
    return
