# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from AthExHelloWorld.HelloWorldConfig import HelloWorldCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg


def fromRunArgs(runArgs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags    

    commonRunArgsToFlags(runArgs,ConfigFlags)
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(HelloWorldCfg())
    cfg.run()
