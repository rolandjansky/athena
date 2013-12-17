#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser

p = OptionParser(usage="usage: -r <run number>", version="0.1")

p.add_option('-r', '--run',        type = 'int',         default = 165231,        dest = 'run',   help = 'run number')
p.add_option('-o', '--outputfile', type = 'string',      default = 'rob-ros.xml', dest = 'xml',   help = 'out xml file')
p.add_option(      '--useROBIN',   action='store_true',  default = False,         dest = 'robin', help = 'use ROBin cards')

(opts, args) = p.parse_args()

import TrigCostExec.TrigCostExecPython as cost

cost.writeROB2ROSXML(opts.run, opts.xml, opts.robin)
