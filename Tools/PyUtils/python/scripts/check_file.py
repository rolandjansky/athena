# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.check_file
# @purpose read a POOL file and dump its content.
# @author Sebastien Binet
# @date February 2010

from __future__ import print_function

__doc__ = "read a POOL file and dump its content."
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='chk-file')
@acmdlib.argument('files', nargs='+',
                  help='path to the POOL file(s) to analyze')
@acmdlib.argument('-d', '--detailed-dump',
                  action='store_true',
                  default=False,
                  help="""Switch to activate or not a detailed dump
                  of each TTree in the POOL file""")
@acmdlib.argument('--sort-fct',
                  choices=('diskSize','memSize','name'),
                  default='diskSize',
                  help="Sorting function used to list containers")
@acmdlib.argument('--fast',
                  action='store_true',
                  default=False,
                  help="""Enable fast mode.
                  Memory size will not be accurate AT ALL""")
@acmdlib.argument('-o', '--output',
                  default=None,
                  help="""name of the output file which will contain the
                  informations gathered during processing.
                  These informations will be stored into a python-shelve or
                  an ASCII/py file (depending on the extension:
                  .pkl,.dat -> shelve; everything else -> ASCII/py)
                  """)
def main(args):
    """read a POOL file and dump its content.
    """
    files = args.files
    if isinstance(files, str):
        files=[files]

    import sys
    import os

    for i,f in enumerate(files):
        files[i] = os.path.expandvars(os.path.expanduser(f))

    exitcode = 0
    for fname in files:
        try:
            import PyUtils.PoolFile as PF
            PF.PoolOpts.FAST_MODE = args.fast
            pool_file = PF.PoolFile(fname)
            pool_file.checkFile(sorting=args.sort_fct)
            if args.detailed_dump:
                dump_file = os.path.basename(fname) + '.txt'
                print ("## dumping details into [%s]" % (dump_file,))
                pool_file.detailedDump(dump_file)
            if args.output:
                oname = args.output
                print ("## saving report into [%s]..." % (oname,))
                pool_file.saveReport(oname)
        except Exception as e:
            print ("## Caught exception [%s] !!" % str(e.__class__))
            print ("## What:",e)
            print (sys.exc_info()[0])
            print (sys.exc_info()[1])
            import traceback
            traceback.print_exc()
            exitcode = 1
            pass

        except Exception:
            print ("## Caught something !! (don't know what)")
            print (sys.exc_info()[0])
            print (sys.exc_info()[1])
            exitcode = 10
            pass
        if len(files) > 1:
            print ("")
        pass # loop over fileNames
    
    print ("## Bye.")
    return exitcode

