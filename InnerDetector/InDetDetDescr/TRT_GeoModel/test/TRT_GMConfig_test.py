#!/usr/bin/env python
"""Run tests on TRT_GeoModel configuration

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
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_SimulationGeometryCfg
        acc = TRT_SimulationGeometryCfg(ConfigFlags)
        f=open('TRT_SimulationGeometryCfg.pkl','wb')
    else:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        acc = TRT_ReadoutGeometryCfg(ConfigFlags)
        f=open('TRT_ReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
