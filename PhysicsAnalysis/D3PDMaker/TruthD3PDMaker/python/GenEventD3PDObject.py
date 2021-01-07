# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file TruthD3PDMaker/python/GenParticleD3PDObject.py
## @brief gen event D3PD object; includes pileup info
## @author Georges Aad
## @date Nov, 2010
##

import TruthD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from TruthD3PDAnalysis.AllTruthFilterTool import AllTruthFilterTool
from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys

def make_GenEvent_D3PDObject( default_prefix, default_sgkey,
                              default_object_name = "",
                              default_filter = AllTruthFilterTool(),
                              default_label = None, **other_defaults ):

    def make_obj( name, prefix, object_name,
                  getter = None, sgkey = None, filter = default_filter,
                  label = default_label, **kw ):

        if sgkey is None: sgkey = default_sgkey
        if label is None: label = TruthD3PDKeys.GenEventGetterLabel()
        if getter is None:
            getter = TruthD3PDMaker.GenEventGetterTool( name + '_Getter',
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

GenEventD3PDObject = make_GenEvent_D3PDObject( TruthD3PDKeys.GenEventPrefix(),
                                               D3PDMakerFlags.TruthSGKey(),
                                               "GenEventD3PDObject" )

GenEventD3PDObject.defineBlock( 0, 'mcevt',
                                TruthD3PDMaker.GenEventFillerTool )

GenEventD3PDObject.defineBlock( 1, 'pileup',
                                TruthD3PDMaker.GenEventPileUpFillerTool,
                                GetterLabel = TruthD3PDKeys.GenEventGetterLabel())
