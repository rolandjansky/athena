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

    configFlags.Input.Files = [args.inputevntfile]
    
    configFlags.Output.HITSFileName = args.outputhitsfile

    configFlags.Sim.CalibrationRun = "Off"
    configFlags.Sim.RecordStepInfo = False
    configFlags.Sim.CavernBG = "Signal"
    configFlags.Sim.ISFRun = False
    configFlags.Sim.BeamPipeSimMode = 'FastSim'
    configFlags.Sim.ReleaseGeoModel = False

    configFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-15"
    configFlags.GeoModel.Align.Dynamic = False
    configFlags.GeoModel.AtlasVersion = args.geometrytag

    configFlags.Input.RunNumber = [284500]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumber = [1]
    
def printAndRun(accessor, configFlags, args):
    """debugging and execution"""
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
    #configFlags.Detector.GeometryBpipe = True #things seem to misbehave (hang on first event) if there is no beampipe...
    configFlags.Detector.GeometryMuon  = False #Not sure why this is there by default... and crashes if present :-(
    configFlags.Detector.GeometryMM  = False #Not sure why this is there by default... does no harm though
    configFlags.Detector.GeometrysTGC  = False #Not sure why this is there by default... does no harm though
    if args.simulate:
        configFlags.Detector.SimulateBpipe = True
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

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    acc.merge(BeamEffectsAlgCfg(configFlags))
    
    return acc


# Argument parsing
parser = ArgumentParser("ITkTest.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
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
parser.add_argument("--geometrytag",default="ATLAS-P2-ITK-22-02-00", type=str,
                    help="The geometry tag to use")
parser.add_argument("--inputevntfile",
                    default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1",
                    help="The input EVNT file to use")
parser.add_argument("--outputhitsfile",default="myHITS.pool.root", type=str,
                    help="The output HITS filename")
args = parser.parse_args()


# Some info about the job
print("----ITkTest----")
print()
print("Using Geometry Tag: "+args.geometrytag)
if args.localgeo:
    print("...overridden by local Geometry Xml files")
print("Input EVNT File:"+args.inputevntfile)
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

# Configure
defaultTestFlags(ConfigFlags, args)
ConfigFlags.lock()

# Construct our accumulator to run
acc = ITkTestCfg(ConfigFlags)
if args.simulate:
    from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
    acc.merge(G4AtlasAlgCfg(ConfigFlags))


# dump pickle
with open("ITkTest.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
