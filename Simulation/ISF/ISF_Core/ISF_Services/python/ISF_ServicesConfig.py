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


def getParticleBrokerSvc(name="ISF_ParticleBrokerSvc", **kwargs):
    kwargs.setdefault('StackFiller'                 , getPublicTool('ISF_StackFiller')                     )
    kwargs.setdefault('EntryLayerTool'              , getPublicTool('ISF_EntryLayerTool')                  )
    #kwargs.setdefault('ParticleOrderingTool'        , getPublicTool('ISF_InToOutSubDetOrderingTool')       )
    kwargs.setdefault('ParticleOrderingTool'        , getPublicTool('ISF_ParticleOrderingTool')            )
    kwargs.setdefault('GeoIDSvc'                    , getService('ISF_GeoIDSvc')                           )
    kwargs.setdefault('AlwaysUseGeoIDSvc'           , False                                                )
    kwargs.setdefault('ValidateGeoIDs'              , ISF_Flags.ValidationMode()                           )
    kwargs.setdefault('ValidationOutput'            , ISF_Flags.ValidationMode()                           )
    kwargs.setdefault('ValidationStreamName'        , "ParticleBroker"                                     )

    from ISF_Services.ISF_ServicesConf import ISF__ParticleBrokerDynamicOnReadIn
    return ISF__ParticleBrokerDynamicOnReadIn(name, **kwargs)

def getAFIIParticleBrokerSvc(name="ISF_AFIIParticleBrokerSvc", **kwargs):
    kwargs.setdefault('EntryLayerTool'              , getPublicTool('ISF_AFIIEntryLayerTool')              )
    return getParticleBrokerSvc(name, **kwargs)

def getParticleBrokerSvcNoOrdering(name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    kwargs.setdefault('StackFiller'                 , getPublicTool('ISF_StackFiller')                     )
    kwargs.setdefault('EntryLayerTool'              , getPublicTool('ISF_EntryLayerTool')                  )
    kwargs.setdefault('GeoIDSvc'                    , getService('ISF_GeoIDSvc')                           )
    kwargs.setdefault('AlwaysUseGeoIDSvc'           , False                                                )
    kwargs.setdefault('ValidateGeoIDs'              , ISF_Flags.ValidationMode()                           )
    kwargs.setdefault('ValidationOutput'            , ISF_Flags.ValidationMode()                           )
    kwargs.setdefault('ValidationStreamName'        , "ParticleBroker"                                     )

    from ISF_Services.ISF_ServicesConf import ISF__ParticleBrokerDynamicOnReadIn
    return ISF__ParticleBrokerDynamicOnReadIn(name, **kwargs)

def getSimHitService(name="ISF_SimHitService", **kwargs):
    kwargs.setdefault('ValidationOutput'    , ISF_Flags.ValidationMode())

    from G4AtlasApps.SimFlags import simFlags

    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun.statusOn:

        if ( simFlags.CalibrationRun.get_Value() == 'Tile' or \
             simFlags.CalibrationRun.get_Value() == 'LAr+Tile' ) :
            kwargs.setdefault('DoTileCalibHits', True)

    from ISF_Services.ISF_ServicesConf import ISF__SimHitSvc
    return ISF__SimHitSvc(name, **kwargs)

def getISFEnvelopeDefSvc(name="ISF_ISFEnvelopeDefSvc", **kwargs):
    # ATLAS common envlope definitions
    kwargs.setdefault("ATLASEnvelopeDefSvc"    , getService("AtlasGeometry_EnvelopeDefSvc"))

    from ISF_Services.ISF_ServicesConf import ISF__ISFEnvelopeDefSvc
    return ISF__ISFEnvelopeDefSvc(name, **kwargs)

def getAFIIEnvelopeDefSvc(name="ISF_AFIIEnvelopeDefSvc", **kwargs):
    from AthenaCommon.SystemOfUnits import mm
    # ATLAS common envlope definitions
    kwargs.setdefault("ISFEnvelopeDefSvc"    , getService("ISF_ISFEnvelopeDefSvc"))
    kwargs.setdefault("InDetMaxExtentZ"      , 3550.*mm                           )

    from ISF_Services.ISF_ServicesConf import ISF__AFIIEnvelopeDefSvc 
    return ISF__AFIIEnvelopeDefSvc(name, **kwargs)


def getGeoIDSvc(name="ISF_GeoIDSvc", **kwargs):
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc"          , getService("ISF_ISFEnvelopeDefSvc"))

    from ISF_Services.ISF_ServicesConf import ISF__GeoIDSvc
    return ISF__GeoIDSvc(name, **kwargs)


def getAFIIGeoIDSvc(name="ISF_AFIIGeoIDSvc", **kwargs):
    kwargs.setdefault("EnvelopeDefSvc"          , getService("ISF_AFIIEnvelopeDefSvc"))
    return getGeoIDSvc(name, **kwargs)
