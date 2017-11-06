#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Extracts a luminosity block map from a dataset.

This map lists filenames from the dataset alongside the luminosity block
numbers contained in the file. It is time-consuming to read this information,
so this LB map can be passed to beamspotman's --lbmap so that you only need to
build the map once instead of for every time you submit jobs.
"""

import sys
from optparse import OptionParser
from InDetBeamSpotExample import DiskUtils

# Command line parsing
parser = OptionParser(usage="usage: %prog [options] INPUTDATA")
parser.add_option('', "--eos", dest='eos', default=False,
        help="Access files using EOS (not needed if using /eos fuse mount)")
parser.add_option('-f', "--file", dest='map_file', default='dataset.lbmap',
        help="Output path")
(options, args) = parser.parse_args()

if len(args) < 1:
    sys.exit('No dataset supplied')

backend = DiskUtils.EOS() if options.eos else DiskUtils.Local()
file_set = DiskUtils.FileSet.from_input(args[0], backend=backend)
DiskUtils.make_lumi_block_map_file(file_set, options.map_file)
