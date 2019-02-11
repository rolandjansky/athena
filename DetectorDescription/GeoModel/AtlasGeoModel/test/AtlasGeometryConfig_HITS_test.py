#!/usr/bin/env python
"""Run a test on Atlas Geometry configuration using a HITS file as input

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    import os
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    # Provide MC input
    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.GeoModel.Align.Dynamic    = False
    ConfigFlags.lock()

    # Construct ComponentAccumulator
    acc = MainServicesSerialCfg()
    acc.merge(PoolReadCfg(ConfigFlags))
    acc.merge(AtlasGeometryCfg(ConfigFlags))
    #acc.getService("StoreGateSvc").Dump=True
    acc.getService("ConditionStore").Dump=True
    acc.printConfig(withDetails=True)
    f=open('AtlasGeoModelCfg_HITS.pkl','w')
    acc.store(f)
    f.close()
    ConfigFlags.dump()
    # Execute and finish
    acc.run(maxEvents=3)
