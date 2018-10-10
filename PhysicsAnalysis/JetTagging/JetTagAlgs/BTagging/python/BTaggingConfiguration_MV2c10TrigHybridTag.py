# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV2c10Tag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV2c10TrigHybridTag = {'IsATagger': False,  # True,
                           'xAODBaseName': 'MV2c00',
                           'DependsOn': ['AtlasExtrapolator',
                                         'BTagCalibrationBrokerTool',
                                         'BTagTrackToVertexTool'],
                           'CalibrationFolders': ['MV2c10TrigHybrid', ],
                           'PassByPointer': {'calibrationTool': 'BTagCalibrationBrokerTool'},
                           'ToolCollection': 'MV2c10TrigHybridTag'}


def toolMV2c10TrigHybridTag(name, useBTagFlagsDefaults=True, **options):
    """Sets up a MV2c10TrigHybridTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MV2c10"
    taggerNameBase                      default: "MV2c10"
    forceMV2CalibrationAlias            default: BTaggingFlags.ForceMV2CalibrationAlias
    MV2CalibAlias                       default: BTaggingFlags.MV2CalibAlias

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    from BTagging.MV2defaultValues import default_values
    from BTagging.MV2defaultValues import MVTM_varNames

    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'Runmodus': BTaggingFlags.Runmodus,
                    'taggerName': 'MV2c10TrigHybrid',
                    'taggerNameBase': 'MV2c10TrigHybrid',
                    'forceMV2CalibrationAlias': BTaggingFlags.ForceMV2CalibrationAlias,
                    'MV2CalibAlias': BTaggingFlags.MV2CalibAlias,
                    'defaultvals': default_values,
                    'MVTMvariableNames': MVTM_varNames,
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
    return Analysis__MV2Tag(**options)
