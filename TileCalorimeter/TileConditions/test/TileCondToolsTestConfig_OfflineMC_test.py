#!/usr/bin/env python
"""Run a test on Tile conditions algorithms configuration on MC offline

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

if __name__ == "__main__":

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    from TileConditions.TileCondToolsTestConfig import TileCondToolsTestCfg
    acc.merge( TileCondToolsTestCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)

    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileCondToolsOfflineMC.pkl','wb') )

    print('All OK')
