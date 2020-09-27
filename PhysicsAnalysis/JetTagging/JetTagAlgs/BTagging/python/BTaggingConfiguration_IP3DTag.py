# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# Configuration functions for IP3DTag
# Author: Wouter van den Wollenberg (2013-2014)
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


metaIP3DTag = { 'IsATagger'         : True,
                'xAODBaseName'      : 'IP3D',
                'DependsOn'         : ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'InDetVKalVxInJetTool',
                                       'BTagFullLinearizedTrackFactory',
                                       'BTagTrackToVertexIPEstimator',
                                       'IP3DNewLikelihoodTool',
                                       'IP3DTrackSelector',
                                       'InDetTrackSelector',
                                       'SpecialTrackAssociator',
                                       'SVForIPTool_IP3D',
                                       'IP3DBasicTrackGradeFactory',
                                       'IP3DDetailedTrackGradeFactory'],
                'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3D',
                                       'trackSelectorTool'          : 'IP3DTrackSelector',
                                       'InDetTrackSelectionTool'    : 'InDetTrackSelector',
                                       'TrackVertexAssociationTool' : 'SpecialTrackAssociator',
                                       'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory',
#                                       'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                       'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                       'LikelihoodTool'             : 'IP3DNewLikelihoodTool'},
#                'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
#                                       'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                'PassTracksAs'      : 'trackAssociationName',
                'JetCollectionList' : 'jetCollectionList',
                'ToolCollection'    : 'IP3DTag' }

def toolIP3DTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "3D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    UseCHypo                            default: True
    SecVxFinderName                     default: "SV1"
    jetPtMinRef                         default: BTaggingFlags.JetPtMinRef

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
        if btagrun1: grades=[ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
        if btagItk: 
            if btagItk_version==5: grades=["A01","A05","A06","A07","A08","A0910","A14A","A14B","B01","B05","B0910","B11","B14","zone_1","zone_2","zone_3","zone_4"]
            elif btagItk_version==7: grades=["A01","A02","A03","A04","A05","A06","A07","A08","A14_1","A14_2","A14_3","A14_4",
                                             "B01","B02","B03","B04","B05","B06","B07","B08","B14_1","B14_2","B14_3","B14_4",
                                             "C01","C020304","C05","C06","C07","C0809","C14_1","C14_2","C14_3","C14_4"]

        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'impactParameterView'              : '3D',
                     'trackGradePartitions'             : grades ,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
#                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
#                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                     'unbiasIPEstimation'               : False,
                     #'trackAssociation'                 : "Tracks" }
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'storeTrackParameters': True,
                     'storeTrackParticles': True,
                     'storeIpValues': True
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)


#------------------------------------------------------------------

metaIP3DDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DTag' }

def toolIP3DDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    hitBLayerGrade                      default: True
    useRun2TrackGrading                 default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
		     'useRun2TrackGrading'    : (btagrun1 == False and not(btagItk)),
                     'useITkTrackGrading'     : (btagItk == True),
                     'ITkTrkGradeVersion'     :  btagItk_version,
                     'useInnerLayers0HitInfo' : (btagrun1 == False),
                     'useDetailSplitHitInfo'  : (btagrun1 == False),
                     'hitBLayerGrade'         : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#------------------------------------------------------------------

metaIP3DBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DTag' }

def toolIP3DBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#------------------------------------------------------------------

metaSVForIPTool_IP3D = { 'ToolCollection' : 'IP3DTag' }

def toolSVForIPTool_IP3D(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3D tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
    return Analysis__SVForIPTool(**options)

#------------------------------------------------------------------

metaIP3DTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                          'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                          'ToolCollection' : 'IP3DTag' }

def toolIP3DTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useBLayerHitPrediction              default: True
    usepTDepTrackSel                    default: False
    nHitBLayer                          default: 0

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useBLayerHitPrediction' : True ,
                     'nHitBLayer'             : 0 ,
                     'usepTDepTrackSel'       : False }
        for option in defaults:
            options.setdefault(option, defaults[option])

        if(commonGeoFlags.Run()=="RUN4"):
            from BTagging.BTaggingConfiguration_InDetEtaDependentCutsSvc import IDEtaDepCutsSvc_IPXD
            InDetEtaDependentCutsSvc = IDEtaDepCutsSvc_IPXD( "IDEtaDepCutsSvc_" + name )
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr += InDetEtaDependentCutsSvc
            options['InDetEtaDependentCutsSvc'] = InDetEtaDependentCutsSvc

    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#------------------------------------------------------------------

metaInDetTrackSelector = { 'ToolCollection' : 'IP3DTag' }
 
def toolInDetTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'CutLevel'               : "Loose",
                     'maxZ0SinTheta'          : 3 }
#                     'maxD0'                  : 2 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    return InDet__InDetTrackSelectionTool(**options)

#------------------------------------------------------------------

metaSpecialTrackAssociator = { 'ToolCollection' : 'IP3DTag' }
 
def toolSpecialTrackAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:
    OutputLevel                         default: BTaggingFlags.OutputLevel
    
    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'dzSinTheta_cut'         : 3,
                     'doPV'                   : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    
    from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool
    return CP__TightTrackVertexAssociationTool(**options)

#------------------------------------------------------------------

metaIP3DNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                              'DependsOn'          : ['BTagCalibrationBrokerTool',],
                              'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                              'ToolCollection'     : 'IP3DTag' }

def toolIP3DNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP3D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'IP3D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
