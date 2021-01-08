# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file TruthD3PDMaker/python/GenParticleD3PDObject.py
## @brief gen particles and truth tracks D3PD object
## @author Georges Aad
## @date Nov, 2010
##

import TruthD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from TruthD3PDAnalysis.AllTruthFilterTool import AllTruthFilterTool
from TruthD3PDAnalysis.StableChargedTruthFilterTool import StableChargedTruthFilterTool
from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags


def make_GenParticle_D3PDObject( default_prefix, default_sgkey,
                                 default_object_name = "",
                                 default_filter = AllTruthFilterTool(),
                                 default_label = None, **other_defaults ):

    def make_obj( name, prefix, object_name,
                  getter = None, sgkey = None, filter = default_filter,
                  label = default_label, **kw ):

        if sgkey is None: sgkey = default_sgkey
        if label is None: label = prefix
        if getter is None:
            getter = TruthD3PDMaker.GenParticleGetterTool (name + "_Getter",
                                                           Label = label,
                                                           SGKey = sgkey,
                                                           Selector = filter )

        defs = other_defaults.copy()
        defs.update( kw )

        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        return D3PDMakerCoreComps.VectorFillerTool( name,
                                                    Prefix = prefix,
                                                    Getter = getter,
                                                    ObjectName = object_name,
                                                    SaveMetadata = \
                                                    D3PDMakerFlags.SaveObjectMetadata(),
                                                    **defs )

    return D3PDObject( make_obj, default_prefix, default_object_name )

GenParticleD3PDObject = make_GenParticle_D3PDObject( TruthD3PDKeys.GenParticlePrefix(),
                                                     D3PDMakerFlags.TruthSGKey(),
                                                     "GenParticleD3PDObject",
                                                     AllTruthFilterTool(),
                                                     TruthD3PDKeys.GenParticleGetterLabel() )

GenParticleD3PDObject.defineBlock( 0, 'GenParticle',
                                   TruthD3PDMaker.GenParticleFillerTool )

if TruthD3PDFlags.GenEventAssocLabel() is not None and TruthD3PDFlags.GenEventAssocLabel() != "":
    GenPartEventAssoc = IndexAssociation( GenParticleD3PDObject,
                                          TruthD3PDMaker.GenParticleEventAssociationTool,
                                          TruthD3PDFlags.GenEventAssocLabel(),
                                          blockname = "GenPartEventAssoc",
                                          prefix = 'mcevt_' )

if TruthD3PDFlags.GenVertexAssocLabel() is not None and TruthD3PDFlags.GenVertexAssocLabel() != "":
    GenPartProdVertexAssoc = IndexAssociation( GenParticleD3PDObject,
                                               TruthD3PDMaker.GenParticleVertexAssociationTool,
                                               TruthD3PDFlags.GenVertexAssocLabel(),
                                               blockname = "GenPartProdVertexAssoc",
                                               prefix = 'mcprodvtx_',
                                               DecayVertex = False )

if TruthD3PDFlags.GenParticleMother():
    GenPartProdVertexAssocM = SimpleAssociation( GenParticleD3PDObject,
                                                 TruthD3PDMaker.GenParticleVertexAssociationTool,
                                                 blockname = "GenPartProdVertexAssocM",
                                                 prefix = '',
                                                 DecayVertex = False)

    GenPartMotherAssoc = IndexMultiAssociation( GenPartProdVertexAssocM,
                                                TruthD3PDMaker.GenVertexParticleAssociationTool,
                                                TruthD3PDKeys.GenParticleGetterLabel(),
                                                blockname = "GenPartMotherAssoc",
                                                prefix = 'mother_',
                                                InParticles = True)

if TruthD3PDFlags.GenVertexAssocLabel() is not None and TruthD3PDFlags.GenVertexAssocLabel() != "":
    GenPartDecayVertexAssoc = IndexAssociation( GenParticleD3PDObject,
                                                TruthD3PDMaker.GenParticleVertexAssociationTool,
                                                TruthD3PDFlags.GenVertexAssocLabel(),
                                                blockname = "GenPartDecayVertexAssoc",
                                                prefix = 'mcdecayvtx_',
                                                DecayVertex = True )

if TruthD3PDFlags.GenParticleChild():
    GenPartDecayVertexAssocC = SimpleAssociation( GenParticleD3PDObject,
                                                  TruthD3PDMaker.GenParticleVertexAssociationTool,
                                                  blockname = "GenPartDecayVertexAssocC",
                                                  prefix = '',
                                                  DecayVertex = True)

    GenPartChildAssoc = IndexMultiAssociation( GenPartDecayVertexAssocC,
                                               TruthD3PDMaker.GenVertexParticleAssociationTool,
                                               TruthD3PDKeys.GenParticleGetterLabel(),
                                               blockname = "GenPartChildAssoc",
                                               prefix = 'child_',
                                               InParticles = False )

if TruthD3PDFlags.TruthTrackAssocLabel() is not None and TruthD3PDFlags.TruthTrackAssocLabel() != "":
    GenPartTruthTrackAssoc = IndexAssociation( GenParticleD3PDObject,
                                               TruthD3PDMaker.GenParticleParticleAssociationTool,
                                               TruthD3PDFlags.TruthTrackAssocLabel(),
                                               blockname = "GenPartTruthTrackAssoc",
                                               prefix = 'truthtracks_' )

GenTruthTrackD3PDObject = make_GenParticle_D3PDObject( TruthD3PDKeys.TruthTrackPrefix(),
                                                       D3PDMakerFlags.TruthSGKey(),
                                                       "GenTruthTrackD3PDObject",
                                                       StableChargedTruthFilterTool(),
                                                       TruthD3PDKeys.TruthTrackGetterLabel() )

GenTruthTrackD3PDObject.defineBlock( 0, 'TruthTracks',
                                     TruthD3PDMaker.GenParticlePerigeeFillerTool )
