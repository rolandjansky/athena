#!/usr/bin/env python
"""Run tests on ISF_ServicesConfigNew.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg

  # Set up logging
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  log.setLevel(DEBUG)


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags

  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
  detectors =['Bpipe', 'BCM', 'DBM',  'Pixel', 'SCT', 'TRT', 'LAr', 'Tile', 'CSC', 'MDT', 'RPC', 'TGC']
  # Setup detector flags
  from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
  setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

  # Finalize
  ConfigFlags.lock()

  from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, InputConverterCfg
  from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags)

  #add the algorithm
  cfg.merge(MC15aPlusTruthServiceCfg(ConfigFlags))
  cfg.merge(InputConverterCfg(ConfigFlags))
  cfg.merge(GeoIDSvcCfg(ConfigFlags))

  # Dump config
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()
