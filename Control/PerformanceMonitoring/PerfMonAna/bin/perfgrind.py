#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file PerfMonAna/bin/perfgrind.py
# @purpose a little script to convert a pmon.gz file into a kcachegrind one
# @date December 2009

__version__ = "$Revision: 270227 $"
__doc__ = "a little script to convert a pmon.gz file into a kcachegrind one"
__author__ = "Sebastien Binet"

### imports -------------------------------------------------------------------
import sys
import argparse

import PyUtils.Logging as L
import PerfMonComps.PerfMonSerializer as pmon_ser

def main():

    msg = L.logging.getLogger('perfgrind')
    msg.setLevel(L.logging.INFO)
    msg.info(':'*80)
    # create the parser
    parser = argparse.ArgumentParser(description=__doc__)

    # add the arguments
    parser.add_argument(
        'fname', #type=list,
        nargs='*',
        help='(a list of) pmon file(s) to be converted to kcachegrind format')

    
    # parse the command line
    args = parser.parse_args()

    msg.info('converting [%s] file%s...',
             len(args.fname), 's' if len(args.fname) else '')
    for fname in args.fname:
        msg.info('converting [%s]...', fname)
        pmon_ser.create_cachegrind_files(fname)
        msg.info('converting [%s]... [done]', fname)
    msg.info('::: bye.')
    return 0

if __name__ == "__main__":
    sys.exit(main())
    
