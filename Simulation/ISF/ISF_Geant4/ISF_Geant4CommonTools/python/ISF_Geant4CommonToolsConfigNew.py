"""ComponentAccumulator Geant4 tools config for ISF

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Services.ISF_ServicesConfigNew import GeoIDSvcCfg, AFIIGeoIDSvcCfg
from ISF_Tools.ISF_ToolsConfigNew import EntryLayerFilterCfg


def EntryLayerToolCfg(flags, name="ISF_EntryLayerTool", **kwargs):
    acc = GeoIDSvcCfg(flags)
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))
    acc_filter = EntryLayerFilterCfg(flags)
    kwargs.setdefault("ParticleFilters", [acc.popToolsAndMerge(acc_filter)])
    if flags.Sim.DoFullChain: # TODO and DetFlags.pileup.any_on():
        kwargs.setdefault("EvtStore", "OriginalEvent_SG") # For Fast Chain
    acc.setPrivateTools(CompFactory.ISF.EntryLayerTool(name, **kwargs))
    return acc


def EntryLayerToolMTCfg(flags, name="ISF_EntryLayerToolMT", **kwargs):
    acc = GeoIDSvcCfg(flags)
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))
    acc_filter = EntryLayerFilterCfg(flags)
    kwargs.setdefault("ParticleFilters", [acc.popToolsAndMerge(acc_filter)])
    if flags.Sim.DoFullChain: # TODO and DetFlags.pileup.any_on():
        kwargs.setdefault("EvtStore", "OriginalEvent_SG") # For Fast Chain
    acc.setPrivateTools(CompFactory.ISF.EntryLayerToolMT(name, **kwargs))
    return acc


def AFIIEntryLayerToolCfg(flags, name="ISF_AFIIEntryLayerTool", **kwargs):
    acc = AFIIGeoIDSvcCfg(flags)
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_AFIIGeoIDSvc"))
    acc.merge(EntryLayerToolCfg(name, **kwargs))
    return acc


def AFIIEntryLayerToolMTCfg(flags, name="ISF_AFIIEntryLayerToolMT", **kwargs):
    acc = AFIIGeoIDSvcCfg(flags)
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_AFIIGeoIDSvc"))
    acc.merge(EntryLayerToolMTCfg(name, **kwargs))
    return acc
