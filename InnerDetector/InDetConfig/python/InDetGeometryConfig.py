#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def InDetGeometryCfg (flags):
    acc = ComponentAccumulator()

    if flags.Detector.GeometryPixel:
        from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
        acc.merge(PixelGeometryCfg( flags ))

    if flags.Detector.GeometrySCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
        acc.merge(SCT_GeometryCfg( flags ))
    
    if flags.Detector.GeometryTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
        acc.merge(TRT_GeometryCfg( flags ))
    
    if flags.Detector.GeometryPixel or flags.Detector.GeometrySCT or flags.Detector.GeometryTRT:
        from InDetServMatGeoModel.InDetServMatGeoModelConfig import InDetServiceMaterialCfg
        acc.merge(InDetServiceMaterialCfg( flags ))

    if flags.Detector.GeometryITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
        acc.merge(ITkPixelGeometryCfg( flags ))

    if flags.Detector.GeometryITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
        acc.merge(ITkStripGeometryCfg( flags ))

    return acc


if __name__ == "__main__":
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  # Set up logging and new style config
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = True
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  # Provide MC input
  ConfigFlags.Input.Files = defaultTestFiles.HITS
  ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
  ConfigFlags.GeoModel.Align.Dynamic    = False
  # Provide data input
  ##from AthenaConfiguration.TestDefaults import defaultTestFiles
  ##ConfigFlags.Input.Files = defaultTestFiles.AOD
  ##ConfigFlags.GeoModel.Align.Dynamic    = True
  #
  ConfigFlags.lock()
  # Construct ComponentAccumulator
  acc = MainServicesCfg(ConfigFlags)
  acc.merge(PoolReadCfg(ConfigFlags))
  acc.merge(InDetGeometryCfg(ConfigFlags)) # FIXME This sets up the whole ID geometry would be nicer just to set up min required
  #acc.getService("StoreGateSvc").Dump=True
  acc.getService("ConditionStore").Dump=True
  acc.printConfig(withDetails=True)
  f=open('InDetGMCfg2.pkl','wb')
  acc.store(f)
  f.close()
  ConfigFlags.dump()
  # Execute and finish
  acc.run(maxEvents=3)
