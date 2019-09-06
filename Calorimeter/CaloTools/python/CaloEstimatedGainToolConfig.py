# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: CaloTools/python/CaloEstimatedGainToolConfig.py
# Created: Aug 2019, sss
# Purpose: Configure CaloEstimatedGainTool.
#


from __future__ import print_function
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloEstimatedGainToolCfg (configFlags):
    result = ComponentAccumulator()

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    result.merge (GeoModelCfg (configFlags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    
    result.merge(LArGMCfg(configFlags))
    result.merge(TileGMCfg(configFlags))

    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg 
    result.merge(LArFebRodMappingCfg(configFlags))
    result.merge(LArCalibIdMappingCfg(configFlags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    result.merge (TileInfoLoaderCfg (configFlags))

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    acc = TileCondToolEmscaleCfg (configFlags)
    emscaleTool = acc.popPrivateTools()
    result.merge (acc)

    from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
    acc = TileCondToolNoiseSampleCfg (configFlags)
    noiseSampleTool = acc.popPrivateTools()
    result.merge (acc)

    from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
    result.merge (LArADC2MeVCondAlgCfg (configFlags))
    adc2mev = result.getCondAlgo ('LArADC2MeVCondAlg')

    from TileConditions.TileConditionsConf import TileCondIdTransforms

    from CaloTools.CaloToolsConf import CaloEstimatedGainTool
    tool = CaloEstimatedGainTool ('CaloEstimatedGainTool',
                                  ADC2MeVKey = adc2mev.LArADC2MeVKey,
                                  TileCondIdTransforms = TileCondIdTransforms(),
                                  TileCondToolEmscale = emscaleTool,
                                  TileCondToolNoiseSample = noiseSampleTool)

    result.setPrivateTools (tool)

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags (quiet = True)

    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RDO
    flags1.lock()
    acc1 = CaloEstimatedGainToolCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    print ('ComponentAccumulator:', acc1.popPrivateTools())
    acc1.wasMerged()
