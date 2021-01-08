# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/egammaTruthD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2011
# @brief Configure egtruth D3PD object.
#
# This is meant to reproduce the egtruth block from the CBNTs.
#


from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags
from egammaD3PDAnalysis.egammaTruthParticleConfig \
                                            import egammaTruthParticleConfig
from AthenaCommon.AlgSequence               import AlgSequence
from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TruthD3PDMaker


#FIXME:
egammaTruth_sgkey = 'egammaTruth'


egammaTruthD3PDObject = make_SGDataVector_D3PDObject \
  ('DataVector<xAOD::TruthParticle_v1>',
   egammaTruth_sgkey,
   'egtruth_',
   'egammaTruthD3PDObject')

def _egammaTruthAlgHook (c, prefix, sgkey,
                         seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                         *args, **kw):
    egammaTruthParticleConfig (seq = seq, prefix = prefix, sgkey = sgkey)
    return
egammaTruthD3PDObject.defineHook (_egammaTruthAlgHook)

egammaTruthD3PDObject.defineBlock (0, 'TruthKin',
                                   EventCommonD3PDMaker.FourMomFillerTool,
                                   WriteM = False)

egammaTruthD3PDObject.defineBlock (0, 'TruthInfo',
                                   TruthD3PDMaker.TruthParticleFillerTool)
egammaTruthD3PDObject.defineBlock (
    0, 'CaloPos',
    D3PDMakerCoreComps.AuxDataFillerTool,
    AuxPrefix = D3PDMakerFlags.EgammaUserDataPrefix(),
    Vars = ['etaCalo #Eta position of the impact of the track in calorimeter sampling 2',
            'phiCalo #Phi position of the impact of the track in calorimeter sampling 2'])
egammaTruthD3PDObject.defineBlock (
    0, 'TruthIso',
    D3PDMakerCoreComps.AuxDataFillerTool,
    AuxPrefix = D3PDMakerFlags.EgammaUserDataPrefix(),
    Vars = ['Etcone20'])


ProdVertexAssoc = SimpleAssociation \
                  (egammaTruthD3PDObject,
                   TruthD3PDMaker.TruthParticleProdVertexAssociationTool,
                   level = 1,
                   prefix = 'vx_',
                   blockname = 'ProdVert')
ProdVertexAssoc.defineBlock (
    1, 'ProdVertPos',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['x', 'y', 'z', 'barcode'])

