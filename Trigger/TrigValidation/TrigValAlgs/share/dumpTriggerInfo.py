#!/bin/env python
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior = 1
# Set the Athena configuration flags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.fillFromArgs()
ConfigFlags.lock()


from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
cfg = MainServicesCfg(ConfigFlags)
cfg.merge(PoolReadCfg(ConfigFlags))

from  AthenaMonitoring.TriggerInterface import getTrigDecisionTool

cfg.merge(getTrigDecisionTool(ConfigFlags))
checker = CompFactory.TrigDecisionChecker()
checker.WriteEventDecision = True
checker.WriteOutFilename = "trigger.counts.log"

cfg.addEventAlgo(checker)
cfg.run()