#!/usr/bin/env python
"""Run tests on TRT_GeoModel configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.lock()

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    acc = TRT_GeometryCfg(ConfigFlags)
    f=open('TRT_GeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
