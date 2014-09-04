#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    checkSG.py
# @purpose: read a POOL file and dump the DataHeader's content
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    May 2008
#
# @example:
# @code
# checkSG aod.pool.root
# checkSG /castor/cern.ch/user/j/johndoe/aod.pool.root
# checkSG somedir/*/*.pool
# @endcode
#

__version__ = "$Revision: 1.1 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the POOL file to analyze" )
    parser.add_option( "-o",
                       "--output",
                       dest = "outFileName",
                       default = None,
                       help = "Name of the output file which will contain the informations gathered during checkSG processing. These informations will be stored into a python-shelve or an ASCII/py file (depending on the extension: .pkl,.dat -> shelve; everything else -> ASCII/py)" )
    
    (options, args) = parser.parse_args()

    fileNames = []
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if not (options.fileName is None):
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    fileNames = set( fileNames )
    sc = 0
    for fileName in fileNames:
        try:
            from AthenaCommon.KeyStore import loadKeyStoreFromPoolFile
            print "## checking [%s]..."%fileName
            ks = loadKeyStoreFromPoolFile(keyStore=os.path.basename(fileName),
                                          pool_file=fileName,
                                          label='inputFile')
            print "="*80
            print "%40s%s%-40s" % ("Container type", " | ","StoreGate keys")
            print "%40s%s%-40s" % ("-"*40, "-+-", "-"*(40-3))
            for name,sgkeys in ks.inputFile.dict().items():
                print "%40s%s%-40s" % (name, " | ", ', '.join(sgkeys))
            print "="*80
            if options.outFileName:
                osp = os.path
                outFileName = options.outFileName
                outFileName = osp.expanduser(outFileName)
                outFileName = osp.expandvars(outFileName)
                print "## saving checkSG report into [%s]..." % outFileName
                if os.path.splitext(outFileName)[1] in ('.pkl', '.dat'):
                    # we explicitely import 'bsddb' to try to always
                    # get that particular backend for the shelve...
                    import bsddb
                    import shelve
                    if os.path.exists(outFileName):
                        os.remove(outFileName)
                    db = shelve.open(outFileName)
                    db['eventdata_items'] = ks.inputFile.dict()
                    db.close()
                else:
                    ks.write(outFileName, label='inputFile')
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass

        except :
            print "## Caught something !! (don't know what)"
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 10
            pass
        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames
    
    print "## Bye."
    sys.exit(sc)
