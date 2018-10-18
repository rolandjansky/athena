# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags

#################################################################################
# Material for the Geometry
#
# if only ID is switched on different COOLDB Folders
#    and different Tag is used
#
#################################################################################

def initialiseCoolDataBaseFolder():
    if TrkDetFlags.ConfigurationOutputLevel() < 3 :
      print '[ Configuration : start ] *** FatrasTrackingGeometry ********************************'
      print '[ TrackingGeometrySvc ]'

    # check whether the material retrieval is ment to be from COOL
    if TrkDetFlags.MaterialSource() is 'COOL' :
        # the tag names
        CoolDataBaseFolder = TrkDetFlags.MaterialStoreGateKey()
        AtlasMaterialTag = TrkDetFlags.MaterialTagBase()+str(TrkDetFlags.MaterialVersion())+'_'

        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
           print '[ TrackingGeometrySvc ] Associating DB folder : ',CoolDataBaseFolder
           print '[ TrackingGeometrySvc ]     base material tag : ',AtlasMaterialTag

        # we need the conditions interface
        from IOVDbSvc.CondDB import conddb
        # use a local database
        if TrkDetFlags.MaterialDatabaseLocal():
            # specify the local database
            DataBasePath  = TrkDetFlags.MaterialDatabaseLocalPath()
            DataBaseName  = TrkDetFlags.MaterialDatabaseLocalName()
            MagicTag      = TrkDetFlags.MaterialMagicTag()
            DataBaseConnection = '<dbConnection>sqlite://X;schema='+DataBasePath+DataBaseName+';dbname=OFLP200</dbConnection>'
            conddb.blockFolder('/GLOBAL/TrackingGeo/LayerMaterial')
            conddb.addFolderWithTag('',DataBaseConnection+CoolDataBaseFolder,AtlasMaterialTag+MagicTag,force=True)
            if TrkDetFlags.ConfigurationOutputLevel() < 3 :
                print '[ TrackingGeometrySvc ] Using Local Database: '+DataBaseConnection
            # make sure that the pool files are in the catalog
            #from PoolSvc.PoolSvcConf import PoolSvc
            #PoolSvc.ReadCatalog += [ DataBasePath+'PoolFileCatalog.xml' ]
        elif TrkDetFlags.SLHC_Geometry() :
            # set the folder to the SLHC location
            CoolDataBaseFolder = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
            ctag = AtlasMaterialTag+TrkDetFlags.MaterialMagicTag()
            cfoldertag = CoolDataBaseFolder+' <tag>'+ctag+'</tag>'
            conddb.addFolderSplitMC('GLOBAL',cfoldertag,cfoldertag)
        else :
            # load the right folders (preparation for calo inclusion)
            cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
            conddb.addFolderSplitMC('GLOBAL',cfolder,cfolder)

    #HACK: CoolDataBaseFolder may not be set at this point! Is this right? -KG
    return CoolDataBaseFolder


#################################################################################
# The Geometry Builder
#################################################################################
def getInDetTrackingGeometryBuilder(name="ISF_InDetTrackingGeometryBuilder", **kwargs):
    # get hand on the ID Tracking Geometry Builder
    kwargs.setdefault("namePrefix"              , 'Fatras')
    kwargs.setdefault("setLayerAssociation"     , False)
    #kwargs.setdefault("VolumeEnclosureOuterR"   , 1148.) ### HACK: Cannot set via imput arguments. Is this right?? -kg
    if TrkDetFlags.ISF_FatrasCustomGeometry() :
        from ISF_FatrasDetDescrTools.CustomInDetTrackingGeometryBuilder import CustomInDetTrackingGeometryBuilder as IDGeometryBuilder
    else :
        if not TrkDetFlags.SLHC_Geometry() :
            kwargs.setdefault("buildTrtStrawLayers" , True)
            from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilder import ConfiguredInDetTrackingGeometryBuilder as IDGeometryBuilder
        else :
            from InDetTrackingGeometry.ConfiguredSLHC_InDetTrackingGeometryBuilder import ConfiguredSLHC_InDetTrackingGeometryBuilder as IDGeometryBuilder
    t = IDGeometryBuilder(name, **kwargs )
    t.VolumeEnclosureOuterR = 1148.
    #t.EnvelopeDefinitionSvc = 'ISF_EnvelopeDefSvc'
    return t


