#!/usr/bin/env python
"""Run tests on BeamPipeGeoModel configuration

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.Detector.SimulatePixel = False
    ConfigFlags.Detector.SimulateSCT   = False
    ConfigFlags.Detector.SimulateTRT   = False
    ConfigFlags.GeoModel.Align.Dynamic    = False
    ConfigFlags.lock()

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    acc = BeamPipeGeometryCfg(ConfigFlags)
    f=open('BeamPipeGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
