# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDMaker/python/TruthParticleD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Dec, 2009
# @brief Define D3PD object for a collection of TruthParticle's.
#


import TruthD3PDMaker
import EventCommonD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags
from TruthD3PDMaker.TruthParticleChildAssociation     import TruthParticleChildAssociation
from TruthD3PDMaker.TruthParticleParentAssociation    import TruthParticleParentAssociation


TruthParticleD3PDObject = make_SGDataVector_D3PDObject \
  ('DataVector<xAOD::TruthParticle_v1>',
   D3PDMakerFlags.TruthParticlesSGKey(),
   'mc_',
   'TruthParticleD3PDObject')

TruthParticleD3PDObject.defineBlock (0, 'TruthKin',
                                     EventCommonD3PDMaker.FourMomFillerTool)

TruthParticleD3PDObject.defineBlock (0, 'TruthInfo',
                                     TruthD3PDMaker.TruthParticleFillerTool)

ProdVertexAssoc = SimpleAssociation \
                  (TruthParticleD3PDObject,
                   TruthD3PDMaker.TruthParticleProdVertexAssociationTool,
                   level = 1,
                   prefix = 'vx_',
                   blockname = 'ProdVert')
ProdVertexAssoc.defineBlock (
    1, 'ProdVertPos',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['x', 'y', 'z', 'barcode'])


ChildAssoc = TruthParticleChildAssociation(
                        parent = TruthParticleD3PDObject,
                        prefix = 'child_',
                        # target = '', # filled by hook
                        level = 0 )

def _TruthParticleChildAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_child_TruthParticleChildAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
TruthParticleD3PDObject.defineHook(_TruthParticleChildAssocHook)

ParentAssoc = TruthParticleParentAssociation(
                        parent = TruthParticleD3PDObject,
                        prefix = 'parent_',
                        # target = '', # filled by hook
                        level = 0 )

def _TruthParticleParentAssocHook (c, prefix, *args, **kw):
    assoc = getattr(c, c.name() + '_parent_TruthParticleParentAssociation', None)
    if assoc:
        indexer = getattr(assoc, assoc.name() + 'Index')
        indexer.Target = prefix
    return
TruthParticleD3PDObject.defineHook(_TruthParticleParentAssocHook)

