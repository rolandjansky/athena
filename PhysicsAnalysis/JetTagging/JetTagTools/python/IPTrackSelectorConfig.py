# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagTrackToVertexToolConfig import BTagTrackToVertexToolCfg

# import the TrackSelector configurable
Analysis__TrackSelector=CompFactory.Analysis__TrackSelector

def IPTrackSelectorCfg(flags, name = 'IPTrackSelector', useBTagFlagsDefaults = True, **options ):
    """Sets up a IPTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    useBLayerHitPrediction              default: True
    usepTDepTrackSel                    default: False
    nHitBLayer                          default: 0

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        trackToVertexTool = acc.popToolsAndMerge(BTagTrackToVertexToolCfg(flags, 'BTagTrackToVertexTool'))
        defaults = {'useBLayerHitPrediction' : True,
                     'nHitBLayer'             : 0 ,
                     'usepTDepTrackSel'       : False,
                     'trackToVertexTool'      : trackToVertexTool}

        for option in defaults:
            options.setdefault(option, defaults[option])

    options['name'] = name
    acc.setPrivateTools(Analysis__TrackSelector( **options))

    return acc
