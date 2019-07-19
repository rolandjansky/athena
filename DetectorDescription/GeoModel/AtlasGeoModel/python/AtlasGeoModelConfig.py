#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AtlasGeometryCfg (flags):
    acc = ComponentAccumulator()

    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    acc.merge(InDetGeometryCfg(flags))
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags))
    #from AtlasGeoModel.ForDetGeoModelConfig import ForDetGeometryCfg
    #acc.merge(ForDetGeometryCfg(flags))
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    acc.merge(BeamPipeGeometryCfg(flags))
    from AtlasGeoModel.CavernGMConfig import CavernGeometryCfg
    acc.merge(CavernGeometryCfg(flags))
    return acc
