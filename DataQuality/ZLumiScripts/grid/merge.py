#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Simple script to invoke DQHistogramMerge for merging Grid output

from __future__ import print_function

import commands, argparse, sys
parser = argparse.ArgumentParser()
parser.add_argument('-o', help='outfilename')
parser.add_argument('infiles', nargs='+', help='infilenames')
args=parser.parse_args()

with open('merge.txt', 'w') as txtfile:
    for f in args.infiles:
        txtfile.write(f + '\n')

status, output = commands.getstatusoutput('DQHistogramMerge.py merge.txt %s' % args.o)
print(output)
sys.exit(status)

