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

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"]
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-03"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(ConfigFlags))

    acc.merge(SCTRawDataProviderCfg(ConfigFlags))
    acc.merge(SCTEventFlagWriterCfg(ConfigFlags))

    acc.run(maxEvents=10)
