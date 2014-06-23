#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Wrapper for BSFilter for ZeroBias overlay on Heavy Ions"""

import sys
import re
import os, commands


ListOfDefaultPositionalKeys = ['inputEvgenFile','PostInclude','TriggerBit','pileupBSFile','extraBSFile','outputEvgenFile','filterFile']

# execute it if not imported
if __name__ == '__main__':

    argMap = {}
    for tmpKeyVal in sys.argv[1:]:
        try:
            tmpMatch = re.search('^([^=]+)=(.+)$',tmpKeyVal)
            if tmpMatch != None:
                mapKey = tmpMatch.group(1)
                mapVal = tmpMatch.group(2)
                if mapKey in ['ipileupBSFile']:
                    # convert to list
                    argMap[mapKey] = mapVal.split(',')
                else:
                    # convert to int
                    try:
                        argMap[mapKey] = eval(mapVal)
                    except:
                        # use string
                        argMap[mapKey] = mapVal
        except:
            pass

    print "arguments : " + str(sys.argv[1:])
    print "arg map   : " + str(argMap)


    zeroBiasFileName=argMap['pileupBSFile']
    triggerBit=argMap['TriggerBit']

    #unpack the tarball with vtx and filter text files.
    extraBSTarball=argMap['extraBSFile']
    comt="tar xvzf %s" %(extraBSTarball)
    retStat = os.system(comt)
    if retStat:
        sys.exit(retStat % 255)

    #BSFilter_trf.py inputBSFile=HIMinBiasOverlay.RAW OutputBSFilterFile=simplefiltered.RAW EventIdFile=events_orig.txt filterfile=filter.txt ; BSFilter_trf.py inputBSFile=simplefiltered.RAW OutputBSFilterFile=simpledummy.RAW EventIdFile=events.txt TriggerBit=5 skipEvents=0 maxEvents=100 ; 

    # execute original trf
    # EventIdFile=events.txt
    com1 = "BSFilter_trf.py inputBSFile=%s EventIdFile=events_orig.txt OutputBSFilterFile=simplefiltered.data.RAW filterfile=%s" % (zeroBiasFileName,argMap['filterFile'])
    retStat = os.system(com1)
    if retStat:
        sys.exit(retStat % 255)

    com2 = "BSFilter_trf.py inputBSFile=simplefiltered.data.RAW EventIdFile=events.txt OutputBSFilterFile=simpledummy.data.RAW TriggerBit=%d skipEvents=0 maxevents=100" % (triggerBit)    
    retStat = os.system(com2)
    if retStat:
        sys.exit(retStat % 255)

    inputEvgenFile=argMap['inputEvgenFile']
    outputEvgenFile=argMap['outputEvgenFile']
    
    try:
        print "renaming %s to %s" % (inputEvgenFile, outputEvgenFile)
        os.rename(inputEvgenFile, outputEvgenFile)
    except:
        pass
            
    sys.exit(retStat % 255)

