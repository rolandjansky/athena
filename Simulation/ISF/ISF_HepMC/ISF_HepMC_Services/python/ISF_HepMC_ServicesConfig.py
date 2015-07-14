# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved


def getGenericTruthService(name="ISF_TruthService", **kwargs):
    kwargs.setdefault('BarcodeSvc'                       , ISF_Flags.BarcodeService() )
    kwargs.setdefault('McEventCollection'                , 'TruthEvent')
    kwargs.setdefault('SkipIfNoChildren'                 , True)
    kwargs.setdefault('SkipIfNoParentBarcode'            , True)
    kwargs.setdefault('StoreExtraBarcodes'               , False)
    kwargs.setdefault('ForceEndVtxInRegions'             , [ ]  )
    if 'longLived' in ISF_Flags.Simulator(): #FIXME this should be configured in a nicer way. ATLASSIM-526
        kwargs.setdefault('QuasiStableParticlesIncluded', True)
    from ISF_HepMC_Services.ISF_HepMC_ServicesConf import ISF__HepMC_TruthSvc
    return ISF__HepMC_TruthSvc(name, **kwargs);

def getMC15TruthService(name="ISF_MC15TruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('BeamPipeTruthStrategies' , [ 'ISF_MCTruthStrategyGroupID_MC15' ] ) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies'       , [ 'ISF_MCTruthStrategyGroupID_MC15', 'ISF_MCTruthStrategyGroupIDHadInt_MC15' ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ 'ISF_MCTruthStrategyGroupCaloMuBrem', 'ISF_MCTruthStrategyGroupCaloDecay_MC15' ])
    kwargs.setdefault('MSTruthStrategies'       , [])
    kwargs.setdefault('IgnoreUndefinedBarcodes' , False)
    kwargs.setdefault('PassWholeVertices'       , False) # new for MC15 - can write out partial vertices.
    kwargs.setdefault('ForceEndVtxInRegions'    , [ AtlasRegion.fAtlasID ] )
    return getGenericTruthService(name, **kwargs);

def getMC15aTruthService(name="ISF_MC15aTruthService", **kwargs):
    kwargs.setdefault('ForceEndVtxInRegions'    , [ ] )
    return getMC15TruthService(name, **kwargs);

def getMC15aPlusTruthService(name="ISF_MC15aPlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions'    , [ AtlasRegion.fAtlasID ] )
    return getMC15TruthService(name, **kwargs);

def getMC12TruthService(name="ISF_MC12TruthService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies' , [ 'ISF_MCTruthStrategyGroupID' ] ) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies'       , [ 'ISF_MCTruthStrategyGroupID', 'ISF_MCTruthStrategyGroupIDHadInt' ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ 'ISF_MCTruthStrategyGroupCaloMuBrem' ])
    kwargs.setdefault('MSTruthStrategies'       , [])
    kwargs.setdefault('IgnoreUndefinedBarcodes' , False)
    kwargs.setdefault('PassWholeVertices'       , True)
    return getGenericTruthService(name, **kwargs);

def getMC12LLPTruthService(name="ISF_MC12TruthLLPService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies' , [ 'ISF_MCTruthStrategyGroupID', 'ISF_LLPTruthStrategy' ] ) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies'       , [ 'ISF_MCTruthStrategyGroupID', 'ISF_MCTruthStrategyGroupIDHadInt', 'ISF_LLPTruthStrategy' ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ 'ISF_MCTruthStrategyGroupCaloMuBrem', 'ISF_LLPTruthStrategy' ])
    kwargs.setdefault('MSTruthStrategies'       , ['ISF_LLPTruthStrategy'])
    return getMC12TruthService(name, **kwargs);

def getMC12PlusTruthService(name="ISF_MC12PlusTruthService", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('ForceEndVtxInRegions'    , [ AtlasRegion.fAtlasID ] )
    return getMC12TruthService(name, **kwargs);

def getValidationTruthService(name="ISF_ValidationTruthService", **kwargs):
    kwargs.setdefault('BeamPipeTruthStrategies' , [])
    kwargs.setdefault('IDTruthStrategies'       , [ 'ISF_ValidationTruthStrategy' ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ 'ISF_ValidationTruthStrategy' ] )
    kwargs.setdefault('MSTruthStrategies'       , [])
    kwargs.setdefault('IgnoreUndefinedBarcodes' , True)
    kwargs.setdefault('PassWholeVertices'       , True)
    return getGenericTruthService(name, **kwargs);

def getTruthService(name="ISF_TruthService", **kwargs):
    if ISF_Flags.ValidationMode() :
      return getValidationTruthService( name, **kwargs )
    else :
      return getMC12TruthService( name, **kwargs );

