#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    checkMetaSG.py
# @purpose: Print the metadata that would be accessible via the IOVMetaDataContainers of the MetaDataStore
# @author:  Will Buttinger <will@cern.ch>
# @date:    Oct 2014
#
# @example:
# @code
# checkMetaSG.py aod.pool.root
# @endcode
#

from __future__ import print_function

__author__  = "Will Buttinger <will@cern.ch>"

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

    if options.fileName is None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if not (options.fileName is None):
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    fileNames = set( fileNames )
    sc = 0
    for fileName in fileNames:
        try:
            from PyUtils import AthFile
            print ("## checking [%s]..."%fileName)
            fileInfo = AthFile.fopen(fileName).fileinfos
            print ("="*91)
            #do the table of extra information
            print ("## Content of AthFile.fopen(%s).fileinfos"%fileName)
            print ("="*91)
            exclude_list = ['stream_tags','file_name','tag_info','det_descr_tags', 'evt_number', 'metadata', 'metadata','metadata_items', 'lumi_block','eventdata_items' ]
            print ("%30s%-28s" % ("key", " | value"))
            print ("%30s%s%-25s" % ("-"*30, "-+-", "-"*(28-3)))
            for a,b in fileInfo.items():
               if a in exclude_list: continue
               print ("%30s%s%-25s" % (a, " | ", b) )
            metadata = fileInfo['metadata']
            print ("")
            print ("="*91)
            print ("## IOVMetaData (fileinfos['metadata']):")
            print ("%30s%-28s%-10s%-30s" % ("folder", " | key "," | type "," | value"))
            print ("%30s%s%-25s%s%-7s%s%-30s" % ("-"*30, "-+-", "-"*(28-3),"-+-","-"*(10-3),"-+-","-"*(20)))
            for metaFolder,metaObj in metadata.items(): #metaObj may be dict (from AthenaAttributeList), list (from CondAttrListCollection)
               first=True
               if isinstance(metaObj,dict): # AthenaAttributeList
                  for metaKey,metaValue in metaObj.items():
                     if first:
                         print ("%30s%s%-25s%s%-7s%s%-30s" % (metaFolder, " | ", metaKey," | ",type(metaValue).__name__," | ",metaValue))
                     else:
                         print ("%30s%s%-25s%s%-7s%s%-30s" % ("", " | ", metaKey," | ",type(metaValue).__name__," | ",metaValue))
                     first=False
                     pass
                  pass
               else: # CondAttrListCollection
                  print ("%30s%s%s" % (metaFolder, " | ", type(metaObj)))
                  for metaObj_i in metaObj:
                     for metaKey,metaValue in metaObj_i.items():
                        print ("%30s%s%-25s%s%-7s%s%-30s" % ("", " | ", metaKey," | ",type(metaValue).__name__," | ",metaValue))

            print ("="*91)
            if options.outFileName:
                osp = os.path
                outFileName = options.outFileName
                outFileName = osp.expanduser(outFileName)
                outFileName = osp.expandvars(outFileName)
                print ("## saving checkSG report into [%s]..." % outFileName)
                if os.path.splitext(outFileName)[1] in ('.pkl', '.dat'):
                    import shelve
                    if os.path.exists(outFileName):
                        os.remove(outFileName)
                    db = shelve.open(outFileName)
                    db['eventdata_items'] = fileInfo
                    db.close()
        except Exception as e:
            print ("## Caught exception [%s] !!" % str(e.__class__))
            print ("## What:",e)
            print (sys.exc_info()[0])
            print (sys.exc_info()[1])
            sc = 1
            pass

        except Exception:
            print ("## Caught something !! (don't know what)")
            print (sys.exc_info()[0])
            print (sys.exc_info()[1])
            sc = 10
            pass
        if len(fileNames) > 1:
            print ("")
        pass # loop over fileNames
    
    print ("## Bye.")
    sys.exit(sc)
