# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id $
#
# @file PhotonD3PDMaker/python/PhotonTruthParticleD3PDObject.py
# @author Mike Hance <mike.hance@cern.ch>
# @date August 2010
# @brief Define D3PD object for a collection of TruthParticle's, for photon analyses...  based on EventCommonD3PDMaker tools
#


import EventCommonD3PDMaker
from PhotonD3PDMaker.PhotonD3PDMakerFlags                   import PhotonD3PDMakerFlags
from PhotonD3PDMaker.PhotonTruthParticleConfig              import PhotonTruthParticleConfig
from PhotonD3PDMaker.PAUTruthPhotonAssociation              import PAUTruthPhotonAssociation
from D3PDMakerCoreComps.D3PDObject                          import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation                   import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags                         import D3PDMakerFlags
from AthenaCommon.AlgSequence                               import AlgSequence
from TruthD3PDMaker.TruthParticleChildAssociation           import TruthParticleChildAssociation
from TruthD3PDMaker.TruthParticleParentAssociation          import TruthParticleParentAssociation
import PhotonD3PDMaker
import TruthD3PDMaker
from AthenaCommon.AppMgr import ToolSvc
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags           import PAUflags
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
from AthenaCommon.Constants import WARNING, INFO, ERROR, VERBOSE
import D3PDMakerCoreComps
DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

PhotonTruthParticleD3PDObject = make_SGDataVector_D3PDObject \
                                ('TruthParticleContainer',
                                 D3PDMakerFlags.TruthParticlesSGKey(),
                                 'mc_',
                                 'PhotonTruthParticleD3PDObject')

def _PhotonTruthParticleAlgHook (c, prefix, sgkey,
                                 seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                                 *args, **kw):
    
    PhotonTruthParticleConfig (seq = seq, prefix = prefix, sgkey = sgkey)
    return

PhotonTruthParticleD3PDObject.defineHook (_PhotonTruthParticleAlgHook)


PhotonTruthParticleD3PDObject.defineBlock (0, 'TruthKin',
                                           EventCommonD3PDMaker.FourMomFillerTool)

PhotonTruthParticleD3PDObject.defineBlock (0, 'TruthInfo',
                                           TruthD3PDMaker.TruthParticleFillerTool)

ProdVertexAssoc = SimpleAssociation \
                  (PhotonTruthParticleD3PDObject,
                   TruthD3PDMaker.TruthParticleProdVertexAssociationTool,
                   level = 1,
                   prefix = 'vx_',
                   blockname = 'ProdVert')
ProdVertexAssoc.defineBlock (1, 'ProdVertPos',
                             EventCommonD3PDMaker.GenVertexFillerTool)


ChildAssoc = TruthParticleChildAssociation(
    parent = PhotonTruthParticleD3PDObject,
    prefix = 'child_',
    # target = '', # filled by hook
    level = 0 )

def _TruthParticleChildAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_TruthParticleChildAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
        return
PhotonTruthParticleD3PDObject.defineHook(_TruthParticleChildAssocHook)
    
ParentAssoc = TruthParticleParentAssociation(
    parent = PhotonTruthParticleD3PDObject,
    prefix = 'parent_',
    # target = '', # filled by hook
    level = 0 )

def _TruthParticleParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_parent_TruthParticleParentAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
PhotonTruthParticleD3PDObject.defineHook(_TruthParticleParentAssocHook)