def getFatrasCaloTrackingGeometryBuilder(name="ISF_FatrasCaloTrackingGeometryBuilder", **kwargs):
    # get hand on the ID Tracking Geometry Builder
    #kwargs.setdefault("RecordLayerIndexCaloSampleMap"   , True) ### HACK: Cannot set via imput arguments. Is this right?? -kg

    from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilder import ConfiguredCaloTrackingGeometryBuilder as ConfiguredCaloGeo
    t = ConfiguredCaloGeo(name, **kwargs)
    t.RecordLayerIndexCaloSampleMap = True
    #t.EnvelopeDefinitionSvc = 'ISF_EnvelopeDefSvc'
    return t


def getFatrasMuonTrackingGeometryBuilder(name="ISF_FatrasMuonTrackingGeometryBuilder", **kwargs):
    # get hand on the MS Tracking Geometry Builder

    from MuonTrackingGeometry.ConfiguredMuonTrackingGeometryBuilder import ConfiguredMuonTrackingGeometryBuilder as ConfiguredMuonGeo
    t = ConfiguredMuonGeo(name, **kwargs)
    return t


def getFatrasGeometryBuilder(name="ISF_FatrasGeometryBuilder", **kwargs):
    # the geometry builder alg tool
    # switch on the Detectors
    #kwargs.setdefault("InDetTrackingGeometry"   , DetFlags.ID_on())
    #kwargs.setdefault("CaloTrackingGeometry"    , DetFlags.Calo_on())
    #kwargs.setdefault("MuonTrackingGeometry"    , DetFlags.Muon_on())
    if DetFlags.ID_on():
        kwargs.setdefault("InDetTrackingGeometryBuilder",'ISF_InDetTrackingGeometryBuilder')
    if DetFlags.Calo_on():
        kwargs.setdefault("CaloTrackingGeometryBuilder" ,'ISF_FatrasCaloTrackingGeometryBuilder')
    if DetFlags.Muon_on():
        kwargs.setdefault("MuonTrackingGeometryBuilder" ,'ISF_FatrasMuonTrackingGeometryBuilder')

    from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
    return Trk__GeometryBuilder(name, **kwargs )


def getFatrasTrackingGeometrySvc(name="ISF_FatrasTrackingGeometrySvc", **kwargs):
    # get hand on the ID Tracking Geometry Builder
    # register the Builder
    kwargs.setdefault("GeometryBuilder"             , 'ISF_FatrasGeometryBuilder')
    # and register the name under which the geometry is registered
    kwargs.setdefault("TrackingGeometryName"        , 'ISF_FatrasTrackingGeometry')
    # global scaling of the material if necessary
    #kwargs.setdefault("GlobalScaleFactor"           , FatrasTuningFlags.MaterialScalor())
    # some flags - only works when ID is on
    #kwargs.setdefault("AssignMaterialFromCOOL"      , True)
    kwargs.setdefault("BuildGeometryFromTagInfo"    , True)
    # switch to the rigth CoolDB folder
    #kwargs.setdefault("InputLayerMaterialSetName"   , initialiseCoolDataBaseFolder()) ## HACK: This is set during "initialiseCoolDataBaseFolder()"

    # then get the tuning parameters
    # if FatrasTuningFlags.MaterialDistortionsX0.statusOn and FatrasTuningFlags.MaterialDistortionsRho.statusOn:
    #    # Material scaling & distortion
    #    kwargs.setdefault("RandomDistortions"           , True)
    #    kwargs.setdefault("DistortionsRadiationLength"  , FatrasTuningFlags.MaterialDistortionsX0())
    #    kwargs.setdefault("DistortionsDensity"          , FatrasTuningFlags.MaterialDistortionsRho())
    #    kwargs.setdefault("RecordDistortions"           , FatrasValidationFlags.MaterialDistortions())
    #    kwargs.setdefault("DistortionOutputFolder"      , '/'+FatrasValidationFlags.ValidationStream()+'/MaterialDistortions')
    #    from TrkDetDescrSvc.TrkDetDescrSvcConf import CalibrationTrackingGeometrySvc as GeometrySvc
    #else:

    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingGeometrySvc as GeometrySvc
    return GeometrySvc(name, **kwargs )


