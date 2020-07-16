# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from JetTagTools.MuonSelectorToolConfig import MuonSelectorToolCfg
from JetTagTools.MuonCorrectionsToolConfig import MuonCorrectionsToolCfg

# import the JetVertexCharger configurable
from JetTagTools.JetTagToolsConf import Analysis__JetVertexCharge

def JetVertexChargeCfg(flags, name = 'JetVertexCharge', scheme = '', useBTagFlagsDefaults = True, **options ):

    """Sets up a JetVertexCharge tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTagging.RunModus
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
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        muonSelectorTool = acc.popToolsAndMerge(MuonSelectorToolCfg('MuonSelectorTool'))
        muonCorrectionsTool = acc.popToolsAndMerge(MuonCorrectionsToolCfg('MuonCorrectionsTool'))
        defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
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
                     'muonCorrectionTool'               : muonCorrectionsTool,
                     'muonSelectorTool'                 : muonSelectorTool,
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    if scheme == 'Trig':
        options['HistosKey'] = 'JetTagTrigCalibHistosKey'
    acc.setPrivateTools(Analysis__JetVertexCharge(**options))
    return acc

