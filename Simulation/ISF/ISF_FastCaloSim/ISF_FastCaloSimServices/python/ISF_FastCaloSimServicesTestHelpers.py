#!/usr/bin/env python
"""FastCaloSimServices test helpers

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from argparse import ArgumentParser
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def JobOptsDumperCfg(flags):
    """Configure event loop for FCSServices"""
    JobOptsDumperAlg = CompFactory.JobOptsDumperAlg
    acc = ComponentAccumulator()
    acc.addEventAlgo(JobOptsDumperAlg(FileName="FCSServicesTestConfig.txt"))
    return acc


def TestMessageSvcCfg(flags):
    """MessageSvc for FCSServices"""
    MessageSvc = CompFactory.MessageSvc
    acc = ComponentAccumulator()
    acc.addService(MessageSvc(setError=["HepMcParticleLink"]))
    return acc


def CommonTestArgumentParser():
    """FCSServices test argument parser"""
    parser = ArgumentParser()
    parser.add_argument("-n", "--maxEvents", default=3, type=int,
                        help="The number of events to run. 0 skips execution")
    parser.add_argument("-t", "--threads", default=1, type=int,
                        help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true",
                        help="Print full details of the AlgSequence for each accumulator")
    parser.add_argument("-S", "--verboseStoreGate", default=False, action="store_true",
                        help="Dump the StoreGate(s) each event iteration")
    parser.add_argument("-o", "--output", default='', type=str,
                        help="Output RDO file")
    parser.add_argument("-s", "--outputSig", default='', type=str,
                        help="Output RDO_SGNL file")
    return parser


def defaultTestFlags(configFlags, args):
    """Fill default FCSServices flags for testing"""

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    configFlags.Input.RunNumber = [284500]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.Files = defaultTestFiles.EVNT # ["root://eosuser.cern.ch///eos/atlas/atlascerngroupdisk/proj-simul/OutputSamples/rel21/mc16_13TeV.photon.E65536.eta20_25.EVNT.merged.pool.root"]
    configFlags.Output.HITSFileName = "myHITSnew.pool.root"

    # Sim configFlags
    # Flags taken from old config:
    configFlags.Sim.TruthStrategy = "MC15aPlus"
    configFlags.Sim.PhysicsList = "FTFP_BERT_ATL"
    configFlags.Sim.CalibrationRun = "Off" #"DeadLAr" 
    configFlags.Sim.RecordStepInfo = False
    configFlags.Sim.CavernBG = "Signal"
    configFlags.Sim.BeamPipeSimMode = 'FastSim'
    configFlags.Sim.ReleaseGeoModel = False
    configFlags.Sim.ISFRun = True
    configFlags.Sim.ISF.Simulator = "G4FastCaloMT"
    configFlags.Sim.FastCalo.ParamsInputFilename = "FastCaloSim/MC16/TFCSparam_v011.root"
    configFlags.Sim.FastCalo.CaloCellsName = "AllCalo"

    configFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-14"
    configFlags.GeoModel.Align.Dynamic = False
    configFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'

    # Set the detector flags:
    # Beam pipe
    configFlags.Detector.SimulateBpipe = True
    configFlags.Detector.GeometryBpipe = True

    # Inner detectors
    configFlags.Detector.SimulateBCM = True
    configFlags.Detector.GeometryBCM = True
    configFlags.Detector.SimulateDBM = True
    configFlags.Detector.GeometryDBM = True
    configFlags.Detector.SimulatePixel = True
    configFlags.Detector.GeometryPixel = True
    configFlags.Detector.SimulateSCT = True
    configFlags.Detector.GeometrySCT = True
    configFlags.Detector.SimulateTRT = True
    configFlags.Detector.GeometryTRT = True

    # Muon
    configFlags.Detector.SimulateMuon = True
    configFlags.Detector.GeometryMuon = True
    configFlags.Detector.SimulateMDT = True
    configFlags.Detector.GeometryMDT = True
    configFlags.Detector.SimulateRPC = True
    configFlags.Detector.GeometryRPC = True
    configFlags.Detector.SimulateTGC = True
    configFlags.Detector.GeometryTGC = True
    configFlags.Detector.SimulateCSC = True
    configFlags.Detector.GeometryCSC = True

    # LAr
    configFlags.Detector.SimulateLAr = True
    configFlags.Detector.GeometryLAr = True
    configFlags.Detector.SimulateTile = True
    configFlags.Detector.GeometryTile = True

    # Frozen showers OFF = 0
    configFlags.Sim.LArParameterization = 0

    if args.output:
        if args.output == 'None':
            configFlags.Output.RDOFileName = ''
        else:
            configFlags.Output.RDOFileName = args.output

    if args.outputSig:
        configFlags.Output.RDO_SGNLFileName = args.outputSig


def postprocessAndLockFlags(configFlags, args):
    """Postprocess and lock config flags for FCSServices"""
    # Flags relating to multithreaded execution
    configFlags.Concurrency.NumThreads = args.threads
    if args.threads > 0:
        configFlags.Scheduler.ShowDataDeps = True
        configFlags.Scheduler.ShowDataFlow = True
        configFlags.Scheduler.ShowControlFlow = True
        configFlags.Concurrency.NumConcurrentEvents = args.threads

    configFlags.lock()


def printAndRun(accessor, configFlags, args):
    """Common debugging and execution for FCSServices tests"""
    # Dump config
    if args.verboseAccumulators:
        accessor.printConfig(withDetails=True)
    if args.verboseStoreGate:
        accessor.getService("StoreGateSvc").Dump = True
    configFlags.dump()

    # Execute and finish
    sc = accessor.run(maxEvents=args.maxEvents)

    # Dump config summary
    accessor.printConfig(withDetails=False)

    # Success should be 0
    return not sc.isSuccess()
