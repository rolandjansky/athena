# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SoftMuonTagChi2
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSoftMuonTagChi2 = { 'IsATagger'            : True,
                        'NeedsMuonAssociator'  : True,
                        'DependsOn'            : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'BTagFullLinearizedTrackFactory',
                                                  'BTagTrackToVertexIPEstimator',
                                                  'SoftMuonTagChi2NewLikelihoodTool'],
                        'PassByPointer'        : {'LikelihoodTool'    : 'SoftMuonTagChi2NewLikelihoodTool',
                                                  'TrackToVertexTool' : 'BTagTrackToVertexTool'},
                        'JetCollectionList'    : 'jetCollectionList',
                        'PassMuonCollectionAs' : 'MuonAssociationName',
                        'ToolCollection'       : 'SoftMuonTagChi2' }

def toolSoftMuonTagChi2(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftMuonTagChi2 tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    originalMuCollectionName            default: BTaggingFlags.MuonCollectionName
    MuonAssociationName                 default: "Muons"
    TaggingAlgType                      default: "CHI2" (this is what switches the chi2 tagger ON)
    RecAlgorithm                        default: 0 (use only combined muons)
    writeInfoPlus                       default: 1 (turn InfoPlus off because the L1D tagger takes care of it (albeit cuts on the chi2...) --> # Turn
                                                   on (for rel 17.2))
    CutMatchChi2                        default: 10000 (no match-chi2 cut)

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'originalMuCollectionName'         : BTaggingFlags.MuonCollectionName,
                     'MuonAssociationName'              : 'Muons',
                     'TaggingAlgType'                   : 'CHI2',
                     'RecAlgorithm'                     : 0,
                     'writeInfoPlus'                    : 1,
                     'CutMatchChi2'                     : 10000 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SoftMuonTag
    return Analysis__SoftMuonTag(**options)

#--------------------------------------------------------------------------

metaSoftMuonTagChi2NewLikelihoodTool = { 'CalibrationFolders' : ['SoftMu',],
                                         'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                         'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                         'ToolCollection'     : 'SoftMuonTagChi2' }

def toolSoftMuonTagChi2NewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftMuonTagChi2NewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SoftMuChi2"
    smoothNTimes                        default: 0
    normalizedProb                      default: True
    interpolate                         default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'SoftMuChi2',
                     'smoothNTimes'                        : 0,
                     'normalizedProb'                      : True,
                     'interpolate'                         : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
