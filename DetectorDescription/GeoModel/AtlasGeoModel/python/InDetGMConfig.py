#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def InDetServiceMaterialCfg (flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc,geoModelSvc = GeoModelCfg( flags )
    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    servMatTool = InDetServMatTool()
    if flags.GeoModel.Run=="RUN4":
        from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatBuilderToolSLHC
        InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
        acc.addPublicTool( InDetServMatBuilderToolSLHC )
        servMatTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    geoModelSvc.DetectorTools += [ servMatTool ]
    acc.addService(geoModelSvc)
    return acc


def InDetGeometryCfg (flags):
    acc = ComponentAccumulator()
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    acc.merge(PixelGeometryCfg( flags ))
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    acc.merge(SCT_GeometryCfg( flags ))
    if not flags.GeoModel.Run=="RUN4":
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
        acc.merge(TRT_GeometryCfg( flags ))
    acc.merge(InDetServiceMaterialCfg( flags ))
    return acc


if __name__ == "__main__":
  import os
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  # Set up logging and new style config
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = True
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  # Provide MC input
  ConfigFlags.Input.Files = defaultTestFiles.HITS
  ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
  ConfigFlags.Detector.SimulatePixel = True
  ConfigFlags.Detector.SimulateSCT   = True
  ConfigFlags.Detector.SimulateTRT   = True
  ConfigFlags.GeoModel.Align.Dynamic    = False
  # Provide data input
  ##from AthenaConfiguration.TestDefaults import defaultTestFiles
  ##ConfigFlags.Input.Files = defaultTestFiles.AOD
  ##ConfigFlags.GeoModel.Align.Dynamic    = True
  #
  ConfigFlags.lock()
  # Construct ComponentAccumulator
  acc = MainServicesSerialCfg()
  acc.merge(PoolReadCfg(ConfigFlags))
  acc.merge(InDetGeometryCfg(ConfigFlags)) # FIXME This sets up the whole ID geometry would be nicer just to set up min required
  #acc.getService("StoreGateSvc").Dump=True
  acc.getService("ConditionStore").Dump=True
  acc.printConfig(withDetails=True)
  f=open('InDetGMCfg2.pkl','w')
  acc.store(f)
  f.close()
  ConfigFlags.dump()
  # Execute and finish
  acc.run(maxEvents=3)
