#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags

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
    if (flags.Detector.Simulate and flags.Beam.Type == "cosmics") or flags.Detector.SimulateCavern:
        from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
        gms.DetectorTools += [ CavernInfraDetectorTool() ]
    return acc
