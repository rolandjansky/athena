# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.SVForIPToolConfig import SVForIPToolCfg
from JetTagTools.IPDetailedTrackGradeFactoryConfig import IPDetailedTrackGradeFactoryCfg
from JetTagTools.IPTrackSelectorConfig import IPTrackSelectorCfg
from JetTagTools.IPNewLikelihoodToolConfig import IPNewLikelihoodToolCfg

# import the IPTag configurable
from JetTagTools.JetTagToolsConf import Analysis__RNNIPTag

# define the class
class RNNIPTagCfg( Analysis__RNNIPTag ):
    # constructor
    def __init__(self, name = 'RNNIP', scheme = '', calibration=None, useBTagFlagsDefaults = True, options = {}):
        """Sets up a RNNIPTag tool and returns it.

        The following options have BTaggingFlags defaults:

        trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
        RejectBadTracks                     default: False
        originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
        jetCollectionList                   default: BTaggingFlags.Jets
        SecVxFinderName                     default: "SV1"

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        
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
            trackToVertexIPEstimator = BTagTrackToVertexIPEstimatorCfg('TrkToVxIPEstimator')
            svForIPTool = SVForIPToolCfg('SVForIPTool')
            trackGradeFactory = IPDetailedTrackGradeFactoryCfg('RNNIPDetailedTrackGradeFactory')
            trackSelectorTool = IPTrackSelectorCfg('RNNIPTrackSelector')

            defaults = {
                'OutputLevel'               : BTaggingFlags.OutputLevel,
                'trackGradePartitions'      : grades ,
                'RejectBadTracks'           : True,
                'originalTPCollectionName'  : BTaggingFlags.TrackParticleCollectionName,
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

        Analysis__RNNIPTag.__init__(self, **options)
