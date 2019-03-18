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
    SCT_DigitizationPUCfg, SCT_DigitizationOverlayCfg, SiliconRangeCfg,
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
acc1, tool = SCT_DigitizationCommonCfg(ConfigFlags)
acc2, tool = SCT_DigitizationToolCfg(ConfigFlags)
acc3, tool = SCT_DigitizationToolHSCfg(ConfigFlags)
acc4, tool = SCT_DigitizationToolPUCfg(ConfigFlags)
acc5, tool = SCT_DigitizationToolOverlayCfg(ConfigFlags)
acc6, tool = SCT_DigitizationToolSplitNoMergePUCfg(ConfigFlags)
acc7, tool = SCT_DigitizationToolGeantinoTruthCfg(ConfigFlags)
tool = SCT_RandomDisabledCellGeneratorCfg(ConfigFlags)
tool = SCT_AmpCfg(ConfigFlags)
acc8, tool = SCT_SurfaceChargesGeneratorCfg(ConfigFlags)
acc9, tool = SCT_FrontEndCfg(ConfigFlags)
accA, tool = SCT_FrontEndPileupCfg(ConfigFlags)
tool = SiliconRangeCfg(ConfigFlags)
accB = SCT_DigitizationHSCfg(ConfigFlags)
accC = SCT_DigitizationPUCfg(ConfigFlags)
accD = SCT_DigitizationOverlayCfg(ConfigFlags)
acc1.mergeAll([acc2, acc3, acc4, acc5, acc6, acc7, acc8, acc9, accA, accB, accC, accD])
acc1.wasMerged()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
from StoreGate.StoreGateConf import StoreGateSvc # FIXME remove this once athena is fixed
acc.addService(StoreGateSvc("ConditionStore"))
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

