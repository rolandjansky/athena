#!/usr/bin/env python
"""Run tests for FastCaloSimServices configuration

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
# log.setLevel(DEBUG)

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import FastCaloSimV2ParamSvcCfg, FastCaloSimV2SvcCfg, DNNCaloSimSvcCfg, FastCaloSimPileupOTSvcCfg, FastCaloToolBaseCfg

# Set up new style config
Configurable.configurableRun3Behavior = True

# Configure
ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1'] #defaultTestFiles.EVNT
ConfigFlags.Output.HITSFileName = "myHITSnew.pool.root"
ConfigFlags.Sim.CalibrationRun = "Off"
ConfigFlags.Sim.RecordStepInfo = False
ConfigFlags.Sim.CavernBG = "Signal"
ConfigFlags.Sim.ISFRun = True
ConfigFlags.Sim.BeamPipeSimMode = 'FastSim'
ConfigFlags.Sim.ReleaseGeoModel = False
ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2015-03-01-00'

ConfigFlags.lock()

# Function tests
accAlg = FastCaloSimV2ParamSvcCfg(ConfigFlags)
accAlg.__init__()

accAlg = FastCaloSimV2SvcCfg(ConfigFlags)
accAlg.__init__()

accAlg = DNNCaloSimSvcCfg(ConfigFlags)
accAlg.__init__()

# # Construct our accumulator to run
# acc = MainServicesCfg(ConfigFlags)
# acc.merge(PoolReadCfg(ConfigFlags))
# acc.merge(PoolWriteCfg(ConfigFlags))

# # Dump config
# accAlg.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName="FCS_TestConfig.txt"))
# accAlg.getService("StoreGateSvc").Dump = True
# accAlg.getService("ConditionStore").Dump = True
# accAlg.printConfig(withDetails=True, summariseProps = True)

# ConfigFlags.dump()

# # Execute and finish
# sc = accAlg.run(maxEvents=4)

# # Print and run
# f = open("test.pkl","wb")
# accAlg.store(f)
# f.close()

