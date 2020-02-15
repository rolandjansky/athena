#!/usr/bin/env python
"""Run tests for MC+MC or MC+data overlay

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from __future__ import print_function

import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from OverlayConfiguration.OverlaySteering import OverlayMainCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, defaultTestFlags, postprocessAndLockFlags, printAndRun

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("OverlayTest.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
args = parser.parse_args()

# Some info about the job
print("Overlay: {}".format("MC+data" if args.data else "MC+MC"))
print("Number of threads: {}".format(args.threads))
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

# Configure
defaultTestFlags(ConfigFlags, args)
postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = OverlayMainCfg(ConfigFlags)

# dump pickle
with open("ConfigOverlay.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
