# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon import CfgMgr

def getEntryLayerTool(name="ISF_EntryLayerTool", **kwargs):
    kwargs.setdefault('GeoIDSvc'        , 'ISF_GeoIDSvc')
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('ParticleFilters' , [ simFlags.TruthStrategy.EntryLayerFilterName() ] )
    return CfgMgr.ISF__EntryLayerTool(name, **kwargs)

def getAFIIEntryLayerTool(name="ISF_AFIIEntryLayerTool", **kwargs):
    kwargs.setdefault('GeoIDSvc'        , 'ISF_AFIIGeoIDSvc')
    return getEntryLayerTool(name, **kwargs)
