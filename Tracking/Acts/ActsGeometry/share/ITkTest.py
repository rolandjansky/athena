#!/usr/bin/env python
"""Run ACTS geometry construction for ITk

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

ConfigFlags.GeoModel.useLocalGeometry = False



from AthenaConfiguration.Enums import ProductionStep
ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
ConfigFlags.GeoModel.AtlasVersion = "ATLAS-P2-ITK-24-00-00"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-14"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1']

ConfigFlags.Detector.GeometryITkPixel = True
ConfigFlags.Detector.GeometryITkStrip = True
ConfigFlags.Detector.GeometryBpipe = True

ConfigFlags.Concurrency.NumThreads = 64
ConfigFlags.Concurrency.NumConcurrentEvents = 64

ConfigFlags.lock()

from AthenaConfiguration.MainServicesConfig import MainServicesCfg
acc = MainServicesCfg(ConfigFlags)

from ActsGeometry.ActsGeometryConfig import ActsExtrapolationAlgCfg, ActsTrackingGeometrySvcCfg

from AthenaCommon.Constants import INFO, VERBOSE
tgSvc = ActsTrackingGeometrySvcCfg(ConfigFlags, 
                                   OutputLevel=VERBOSE, 
                                   ObjDebugOutput=True)
acc.merge(tgSvc)

alg = ActsExtrapolationAlgCfg(ConfigFlags,
                              OutputLevel=INFO,
                              NParticlesPerEvent = int(100),
                              WritePropStep = True,
                              EtaRange = [-5, 5],
                              PtRange = [20, 100])

acc.merge(alg)

acc.printConfig()



acc.run(200)
