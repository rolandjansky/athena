#!/usr/bin/env python
"""Run tests on PixelGeoModel configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.Detector.SimulatePixel = False
    ConfigFlags.Detector.SimulateSCT   = False
    ConfigFlags.Detector.SimulateTRT   = False
    ConfigFlags.GeoModel.Align.Dynamic    = False
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    acc = PixelGeometryCfg(ConfigFlags)
    f=open('PixelGeometryCfg.pkl','w')
    acc.store(f)
    f.close()
