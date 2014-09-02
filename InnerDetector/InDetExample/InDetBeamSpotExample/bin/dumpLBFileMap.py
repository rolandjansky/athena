#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from optparse import OptionParser
from InDetBeamSpotExample.ExtractLBFileMap import extract

if __name__ == "__main__":

    # Command line parsing
    parser = OptionParser(usage="usage: %prog [options] dir")
    parser.add_option( "-f", "--filter", dest = "filter",  default = 'AOD',  help = "The pattern to search for files in the directory" )
    parser.add_option('', '--prefix', dest='prefix', default='', help='Prefix for reading files from mass storage (Default: determine from filename (`\'\') ')

    (options, args) = parser.parse_args()

    # Check files provided
    if not len(args): 
        print 'No directory supplied'
        sys.exit(1)

    lbDict = extract(args, options.filter, prefix = options.prefix if options.prefix else True)

    # Write to file ...
    out = open('lbfilemap.txt', 'w')
    for fname,lb in sorted(lbDict.items()):
        out.write(fname + ' ' + ','.join([str(l) for l in lb]) + '\n')

    out.close()
