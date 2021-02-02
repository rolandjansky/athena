#!/usr/bin/env python
"""Run tests for ITk

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from __future__ import print_function

import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

def defaultTestFlags(configFlags, args):
    """Fill default ITk flags for testing"""
    if args.localgeo:
        configFlags.ITk.useLocalGeometry = True
    setupITkDetectorFlags(configFlags, args.detectors if 'detectors' in args else None, args)

    configFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1']
    
    configFlags.Output.HITSFileName = "myHITS.pool.root"

    configFlags.Sim.CalibrationRun = "Off"
    configFlags.Sim.RecordStepInfo = False
    configFlags.Sim.CavernBG = "Signal"
    configFlags.Sim.ISFRun = False
    configFlags.Sim.BeamPipeSimMode = 'FastSim'
    configFlags.Sim.ReleaseGeoModel = False

    configFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-15"
    configFlags.GeoModel.Align.Dynamic = False
    configFlags.GeoModel.AtlasVersion = 'ATLAS-P2-ITK-22-02-00'
    
def printAndRun(accessor, configFlags, args):
    """Common debugging and execution for overlay tests"""
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



def setupITkDetectorFlags(configFlags, detectors, args):
    if not detectors or 'ITkStrip' in detectors or 'ITk' in detectors:
        configFlags.Detector.GeometryITkStrip = True
        if args.simulate:
            configFlags.Detector.SimulateITkStrip = True
    if not detectors or 'ITkPixel' in detectors or 'ITk' in detectors:
        configFlags.Detector.GeometryITkPixel = True
        if args.simulate:
            configFlags.Detector.SimulateITkPixel = True

def ITkTestCfg(configFlags):
    acc = MainServicesCfg(configFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))
    myRunNumber = 284500
    myFirstLB = 1
    myInitialTimeStamp = 1446539185
    eventSelector = acc.getService("EventSelector")
    eventSelector.OverrideRunNumber = True
    eventSelector.RunNumber = myRunNumber
    eventSelector.FirstLB = myFirstLB
    eventSelector.InitialTimeStamp = myInitialTimeStamp # Necessary to avoid a crash
    if hasattr(eventSelector, "OverrideRunNumberFromInput"):
        eventSelector.OverrideRunNumberFromInput = True
    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    acc.merge(BeamEffectsAlgCfg(configFlags))
    
    return acc


# Argument parsing
parser = ArgumentParser("ITkTest.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
parser.add_argument("--profile", default=False, action="store_true",
                    help="Profile using VTune")
parser.add_argument("--simulate", default=True, action="store_true",
                    help="Run Simulation")
parser.add_argument("--localgeo", default=False, action="store_true",
                    help="Use local geometry Xml files")
parser.add_argument("-V", "--verboseAccumulators", default=False, 
                    action="store_true",
                    help="Print full details of the AlgSequence")
parser.add_argument("-S", "--verboseStoreGate", default=False, 
                    action="store_true",
                    help="Dump the StoreGate(s) each event iteration")
parser.add_argument("--maxEvents",default=3, type=int,
                    help="The number of events to run. 0 skips execution")
args = parser.parse_args()

# Some info about the job
print()
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()
if args.profile:
    print("Profiling...")
    print()

# Configure
defaultTestFlags(ConfigFlags, args)
ConfigFlags.lock()

# Construct our accumulator to run
acc = ITkTestCfg(ConfigFlags)
if args.simulate:
    from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
    acc.merge(G4AtlasAlgCfg(ConfigFlags))
if args.profile:
    from PerfMonVTune.PerfMonVTuneConfig import VTuneProfilerServiceCfg
    acc.merge(VTuneProfilerServiceCfg(ConfigFlags))


# dump pickle
with open("ITkTest.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
