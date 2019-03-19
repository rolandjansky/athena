#!/usr/bin/env python
"""Run tests on BCM_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import os
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
from TrigUpgradeTest.InDetConfig import InDetGMConfig # FIXME This module would ideally be located somewhere else

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
from StoreGate.StoreGateConf import StoreGateSvc # FIXME remove this once athena is fixed
acc.addService(StoreGateSvc("ConditionStore"))
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(InDetGMConfig(ConfigFlags)) # FIXME This sets up the whole ID geometry would be nicer just to set up min required for BCM
# Add configuration to write HITS pool file
outConfig = OutputStreamCfg(ConfigFlags, "RDO",
    ItemList=["InDetSimDataCollection#*", "BCM_RDO_Container#*"])
acc.merge(outConfig)
# Dump config
acc.getService("StoreGateSvc").Dump=True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
acc.run(maxEvents=3)

