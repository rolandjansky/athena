# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for ExKtbbTag
# Author: Qi Zeng (2015)

from BTagging.BTaggingFlags import BTaggingFlags

metaExKtbb_Hbb_MV2OnlyTag = { 'IsATagger'         : True,
                              'xAODBaseName'      : 'ExKtbb',
                              'DependsOn'         : ['MV2c20Tag',
                                                     'BTagCalibrationBrokerTool'],
                              'CalibrationFolders' :['ExKtbb',],
                              'PassByPointer'     : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                              'ToolCollection'    : 'ExKtbbTag',
                            }

metaExKtbb_Hbb_MV2andJFDRSigTag = { 'IsATagger'         : True,
                              'xAODBaseName'      : 'ExKtbb',
                              'DependsOn'         : ['MV2c20Tag',
                                                     'BTagCalibrationBrokerTool'],
                              'CalibrationFolders' :['ExKtbb',],
                              'PassByPointer'     : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                              'ToolCollection'    : 'ExKtbbTag',
                               }

metaExKtbb_Hbb_MV2andToposTag = { 'IsATagger'         : True,
                              'xAODBaseName'      : 'ExKtbb',
                              'DependsOn'         : ['MV2c20Tag',
                                                     'BTagCalibrationBrokerTool'],
                              'CalibrationFolders' :['ExKtbb',],
                              'PassByPointer'     : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                              'ToolCollection'    : 'ExKtbbTag',
                            }

def toolExKtbb_Hbb_MV2OnlyTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ExKtbbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    see below

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'tagMode'                          : 'Hbb',
                     'BDTMode'                          : 'MV2Only',
                     'taggerNameBase'                   : 'ExKtbb',
                     'ForceCalibrationChannel'          : 'Default',
                     'debug'                            : False,

                     'SubJetLabel'                      : 'ExKt2SubJets',
                     'JFOnlyVtx'                        : False,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__ExKtbbTag
    return Analysis__ExKtbbTag(**options)

def toolExKtbb_Hbb_MV2andJFDRSigTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ExKtbbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    see below

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'tagMode'                          : 'Hbb',
                     'BDTMode'                          : 'MV2andJFDRSig',
                     'taggerNameBase'                   : 'ExKtbb',
                     'ForceCalibrationChannel'          : 'Default',
                     'debug'                            : False,

                     'SubJetLabel'                      : 'ExKt2SubJets',
                     'JFOnlyVtx'                        : False,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__ExKtbbTag
    return Analysis__ExKtbbTag(**options)

def toolExKtbb_Hbb_MV2andToposTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ExKtbbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    see below

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'tagMode'                          : 'Hbb',
                     'BDTMode'                          : 'MV2andTopos',
                     'taggerNameBase'                   : 'ExKtbb',
                     'ForceCalibrationChannel'          : 'Default',
                     'debug'                            : False,

                     'SubJetLabel'                      : 'ExKt2SubJets',
                     'JFOnlyVtx'                        : False,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__ExKtbbTag
    return Analysis__ExKtbbTag(**options)

