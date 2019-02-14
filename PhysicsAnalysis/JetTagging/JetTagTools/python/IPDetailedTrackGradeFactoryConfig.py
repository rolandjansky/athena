# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import the DetailedTrackGradeFactory configurable
from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory

# define the class
class IPDetailedTrackGradeFactoryCfg( Analysis__DetailedTrackGradeFactory ):
     # constructor
    def __init__(self, name = 'IPDetailedTrackGradeFactory', useBTagFlagsDefaults = True, options = {}):
        """Sets up a IPDetailedTrackGradeFactory tool and returns it.

        The following options have BTaggingFlags defaults:

        useSharedHitInfo                    default: True
        useDetailSharedHitInfo              default: True
        useRun2TrackGrading                 default: False
        hitBLayerGrade                      default: True

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        #btagrun1 was used to check old config, not sure we need it for Run3
        btagrun1 = False

        if useBTagFlagsDefaults:
            defaults = { 'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'useRun2TrackGrading'    : (btagrun1 == False),
                     'useInnerLayers0HitInfo' : (btagrun1 == False),
                     'useDetailSplitHitInfo'  : (btagrun1 == False),
                     'hitBLayerGrade'         : True }
            for option in defaults:
                options.setdefault(option, defaults[option])
        options['name'] = name
        Analysis__DetailedTrackGradeFactory.__init__(self, **options)
