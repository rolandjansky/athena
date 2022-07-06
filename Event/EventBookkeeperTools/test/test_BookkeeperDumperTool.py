#!/usr/bin/env python
"""Dump CutBookkeepers

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys
from argparse import ArgumentParser

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

from EventBookkeeperTools.EventBookkeeperToolsConfig import BookkeeperDumperToolCfg

# Argument parsing
parser = ArgumentParser(prog='test_BookkeeperDumpertool')
parser.add_argument('input', type=str, nargs='?',
                    help='Specify the input file')
args = parser.parse_args()

if args.input:
    ConfigFlags.Input.Files = [args.input]
else:
    ConfigFlags.Input.Files = defaultTestFiles.AOD_MC
ConfigFlags.lock()

# Setup tools
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(BookkeeperDumperToolCfg(ConfigFlags))

# Execute and finish
sc = acc.run(maxEvents=1)

# Success should be 0
sys.exit(not sc.isSuccess())
