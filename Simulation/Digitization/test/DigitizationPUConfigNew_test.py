#!/usr/bin/env python
"""ComponentAccumulator PileUp (PU) Digitization configuration test

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from Digitization.DigitizationSteering import DigitizationMainCfg, DigitizationMessageSvcCfg, DigitizationTestingPostInclude, setupDigitizationDetectorFlags
from Digitization.PileUpUtils import generateBackgroundInputCollections, setupPileUpFlags

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

ConfigFlags.Exec.MaxEvents = 4

ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "mc16d_ttbar.CA.RDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-20"

ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

ConfigFlags.Beam.BunchSpacing = 25
ConfigFlags.Beam.NumberOfCollisions = 20.

ConfigFlags.LAr.ROD.NumberOfCollisions = 20
ConfigFlags.LAr.ROD.nSamples = 4
ConfigFlags.LAr.ROD.DoOFCPileupOptimization = True
ConfigFlags.LAr.ROD.FirstSample = 0
ConfigFlags.LAr.ROD.UseHighestGainAutoCorr = True

ConfigFlags.Digitization.HighGainEMECIW = False

ConfigFlags.Tile.BestPhaseFromCOOL = False
ConfigFlags.Tile.correctTime = False

ConfigFlags.Digitization.PileUp = True
ConfigFlags.Digitization.DoDigiTruth = False
ConfigFlags.Digitization.RandomSeedOffset = 170

ConfigFlags.Digitization.DigiSteeringConf = 'StandardSignalOnlyTruthPileUpToolsAlg'
ConfigFlags.Digitization.DoXingByXingPileUp = True

ConfigFlags.Digitization.PU.BunchSpacing = 25
ConfigFlags.Digitization.PU.CavernIgnoresBeamInt = True
ConfigFlags.Digitization.PU.NumberOfCavern = 0.0
ConfigFlags.Digitization.PU.NumberOfHighPtMinBias = 0.2099789464
ConfigFlags.Digitization.PU.NumberOfLowPtMinBias = 80.290021063135

setupDigitizationDetectorFlags(ConfigFlags, '')

cols = generateBackgroundInputCollections(ConfigFlags, defaultTestFiles.HITS_MINBIAS_HIGH,
                       ConfigFlags.Digitization.PU.NumberOfHighPtMinBias, True)
ConfigFlags.Digitization.PU.HighPtMinBiasInputCols = cols

cols = generateBackgroundInputCollections(ConfigFlags, defaultTestFiles.HITS_MINBIAS_LOW,
                       ConfigFlags.Digitization.PU.NumberOfLowPtMinBias, True)
ConfigFlags.Digitization.PU.LowPtMinBiasInputCols = cols

setupPileUpFlags(ConfigFlags, 'RunDependentSimData.BunchTrains_MC20_2017', 'RunDependentSimData.PileUpProfile_run300000_MC20d')

ConfigFlags.lock()

# Construct our accumulator to run
acc = DigitizationMainCfg(ConfigFlags)
acc.merge(DigitizationMessageSvcCfg(ConfigFlags))

DigitizationTestingPostInclude(ConfigFlags, acc)

# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True, summariseProps=True)
ConfigFlags.dump()
# print services
from AthenaConfiguration.ComponentAccumulator import filterComponents
for s, _ in filterComponents(acc._services):
    acc._msg.info(s)
# print conditions
for s in acc._conditionsAlgs:
    acc._msg.info(s)

# Execute and finish
sc = acc.run(maxEvents=ConfigFlags.Exec.MaxEvents)
# Success should be 0
sys.exit(not sc.isSuccess())
