# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file TruthD3PDMaker/python/GenParticleD3PDObject.py
## @brief gen vertex D3PD object
## @author Georges Aad
## @date Nov, 2010
##

import TruthD3PDMaker
import D3PDMakerCoreComps

from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TruthD3PDAnalysis.AllTruthFilterTool import AllTruthFilterTool
from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags


def make_GenVertex_D3PDObject( default_prefix, default_sgkey,
                               default_object_name = "",
                               default_filter = AllTruthFilterTool(),
                               default_label = None, **other_defaults ):

    def make_obj( name, prefix, object_name,
                  getter = None, sgkey = None, filter = default_filter,
                  label = default_label, **kw ):

        if sgkey is None: sgkey = default_sgkey
        if label is None: label = TruthD3PDKeys.GenVertexGetterLabel()
        if getter is None:
            getter = TruthD3PDMaker.GenVertexGetterTool( name + '_Getter',
                                                         Label = label,
                                                         Selector = filter,
                                                         SGKey = sgkey )

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

GenVertexD3PDObject = make_GenVertex_D3PDObject( TruthD3PDKeys.GenVertexPrefix(),
                                                 D3PDMakerFlags.TruthSGKey(),
                                                 "GenVertexD3PDObject" )

GenVertexD3PDObject.defineBlock( 0,
                                 'GenVertex',
                                 TruthD3PDMaker.GenVertexFillerTool,
                                 WriteID=TruthD3PDFlags.WriteTruthVertexIDs() )

if TruthD3PDFlags.GenParticleAssocLabel() is not None and TruthD3PDFlags.GenParticleAssocLabel() != "":
    if TruthD3PDFlags.GenVertexInPartAssoc():
        GenVertexPartInAssoc = \
           IndexMultiAssociation( GenVertexD3PDObject,
                                  TruthD3PDMaker.GenVertexParticleAssociationTool,
                                  TruthD3PDFlags.GenParticleAssocLabel(),
                                  blockname = "GenVertexPartInAssoc",
                                  prefix = 'inpart_',
                                  InParticles = True )

    if TruthD3PDFlags.GenVertexOutPartAssoc():
        GenVertexPartOutAssoc = \
           IndexMultiAssociation( GenVertexD3PDObject,
                                  TruthD3PDMaker.GenVertexParticleAssociationTool,
                                  TruthD3PDFlags.GenParticleAssocLabel(),
                                  blockname = "GenVertexPartOutAssoc",
                                  prefix = 'outpart_',
                                  InParticles = False )

if TruthD3PDFlags.GenEventAssocLabel() is not None and TruthD3PDFlags.GenEventAssocLabel() != "":
    GenVertexEventAssoc = IndexAssociation( GenVertexD3PDObject,
                                            TruthD3PDMaker.GenVertexEventAssociationTool,
                                            TruthD3PDFlags.GenEventAssocLabel(),
                                            level = 1,
                                            blockname = "GenVertexEventAssoc",
                                            prefix = 'mcevt_' )
