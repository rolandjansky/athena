# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterCOMB
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterCOMB = { 'IsATagger'       : True,
                      'DependsOn'       : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'JetFitterTag',
                                           'InDetVKalVxInJetTool',
                                           'BTagFullLinearizedTrackFactory',
                                           'BTagTrackToVertexIPEstimator',
                                           'IP3DTag'],
                      'ToolCollection'  : 'JetFitterCOMB' }

def toolJetFitterCOMB(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterCOMB tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    combinedTagString                   default: "JetFitterCOMB"
    listTaggers                         default: [ "JetFitterTag","IP3D" ]

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'combinedTagString'                : 'JetFitterCOMB',
                     'listTaggers'                      : [ "JetFitterTag","IP3D" ] }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
    return Analysis__CombinerToolTag(**options)
