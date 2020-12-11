#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file: perf-dpmon.py
# @purpose: analyze the output of PerfMon::StorePayloadMon to display the
#           DataProxies' payload sizes
# @author: Sebastien Binet <binet@.cern.ch>
# $Id$

__author__  = "Sebastien Binet"
__version__ = "$Revision: 289397 $"
__doc__     = "analyze the output of PerfMon::StorePayloadMon to display the DataProxies' payload sizes"

import os, glob
import sys

def ana(fname, n_consumers):
    
    import imp
    mod_name = 'perf_dpmon_data_%s' % (
        os.path.splitext(os.path.basename(fname))[0],
        )
    mod_file = open(fname)
    mod = imp.load_module(mod_name, mod_file, fname, ('', '', imp.PY_SOURCE))

    dp_mon_data = getattr(mod, 'data')

    nevts = len(dp_mon_data)
    for ievt in range(nevts):
        data = dp_mon_data[ievt]
        store= data[-1]
        print( "::: evt=%4i: %10d -> %10d -- delta= %10d (= %10.3f kb)" % (
            ievt, store[0], store[1], store[2], store[2]/1024.) )
        top_consumers = [ d for d in data[:-1] ]
        top_consumers = sorted(top_consumers, key=lambda x: x[2])
        print( ":::  top-consumers: (%s/%s)" % (n_consumers,len(top_consumers)) )
        for c in top_consumers[:n_consumers]:
            print( "%4s %10d -> %10d -- delta= %10d (= %10.3f kb) [%s#%s]" % (
                '', c[0], c[1], c[2], c[2]/1024., c[3], c[4],
                ) )

    del dp_mon_data
    del mod
    del sys.modules[mod_name]
    
def main():
    """main entry point"""

    from optparse import OptionParser
    parser = OptionParser( usage = "usage: %prog [options] [-i] f1.pmon f2.pmon ..." )
    p = parser.add_option
    p("-i",
      "--input-files",
      dest    = "input_files",
      default = [],
      help    = "path to the performance monitoring files to analyze"
      )

    p("--n_consumers",
      dest = "n_consumers",
      default = 40,
      type=int,
      help = "number of top consumers to display the stats of")

    (options, args) = parser.parse_args()

    if isinstance(options.input_files, str):
        options.input_files = [ options.input_files ]

    for arg in args:
        if arg[0] != '-':
            options.input_files += [ arg ]
    
    input_files = []
    for f in options.input_files:
        f = glob.glob(os.path.expandvars(os.path.expanduser(f)))
        f.sort()
        input_files += f
        
    if len(input_files) == 0:
        print( "ERROR: invalid input files (do they exist ?)" )
        print( "ERROR: got: %r" % options.input_files )
        return 1

    all_good = True
    for fname in input_files:
        try:
            print( ":"*80 )
            print( "::: analyzing: [%s]..." % (fname,) )
            ana(fname, options.n_consumers)
            print( "::: analyzing: [%s]... [done]" % (fname,) )
            print( "" )
        except Exception as err:
            print( "ERROR: caught:\n%s" % (err,) )
            all_good = False

    print( "::: bye." )
    if all_good:
        return 0
    return 1

if __name__ == "__main__":
    sys.exit(main())
    
            

