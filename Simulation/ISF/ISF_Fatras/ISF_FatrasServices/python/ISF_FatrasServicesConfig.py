# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF for ISF_FatrasServicesConfig
KG Tan, 04/12/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
from ISF_FatrasServices.FatrasTuning import FatrasTuningFlags
from ISF_FatrasServices.FatrasValidation import FatrasValidationFlags

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
        kwargs.setdefault("InDetTrackingGeometryBuilder",getPublicTool('ISF_InDetTrackingGeometryBuilder'))
    if DetFlags.Calo_on():
        kwargs.setdefault("CaloTrackingGeometryBuilder" ,getPublicTool('ISF_FatrasCaloTrackingGeometryBuilder'))
    if DetFlags.Muon_on():
        kwargs.setdefault("MuonTrackingGeometryBuilder" ,getPublicTool('ISF_FatrasMuonTrackingGeometryBuilder'))

    from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
    return Trk__GeometryBuilder(name, **kwargs )

def getFatrasTrackingGeometrySvc(name="ISF_FatrasTrackingGeometrySvc", **kwargs):
    # get hand on the ID Tracking Geometry Builder
    # register the Builder
    kwargs.setdefault("GeometryBuilder"             , getPublicTool('ISF_FatrasGeometryBuilder'))
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
    kwargs.setdefault("TrackingGeometrySvc"  , getService('ISF_FatrasTrackingGeometrySvc'))

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

######################################################################################
# validation & process sampling
def getFatrasPhysicsValidationTool(name="ISF_FatrasPhysicsValidationTool", **kwargs):
    kwargs.setdefault("ValidationStreamName", "ISFFatras")

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__PhysicsValidationTool
    return iFatras__PhysicsValidationTool(name, **kwargs)


################################################################################
# PARTICLE DECAY SECTION
################################################################################

def getG4RunManagerHelper(name="ISF_G4RunManagerHelper", **kwargs):
    from ISF_Geant4Tools.ISF_Geant4ToolsConf import iGeant4__G4RunManagerHelper
    return iGeant4__G4RunManagerHelper(name, **kwargs)

def getFatrasPdgG4Particle(name="ISF_FatrasPdgG4Particle", **kwargs):
    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__PDGToG4Particle
    return iFatras__PDGToG4Particle(name, **kwargs )

def getFatrasParticleDecayer(name="ISF_FatrasParticleDecayer", **kwargs):
    kwargs.setdefault("PDGToG4ParticleConverter", getPublicTool('ISF_FatrasPdgG4Particle'))
    #kwargs.setdefault("G4RunManagerHelper"  , getPublicTool('ISF_G4RunManagerHelper'))

    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__G4ParticleDecayCreator
    return iFatras__G4ParticleDecayCreator(name, **kwargs )

