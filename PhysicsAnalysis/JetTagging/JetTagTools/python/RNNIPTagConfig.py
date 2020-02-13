# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.SVForIPToolConfig import SVForIPToolCfg
from JetTagTools.IPDetailedTrackGradeFactoryConfig import IPDetailedTrackGradeFactoryCfg
from JetTagTools.IPTrackSelectorConfig import IPTrackSelectorCfg

# import the IPTag configurable
Analysis__RNNIPTag=CompFactory.Analysis__RNNIPTag

def RNNIPTagCfg( flags, name = 'RNNIP', scheme = '', calibration=None, useBTagFlagsDefaults = True, **options ):
    """Sets up a RNNIPTag tool and returns it.

    The following options have BTaggingFlags defaults:

    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    SecVxFinderName                     default: "SV1"

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""

    acc = ComponentAccumulator()   
    options['name'] = name
    basename = 'RNNIP'
    options['xAODBaseName'] = basename
    options['trackAssociationName'] = 'BTagTrackToJetAssociator'
    cal_dir = calibration or basename
    is_flipped=False
    if (scheme == "Flip"):
        is_flipped=True

    if useBTagFlagsDefaults:
        grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]
        trackToVertexIPEstimator = acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(flags, 'TrkToVxIPEstimator'))
        svForIPTool = acc.popToolsAndMerge(SVForIPToolCfg('SVForIPTool'))
        trackGradeFactory = acc.popToolsAndMerge(IPDetailedTrackGradeFactoryCfg('RNNIPDetailedTrackGradeFactory'))
        trackSelectorTool = acc.popToolsAndMerge(IPTrackSelectorCfg(flags, 'RNNIPTrackSelector'))

        defaults = {
                'trackGradePartitions'      : grades ,
                'RejectBadTracks'           : True,
                'NetworkConfig'             : BTaggingFlags.RNNIPConfig,
                'unbiasIPEstimation'        : False,
                'SecVxFinderName'           : 'SV1',
                'calibration_directory'     : cal_dir,
                'writeInputsToBtagObject'   : BTaggingFlags.WriteRNNInputs,
                'trackSelectorTool'         : trackSelectorTool,
                'SVForIPTool'               : svForIPTool,
                'trackGradeFactory'         : trackGradeFactory,
                'TrackToVertexIPEstimator'  : trackToVertexIPEstimator,
            }
        if is_flipped:
            defaults.update({
                    'flipIPSign' : True,
                    'usePosIP'   : True,
                    'useNegIP'   : False,
                })

        for option in defaults:
            options.setdefault(option, defaults[option])

    acc.setPrivateTools(Analysis__RNNIPTag( **options))

    return acc
