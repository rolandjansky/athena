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


def getTruthService(name="ISF_TruthService", **kwargs):
    if ISF_Flags.ValidationMode() :
      return getValidationTruthService( name, **kwargs )
    else :
      return getMC12TruthService( name, **kwargs );

def getMC12TruthService(name="ISF_MC12TruthService", **kwargs):
    kwargs.setdefault('BarcodeSvc'              , getService('ISF_MC12BarcodeSvc'))
    kwargs.setdefault('McEventCollection'       , 'TruthEvent')
    kwargs.setdefault('SkipIfNoSecondaries'     , True)
    kwargs.setdefault('SkipIfNoPrimaryBarcode'  , True)
    kwargs.setdefault('BeamPipeTruthStrategies' , [ getPublicTool('ISF_MCTruthStrategyGroupID') ] ) # this is used for beam pipe but not BeamPipeCentral which uses same as ID
    kwargs.setdefault('IDTruthStrategies'       , [ getPublicTool('ISF_MCTruthStrategyGroupID'), getPublicTool('ISF_MCTruthStrategyGroupIDHadInt') ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ getPublicTool('ISF_MCTruthStrategyGroupCaloMuBrem') ])
    kwargs.setdefault('MSTruthStrategies'       , [])
    kwargs.setdefault('IgnoreUndefinedBarcodes' , False)
    kwargs.setdefault('StoreExtraBarcodes'      , False)

    from ISF_HepMC_Services.ISF_HepMC_ServicesConf import ISF__HepMC_TruthSvc
    return ISF__HepMC_TruthSvc(name, **kwargs);

def getValidationTruthService(name="ISF_ValidationTruthService", **kwargs):
    kwargs.setdefault('BarcodeSvc'              , getService('ISF_ValidationBarcodeSvc'))
    kwargs.setdefault('McEventCollection'       , 'TruthEvent')
    kwargs.setdefault('SkipIfNoSecondaries'     , True)
    kwargs.setdefault('SkipIfNoPrimaryBarcode'  , True)
    kwargs.setdefault('BeamPipeTruthStrategies' , [])
    kwargs.setdefault('IDTruthStrategies'       , [ getPublicTool('ISF_ValidationTruthStrategy') ] )
    kwargs.setdefault('CaloTruthStrategies'     , [ getPublicTool('ISF_ValidationTruthStrategy') ] )
    kwargs.setdefault('MSTruthStrategies'       , [])
    kwargs.setdefault('IgnoreUndefinedBarcodes' , True)
    kwargs.setdefault('StoreExtraBarcodes'      , False)

    from ISF_HepMC_Services.ISF_HepMC_ServicesConf import ISF__HepMC_TruthSvc
    return ISF__HepMC_TruthSvc(name, **kwargs);
