#!/usr/bin/env python
"""Run HitAnalysis

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# Set up logging and new style config
Configurable.configurableRun3Behavior = True


# Argument parsing
parser = ArgumentParser("RunRDOAnalysis.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
parser.add_argument("--localgeo", default=False, action="store_true",
                    help="Use local geometry XML files")
parser.add_argument("-n", "--maxEvents", default=-1, type=int,
                    help="The number of events to run. -1 runs all events.")
parser.add_argument("-i", "--input",
                    help="The input RDO file to use")
args = parser.parse_args()


# Some info about the job
print("----RDOAnalysis----")
print()
if args.localgeo:
    print("Using local Geometry XML files")
print(f"Input file: {args.input}")
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

# Configure
ConfigFlags.Input.Files = [args.input]
if args.localgeo:
    ConfigFlags.ITk.Geometry.AllLocal = True
if args.detectors:
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, args.detectors, toggle_geometry=True)
ConfigFlags.lock()

# Construct our accumulator to run
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
acc = MainServicesCfg(ConfigFlags)
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
acc.merge(PoolReadCfg(ConfigFlags))
from RDOAnalysis.RDOAnalysisConfig import RDOAnalysisCfg
acc.merge(RDOAnalysisCfg(ConfigFlags))

# Execute and finish
sc = acc.run(maxEvents=args.maxEvents)

# Success should be 0
sys.exit(not sc.isSuccess())
