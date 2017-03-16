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
    kwargs.setdefault('BarcodeService', ISF_Flags.BarcodeService())
    return CfgMgr.ISF__ParticleBrokerDynamicOnReadIn(name, **kwargs)

def getParticleBrokerSvc(name="ISF_ParticleBrokerSvc", **kwargs):
    #kwargs.setdefault('ParticleOrderingTool', 'ISF_InToOutSubDetOrderingTool')
    kwargs.setdefault('ParticleOrderingTool', 'ISF_ParticleOrderingTool')
    return getParticleBrokerSvcNoOrdering(name, **kwargs)

def getAFIIParticleBrokerSvc(name="ISF_AFIIParticleBrokerSvc", **kwargs):
    kwargs.setdefault('EntryLayerTool', 'ISF_AFIIEntryLayerTool')
    return getParticleBrokerSvc(name, **kwargs)

def getSimHitService(name="ISF_SimHitService", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('ValidationOutput', ISF_Flags.ValidationMode())
    return CfgMgr.ISF__SimHitSvc(name, **kwargs)

def getNoG4SimHitService(name="ISF_NoG4SimHitService", **kwargs):
    kwargs.setdefault("SensitiveDetectorMasterTool", "EmptySensitiveDetectorMasterTool")
    kwargs.setdefault("FastSimulationMasterTool", "EmptyFastSimulationMasterTool")
    return getSimHitService(name, **kwargs)

def getPileupSimHitService(name="ISF_PileupSimHitService", **kwargs):
    kwargs.setdefault('SeparateInDetPileupHits', True)
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
    return getInputConverter(name, **kwargs)

def getGenericTruthService(name="ISF_TruthService", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault('BarcodeSvc', ISF_Flags.BarcodeService())
    kwargs.setdefault('SkipIfNoChildren', True)
    kwargs.setdefault('SkipIfNoParentBarcode', True)
    kwargs.setdefault('ForceEndVtxInRegions', [])
    if 'longLived' in ISF_Flags.Simulator(): #FIXME this should be configured in a nicer way. ATLASSIM-526
        kwargs.setdefault('QuasiStableParticlesIncluded', True)
    return CfgMgr.ISF__TruthSvc(name, **kwargs)

def getMC15TruthService(name="ISF_MC15TruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('BeamPipeTruthStrategies', ['ISF_MCTruthStrategyGroupID_MC15']) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies', ['ISF_MCTruthStrategyGroupID_MC15', 'ISF_MCTruthStrategyGroupIDHadInt_MC15'])
    kwargs.setdefault('CaloTruthStrategies', ['ISF_MCTruthStrategyGroupCaloMuBrem', 'ISF_MCTruthStrategyGroupCaloDecay_MC15'])
    kwargs.setdefault('MSTruthStrategies', [])
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

def getMC12TruthService(name="ISF_MC12TruthService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies', ['ISF_MCTruthStrategyGroupID']) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies', ['ISF_MCTruthStrategyGroupID', 'ISF_MCTruthStrategyGroupIDHadInt'])
    kwargs.setdefault('CaloTruthStrategies', ['ISF_MCTruthStrategyGroupCaloMuBrem'])
    kwargs.setdefault('MSTruthStrategies', [])
    kwargs.setdefault('IgnoreUndefinedBarcodes', False)
    kwargs.setdefault('PassWholeVertices', True)
    return getGenericTruthService(name, **kwargs)

def getMC12LLPTruthService(name="ISF_MC12TruthLLPService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies', ['ISF_MCTruthStrategyGroupID', 'ISF_LLPTruthStrategy']) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies', ['ISF_MCTruthStrategyGroupID', 'ISF_MCTruthStrategyGroupIDHadInt', 'ISF_LLPTruthStrategy'])
    kwargs.setdefault('CaloTruthStrategies', ['ISF_MCTruthStrategyGroupCaloMuBrem', 'ISF_LLPTruthStrategy'])
    kwargs.setdefault('MSTruthStrategies', ['ISF_LLPTruthStrategy'])
    return getMC12TruthService(name, **kwargs)

def getMC12PlusTruthService(name="ISF_MC12PlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions', [AtlasRegion.fAtlasID] )
    return getMC12TruthService(name, **kwargs)

def getValidationTruthService(name="ISF_ValidationTruthService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies', [])
    kwargs.setdefault('IDTruthStrategies', ['ISF_ValidationTruthStrategy'] )
    kwargs.setdefault('CaloTruthStrategies', ['ISF_ValidationTruthStrategy'] )
    kwargs.setdefault('MSTruthStrategies', [])
    kwargs.setdefault('IgnoreUndefinedBarcodes', True)
    kwargs.setdefault('PassWholeVertices', True)
    return getGenericTruthService(name, **kwargs)

def getTruthService(name="ISF_TruthService", **kwargs):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode() :
      return getValidationTruthService(name, **kwargs)
    else :
      return getMC12TruthService(name, **kwargs)
