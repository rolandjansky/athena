#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetConfig.InDetRecToolConfig import SCT_CablingToolCfg, SCT_ConfigurationConditionsToolCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg

def SCT_RodDecoderCfg(flags, prefix="InDet", suffix="", **kwargs):
    acc = ComponentAccumulator()
    acc.merge(SCT_GeometryCfg(flags))
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))
    kwargs.setdefault("ConfigTool", acc.popToolsAndMerge(SCT_ConfigurationConditionsToolCfg(flags)))
    acc.setPrivateTools(CompFactory.SCT_RodDecoder(name=prefix+"SCTRodDecoder"+suffix,
                                                   **kwargs))
    return acc

def SCTRawDataProviderToolCfg(flags, prefix="InDet", suffix="", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("Decoder", acc.popToolsAndMerge(SCT_RodDecoderCfg(flags, prefix=prefix, suffix=suffix)))
    acc.setPrivateTools(CompFactory.SCTRawDataProviderTool(name=prefix+"SCTRawDataProviderTool"+suffix,
                                                           **kwargs))
    return acc

def SCTRawDataProviderCfg(flags, prefix="InDet", suffix="", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ProviderTool", acc.popToolsAndMerge(SCTRawDataProviderToolCfg(flags, prefix, suffix)))
    acc.addEventAlgo(CompFactory.SCTRawDataProvider(name=prefix+"SCTRawDataProvider"+suffix,
                                                    **kwargs))
    return acc

def SCTEventFlagWriterCfg(flags, prefix="InDet", suffix="", **kwargs):
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.SCTEventFlagWriter(name=prefix+"SCTEventFlagWriter"+suffix,
                                                    **kwargs))
    return acc
