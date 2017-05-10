# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon import CfgMgr

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *


def getParticleBrokerSvcNoOrdering(name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    kwargs.setdefault('EntryLayerTool', 'ISF_EntryLayerTool')
    kwargs.setdefault('GeoIDSvc', 'ISF_GeoIDSvc')
    kwargs.setdefault('AlwaysUseGeoIDSvc', False)
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('ValidateGeoIDs', ISF_Flags.ValidationMode())
    kwargs.setdefault('ValidationOutput', ISF_Flags.ValidationMode())
    kwargs.setdefault('ValidationStreamName', "ParticleBroker")
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BarcodeService', simFlags.TruthStrategy.BarcodeServiceName())
    return CfgMgr.ISF__ParticleBrokerDynamicOnReadIn(name, **kwargs)

def getParticleBrokerSvc(name="ISF_ParticleBrokerSvc", **kwargs):
    #kwargs.setdefault('ParticleOrderingTool', 'ISF_InToOutSubDetOrderingTool')
    kwargs.setdefault('ParticleOrderingTool', 'ISF_ParticleOrderingTool')
    return getParticleBrokerSvcNoOrdering(name, **kwargs)

def getAFIIParticleBrokerSvc(name="ISF_AFIIParticleBrokerSvc", **kwargs):
    kwargs.setdefault('EntryLayerTool', 'ISF_AFIIEntryLayerTool')
    return getParticleBrokerSvc(name, **kwargs)


def getSimHitService(name="ISF_SimHitService", **kwargs):
    return CfgMgr.ISF__SimHitSvc(name, **kwargs)

def getNoG4SimHitService(name="ISF_NoG4SimHitService", **kwargs):
    kwargs.setdefault("SensitiveDetectorMasterTool", "EmptySensitiveDetectorMasterTool")
    kwargs.setdefault("FastSimulationMasterTool", "EmptyFastSimulationMasterTool")
    return getSimHitService(name, **kwargs)

def getPileupSimHitService(name="ISF_PileupSimHitService", **kwargs):
    return getNoG4SimHitService(name, **kwargs)


def getISFEnvelopeDefSvc(name="ISF_ISFEnvelopeDefSvc", **kwargs):
    # ATLAS common envlope definitions
    kwargs.setdefault("ATLASEnvelopeDefSvc", "AtlasGeometry_EnvelopeDefSvc")
    return CfgMgr.ISF__ISFEnvelopeDefSvc(name, **kwargs)

def getAFIIEnvelopeDefSvc(name="ISF_AFIIEnvelopeDefSvc", **kwargs):
    from AthenaCommon.SystemOfUnits import mm
    # ATLAS common envlope definitions
    kwargs.setdefault("ISFEnvelopeDefSvc", "ISF_ISFEnvelopeDefSvc")
    kwargs.setdefault("InDetMaxExtentZ", 3549.5*mm)
    return CfgMgr.ISF__AFIIEnvelopeDefSvc(name, **kwargs)


def getGeoIDSvc(name="ISF_GeoIDSvc", **kwargs):
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc", "ISF_ISFEnvelopeDefSvc")
    return CfgMgr.ISF__GeoIDSvc(name, **kwargs)


def getAFIIGeoIDSvc(name="ISF_AFIIGeoIDSvc", **kwargs):
    kwargs.setdefault("EnvelopeDefSvc", "ISF_AFIIEnvelopeDefSvc")
    return getGeoIDSvc(name, **kwargs)

def getParticleKillerSvc(name="ISF_ParticleKillerSvc", **kwargs):
    kwargs.setdefault('Identifier',           "ParticleKiller")
    return CfgMgr.ISF__ParticleKillerSimSvc(name, **kwargs)

def getInputConverter(name="ISF_InputConverter", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    kwargs.setdefault("UseGeneratedParticleMass", False)
    genParticleFilters = ['ISF_ParticleFinalStateFilter']
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() != "cosmics":
        genParticleFilters += ['ISF_ParticlePositionFilterDynamic',
                                'ISF_EtaPhiFilter']
    genParticleFilters += ['ISF_GenParticleInteractingFilter']
    kwargs.setdefault("GenParticleFilters", genParticleFilters)
    return CfgMgr.ISF__InputConverter(name, **kwargs)


def getLongLivedInputConverter(name="ISF_LongLivedInputConverter", **kwargs):
    kwargs.setdefault("GenParticleFilters"      , [ 'ISF_ParticleSimWhiteList',
                                                    'ISF_ParticlePositionFilterDynamic',
                                                    'ISF_EtaPhiFilter',
                                                    'ISF_GenParticleInteractingFilter', ] )
    kwargs.setdefault('QuasiStableParticlesIncluded', True)
    return getInputConverter(name, **kwargs)


#
# Generic Truth Service Configurations
#

def getGenericTruthService(name="ISF_TruthService", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    kwargs.setdefault('SkipIfNoChildren', True)
    kwargs.setdefault('SkipIfNoParentBarcode', True)
    kwargs.setdefault('ForceEndVtxInRegions', [])
    long_lived_simulators = ['LongLived', 'longLived']
    from ISF_Config.ISF_jobProperties import ISF_Flags
    is_long_lived_simulation = any(x in ISF_Flags.Simulator() for x in long_lived_simulators) #FIXME this should be set in a nicer way.
    if is_long_lived_simulation:
        kwargs.setdefault('QuasiStableParticlesIncluded', True)
    return CfgMgr.ISF__TruthSvc(name, **kwargs)

def getValidationTruthService(name="ISF_ValidationTruthService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies', [])
    kwargs.setdefault('IDTruthStrategies', ['ISF_ValidationTruthStrategy'] )
    kwargs.setdefault('CaloTruthStrategies', ['ISF_ValidationTruthStrategy'] )
    kwargs.setdefault('MSTruthStrategies', [])
    kwargs.setdefault('IgnoreUndefinedBarcodes', True)
    kwargs.setdefault('PassWholeVertices', True)
    return getGenericTruthService(name, **kwargs)


#
# MC12 Truth Service Configurations
#

def getMC12BeamPipeTruthStrategies():
    return ['ISF_MCTruthStrategyGroupID']

def getMC12IDTruthStrategies():
    return ['ISF_MCTruthStrategyGroupID', 'ISF_MCTruthStrategyGroupIDHadInt']

def getMC12CaloTruthStrategies():
    return ['ISF_MCTruthStrategyGroupCaloMuBrem']

def getMC12MSTruthStrategies():
    return []

def getMC12TruthService(name="ISF_MC12TruthService", **kwargs):
    beam_pipe_strategies = getMC12BeamPipeTruthStrategies()
    id_strategies = getMC12IDTruthStrategies()
    calo_strategies = getMC12CaloTruthStrategies()
    ms_strategies = getMC12MSTruthStrategies()

    kwargs.setdefault('BeamPipeTruthStrategies', beam_pipe_strategies) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies', id_strategies)
    kwargs.setdefault('CaloTruthStrategies', calo_strategies)
    kwargs.setdefault('MSTruthStrategies', ms_strategies)
    kwargs.setdefault('IgnoreUndefinedBarcodes', False)
    kwargs.setdefault('PassWholeVertices', True)
    return getGenericTruthService(name, **kwargs)

def getTruthService(name="ISF_TruthService", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode() :
      return getValidationTruthService(name, **kwargs)
    else:
      return getMC12TruthService(name, **kwargs)

def getMC12LLPTruthService(name="ISF_MC12TruthLLPService", **kwargs):
    llp_strategies = ['ISF_LLPTruthStrategy']
    beam_pipe_strategies =  getMC12BeamPipeTruthStrategies() + llp_strategies
    id_strategies = getMC12IDTruthStrategies() + llp_strategies
    calo_strategies = getMC12CaloTruthStrategies() + llp_strategies
    ms_strategies = getMC12MSTruthStrategies() + llp_strategies

    kwargs.setdefault('BeamPipeTruthStrategies', beam_pipe_strategies)
    kwargs.setdefault('IDTruthStrategies', id_strategies)
    kwargs.setdefault('CaloTruthStrategies', calo_strategies)
    kwargs.setdefault('MSTruthStrategies', ms_strategies)
    return getMC12TruthService(name, **kwargs)

def getMC12PlusTruthService(name="ISF_MC12PlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID] )
    return getMC12TruthService(name, **kwargs)


#
# MC15 Truth Service Configurations
#

def getMC15BeamPipeTruthStrategies():
    return ['ISF_MCTruthStrategyGroupID_MC15']

def getMC15IDTruthStrategies():
    return ['ISF_MCTruthStrategyGroupID_MC15', 'ISF_MCTruthStrategyGroupIDHadInt_MC15']

def getMC15CaloTruthStrategies():
    return ['ISF_MCTruthStrategyGroupCaloMuBrem', 'ISF_MCTruthStrategyGroupCaloDecay_MC15']

def getMC15MSTruthStrategies():
    return []

def getMC15TruthService(name="ISF_MC15TruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    beam_pipe_strategies = getMC15BeamPipeTruthStrategies()
    id_strategies = getMC15IDTruthStrategies()
    calo_strategies = getMC15CaloTruthStrategies()
    ms_strategies = getMC15MSTruthStrategies()

    kwargs.setdefault('BeamPipeTruthStrategies', beam_pipe_strategies) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies', id_strategies)
    kwargs.setdefault('CaloTruthStrategies', calo_strategies)
    kwargs.setdefault('MSTruthStrategies', ms_strategies)

    kwargs.setdefault('IgnoreUndefinedBarcodes', False)
    kwargs.setdefault('PassWholeVertices', False) # new for MC15 - can write out partial vertices.
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID])
    return getGenericTruthService(name, **kwargs)

def getMC15aTruthService(name="ISF_MC15aTruthService", **kwargs):
    kwargs.setdefault('ForceEndVtxInRegions', [])
    return getMC15TruthService(name, **kwargs)

def getMC15aPlusTruthService(name="ISF_MC15aPlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID])
    return getMC15TruthService(name, **kwargs)

def getMC15aPlusLLPTruthService(name="ISF_MC15aPlusLLPTruthService", **kwargs):
    llp_strategies = ['ISF_LLPTruthStrategy']
    beam_pipe_strategies =  getMC15BeamPipeTruthStrategies() + llp_strategies
    id_strategies = getMC15IDTruthStrategies() + llp_strategies
    calo_strategies = getMC15CaloTruthStrategies() + llp_strategies
    ms_strategies = getMC15MSTruthStrategies() + llp_strategies

    kwargs.setdefault('BeamPipeTruthStrategies', beam_pipe_strategies)
    kwargs.setdefault('IDTruthStrategies', id_strategies)
    kwargs.setdefault('CaloTruthStrategies', calo_strategies)
    kwargs.setdefault('MSTruthStrategies', ms_strategies)
    return getMC15aPlusTruthService(name, **kwargs)


#
# MC16 Truth Service Configurations
#

def getMC16TruthService(name="ISF_MC16TruthService", **kwargs):
    return getMC15aPlusTruthService(name, **kwargs)

def getMC16LLPTruthService(name="ISF_MC16LLPTruthService", **kwargs):
    return getMC15aPlusLLPTruthService(name, **kwargs)


#
# MC18 Truth Service Configurations
#

def getMC18TruthService(name="ISF_MC18TruthService", **kwargs):
    return getMC15aPlusTruthService(name, **kwargs)


def getMC18LLPTruthService(name="ISF_MC18LLPTruthService", **kwargs):
    return getMC15aPlusLLPTruthService(name, **kwargs)
