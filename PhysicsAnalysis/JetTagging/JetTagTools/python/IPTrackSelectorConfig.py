# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagTrackToVertexToolConfig import BTagTrackToVertexToolCfg
from JetTagTools.InDetEtaDependentCutsSvcConfig import IDEtaDependentCuts_IPXD_SvcCfg

# import the TrackSelector configurable
Analysis__TrackSelector=CompFactory.Analysis.TrackSelector

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

        if flags.GeoModel.Run not in ['RUN1', 'RUN2', 'RUN3']:
            acc.merge(IDEtaDependentCuts_IPXD_SvcCfg(flags, name="IDEtaDepCutsSvc_" + name))
            options.setdefault("InDetEtaDependentCutsSvc", acc.getService("IDEtaDepCutsSvc_" + name))

    options['name'] = name
    acc.setPrivateTools(Analysis__TrackSelector( **options))

    return acc
