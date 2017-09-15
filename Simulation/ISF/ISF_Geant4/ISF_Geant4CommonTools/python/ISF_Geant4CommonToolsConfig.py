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


def getEntryLayerTool(name="ISF_EntryLayerTool", **kwargs):
    kwargs.setdefault('GeoIDSvc'        , 'ISF_GeoIDSvc')
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('ParticleFilters' , [ simFlags.TruthStrategy.EntryLayerFilterName() ] )

    from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConf import ISF__EntryLayerTool
    return ISF__EntryLayerTool(name, **kwargs)

def getAFIIEntryLayerTool(name="ISF_AFIIEntryLayerTool", **kwargs):
    kwargs.setdefault('GeoIDSvc'        , 'ISF_AFIIGeoIDSvc')
    return getEntryLayerTool(name, **kwargs)
