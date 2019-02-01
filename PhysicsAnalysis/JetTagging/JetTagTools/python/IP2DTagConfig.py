# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.SVForIPToolConfig import SVForIPToolCfg
from JetTagTools.IPDetailedTrackGradeFactoryConfig import IPDetailedTrackGradeFactoryCfg
from JetTagTools.IPTrackSelectorConfig import IPTrackSelectorCfg
from JetTagTools.IPNewLikelihoodToolConfig import IPNewLikelihoodToolCfg

# import the IPTag configurable
from JetTagTools.JetTagToolsConf import Analysis__IPTag

# define the class
class IP2DTagCfg( Analysis__IPTag ):
    # constructor
    def __init__(self, name = 'IP2DTag', scheme = '', useBTagFlagsDefaults = True, options = {}):

        """Sets up a IP2DTag tool and returns it.

        The following options have BTaggingFlags defaults:

        Runmodus                            default: BTaggingFlags.Runmodus
        referenceType                       default: BTaggingFlags.ReferenceType
        impactParameterView                 default: "2D"
        trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
        RejectBadTracks                     default: False
        originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
        jetCollectionList                   default: BTaggingFlags.Jets
        unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
        SecVxFinderName                     default: "SV1"
        UseCHypo                            default: True

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        options['name'] = name
        options['xAODBaseName'] = 'IP2D'
        options['trackAssociationName'] = 'BTagTrackToJetAssociator'

        if (scheme == ""): 
            if useBTagFlagsDefaults:
                grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]
                trackToVertexIPEstimator = BTagTrackToVertexIPEstimatorCfg('TrkToVxIPEstimator')
                svForIPTool = SVForIPToolCfg('SVForIPTool')
                trackGradeFactory = IPDetailedTrackGradeFactoryCfg('IP2DDetailedTrackGradeFactory')
                trackSelectorTool = IPTrackSelectorCfg('IP2DTrackSelector')
                likelihood = IPNewLikelihoodToolCfg('IP2DNewLikelihoodTool', 'IP2D')

                defaults = { 'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : grades,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'unbiasIPEstimation'               : False,
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'storeTrackParticles': True,
                     'storeTrackParameters': True,
                     'storeIpValues': False,
                     'LikelihoodTool'                   : likelihood,
                     'trackSelectorTool'                : trackSelectorTool,
                     'SVForIPTool'                      : svForIPTool,
                     'trackGradeFactory'                : trackGradeFactory,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator,
                     }
                for option in defaults:
                    options.setdefault(option, defaults[option])

        Analysis__IPTag.__init__(self, **options)
