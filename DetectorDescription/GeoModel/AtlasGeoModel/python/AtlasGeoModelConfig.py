#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AtlasGeometryCfg (flags):
    acc = ComponentAccumulator()
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    acc.merge(BeamPipeGeometryCfg(flags))
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    acc.merge(PixelGeometryCfg(flags))
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    acc.merge(SCT_GeometryCfg(flags))
    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    acc.merge(TRT_GeometryCfg(flags))
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
