# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys

if len(sys.argv) < 2:
    print 'ERROR: please supply a directory for the output'
    sys.exit(0)

newDir = sys.argv[1]

os.system('python parseText.py')

print 'Moving output to',newDir

os.system('mkdir -p ' + newDir)

os.system('mv results_iterC??.txt ' + newDir)
os.system('mv out_iterC??.log ' + newDir)
os.system('mv monitoring_iterC??.root ' + newDir)
os.system('mv pulls_iterC??.txt ' + newDir)
os.system('mv iterC?? ' + newDir)
os.system('mv param_iterC??.py ' + newDir)
os.system('mv testOutput.txt ' + newDir)
