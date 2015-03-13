# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for InDetVKalVxNegativeTagInJetTool
# Author: Wouter van den Wollenberg (2013-2014)
# Note: This tool has its own file because it is common to many different taggers
#       so deletion of the one tagger that has this function won't break the others.
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags

metaInDetVKalVxNegativeTagInJetTool = { 'IsAVertexFinder' : True,
                                        'VertexFinderxAODBaseName' : 'SV1Flip',
                                        'OneInTotal'      : True,
                                        'DependsOn'       : ['AtlasExtrapolator',
                                                             'BTagTrackToVertexTool'] }

def toolInDetVKalVxNegativeTagInJetTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalVxNegativeTagInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    getNegativeTag                      default: True
    CutA0                               default: 2.0 (new tuned cuts for SV0 (Sara))
    CutZVrt                             default: 2.0
    CutPixelHits                        default: 2

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
                     'ExistIBL'         : geoFlags.isIBL(),
#                     'TrackSummaryTool' : None,
                     'getNegativeTag'   : True,
                     'CutA0'            : 2.0,
                     'CutZVrt'          : 2.0,
                     'CutPixelHits'     : 2 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
    return InDet__InDetVKalVxInJetTool(**options)
