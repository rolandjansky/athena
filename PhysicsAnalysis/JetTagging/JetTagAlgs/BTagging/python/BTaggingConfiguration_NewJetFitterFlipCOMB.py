# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterFlipCOMB
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaNewJetFitterFlipCOMB = { 'IsATagger'       : True,
                             'DependsOn'       : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'NewJetFitterTagFlip',
                                                  'IP3DNegTag'],
                             'ToolCollection'  : 'NewJetFitterFlipCOMB' }

def toolNewJetFitterFlipCOMB(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterFlipCOMB tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    combinedTagString                   default: "JetFitterCOMBFlip"
    listTaggers                         default: [ "JetFitterTagFlip","IP3DNeg" ]

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'combinedTagString'                : 'JetFitterCOMBFlip',
                     'listTaggers'                      : [ "JetFitterTagFlip","IP3DNeg" ] }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__CombinerToolTag
    return Analysis__CombinerToolTag(**options)
