# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import argparse
mainParser = argparse.ArgumentParser()

mainParser.add_argument('-t','--tuning-tag', action='store', 
                               dest='tag', required = True, 
                               help = "The tuning name tag")



import os
import sys

import sys, os
if len(sys.argv)==1:
  mainParser.print_help()
  sys.exit(1)

args=mainParser.parse_args()

basepath = '/afs/cern.ch/work/j/jodafons/public/Online/rgtuning-configs/trigger'
athenaBasepath = 'Trigger/TrigHypothesis/TrigMultiVarHypo/python/'

if args.tag == 'v3':
  tpath = basepath+'/mc15_20160901_v3/*.py'
elif args.tag == 'v3_v4_merge_plus':
  tpath = basepath+'/mc15_20161201_v3_v4_merge/*.py'

else:
  print 'tuning name not found'
  sys.exit(0)


cmd = ('cp %s %s') % (tpath,athenaBasepath) 
print cmd
os.system(cmd)

