#!/usr/bin/env python
"""Run tests on PixelGeoModel configuration

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.lock()

    if ConfigFlags.Common.Project == "AthSimulation":
        from PixelGeoModel.PixelGeoModelConfig import PixelSimulationGeometryCfg
        acc = PixelSimulationGeometryCfg(ConfigFlags)
        f=open('PixelSimulationGeometryCfg.pkl','wb')
    else:
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        acc = PixelReadoutGeometryCfg(ConfigFlags)
        f=open('PixelReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
