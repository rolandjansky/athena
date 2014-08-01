#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==================================================================
# the trf wrapper is a python script (to be included in the release)
# that expose to ProdSys-1 a working interface like
# Train_reco_tf.py --inputNTUP_COMMONFile=NTUP_COMMON.01316372._001218.root.1 --outputDNTUP_SM1File=DNTUP_SM1.01316373._000001.root.1
# Internally, the wrapper will call the trf as needed:
# Train_reco_tf.py --inputNTUP_COMMONFile=NTUP_COMMON.01316372._001218.root.1 --outputDNTUPFile DNTUP --reductionConf SM1
#==================================================================

import re
import os, commands
from time import sleep
import sys
import pickle
import tempfile

def get_filename(subname):
    try:
        return os.listdir('.')[(i for i, name in enumerate(os.listdir('.')) if subname in name).next()]
    except:
        return None

def generate_formatflag(name):
    strippedLeft = name[14:]
    strippedRight = strippedLeft[:-4]
    return strippedRight

if __name__ == '__main__':

    trfName = 'Reco_tf.py'

    # build argument map 
    argMap = {}
    fileNameMap = {}
    formatFlags = []
    for tmpKeyVal in sys.argv[1:]:
        try:
            tmpMatch = re.search('^([^=]+)=(.+)$',tmpKeyVal)
            if tmpMatch != None:
                mapKey = tmpMatch.group(1)
                mapVal = tmpMatch.group(2)
                search = mapKey.find('--output')
                if (search != -1):
                    formatFlag = generate_formatflag(mapKey)
                    formatFlags.append(formatFlag)
                    outputFileName = 'DNTUP_'+formatFlag#+'.root'
                    fileNameMap[outputFileName] = mapVal
                try:
                    argMap[mapKey] = eval(mapVal)
                except:
                    # use string
                    argMap[mapKey] = mapVal
        except:
            print "warning: %s arg not recognised, skipping it" % tmpKeyVal
    
    #print "Reco_wrap_tf.py: arguments : " + str(sys.argv[1:])
    #print "Reco_wrap_tf.py: arg map   : " + str(argMap)

    # Build Reco_tf.py arguments
    trfArgs = '--inputNTUP_COMMONFile '+argMap['--inputNTUP_COMMONFile']+' --outputDNTUPFile DNTUP --reductionConf '+' '.join(formatFlags)

    # execute original trf
    com = '%s %s ' % (trfName,trfArgs)
    sys.stdout.flush()
    print "Reco_tf.py: running %s" % com
    retStat = os.system(com)
    try:
        os.remove(tmpName)
    except:
        pass

# Rename files and edit xml
    xmlfile = open('metadata.xml','r')
    newxmlfile = open('new.xml','w')
    xmlcontents = xmlfile.read()
    for key in fileNameMap.keys():
        mvString = "mv %s %s" % (key,fileNameMap[key])
        sys.stdout.flush()
        print "Renaming file %s --> %s" % (key,fileNameMap[key])
        retStat = os.system(mvString)
        newxml = xmlcontents.replace(key,fileNameMap[key])
        xmlcontents = newxml
    newxmlfile.write(newxml)
    xmlfile.close()
    newxmlfile.close()
    os.system("mv new.xml metadata.xml") 
