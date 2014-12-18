#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
## Simple wrapper to invoke AthFileLite metadata grabber and
#  produce AthFile-like text output
#
# $Id: dumpAthfilelite.py 618684 2014-09-26 11:46:14Z graemes $

import argparse
import os
import pprint
import sys

import PyUtils.AthFileLite as AthFileLite

def main():
    parser = argparse.ArgumentParser(description="Use AthFileLite interface to retrieve file metadata. "
                                     "Note that the '--type' argument is mandatory as AthFileLite will "
                                     "not even try to guess the type of file.")
    parser.add_argument('--type', '-t', metavar='FILETYPE', 
                        help="Specifie filetype: POOL, BS or TAG", required=True,
                        choices=("POOL", "BS", "TAG"))
    parser.add_argument('input_files', nargs="+", help="Input files")

    args = vars(parser.parse_args(sys.argv[1:]))
    
    for filename in args['input_files']:
        if args["type"] == "POOL":
            afl = AthFileLite.AthPoolFile(filename)
        elif args["type"] == "BS":
            afl = AthFileLite.AthBSFile(filename)
        elif args["type"] == "TAG":
            afl = AthFileLite.AthTagFile(filename)
            
        metadata = afl.fileinfo
        
        print "="*80
        print filename
        print "="*80
        pprint.pprint(metadata)
        print "="*80

if __name__ == "__main__":
    main()
