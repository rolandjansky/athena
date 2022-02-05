#!/usr/bin/env python
"""Run tests on SCT_GeoModel configuration

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
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_SimulationGeometryCfg
        acc = SCT_SimulationGeometryCfg(ConfigFlags)
        f=open('SCT_SimulationGeometryCfg.pkl','wb')
    else:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        acc = SCT_ReadoutGeometryCfg(ConfigFlags)
        f=open('SCT_ReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
