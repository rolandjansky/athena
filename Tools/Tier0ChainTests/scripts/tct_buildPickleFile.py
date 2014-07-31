#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# usage: tct_buildPickleFile.py pickleFileName inputFileList outputFileName

import sys,pickle

if len(sys.argv) < 3:
    print "usage: tct_buildPickleFile.py pickleFileName inputFileList outputFileName"
    sys.exit()

data = {'inputROOTFiles':[],\
        'outputROOTFile':sys.argv[3]+"#"+sys.argv[3]}

inputFiles = open(sys.argv[2],'r')
lines = inputFiles.readlines()
[ data['inputROOTFiles'].append(line.strip('\n')+"#"+line.strip('\n')) for line in lines ]

output = open(sys.argv[1],'w')
pickle.dump(data,output)
output.close()
