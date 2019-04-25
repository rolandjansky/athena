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
tacc.popPrivateTools()
acc1=SCT_DigitizationToolCfg(ConfigFlags)
acc1.popPrivateTools()
tacc.merge(acc1)
acc2=SCT_DigitizationToolHSCfg(ConfigFlags)
acc2.popPrivateTools()
tacc.merge(acc2)
acc3=SCT_DigitizationToolPUCfg(ConfigFlags)
acc3.popPrivateTools()
tacc.merge(acc3)
acc4=SCT_DigitizationToolOverlayCfg(ConfigFlags)
acc4.popPrivateTools()
tacc.merge(acc4)
acc6=SCT_DigitizationToolSplitNoMergePUCfg(ConfigFlags)
acc6.popPrivateTools()
tacc.merge(acc6)
acc7=SCT_DigitizationToolGeantinoTruthCfg(ConfigFlags)
acc7.popPrivateTools()
tacc.merge(acc7)
tool = SCT_RandomDisabledCellGeneratorCfg(ConfigFlags)
tool = SCT_AmpCfg(ConfigFlags)
acc8=SCT_SurfaceChargesGeneratorCfg(ConfigFlags)
acc8.popPrivateTools()
tacc.merge(acc8)
acc9=SCT_FrontEndCfg(ConfigFlags)
acc9.popPrivateTools()
tacc.merge(acc9)
acc10=SCT_FrontEndPileupCfg(ConfigFlags)
acc10.popPrivateTools()
tacc.merge(acc10)
tool = SCT_RangeCfg(ConfigFlags)
acc5=SCT_DigitizationHSCfg(ConfigFlags)
acc5.popPrivateTools()
tacc.merge(acc5)
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

