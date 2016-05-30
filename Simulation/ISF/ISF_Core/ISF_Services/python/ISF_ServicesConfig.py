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


def getParticleBrokerSvcNoOrdering(name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    kwargs.setdefault('StackFiller'                 , 'ISF_StackFiller'                     )
    kwargs.setdefault('EntryLayerTool'              , 'ISF_EntryLayerTool'                  )
    kwargs.setdefault('GeoIDSvc'                    , 'ISF_GeoIDSvc'                        )
    kwargs.setdefault('AlwaysUseGeoIDSvc'           , False                                 )
    kwargs.setdefault('ValidateGeoIDs'              , ISF_Flags.ValidationMode()            )
    kwargs.setdefault('ValidationOutput'            , ISF_Flags.ValidationMode()            )
    kwargs.setdefault('ValidationStreamName'        , "ParticleBroker"                      )
    kwargs.setdefault('BarcodeService'              , ISF_Flags.BarcodeService()            )

    from ISF_Services.ISF_ServicesConf import ISF__ParticleBrokerDynamicOnReadIn
    return ISF__ParticleBrokerDynamicOnReadIn(name, **kwargs)

def getParticleBrokerSvc(name="ISF_ParticleBrokerSvc", **kwargs):
    #kwargs.setdefault('ParticleOrderingTool'       , 'ISF_InToOutSubDetOrderingTool'       )
    kwargs.setdefault('ParticleOrderingTool'        , 'ISF_ParticleOrderingTool'            )
    return getParticleBrokerSvcNoOrdering(name, **kwargs)

def getLongLivedParticleBrokerSvc(name="ISF_LongLivedParticleBrokerSvc", **kwargs):
    kwargs.setdefault('StackFiller'                 , 'ISF_LongLivedStackFiller'            )
    return getParticleBrokerSvcNoOrdering(name, **kwargs)

def getAFIIParticleBrokerSvc(name="ISF_AFIIParticleBrokerSvc", **kwargs):
    kwargs.setdefault('EntryLayerTool'              , 'ISF_AFIIEntryLayerTool'              )
    return getParticleBrokerSvc(name, **kwargs)

def getSimHitService(name="ISF_SimHitService", **kwargs):
    kwargs.setdefault('ValidationOutput'    , ISF_Flags.ValidationMode()                    )

    from ISF_Services.ISF_ServicesConf import ISF__SimHitSvc
    return ISF__SimHitSvc(name, **kwargs)

def getNoG4SimHitService(name="ISF_NoG4SimHitService", **kwargs):
    kwargs.setdefault("SensitiveDetectorMasterTool","EmptySensitiveDetectorMasterTool")
    kwargs.setdefault("FastSimulationMasterTool"   ,"EmptyFastSimulationMasterTool"   )
    return getSimHitService(name, **kwargs)

def getPileupSimHitService(name="ISF_PileupSimHitService", **kwargs):
    kwargs.setdefault('SeparateInDetPileupHits'    , True )
    return getNoG4SimHitService(name, **kwargs)

def getISFEnvelopeDefSvc(name="ISF_ISFEnvelopeDefSvc", **kwargs):
    # ATLAS common envlope definitions
    kwargs.setdefault("ATLASEnvelopeDefSvc"    , "AtlasGeometry_EnvelopeDefSvc")

    from ISF_Services.ISF_ServicesConf import ISF__ISFEnvelopeDefSvc
    return ISF__ISFEnvelopeDefSvc(name, **kwargs)

def getAFIIEnvelopeDefSvc(name="ISF_AFIIEnvelopeDefSvc", **kwargs):
    from AthenaCommon.SystemOfUnits import mm
    # ATLAS common envlope definitions
    kwargs.setdefault("ISFEnvelopeDefSvc"    , "ISF_ISFEnvelopeDefSvc"         )
    kwargs.setdefault("InDetMaxExtentZ"      , 3549.5*mm                       )

    from ISF_Services.ISF_ServicesConf import ISF__AFIIEnvelopeDefSvc
    return ISF__AFIIEnvelopeDefSvc(name, **kwargs)


def getGeoIDSvc(name="ISF_GeoIDSvc", **kwargs):
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc"          , "ISF_ISFEnvelopeDefSvc"      )

    from ISF_Services.ISF_ServicesConf import ISF__GeoIDSvc
    return ISF__GeoIDSvc(name, **kwargs)


def getAFIIGeoIDSvc(name="ISF_AFIIGeoIDSvc", **kwargs):
    kwargs.setdefault("EnvelopeDefSvc"          , "ISF_AFIIEnvelopeDefSvc"     )
    return getGeoIDSvc(name, **kwargs)