################################################################################
# TRANSPORT SECTION
#   TODO: should be in the future rationalized to one transport tool
#         needs new ExtrapolationEngine for this
################################################################################
def getFatrasNavigator(name="ISF_FatrasNavigator", **kwargs):
    # the Navigator (needed for several instances)
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    kwargs.setdefault("TrackingGeometrySvc"  , AtlasTrackingGeometrySvc)#'ISF_FatrasTrackingGeometrySvc')

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name, **kwargs )


def getFatrasNeutralPropagatorID(name="ISF_FatrasNeutralPropagatorID", **kwargs):
    # the neutral particle propagator
    from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as NeutralPropagator
    return NeutralPropagator(name, **kwargs )


def getFatrasChargedPropagator(name="ISF_FatrasChargedPropagator", **kwargs):
    # the charged particle propagator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as ChargedPropagator
    return ChargedPropagator(name, **kwargs )
    #from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as ChargedPropagator
    #return ChargedPropagator(name, **kwargs )


# Propagators for the Extrapolation Engine
# load the RungeKutta Propagator
def getFatrasPropagator(name="ISF_FatrasPropagator", **kwargs):
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as RungeKuttaPropagator
    return RungeKuttaPropagator(name, **kwargs)


# from the Propagator create a Propagation engine to handle path length
def getFatrasStaticPropagator(name="ISF_FatrasStaticPropagator", **kwargs):
    kwargs.setdefault("Propagator", 'ISF_FatrasPropagator')
    # configure output formatting
    kwargs.setdefault("OutputPrefix", '[SP] - ')
    kwargs.setdefault("OutputPostfix", ' - ')
    kwargs.setdefault("OutputLevel", ISF_FatrasFlags.OutputLevelGeneral())
    from TrkExEngine.TrkExEngineConf import Trk__PropagationEngine as StaticPropagator
    return StaticPropagator(name, **kwargs)


# load the static navigation engine
def getFatrasStaticNavigationEngine(name="ISF_FatrasStaticNavigationEngine", **kwargs):
    #give the tools it needs
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    kwargs.setdefault("PropagationEngine", 'ISF_FatrasStaticPropagator')
    kwargs.setdefault("MaterialEffectsEngine", 'ISF_FatrasMaterialEffectsEngine')
    kwargs.setdefault("TrackingGeometry", AtlasTrackingGeometrySvc.TrackingGeometryName)
    # configure output formatting
    kwargs.setdefault("OutputPrefix", '[SN] - ')
    kwargs.setdefault("OutputPostfix", ' - ')
    kwargs.setdefault("OutputLevel", ISF_FatrasFlags.OutputLevelGeneral())
    from TrkExEngine.TrkExEngineConf import Trk__StaticNavigationEngine
    return Trk__StaticNavigationEngine(name, **kwargs)


