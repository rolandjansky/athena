"""ComponentAccumulator Geant4 tools config for ISF

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg, AFIIGeoIDSvcCfg
from ISF_Tools.ISF_ToolsConfigNew import EntryLayerFilterCfg


def EntryLayerToolCfg(flags, name="ISF_EntryLayerTool", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(GeoIDSvcCfg(flags))
    kwargs["GeoIDSvc"] = acc.getService("ISF_GeoIDSvc")

    filt = acc.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        acc.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc) # For Fast Chain

    acc.addPublicTool(CompFactory.ISF.EntryLayerTool(name, **kwargs))
    return acc


def EntryLayerToolMTCfg(flags, name="ISF_EntryLayerToolMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(GeoIDSvcCfg(flags))
    kwargs["GeoIDSvc"] = acc.getService("ISF_GeoIDSvc")

    filt = acc.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        acc.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc) # For Fast Chain

    acc.addPublicTool(CompFactory.ISF.EntryLayerToolMT(name, **kwargs))
    return acc


def AFIIEntryLayerToolCfg(flags, name="ISF_AFIIEntryLayerTool", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(AFIIGeoIDSvcCfg(flags))
    kwargs["GeoIDSvc"] = acc.getService("ISF_AFIIGeoIDSvc")

    filt = acc.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        acc.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc) # For Fast Chain

    acc.addPublicTool(CompFactory.ISF.EntryLayerTool(name, **kwargs))
    return acc


def AFIIEntryLayerToolMTCfg(flags, name="ISF_AFIIEntryLayerToolMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(AFIIGeoIDSvcCfg(flags))
    kwargs["GeoIDSvc"] = acc.getService("ISF_AFIIGeoIDSvc")

    filt = acc.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        acc.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc) # For Fast Chain

    acc.addPublicTool(CompFactory.ISF.EntryLayerToolMT(name, **kwargs))
    return acc
