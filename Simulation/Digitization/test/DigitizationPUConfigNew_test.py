#!/usr/bin/env python
"""ComponentAccumulator PileUp (PU) Digitization configuration test

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
from DigitizationPUConfigNew_test_setup import (
    test_HighPtMinbiasHitsFile,
    test_LowPtMinbiasHitsFile,
    test_BeamIntensityPattern,
    test_fragment,
    pileUpCalc,
    makeBkgInputCol,
)

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

evtMax = 4

# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-20"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1

ConfigFlags.Beam.BunchSpacing = 25
# TODO deduplicate NumberOfCollisions?
ConfigFlags.Beam.NumberOfCollisions = 20.

ConfigFlags.Exec.MaxEvents = evtMax

ConfigFlags.LAr.ROD.NumberOfCollisions = 20
ConfigFlags.LAr.ROD.nSamples = 4
ConfigFlags.LAr.ROD.DoOFCPileupOptimization = True
ConfigFlags.LAr.ROD.FirstSample = 0
ConfigFlags.LAr.ROD.UseHighestGainAutoCorr = True

ConfigFlags.Digitization.DoXingByXingPileUp = True
ConfigFlags.Digitization.HighGainEMECIW = False
ConfigFlags.Digitization.PU.BunchSpacing = 25
ConfigFlags.Digitization.PU.CavernIgnoresBeamInt = True
ConfigFlags.Digitization.PU.NumberOfCavern = 0.0
ConfigFlags.Digitization.PU.NumberOfHighPtMinBias = 0.2099789464
ConfigFlags.Digitization.PU.NumberOfLowPtMinBias = 80.290021063135
ConfigFlags.Digitization.PU.BeamIntensityPattern = test_BeamIntensityPattern
ConfigFlags.Digitization.PU.RunAndLumiOverrideList = test_fragment

cols = makeBkgInputCol(ConfigFlags, [test_HighPtMinbiasHitsFile],
                       ConfigFlags.Digitization.PU.NumberOfHighPtMinBias, True)
ConfigFlags.Digitization.PU.HighPtMinBiasInputCols = cols

cols = makeBkgInputCol(ConfigFlags, [test_LowPtMinbiasHitsFile],
                       ConfigFlags.Digitization.PU.NumberOfLowPtMinBias, True)
ConfigFlags.Digitization.PU.LowPtMinBiasInputCols = cols


ConfigFlags.lock()

# Core components
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PoolWriteCfg(ConfigFlags))
acc.merge(writeDigitizationMetadata(ConfigFlags))

# Old EventInfo conversion
if "EventInfo" not in ConfigFlags.Input.Collections:
    acc.merge(EventInfoCnvAlgCfg(ConfigFlags,
                                 inputKey="McEventInfo",
                                 outputKey="EventInfo"))

from Digitization.RunDependentConfigNew import EvtIdModifierSvcCfg
acc.merge(EvtIdModifierSvcCfg(ConfigFlags))

from Digitization.PileUpToolsConfig import PileUpToolsCfg
from MCTruthSimAlgs.MCTruthSimAlgsConfigNew import (
    SignalOnlyMcEventCollCfg,
    MergeTruthJetsCfg,
    MergeMuonEntryLayerCfg,
    MergeCalibHitsCfg,
)

acc.merge(SignalOnlyMcEventCollCfg(ConfigFlags))
acc.merge(MergeTruthJetsCfg(ConfigFlags))
acc.merge(MergeMuonEntryLayerCfg(ConfigFlags))
acc.merge(MergeCalibHitsCfg(ConfigFlags))


# Migration of --digiSteeringConf
# TODO configure output
#acc.getEventAlgo("OutputStreamRDO").AcceptAlgs = ["StandardPileUpToolsAlg"]

# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()

# Was not merged; this prevents errors
acc.wasMerged()
# TODO uncomment running in the test once successful
# Execute and finish
#sc = acc.run(maxEvents=evtMax)
# Success should be 0
#sys.exit(not sc.isSuccess())
