# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SoftMuonTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSoftMuonTag = { 'IsATagger'            : True,
                    'NeedsMuonAssociator'  : True,
                    'DependsOn'            : [#'AtlasExtrapolator',
                                              #'BTagTrackToVertexTool',
                                              #'BTagFullLinearizedTrackFactory',
                                              #'BTagTrackToVertexIPEstimator',
                                              #'SoftMuonTagNewLikelihoodTool',
                                              # 'MuonSelectorTool'],
                                             ],
                    'PassByPointer'        : {#'LikelihoodTool'    : 'SoftMuonTagNewLikelihoodTool',
                                              #'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                              #'muonSelectorTool': 'MuonSelectorTool',
                        },
#                                              'TrackToVertexTool' : 'BTagTrackToVertexTool'},
                    'JetCollectionList'    : 'jetCollectionList',
                    'ToolCollection'       : 'SoftMuonTag' }

def toolSoftMuonTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftMuonTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    originalMuCollectionName            default: BTaggingFlags.MuonCollectionName
    BTagJetEtamin                       default: 2.5 (only if BTaggingFlags.Runmodus == 'reference')
    MuonQuality                         default: xAOD::Muon::Medium

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        from BTagging.BTaggingConfiguration_CommonTools import toolBTagTrackToVertexIPEstimator as toolBTagTrackToVertexIPEstimator
        trackToVertexIPEstimator = toolBTagTrackToVertexIPEstimator('TrkToVxIPEstimator')
        #from BTagging.BTaggingConfiguration_CommonTools import toolBTagTrackToVertexTool as toolBTagTrackToVertexTool
        #trackToVertexTool = toolBTagTrackToVertexTool('BTagTrackToVertexTool')
        muonSelectorTool = toolMuonSelectorTool('MuonSelectorTool')
        likelihood = toolSoftMuonTagNewLikelihoodTool('SoftMuonTagNewLikelihoodTool')
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'originalMuCollectionName'         : BTaggingFlags.MuonCollectionName,
                     'MuonQuality'                      : 2,
                     'muonSelectorTool'                 : muonSelectorTool,
                     'LikelihoodTool'                   : likelihood,
                     #'TrackToVertexTool'                : trackToVertexTool,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator, }
        if(BTaggingFlags.Runmodus == 'reference'):
            defaults['BTagJetEtamin'] = 2.5
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SoftMuonTag
    return Analysis__SoftMuonTag(**options)

#----------------------------------------------------------------------

metaSoftMuonTagNewLikelihoodTool = { 'CalibrationTaggers' : ['SMT',],
                                     'ToolCollection'     : 'SoftMuonTag' }

def toolSoftMuonTagNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftMuonTagNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SMT"
    smoothNTimes                        default: 0 (1 if BTaggingFlags.Runmodus == 'reference')
    normalizedProb                      default: True
    interpolate                         default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'SMT',
                     'smoothNTimes'                        : 0,
                     'normalizedProb'                      : True,
                     'interpolate'                         : True }
        if(BTaggingFlags.Runmodus == 'reference'):
            defaults['smoothNTimes'] = 1
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)


metaMuonSelectorTool = {  'ToolCollection' : 'SoftMuonTag' }

def toolMuonSelectorTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up the CP MuonSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    MaxEta                              default: 2.7"""
    
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'MaxEta'                 : 2.7 ,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool 
    return CP__MuonSelectionTool(**options)
