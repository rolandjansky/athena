#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ITkGeometryCfg (flags):
    acc = ComponentAccumulator()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc")) #Beampipe builder expects "InDet" rather than "ITk" - can this be steered?
    if flags.Detector.GeometryITkPixel:
        #Need new module in PixelGeoModelXml to import here once it exists...
        #from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
        #acc.merge(PixelGeometryCfg( flags ))
        print("ITk Pixel not yet supported in this release...")
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
  ConfigFlags.Detector.SimulateITkPixel = True
  ConfigFlags.Detector.SimulateITkStrip   = True
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
  acc.merge(ITkGeometryCfg(ConfigFlags)) # FIXME This sets up the whole ID geometry would be nicer just to set up min required
  #acc.getService("StoreGateSvc").Dump=True
  acc.getService("ConditionStore").Dump=True
  acc.printConfig(withDetails=True)
  f=open('ITkGMCfg2.pkl','wb')
  acc.store(f)
  f.close()
  ConfigFlags.dump()
  # Execute and finish
  acc.run(maxEvents=3)
