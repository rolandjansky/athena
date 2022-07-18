# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg    
from AthenaCommon.Logging import log

## Just enable ID for the moment.
ConfigFlags.Input.isMC             = True

ConfigFlags.Input.Files = []

ConfigFlags.ITk.Geometry.AllLocal = False
if ConfigFlags.ITk.Geometry.AllLocal:
  detectors = [
    "ITkPixel",
    "ITkStrip",
    "Bpipe"
  ]
  from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
  setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)
  ConfigFlags.TrackingGeometry.MaterialSource = "Input"


ConfigFlags.GeoModel.AtlasVersion = "ATLAS-P2-RUN4-01-00-00"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
ConfigFlags.GeoModel.Align.Dynamic = False

ConfigFlags.Detector.GeometryCalo = False
ConfigFlags.Detector.GeometryMuon = False

# This should run serially for the moment.
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1

log.debug('Lock config flags now.')
ConfigFlags.lock()

log.debug('dumping config flags now.')
ConfigFlags.dump()

cfg=MainServicesCfg(ConfigFlags)    

from InDetAlignGenAlgs.InDetAlignAlgsConfig import CreateITkMisalignAlgCfg
topoAcc=CreateITkMisalignAlgCfg(ConfigFlags)

cfg.merge(topoAcc)

cfg.printConfig()

cfg.run(10)
f=open("CreateMisalignmentITkConfig.pkl","wb")
cfg.store(f)
f.close()
