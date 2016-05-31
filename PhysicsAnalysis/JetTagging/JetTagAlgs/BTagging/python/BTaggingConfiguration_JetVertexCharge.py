# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetVertexCharge
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetVertexCharge = { 'IsATagger'           : True,
                       'NeedsMuonAssociator'  : True,
                       'DependsOn'            : ['AtlasExtrapolator',
                                                 'BTagTrackToVertexTool',   #LC FIXME  check if it works  
                                                 'NewJetFitterVxFinder',
                                                 'BTagCalibrationBrokerTool',
                                                 'MuonCorrectionsTool',
                                                 'MuonSelectorTool',
                                                 ],
                        'CalibrationFolders'  : ['JetVertexCharge',], 
                        'PassByPointer'       : {'calibrationTool'    : 'BTagCalibrationBrokerTool',
                                                 'muonCorrectionTool' : 'MuonCorrectionsTool' ,
                                                 'muonSelectorTool'   : 'MuonSelectorTool' },
                        'ToolCollection'      : 'JetVertexCharge' }

def toolJetVertexCharge(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetVertexCharge tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerNameBase                      default: "JetVertexCharge"
    useForcedCalibration                default: False 
    kFactor                             default: 1.1
    kFactorSV                           default: 0.7
    kFactorTV                           default: 0.7
    Trkd0Cut                            default: 3.5
    Trkz0Cut                            default: 4.5
    TrkPtCut                            default: 500.0
    TrkChi2Cut                          default: 5.0
    CutPrecisionHits                    default: 9
    CutPixelHits                        default: 1
    CutTRTHits                          default: 9
    CutIBLHits                          default: 0
    CutSCTHits                          default: 4
    CutSharedHits                       default: 2
    MuonQuality                         default: xAOD::Muon::Medium


    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerNameBase'                   : 'JetVertexCharge',
                     'useForcedCalibration'             : False,
                     'kFactor'                          : 1.1,
                     'kFactorSV'                        : 0.7,
                     'kFactorTV'                        : 0.7,
                     'Trkd0Cut'                         : 3.5,
                     'Trkz0Cut'                         : 4.5,
                     'TrkPtCut'                         : 500.0,
                     'TrkChi2Cut'                       : 5.0,
                     'CutPrecisionHits'                 : 9,
                     'CutPixelHits'                     : 1,
                     'CutTRTHits'                       : 9,
                     'CutIBLHits'                       : 0,
                     'CutSCTHits'                       : 4,
                     'CutSharedHits'                    : 2,
                     'MuonQuality'                      : 2,
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetVertexCharge
    return Analysis__JetVertexCharge(**options)



metaMuonCorrectionsTool = {  'ToolCollection' : 'JetVertexCharge' }

def toolMuonCorrectionsTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up the CP MuonCorrection tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel"""
    
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool 
    return CP__MuonCalibrationAndSmearingTool(**options)





metaMuonSelectorTool = {  'ToolCollection' : 'JetVertexCharge' }

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
