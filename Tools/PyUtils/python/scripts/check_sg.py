# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.check_sg
# @purpose read a POOL file and dump the DataHeader's content
# @author Sebastien Binet
# @date February 2010

from __future__ import print_function

__doc__ = "read a POOL file and dump the DataHeader's content"
__author__ = "Sebastien Binet"


### imports -------------------------------------------------------------------
import PyUtils.acmdlib as acmdlib

@acmdlib.command(name='chk-sg')
@acmdlib.argument('files', nargs='+',
                  help='path to the POOL file(s) to analyze')
@acmdlib.argument('-o', '--output',
                  default=None,
                  help="""name of the output file which will contain the
                  informations gathered during checkSG processing.
                  These informations will be stored into a python-shelve or
                  an ASCII/py file (depending on the extension:
                  .pkl,.dat -> shelve; everything else -> ASCII/py)
                  """)
def main(args):
    """read a POOL file and dump the DataHeader's content

    ex:
     $ check-sg aod.pool.root
     $ check-sg /castor/foo.pool
     $ check-sg root://castoratlas//castor/foo.pool
     $ check-sg LFN:ttbar.pool
    """
    files = args.files
    if isinstance(files, str):
        files = [files]

    import os
    import sys

    for i,f in enumerate(files):
        files[i] = os.path.expandvars(os.path.expanduser(f))

    exitcode = 0
    for fname in files:
        try:
            import AthenaCommon.KeyStore as acks
            print ("## checking [%s]..." % (fname,))
            ks = acks.loadKeyStoreFromPoolFile(
                keyStore=os.path.basename(fname),
                pool_file=fname,
                label='inputFile')

            print ("="*80)
            print ("%40s%s%-40s" % ("Container type", " | ","StoreGate keys"))
            print ("%40s%s%-40s" % ("-"*40, "-+-", "-"*(40-3)))
            for name,sgkeys in ks.inputFile.dict().items():
                print ("%40s%s%-40s" % (name, " | ", ', '.join(sgkeys)))
            print ("="*80)
            if args.output:
                outFileName = args.output
                outFileName = os.path.expanduser(outFileName)
                outFileName = os.path.expandvars(outFileName)
                print ("## saving report into [%s]..." % (outFileName,))
                if os.path.splitext(outFileName)[1] in ('.pkl', '.dat'):
                    # we explicitely import 'bsddb' to try to always
                    # get that particular backend for the shelve...
                    import bsddb   # noqa: F401
                    import shelve
                    if os.path.exists(outFileName):
                        os.remove(outFileName)
                    db = shelve.open(outFileName)
                    db['eventdata_items'] = ks.inputFile.dict()
                    db.close()
                else:
                    ks.write(outFileName, label='inputFile')
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

