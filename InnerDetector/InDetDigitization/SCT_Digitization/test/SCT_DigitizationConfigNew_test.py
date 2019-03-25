#!/usr/bin/env python
"""Run tests on SCT_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from SCT_Digitization.SCT_DigitizationConfigNew import (
    SCT_RandomDisabledCellGeneratorCfg, SCT_AmpCfg, SCT_SurfaceChargesGeneratorCfg,
    SCT_FrontEndCfg, SCT_FrontEndPileupCfg, SCT_DigitizationCommonCfg, SCT_DigitizationToolCfg,
    SCT_DigitizationToolGeantinoTruthCfg, SCT_DigitizationToolHSCfg, SCT_DigitizationToolPUCfg,
    SCT_DigitizationToolSplitNoMergePUCfg, SCT_DigitizationToolOverlayCfg, SCT_DigitizationHSCfg,
    SCT_DigitizationPUCfg, SCT_DigitizationOverlayCfg, SCT_RangeCfg,
)
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.lock()
# Function tests
tacc = SCT_DigitizationCommonCfg(ConfigFlags)
tacc.merge(SCT_DigitizationToolCfg(ConfigFlags))
tacc.merge(SCT_DigitizationToolHSCfg(ConfigFlags))
tacc.merge(SCT_DigitizationToolPUCfg(ConfigFlags))
tacc.merge(SCT_DigitizationToolOverlayCfg(ConfigFlags))
tacc.merge(SCT_DigitizationToolSplitNoMergePUCfg(ConfigFlags))
tacc.merge(SCT_DigitizationToolGeantinoTruthCfg(ConfigFlags))
tool = SCT_RandomDisabledCellGeneratorCfg(ConfigFlags)
tool = SCT_AmpCfg(ConfigFlags)
tacc.merge(SCT_SurfaceChargesGeneratorCfg(ConfigFlags))
tacc.merge(SCT_FrontEndCfg(ConfigFlags))
tacc.merge(SCT_FrontEndPileupCfg(ConfigFlags))
tool = SCT_RangeCfg(ConfigFlags)
tacc.merge(SCT_DigitizationHSCfg(ConfigFlags))
tacc.merge(SCT_DigitizationPUCfg(ConfigFlags))
tacc.merge(SCT_DigitizationOverlayCfg(ConfigFlags))
# reset to prevent errors on deletion
tacc.__init__()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(SCT_DigitizationHSCfg(ConfigFlags))
# Add configuration to write HITS pool file
outConfig = OutputStreamCfg(ConfigFlags, "RDO",
                            ItemList=["InDet::SiClusterContainer#*", "SCT_RDO_Container#*"])
acc.merge(outConfig)
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
acc.run(maxEvents=3)

