#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import ProductionStep


def AtlasGeometryCfg (flags):
    acc = ComponentAccumulator()
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    acc.merge(BeamPipeGeometryCfg(flags))
    if flags.Common.ProductionStep == ProductionStep.Simulation:
        from PixelGeoModel.PixelGeoModelConfig import PixelSimulationGeometryCfg
        acc.merge(PixelSimulationGeometryCfg(flags))
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_SimulationGeometryCfg
        acc.merge(SCT_SimulationGeometryCfg(flags))
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_SimulationGeometryCfg
        acc.merge(TRT_SimulationGeometryCfg(flags))
    else:
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        acc.merge(PixelReadoutGeometryCfg(flags))
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        acc.merge(SCT_ReadoutGeometryCfg(flags))
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        acc.merge(TRT_ReadoutGeometryCfg(flags))
    from InDetServMatGeoModel.InDetServMatGeoModelConfig import InDetServiceMaterialCfg
    acc.merge(InDetServiceMaterialCfg(flags))
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))
    #from AtlasGeoModel.ForDetGeoModelConfig import ForDetGeometryCfg
    #acc.merge(ForDetGeometryCfg(flags))
    from AtlasGeoModel.CavernGMConfig import CavernGeometryCfg
    acc.merge(CavernGeometryCfg(flags))
    return acc
