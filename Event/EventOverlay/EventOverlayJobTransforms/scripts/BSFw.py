#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Wrapper for BSFilter for ZeroBias overlay"""

import sys
import re
import os, commands

#ListOfDefaultPositionalKeys = ['inputEvgenFile','EventIdFile','OutputBSFilterFile','PostInclude','TriggerBit','ZeroBiasFile','outputRAWFile']
ListOfDefaultPositionalKeys = ['inputEvgenFile','PostInclude','TriggerBit','pileupBSFile','outputEvgenFile']

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
    triggerBit=argMap['TriggerBit']
    
    # execute original trf
    # EventIdFile=events.txt
    com = "BSFilter_trf.py inputBSFile=%s EventIdFile=events.txt OutputBSFilterFile=simpledummy.data.RAW TriggerBit=%d skipEvents=0 maxEvents=100" % (zeroBiasFileName,triggerBit)    

    retStat = os.system(com)

    inputEvgenFile=argMap['inputEvgenFile']
    outputEvgenFile=argMap['outputEvgenFile']
    
    try:
        print "renaming %s to %s" % (inputEvgenFile, outputEvgenFile)
        os.rename(inputEvgenFile, outputEvgenFile)
    except:
        pass
            
    sys.exit(retStat % 255)
