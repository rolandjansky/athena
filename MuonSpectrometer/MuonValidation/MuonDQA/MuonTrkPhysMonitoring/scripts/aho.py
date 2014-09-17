#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,os
outTxt = open('list_3.txt','w')
for inFile in sys.argv[1].split(','):
   outTxt.write(inFile+'\n')
outTxt.close()
status = os.system("DQHistogramMerge.py list_3.txt %s 1 0 1 0" % sys.argv[2])
sys.exit(status % 255)

