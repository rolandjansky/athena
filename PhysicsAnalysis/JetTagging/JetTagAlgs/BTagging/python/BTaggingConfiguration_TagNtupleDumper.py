# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1Tag
# Author: Dan Guest (March 2016)
from BTagging.BTaggingFlags import BTaggingFlags

metaTagNtupleDumper = { 'IsATagger'          : False,#True,
                        'DependsOn'          : ['AtlasExtrapolator',
                                                'BTagTrackToVertexTool'],
                        'ToolCollection'     : 'TagNtupleDumper'}

def toolTagNtupleDumper(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TagNtupleDumper tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Stream                              default: 'FTAG'

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {
            'OutputLevel'  : BTaggingFlags.OutputLevel,
            'Stream'  : 'FTAG',
        }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TagNtupleDumper
    return Analysis__TagNtupleDumper(**options)
