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
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

def defaultTestFlags(configFlags, args):
    """Fill default ITk flags for testing"""
    if args.localgeo:
        configFlags.ITk.useLocalGeometry = True

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

    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    detectors = args.detectors if 'detectors' in args and args.detectors else ['ITkPixel', 'ITkStrip']
    detectors.append('Bpipe')  # always run with beam pipe
    setupDetectorsFromList(configFlags, detectors, toggle_geometry=True)


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


def LengthIntegratorUserActionSvcCfg(configFlags, name="G4UA::LengthIntegratorUserActionSvc", **kwargs):
    
    result = ComponentAccumulator()
    
    #Setting up the CA for the LengthIntegrator
    from G4UserActions.G4UserActionsConfigNew import LengthIntegratorToolCfg
    actionAcc = ComponentAccumulator()
    actions = []
    actions += [actionAcc.popToolsAndMerge(LengthIntegratorToolCfg(configFlags))]
    actionAcc.setPrivateTools(actions)
    lengthIntegratorAction = result.popToolsAndMerge(actionAcc)
    
    #Retrieving the default action list
    from G4AtlasServices.G4AtlasUserActionConfigNew import getDefaultActions
    defaultActions = result.popToolsAndMerge(getDefaultActions(configFlags))

    #Adding LengthIntegrator to defaults
    actionList = (defaultActions + lengthIntegratorAction)

    #Setting up UserActionsService
    kwargs.setdefault("UserActionTools",actionList)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))

    return result


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
parser.add_argument("--geometrytag",default="ATLAS-P2-ITK-24-00-00", type=str,
                    help="The geometry tag to use")
parser.add_argument("--inputevntfile",
                    default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1",
                    help="The input EVNT file to use")
parser.add_argument("--outputhitsfile",default="myHITS.pool.root", type=str,
                    help="The output HITS filename")
parser.add_argument("--lengthintegrator", default=False, action="store_true",
                    help="Run LengthIntegrator to produce material distribution")
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
kwargs = {}
if args.lengthintegrator:
    svcName = "G4UA::LengthIntegratorUserActionSvc"
    acc.merge(LengthIntegratorUserActionSvcCfg(ConfigFlags,svcName,**kwargs))
    kwargs.update(UserActionSvc=svcName)
if args.simulate:
    from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
    acc.merge(G4AtlasAlgCfg(ConfigFlags, "ITkG4AtlasAlg", **kwargs))

# dump pickle
with open("ITkTest.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
