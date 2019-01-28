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
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from BCM_Digitization.BCM_DigitizationConfigFlags import createBCMCfgFlags
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from TrigUpgradeTest.InDetConfig import InDetGMConfig # FIXME This module would ideally be located somewhere else
# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Provide input
dataDir = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art"
inputDir = os.environ.get("ATLAS_REFERENCE_DATA", dataDir)
fileDir = "/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
ConfigFlags.Input.Files = [inputDir + fileDir]
# Specify output
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.lock()
# Construct ComponentAccumulator
cfg = MainServicesSerialCfg()
cfg.merge(PoolReadCfg(ConfigFlags))
cfg.merge(InDetGMConfig(ConfigFlags)) # FIXME This sets up the whole ID geometry would be nicer just to set up min required for BCM
# Use BCM tools
BCMflags = createBCMCfgFlags()
acc = BCM_DigitizationCfg(BCMflags)
cfg.merge(acc)
# Add configuration to write HITS pool file
outConfig = OutputStreamCfg(ConfigFlags, "RDO",
    ItemList=["InDetSimDataCollection#*", "BCM_RDO_Container#*"])
cfg.merge(outConfig)
cfg.getService("StoreGateSvc").Dump=True
cfg.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
cfg.run(maxEvents=3)

