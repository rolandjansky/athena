# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration for RNNIP
#
# Note that a lot of tools and settings are borrowed from IP3D, so
# this probably won't work without IP3D also turned on.  We'll need an
# expter to figure out how to make taggers independant.
from BTagging.BTaggingFlags import BTaggingFlags

def buildRNNIP(basename, is_flipped=False, calibration=None):
    cal_dir = calibration or basename
    meta = {'IsATagger'         : True,
            'xAODBaseName'      : basename,
            'CalibrationFolders' : [cal_dir],
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
            'PassByPointer'     : {
                'SVForIPTool'                : 'SVForIPTool_IP3D',
                'trackSelectorTool'          : 'IP3DTrackSelector',
                'calibrationTool'            : 'BTagCalibrationBrokerTool',
                'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory',
                'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator'},
            'PassTracksAs'      : 'trackAssociationName',
            'ToolCollection'    : 'IP3DTag' }

    def tool(name, useBTagFlagsDefaults = True, **options):
        """Sets up a IP3DTag tool and returns it.

        The following options have BTaggingFlags defaults:

        """
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
                'calibration_directory'     : cal_dir,
                'writeInputsToBtagObject': BTaggingFlags.WriteRNNInputs
            }
            if is_flipped:
                defaults.update({
                    'flipIPSign' : True,
                    'usePosIP'   : True,
                    'useNegIP'   : False,
                })
            for option in defaults:
                options.setdefault(option, defaults[option])
        options['name'] = name
        from JetTagTools.JetTagToolsConf import Analysis__RNNIPTag
        return Analysis__RNNIPTag(**options)

    return tool, meta


toolRNNIPTag, metaRNNIPTag = buildRNNIP('RNNIP')
toolRNNIPNegTag, metaRNNIPNegTag = buildRNNIP(
    'RNNIPNegTag', is_flipped=True, calibration='RNNIP')
