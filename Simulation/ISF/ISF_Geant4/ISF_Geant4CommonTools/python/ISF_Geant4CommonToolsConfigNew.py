"""ComponentAccumulator Geant4 tools config for ISF

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg, AFIIGeoIDSvcCfg
from ISF_Tools.ISF_ToolsConfigNew import EntryLayerFilterCfg


def EntryLayerToolCfg(flags, name="ISF_EntryLayerTool", **kwargs):
    result = ComponentAccumulator()
    kwargs["GeoIDSvc"] = result.getPrimaryAndMerge(GeoIDSvcCfg(flags)).name

    filt = result.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.GeoModel.Run in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        kwargs.setdefault("CaloEntryVolumeString", "IDET::IDET")
    else:
        kwargs.setdefault("CaloEntryVolumeString", "ITK::ITK")

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        result.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc.name) # For Fast Chain

    result.setPrivateTools(CompFactory.ISF.EntryLayerTool(name, **kwargs))
    return result


def EntryLayerToolMTCfg(flags, name="ISF_EntryLayerToolMT", **kwargs):
    result = ComponentAccumulator()
    kwargs["GeoIDSvc"] = result.getPrimaryAndMerge(GeoIDSvcCfg(flags)).name

    filt = result.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.GeoModel.Run in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        kwargs.setdefault("CaloEntryVolumeString", "IDET::IDET")
    else:
        kwargs.setdefault("CaloEntryVolumeString", "ITK::ITK")

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        result.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc.name) # For Fast Chain

    result.setPrivateTools(CompFactory.ISF.EntryLayerToolMT(name, **kwargs))
    return result


def AFIIEntryLayerToolCfg(flags, name="ISF_AFIIEntryLayerTool", **kwargs):
    result = ComponentAccumulator()
    kwargs["GeoIDSvc"] = result.getPrimaryAndMerge(AFIIGeoIDSvcCfg(flags)).name

    filt = result.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.GeoModel.Run in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        kwargs.setdefault("CaloEntryVolumeString", "IDET::IDET")
    else:
        kwargs.setdefault("CaloEntryVolumeString", "ITK::ITK")

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        result.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc.name) # For Fast Chain

    result.setPrivateTools(CompFactory.ISF.EntryLayerTool(name, **kwargs))
    return result


def AFIIEntryLayerToolMTCfg(flags, name="ISF_AFIIEntryLayerToolMT", **kwargs):
    result = ComponentAccumulator()
    kwargs["GeoIDSvc"] = result.getPrimaryAndMerge(AFIIGeoIDSvcCfg(flags)).name

    filt = result.popToolsAndMerge(EntryLayerFilterCfg(flags))
    kwargs.setdefault("ParticleFilters", [filt])

    if flags.GeoModel.Run in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
        kwargs.setdefault("CaloEntryVolumeString", "IDET::IDET")
    else:
        kwargs.setdefault("CaloEntryVolumeString", "ITK::ITK")

    if flags.Sim.DoFullChain:
        OEsvc = CompFactory.StoreGateSvc("OriginalEvent_SG")
        result.addService(OEsvc)
        kwargs.setdefault("EvtStore", OEsvc.name) # For Fast Chain

    result.setPrivateTools(CompFactory.ISF.EntryLayerToolMT(name, **kwargs))
    return result
