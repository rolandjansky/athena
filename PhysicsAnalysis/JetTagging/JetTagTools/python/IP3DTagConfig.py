# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.SVForIPToolConfig import SVForIPToolCfg
from JetTagTools.IPDetailedTrackGradeFactoryConfig import IPDetailedTrackGradeFactoryCfg
from JetTagTools.IPTrackSelectorConfig import IPTrackSelectorCfg
from JetTagTools.NewLikelihoodToolConfig import NewLikelihoodToolCfg
from JetTagTools.InDetTrackSelectorConfig import InDetTrackSelectorCfg
from JetTagTools.SpecialTrackAssociatorConfig import SpecialTrackAssociatorCfg

# import the IPTag configurable
from JetTagTools.JetTagToolsConf import Analysis__IPTag

def IP3DTagCfg( flags, name = 'IP3DTag', scheme = '', useBTagFlagsDefaults = True, **options ):
    """Sets up a IP3DTag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "3D"
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
    acc = ComponentAccumulator()
    options['name'] = name
    options['xAODBaseName'] = 'IP3D'
    options['trackAssociationName'] = 'BTagTrackToJetAssociator'

    if (scheme == ""): 
        if useBTagFlagsDefaults:
            grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]
            trackToVertexIPEstimator = acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(flags, 'TrkToVxIPEstimator'))
            svForIPTool = acc.popToolsAndMerge(SVForIPToolCfg('SVForIPTool'))
            trackGradeFactory = acc.popToolsAndMerge(IPDetailedTrackGradeFactoryCfg('IP3DDetailedTrackGradeFactory'))
            trackSelectorTool = acc.popToolsAndMerge(IPTrackSelectorCfg(flags, 'IP3DTrackSelector'))
            likelihood = acc.popToolsAndMerge(NewLikelihoodToolCfg('IP3DNewLikelihoodTool', 'IP3D'))
            inDetTrackSelectionTool = acc.popToolsAndMerge(InDetTrackSelectorCfg('InDetTrackSelector'))
            trackVertexAssociationTool = acc.popToolsAndMerge(SpecialTrackAssociatorCfg('SpecialTrackAssociator'))

            defaults = { 'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'impactParameterView'              : '3D',
                     'trackGradePartitions'             : grades,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'unbiasIPEstimation'               : False,
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'storeTrackParticles': True,
                     'storeTrackParameters': True,
                     'storeIpValues': True,
                     'LikelihoodTool'                   : likelihood,
                     'trackSelectorTool'                : trackSelectorTool,
                     'SVForIPTool'                      : svForIPTool,
                     'trackGradeFactory'                : trackGradeFactory,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator,
                     'InDetTrackSelectionTool'          : inDetTrackSelectionTool,
                     'TrackVertexAssociationTool'       : trackVertexAssociationTool,
                     }
            for option in defaults:
                options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__IPTag( **options))
   
    return acc
