#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
A small utility to deal with pickled files.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: pickleTool.py 216126 2009-09-29 16:12:59Z atlidbs $'
__usage__   = 'pickleTool [options] arg1 [arg2]'


import sys, pickle, pprint

# Option parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-c', '--create', dest='create', action='store_true', default=False, help='Pickle input file')
parser.add_option('-p', '--pretty', dest='pretty', action='store_true', default=False, help='Pretty-print/write data')
(options,args) = parser.parse_args()

if options.create:
    if len(args)<2:
        parser.error('missing input or output file')
    inFile = open(args[0], 'r')
    data = inFile.read()
    inFile.close()
    object = eval(data)
    outFile = open(args[1], 'w')
    pickle.dump(object,outFile)
    outFile.close()

else:
    if len(args)<1:
        parser.error('missing input file')
    inFile = open(args[0], 'r')
    data = pickle.load(inFile)
    inFile.close()
    if options.pretty:
        s = pprint.pformat(data)
    else:
        s = repr(data)
    if len(args)>1:
        outFile = open(args[1], 'w')
        outFile.write(s)
        outFile.write('\n')
        outFile.close()
    else:
        print s