############################################################################
# (1)  Charged Leptons and Hadrons
#
# FATRAS internal physics list
#
# SubProcesses handled
# (iii) Multiple scattering
def getFatrasMultipleScatteringUpdator(name="ISF_FatrasMultipleScatteringUpdator", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "TrkExRnd" ):
      simFlags.RandomSeedList.addSeed( "TrkExRnd" , 12412330, 37849324 )
    kwargs.setdefault("RandomNumberService"   , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"      , 'TrkExRnd' ) # TODO: read stream name "TrkExRnd" from Fatras jobProperties
    kwargs.setdefault("GaussianMixtureModel"  , FatrasTuningFlags.GaussianMixtureModel())

    from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator
    return Trk__MultipleScatteringUpdator(name, **kwargs )


def getFatrasSTEP_Propagator(name="ISF_FatrasSTEP_Propagator", **kwargs):
    kwargs.setdefault("MomentumCutOff"                 , FatrasTuningFlags.MomCutOffSec() )
    kwargs.setdefault("SimulationMode"                 , True )
    kwargs.setdefault("SimMatEffUpdator"               , 'ISF_FatrasMaterialUpdator' )
    # the step propagaor
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
    return StepPropagator(name, **kwargs )


# the definition of an extrapolator (to be cleaned up)
def getFatrasExtrapolator(name="ISF_FatrasExtrapolator", **kwargs):
    # Charged Transport Tool
    # assign the tools
    kwargs.setdefault("Navigator"               , 'ISF_FatrasNavigator')
    kwargs.setdefault("MaterialEffectsUpdators" , [ 'ISF_FatrasMaterialUpdator' ])
    kwargs.setdefault("Propagators"             , [ 'ISF_FatrasChargedPropagator' ])
    kwargs.setdefault("STEP_Propagator"         , 'ISF_FatrasSTEP_Propagator')
    # Fatras specific: stop the trajectory
    kwargs.setdefault("StopWithNavigationBreak" , True)
    kwargs.setdefault("StopWithUpdateKill"      , True)
    kwargs.setdefault("RobustSampling"          , True)
    kwargs.setdefault("ResolveMuonStation"      , True)
    kwargs.setdefault("UseMuonMatApproximation" , True)

    from TrkExTools.TrkExToolsConf import Trk__TimedExtrapolator as TimedExtrapolator
    return TimedExtrapolator(name, **kwargs )
    #from TrkExTools.TrkExToolsConf import Trk__Extrapolator as Extrapolator
    #return Extrapolator(name, **kwargs )


# load the Static ExtrapolationEngine
def getFatrasStaticExtrapolator(name="ISF_FatrasStaticExEngine", **kwargs):
    # give the tools it needs
    kwargs.setdefault("PropagationEngine", 'ISF_FatrasStaticPropagator')
    kwargs.setdefault("MaterialEffectsEngine", 'ISF_FatrasMaterialEffectsEngine')
    kwargs.setdefault("NavigationEngine", 'ISF_FatrasStaticNavigationEngine')
    # configure output formatting
    kwargs.setdefault("OutputPrefix", '[SE] - ')
    kwargs.setdefault("OutputPostfix", ' - ')
    kwargs.setdefault("OutputLevel", ISF_FatrasFlags.OutputLevelGeneral())
    from TrkExEngine.TrkExEngineConf import Trk__StaticEngine
    return Trk__StaticEngine(name, **kwargs)


def getFatrasExEngine(name="ISF_FatrasExEngine", **kwargs):
    # load the tracking geometry service
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    # assign the tools
    kwargs.setdefault("ExtrapolationEngines"    , [ 'ISF_FatrasStaticExEngine' ] )
    kwargs.setdefault("TrackingGeometrySvc"     , AtlasTrackingGeometrySvc)
    kwargs.setdefault("PropagationEngine"       , 'ISF_FatrasStaticPropagator')
    # configure output formatting
    kwargs.setdefault("OutputPrefix"     , '[ME] - ')
    kwargs.setdefault("OutputPostfix"    , ' - ')
    kwargs.setdefault("OutputLevel"      ,  ISF_FatrasFlags.OutputLevelGeneral())

    from TrkExEngine.TrkExEngineConf import Trk__ExtrapolationEngine
    return Trk__ExtrapolationEngine(name="ISF_FatrasExEngine", **kwargs )


