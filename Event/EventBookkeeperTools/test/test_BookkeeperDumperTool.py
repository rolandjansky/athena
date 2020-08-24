#!/usr/bin/env python
"""Dump CutBookkeepers

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

from EventBookkeeperTools.EventBookkeeperToolsConfig import BookkeeperDumperToolCfg

# Argument parsing
parser = ArgumentParser(prog='dump-cbk')
parser.add_argument('input', metavar='input', type=str, nargs='?',
                    help='Specify the input file')
args = parser.parse_args()

# Setup configuration
Configurable.configurableRun3Behavior = True

if args.input:
    ConfigFlags.Input.Files = [args.input]
else:
    ConfigFlags.Input.Files = defaultTestFiles.AOD
ConfigFlags.lock()

# Setup tools
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(BookkeeperDumperToolCfg(ConfigFlags))

# Execute and finish
sc = acc.run(maxEvents=1)

# Success should be 0
sys.exit(not sc.isSuccess())
