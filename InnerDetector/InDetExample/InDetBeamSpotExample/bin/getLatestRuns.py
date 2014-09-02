#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Get list of runs taken in last xx seconds
"""

__version__ = ''
__usage__   = '''%prog [options]

Get list of runs taken in last xx seconds
'''
from DQUtils.oracle import fetch_recent_runs
import os

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('', '--outputDir', dest='outputDir', default='/afs/cern.ch/user/a/atlidbs/nt/DataPeriods/data_latest', help='Output directory for file with run list')
(options,args) = parser.parse_args()


if(len(args)) < 2:
   parser.error('wrong number of command line arguments')
interval = args[0]
fileLabel = args[1]

filename = options.outputDir+'/'+fileLabel+'.runs.list'

path = os.path
if path.exists(filename):
    os.system('rm ' + filename)

with open(filename, 'w') as f:
    f.write('\n'.join([str(r[1]) for r in fetch_recent_runs(int(interval))]))

