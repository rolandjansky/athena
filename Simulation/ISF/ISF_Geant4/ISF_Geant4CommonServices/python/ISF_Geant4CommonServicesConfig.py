# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon import CfgMgr

def getG4PolyconeGeoIDSvc(name="ISF_G4PolyconeGeoIDSvc", **kwargs):
    # with ISF volume definitions
    kwargs.setdefault("EnvelopeDefSvc"          , "ISF_ISFEnvelopeDefSvc")
    return CfgMgr.ISF__G4PolyconeGeoIDSvc(name, **kwargs)

def getAFIIG4PolyconeGeoIDSvc(name="ISF_AFIIG4PolyconeGeoIDSvc", **kwargs):
    kwargs.setdefault("EnvelopeDefSvc"          , "ISF_AFIIEnvelopeDefSvc")
    return getG4PolyconeGeoIDSvc(name, **kwargs)

## def getG4PolyconeGeoIDSvc_G4(name="ISF_G4PolyconeGeoIDSvc_G4", **kwargs):
##     # with G4 volume definitions
##     kwargs.setdefault("GeomDBNodeSuffix"        , 'EnvelopeG4')
##     return CfgMgr.ISF__G4PolyconeGeoIDSvc(name, **kwargs)
