# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration for RNNIP
#
# Note that a lot of tools and settings are borrowed from IP3D, so
# this probably won't work without IP3D also turned on.  We'll need an
# expter to figure out how to make taggers independant.
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
from IOVDbSvc.CondDB import conddb

btagrun1=False
btagItk=False
btagItk_version=5

if conddb.dbdata == 'COMP200':
    btagrun1=True
elif conddb.isMC:
    btagrun1 = (commonGeoFlags.Run() == "RUN1" or (commonGeoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False))
    if(commonGeoFlags.Run()=="RUN4"):
       btagItk=True

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
            if btagrun1: grades=[ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
            if btagItk: 
                if btagItk_version==5: grades=["A01","A05","A06","A07","A08","A0910","A14A","A14B","B01","B05","B0910","B11","B14","zone_1","zone_2","zone_3","zone_4"]
                elif btagItk_version==7: grades=["A01","A02","A03","A04","A05","A06","A07","A08","A14_1","A14_2","A14_3","A14_4",
				                                             "B01","B02","B03","B04","B05","B06","B07","B08","B14_1","B14_2","B14_3","B14_4",
									                                                  "C01","C020304","C05","C06","C07","C0809","C14_1","C14_2","C14_3","C14_4"]


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
