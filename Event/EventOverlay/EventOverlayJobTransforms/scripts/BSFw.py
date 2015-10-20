#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Wrapper for BSFilter for ZeroBias overlay"""

import sys
import re
import os, commands

ListOfDefaultPositionalKeys = ['inputEvgenFile','triggerBit','pileupBSFile','outputEvgenFile','filterFile']

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

    #print "zeroBiasFileName", argMap['ZeroBiasFile']

    zeroBiasFileName=argMap['pileupBSFile']
    triggerBit=-1
    myfilterfile=''
    if 'filterFile' in argMap: myfilterfile=argMap['filterFile']
    if 'triggerBit' in argMap: triggerBit=argMap['triggerBit']

    # execute original trf
    #com = "BSFilter_trf.py inputBSFile=%s EventIdFile=events.txt OutputBSFilterFile=simpledummy.data.RAW TriggerBit=%d skipEvents=0 maxEvents=100" % (zeroBiasFileName,triggerBit)    
    com = "BSOverlayFilter_tf.py --inputZeroBiasBSFile=%s --outputBS_SKIMFile=simplefiltered.data.RAW --outputTXT_EVENTIDFile=events.txt --inputFilterFile=%s --triggerBit=%d --skipEvents=0 --maxEvents=100" % (zeroBiasFileName,myfilterfile,triggerBit)
    print "command = ",com
    retStat = os.system(com)

    inputEvgenFile=argMap['inputEvgenFile']
    outputEvgenFile=argMap['outputEvgenFile']
    
    try:
        print "renaming %s to %s" % (inputEvgenFile, outputEvgenFile)
        os.rename(inputEvgenFile, outputEvgenFile)
    except:
        pass
            
    sys.exit(retStat % 255)
