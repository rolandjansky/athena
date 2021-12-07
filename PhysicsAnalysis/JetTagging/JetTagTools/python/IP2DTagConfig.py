# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from JetTagTools.SVForIPToolConfig import SVForIPToolCfg
from JetTagTools.IPDetailedTrackGradeFactoryConfig import IPDetailedTrackGradeFactoryCfg
from JetTagTools.IPTrackSelectorConfig import IPTrackSelectorCfg
from JetTagTools.NewLikelihoodToolConfig import NewLikelihoodToolCfg

# import the IPTag configurable
Analysis__IPTag=CompFactory.Analysis.IPTag

def IP2DTagCfg( flags, name = 'IP2DTag', scheme = '', useBTagFlagsDefaults = True, FlipOption='STANDARD', **options ):
    """Sets up a IP2DTag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTagging.RunModus
    referenceType                       default: BTagging.ReferenceType
    impactParameterView                 default: "2D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
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
    options['xAODBaseName'] = 'IP2D'
    options['trackAssociationName'] = 'BTagTrackToJetAssociator'

    if useBTagFlagsDefaults:
        trackToVertexIPEstimator = acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(flags, 'TrkToVxIPEstimator'))
        svForIPTool = acc.popToolsAndMerge(SVForIPToolCfg('SVForIPTool'))
        trackGradeFactory = acc.popToolsAndMerge(IPDetailedTrackGradeFactoryCfg(flags, 'IP2DDetailedTrackGradeFactory'))
        trackSelectorTool = acc.popToolsAndMerge(IPTrackSelectorCfg(flags, 'IP2DTrackSelector'))
        likelihood = acc.popToolsAndMerge(NewLikelihoodToolCfg(flags, 'IP2DNewLikelihoodTool', 'IP2D', scheme))

        defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'referenceType'                    : flags.BTagging.ReferenceType,
                     'jetPtMinRef'                      : flags.BTagging.JetPtMinRef,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : flags.BTagging.Grades,
                     'RejectBadTracks'                  : True,
                     'jetCollectionList'                : [], #used only in reference mode
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

    # Define the settings for the IP2DNeg ("NEGATIVE_IP_ONLY") and IP2DFlip ("FLIP_SIGN") taggers. 
    # Set as well a different name to distinguish from the "Standard" taggers    
 
    if FlipOption=='NEGATIVE_IP_ONLY':
        negtagoptions = {
            'flipIPSign'                       : True,
            'flipZIPSign'                      : True,
            'usePosIP'                         : True,
            'useNegIP'                         : False,

        }
        # make sure that the name includes 'Neg' if setup 'Neg' tagging option
        nameNeg=''
        if 'NegTag' in name:
            nameNeg=name
        elif 'Tag' in name:
            nameNeg=name.replace("Tag","NegTag")
        else:
            nameNeg=name+"Neg"
        options['name'] = nameNeg
        options['xAODBaseName'] = 'IP2DNeg' 
        for option in negtagoptions:
            options.setdefault(option, negtagoptions[option])
        
    elif FlipOption=='FLIP_SIGN':
        flipoptions = {
                'flipIPSign'                   : True,
                'flipZIPSign'                  : True, #would naively say 'True' but in Rel.21 (https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/JetTagging/JetTagAlgs/BTagging/python/BTaggingConfiguration_IP3DFlipTag.py), the default from 'False' is not changed
                'usePosIP'                     : True,
                'useNegIP'                     : True	    
        }
        nameFlip=''
        if 'FlipTag' in name:
            nameFlip=name
        elif 'Tag' in name:
            nameFlip=name.replace("Tag","FlipTag")
        else:
            nameFlip=name+"Flip"
        options['name'] = nameFlip
        options['xAODBaseName'] = 'IP2DFlip' 
        for option in flipoptions:
            options.setdefault(option, flipoptions[option])    



    acc.setPrivateTools(Analysis__IPTag( **options))

    return acc