def getFatrasParticleDecayHelper(name="ISF_FatrasParticleDecayHelper", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    simFlags.RandomSeedList.addSeed( "FatrasG4" , 23491234, 23470291 )
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("G4RandomStreamName"    , "FatrasG4") # TODO: read stream name "FatrasG4" from Fatras jobProperties
    kwargs.setdefault("ParticleBroker"  , getService('ISF_ParticleBrokerSvc'))
    kwargs.setdefault("ParticleTruthSvc", ISF_Flags.TruthService.get_Value())
    kwargs.setdefault("ParticleDecayer" , getPublicTool('ISF_FatrasParticleDecayer'))
    kwargs.setdefault("PDGToG4ParticleConverter", getPublicTool('ISF_FatrasPdgG4Particle'))
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    #kwargs.setdefault("G4RunManagerHelper"  , getPublicTool('ISF_G4RunManagerHelper'))

    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__G4ParticleDecayHelper
    return iFatras__G4ParticleDecayHelper(name, **kwargs )

############################################################################
# (1)  Charged Leptons and Hadrons
#
# FATRAS internal physics list
#
# SubProcesses handled
# (i) Hadronic Interactions
#     the G4 MaterialEffectsUpdator

def getFatrasG4HadIntProcessor(name="ISF_FatrasG4HadIntProcessor", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ParticleBroker"      , getService('ISF_ParticleBrokerSvc'))
    kwargs.setdefault("TruthRecordSvc"      , ISF_Flags.TruthService.get_Value())
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())
    kwargs.setdefault("MinEnergyCut"        , FatrasTuningFlags.MomCutOffSec())

    from ISF_FatrasToolsG4.ISF_FatrasToolsG4Conf import iFatras__G4HadIntProcessor
    return iFatras__G4HadIntProcessor(name, **kwargs )

def getFatrasParametricHadIntProcessor(name="ISF_FatrasParametricHadIntProcessor", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    #   Fatras Hadronic Interaction Processor
    #   hadronic interaction creator
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ParticleBroker"      , getService('ISF_ParticleBrokerSvc'))
    kwargs.setdefault("TruthRecordSvc"      , ISF_Flags.TruthService.get_Value())
    kwargs.setdefault("HadronicInteractionScaleFactor"  , FatrasTuningFlags.HadronicInteractionProbabilityScalor())
    kwargs.setdefault("MinimumHadronicInitialEnergy"    , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("MinimumHadronicOutEnergy"        , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("HadronicInteractionValidation"   , False)
    kwargs.setdefault("PhysicsProcessCode"              , 121) # TODO: to be taken from central definition
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())


    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__HadIntProcessorParametric
    return iFatras__HadIntProcessorParametric(name, **kwargs )

def getFatrasProcessSamplingTool(name="ISF_FatrasProcessSamplingTool", **kwargs):

    # random number service
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault( "RandomNumberService", simFlags.RandomSvc())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__ProcessSamplingTool
    return iFatras__ProcessSamplingTool(name, **kwargs)
#
# (ii) Energy Loss
#      - Ionization and Bremstrahlung loss
#      - assing the Bethe-Heitler Eloss updator
def getFatrasBetheHeitlerEnergyLossUpdator(name="ISF_FatrasBetheHeitlerEnergyLossUpdator", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ScaleFactor"  , FatrasTuningFlags.BetheHeitlerScalor())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McBetheHeitlerEnergyLossUpdator
    return iFatras__McBetheHeitlerEnergyLossUpdator(name, **kwargs )

def getFatrasEnergyLossUpdator(name="ISF_FatrasEnergyLossUpdator", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ElectronEnergyLossUpdator"  , getPublicTool('ISF_FatrasBetheHeitlerEnergyLossUpdator'))

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McEnergyLossUpdator
    return iFatras__McEnergyLossUpdator(name, **kwargs )

#
# (iii) Multiple scattering
def getFatrasMultipleScatteringUpdator(name="ISF_FatrasMultipleScatteringUpdator", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    simFlags.RandomSeedList.addSeed( "TrkExRnd" , 12412330, 37849324 )
    kwargs.setdefault("RandomNumberService"   , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"      , 'TrkExRnd' ) # TODO: read stream name "TrkExRnd" from Fatras jobProperties
    kwargs.setdefault("GaussianMixtureModel"  , FatrasTuningFlags.GaussianMixtureModel())

    from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator
    return Trk__MultipleScatteringUpdator(name, **kwargs )

# (iv) photon conversion
def getFatrasConversionCreator(name="ISF_FatrasConversionCreator", **kwargs):
    #   Fatras Hadronic Interaction Processor
    #   hadronic interaction creator
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ParticleBroker"  , getService('ISF_ParticleBrokerSvc'))
    kwargs.setdefault("TruthRecordSvc"  , ISF_Flags.TruthService.get_Value())
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    kwargs.setdefault("PhysicsProcessCode"              , 14) # TODO: to be taken from central definition
    kwargs.setdefault('ValidationMode'      , ISF_Flags.ValidationMode())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__PhotonConversionTool
    return iFatras__PhotonConversionTool(name, **kwargs )


# Combining all in the MaterialEffectsUpdator
def getFatrasMaterialUpdator(name="ISF_FatrasMaterialUpdator", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("ParticleBroker"              , getService('ISF_ParticleBrokerSvc'))
    kwargs.setdefault("TruthRecordSvc"              , ISF_Flags.TruthService.get_Value())
    # hadronic interactions
    kwargs.setdefault("HadronicInteraction"         , True)
    #kwargs.setdefault("HadronicInteractionProcessor", getPublicTool('ISF_FatrasParametricHadIntProcessor'))
    kwargs.setdefault("HadronicInteractionProcessor", getPublicTool('ISF_FatrasG4HadIntProcessor'))
    # enrgy loss
    kwargs.setdefault("EnergyLoss"                  , True)
    kwargs.setdefault("EnergyLossUpdator"           , getPublicTool('ISF_FatrasEnergyLossUpdator'))
    # mutiple scattering
    kwargs.setdefault("MultipleScattering"          , True)
    kwargs.setdefault("MultipleScatteringUpdator"   , getPublicTool('ISF_FatrasMultipleScatteringUpdator'))
    # photon conversion
    kwargs.setdefault("PhotonConversionTool"        , getPublicTool('ISF_FatrasConversionCreator'))
    # the validation output
    kwargs.setdefault("ValidationMode"              , ISF_Flags.ValidationMode())
    kwargs.setdefault("BremPhotonValidation"        , False)
    kwargs.setdefault("EnergyDepositValidation"     , False)
    # the properties given throuth the JobProperties interface
    kwargs.setdefault("MomentumCut"                 , FatrasTuningFlags.MomCutOffSec())
    kwargs.setdefault("MinimumBremPhotonMomentum"   , FatrasTuningFlags.MomCutOffSec())
    #
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    kwargs.setdefault("ProcessSamplingTool"         , getPublicTool('ISF_FatrasProcessSamplingTool'))
    kwargs.setdefault("ParticleDecayer"             , getPublicTool('ISF_FatrasParticleDecayer'))
    # MCTruth Process Code
    kwargs.setdefault("BremProcessCode"             , 3) # TODO: to be taken from central definition

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__McMaterialEffectsUpdator
    return iFatras__McMaterialEffectsUpdator(name, **kwargs )

def getFatrasSTEP_Propagator(name="ISF_FatrasSTEP_Propagator", **kwargs):
    kwargs.setdefault("MomentumCutOff"                 , FatrasTuningFlags.MomCutOffSec() )
    kwargs.setdefault("SimulationMode"                 , True )
    kwargs.setdefault("SimMatEffUpdator"               , getPublicTool('ISF_FatrasMaterialUpdator') )
    # the step propagaor
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
    return StepPropagator(name, **kwargs )

# the definition of an extrapolator (to be cleaned up)
def getFatrasExtrapolator(name="ISF_FatrasExtrapolator", **kwargs):
    # Charged Transport Tool
    # assign the tools
    kwargs.setdefault("Navigator"               , getPublicTool('ISF_FatrasNavigator'))
    kwargs.setdefault("MaterialEffectsUpdators" , [ getPublicTool('ISF_FatrasMaterialUpdator') ])
    kwargs.setdefault("Propagators"             , [ getPublicTool('ISF_FatrasChargedPropagator') ])
    kwargs.setdefault("STEP_Propagator"         , getPublicTool('ISF_FatrasSTEP_Propagator'))
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


################################################################################
# HIT CREATION SECTION
################################################################################

#   Fatras Hadronic Interaction Processor
#   hadronic interaction creator
def getFatrasHitCreatorPixel(name="ISF_FatrasHitCreatorPixel", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("IdHelperName"    , 'PixelID')
    kwargs.setdefault("CollectionName"  , 'PixelHits')
    kwargs.setdefault("UseConditionsSvc", False)

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorSilicon
    return iFatras__HitCreatorSilicon(name, **kwargs )

def getFatrasHitCreatorSCT(name="ISF_FatrasHitCreatorSCT", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("IdHelperName"    , 'SCT_ID')
    kwargs.setdefault("CollectionName"  , 'SCT_Hits')
    kwargs.setdefault("UseConditionsSvc", False)

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorSilicon
    return iFatras__HitCreatorSilicon(name, **kwargs )

def getFatrasHitCreatorTRT(name="ISF_FatrasHitCreatorTRT", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("CollectionName"  , 'TRTUncompressedHits')

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorTRT
    return iFatras__HitCreatorTRT(name, **kwargs )

################################################################################
# TRACK CREATION SECTION
################################################################################
def getFatrasSimHitCreatorID(name="ISF_FatrasSimHitCreatorID", **kwargs):
    kwargs.setdefault("PixelHitCreator" , getPublicTool('ISF_FatrasHitCreatorPixel'))
    kwargs.setdefault("SctHitCreator"   , getPublicTool('ISF_FatrasHitCreatorSCT'))
    kwargs.setdefault("TrtHitCreator"   , getPublicTool('ISF_FatrasHitCreatorTRT'))
    kwargs.setdefault("OutputLevel"     , ISF_FatrasFlags.OutputLevelGeneral())

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__SimHitCreatorID
    return iFatras__SimHitCreatorID(name, **kwargs )

def getFatrasSimHitCreatorMS(name="ISF_FatrasSimHitCreatorMS", **kwargs):
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("Extrapolator" , getPublicTool('ISF_FatrasExtrapolator'))
    from ISF_FatrasToolsMS.ISF_FatrasToolsMSConf import iFatras__SimHitCreatorMS
    return iFatras__SimHitCreatorMS(name, **kwargs )

################################################################################
# SIMULATION TOOL and SERVICE
################################################################################
def getFatrasKinematicFilter(name="ISF_FatrasKinematicFilter", **kwargs):
    kwargs.setdefault("MaxEtaSymmetric" , 10.)
    kwargs.setdefault("MinMomentum"     , FatrasTuningFlags.MomCutOffSec())

    from ISF_Tools.ISF_ToolsConf import ISF__KinematicParticleFilter
    return ISF__KinematicParticleFilter(name, **kwargs )

def getFatrasSimTool(name="ISF_FatrasSimTool", **kwargs):
    kwargs.setdefault("SimHitCreatorID" , getPublicTool('ISF_FatrasSimHitCreatorID'))
    kwargs.setdefault("SimHitCreatorMS" , getPublicTool('ISF_FatrasSimHitCreatorMS'))
    # TODO: G4 Tools can not be used at the same time as Geant4 inside ISF
    kwargs.setdefault("ParticleDecayHelper" , getPublicTool('ISF_FatrasParticleDecayHelper'))
    kwargs.setdefault("ParticleHelper"      , getPublicTool('ISF_ParticleHelper'))
    # the filter setup
    kwargs.setdefault("TrackFilter"         , getPublicTool('ISF_FatrasKinematicFilter'))
    kwargs.setdefault("NeutralFilter"       , getPublicTool('ISF_FatrasKinematicFilter'))
    kwargs.setdefault("PhotonFilter"        , getPublicTool('ISF_FatrasKinematicFilter'))
    # extrapolator - test setup
    kwargs.setdefault("Extrapolator"           , getPublicTool('ISF_FatrasExtrapolator'))
    #
    kwargs.setdefault("PhysicsValidationTool"       , getPublicTool('ISF_FatrasPhysicsValidationTool'))
    kwargs.setdefault("ProcessSamplingTool"         , getPublicTool('ISF_FatrasProcessSamplingTool'))
    # set the output level
    kwargs.setdefault("OutputLevel"         , ISF_FatrasFlags.OutputLevelGeneral())
    kwargs.setdefault('ValidationOutput'    , ISF_Flags.ValidationMode())
    # random number service
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    kwargs.setdefault( "RandomNumberService", simFlags.RandomSvc())

    from ISF_FatrasTools.ISF_FatrasToolsConf import iFatras__TransportTool
    return iFatras__TransportTool(name, **kwargs )

def getFatrasSimServiceID(name="ISF_FatrasSimSvc", **kwargs):
    kwargs.setdefault("Identifier"      , "Fatras")
    kwargs.setdefault("SimulationTool"  , getPublicTool('ISF_FatrasSimTool'))
    # set the output level
    kwargs.setdefault("OutputLevel"     , ISF_FatrasFlags.OutputLevelGeneral())

    # register Fatras random number stream if not already registered
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasRnd" ):
      simFlags.RandomSeedList.addSeed( "FatrasRnd", 81234740, 23474923 )

    from ISF_FatrasServices.ISF_FatrasServicesConf import iFatras__FatrasSimSvc
    return iFatras__FatrasSimSvc(name, **kwargs )

def getFatrasNewExtrapolationSimServiceID(name="ISF_FatrasNewExtrapolationSimSvc", **kwargs):
    kwargs.setdefault("SimulationTool"  , getPublicTool('ISF_FatrasNewExtrapolationSimTool'))
    return getFatrasSimServiceID(name, **kwargs )

def getFatrasGeoIDFixSimServiceID(name="ISF_FatrasGeoIDFixSimSvc", **kwargs):
    kwargs.setdefault("EnableGeoIDOverride"      , True  )
    kwargs.setdefault("GeoIDOverrideZ"           , 3150. )
    kwargs.setdefault("GeoIDOverride"            , 3     ) # ISF::fAtlasCalo
    return getFatrasSimServiceID(name, **kwargs )

def getFatrasNewExtrapolationSimTool(name="ISF_FatrasNewExtrapolationSimTool", **kwargs):
    # add your custom configuration of the SimTool here
    #   NB: only the 'delta' to the normal SimTool configuration needs to be configured here, eg:
    # kwargs.setdefault("SimHitCreatorMS" , '' )
    # kwargs.setdefault("SimHitCreatorID" , 'ISF_FatrasNewExtrapolationSimHitCreatorID' )
    # ..
    return getFatrasSimTool(name, **kwargs )

def getFatrasPileupSimTool(name="ISF_FatrasPileupSimTool", **kwargs):
    ## MB: Turn off track hit creation
    kwargs.setdefault("TurnOffHitCreation"  , True)
    return getFatrasSimTool( name, **kwargs )

def getFatrasPileupSimServiceID(name="ISF_FatrasPileupSimSvc", **kwargs):
    kwargs.setdefault("Identifier"      , "Fatras")
    kwargs.setdefault("SimulationTool"  , getPublicTool('ISF_FatrasPileupSimTool'))
    # set the output level
    kwargs.setdefault("OutputLevel"     , ISF_FatrasFlags.OutputLevelGeneral())

    # register Fatras random number stream if not already registered
    from G4AtlasApps.SimFlags import SimFlags,simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "FatrasRnd" ):
      simFlags.RandomSeedList.addSeed( "FatrasRnd", 81234740, 23474923 )

    from ISF_FatrasServices.ISF_FatrasServicesConf import iFatras__FatrasSimSvc
    return iFatras__FatrasSimSvc(name, **kwargs )
