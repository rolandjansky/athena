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


def getAFIIG4PolyconeGeoIDSvc(name="ISF_AFIIG4PolyconeGeoIDSvc", **kwargs):
    kwargs.setdefault("EnvelopeDefSvc"          , getService("ISF_AFIIEnvelopeDefSvc"))
    return getG4PolyconeGeoIDSvc(name, **kwargs)


def getG4PolyconeGeoIDSvc(name="ISF_G4PolyconeGeoIDSvc", **kwargs):
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc"          , getService("ISF_ISFEnvelopeDefSvc"))

    from ISF_Geant4CommonServices.ISF_Geant4CommonServicesConf import ISF__G4PolyconeGeoIDSvc 
    return ISF__G4PolyconeGeoIDSvc(name, **kwargs)


def getG4PolyconeGeoIDSvc_G4(name="ISF_G4PolyconeGeoIDSvc_G4", **kwargs):
    # with G4 volume definitions
    kwargs.setdefault("GeomDBNodeSuffix"        , 'EnvelopeG4')

    from ISF_Geant4CommonServices.ISF_Geant4CommonServicesConf import ISF__G4PolyconeGeoIDSvc 
    return ISF__G4PolyconeGeoIDSvc(name, **kwargs)
