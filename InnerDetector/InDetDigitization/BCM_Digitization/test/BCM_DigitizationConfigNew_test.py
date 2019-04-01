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
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from BCM_Digitization.BCM_DigitizationConfigNew import (
    BCM_RangeCfg, BCM_DigitizationToolCfg, BCM_DigitizationCfg,
    BCM_OverlayDigitizationToolCfg, BCM_OverlayDigitizationCfg,
)

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.lock()
# Function tests
tool = BCM_RangeCfg(ConfigFlags)
tacc = BCM_DigitizationToolCfg(ConfigFlags)
tacc.merge(BCM_OverlayDigitizationToolCfg(ConfigFlags))
tacc.merge(BCM_DigitizationToolCfg(ConfigFlags))
tacc.merge(BCM_OverlayDigitizationCfg(ConfigFlags))
# reset to prevent errors on deletion
tacc.__init__()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PixelGeometryCfg(ConfigFlags))
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

