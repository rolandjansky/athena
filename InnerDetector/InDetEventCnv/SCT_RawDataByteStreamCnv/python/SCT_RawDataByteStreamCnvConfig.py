#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetConfig.InDetRecToolConfig import SCT_CablingToolCfg, SCT_ConfigurationConditionsToolCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg

def SCT_RodDecoderCfg(flags):
    acc = ComponentAccumulator()
    acc.merge(SCT_GeometryCfg(flags))
    SCT_CablingTool = acc.popToolsAndMerge(SCT_CablingToolCfg(flags))
    SCT_ConfigurationConditionsTool = acc.popToolsAndMerge(SCT_ConfigurationConditionsToolCfg(flags))
    acc.setPrivateTools(CompFactory.SCT_RodDecoder(name="InDetSCTRodDecoder",
                                                   SCT_CablingTool=SCT_CablingTool,
                                                   ConfigTool=SCT_ConfigurationConditionsTool))
    return acc

def SCTRawDataProviderToolCfg(flags):
    acc = ComponentAccumulator()
    InDetSCTRodDecoder = acc.popToolsAndMerge(SCT_RodDecoderCfg(flags))
    acc.setPrivateTools(CompFactory.SCTRawDataProviderTool(name="InDetSCTRawDataProviderTool",
                                                           Decoder=InDetSCTRodDecoder))
    return acc

def SCTRawDataProviderCfg(flags):
    acc = ComponentAccumulator()
    InDetSCTRawDataProviderTool = acc.popToolsAndMerge(SCTRawDataProviderToolCfg(flags))
    acc.addEventAlgo(CompFactory.SCTRawDataProvider(name="InDetSCTRawDataProvider",
                                                    ProviderTool=InDetSCTRawDataProviderTool))
    return acc

def SCTEventFlagWriterCfg(flags):
    acc = ComponentAccumulator()
    acc.addEventAlgo(CompFactory.SCTEventFlagWriter(name="InDetSCTEventFlagWriter"))
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
