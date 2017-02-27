# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration for RNNIP
#
# Note that a lot of tools and settings are borrowed from IP3D, so
# this probably won't work without IP3D also turned on.  We'll need an
# expter to figure out how to make taggers independant.
from BTagging.BTaggingFlags import BTaggingFlags

metaRNNIPTag = {'IsATagger'         : True,
                'xAODBaseName'      : 'RNNIP',
                'CalibrationFolders' : ['RNNIP'],
                'DependsOn'         : ['AtlasExtrapolator',
                                       'BTagCalibrationBrokerTool',
                                       'BTagTrackToVertexTool',
                                       'InDetVKalVxInJetTool',
                                       'BTagTrackToVertexIPEstimator',
                                       'IP3DTrackSelector',
                                       'InDetTrackSelector',
                                       'SpecialTrackAssociator',
                                       'SVForIPTool_IP3D',
                                       'IP3DBasicTrackGradeFactory',
                                       'IP3DDetailedTrackGradeFactory'],
                'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3D',
                                       'trackSelectorTool'          : 'IP3DTrackSelector',
                                       'calibrationTool'            : 'BTagCalibrationBrokerTool',
                                       'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory',
                                       'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator'},
                'PassTracksAs'      : 'trackAssociationName',
                'ToolCollection'    : 'IP3DTag' }

def toolRNNIPTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: True
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    SecVxFinderName                     default: "SV1"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]

        defaults = {
            'OutputLevel'               : BTaggingFlags.OutputLevel,
            'trackGradePartitions'      : grades ,
            'RejectBadTracks'           : True,
            'originalTPCollectionName'  : BTaggingFlags.TrackParticleCollectionName,
            'NetworkConfig'             : BTaggingFlags.RNNIPConfig,
            'unbiasIPEstimation'        : False,
            #'trackAssociation'          : "Tracks" }
            'SecVxFinderName'           : 'SV1',
            'calibration_directory'     : 'RNNIP',
            'writeInputsToBtagObject': BTaggingFlags.WriteRNNInputs
        }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__RNNIPTag
    return Analysis__RNNIPTag(**options)

