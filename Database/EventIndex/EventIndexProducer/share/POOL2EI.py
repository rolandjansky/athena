#!/bin/env python

import os
# prevent from running athena-mp in child processes
os.putenv('ATHENA_PROC_NUMBER','0')

# prevent from running athena in interactive mode (and freeze)
if 'PYTHONINSPECT' in os.environ:
    del os.environ['PYTHONINSPECT']

try:
    FNAME = In
except:
    FNAME = ['../1/AOD.01234377._000003.pool.root.1']


include("PyAnalysisCore/InitPyAnalysisCore.py")
include('EventIndexProducer/POOL2EI_joboptions.py')

try:
  svcMgr.MessageSvc.OutputLevel = Level
  job.pool2ei.OutputLevel = Level
except:
  svcMgr.MessageSvc.OutputLevel = INFO
  job.pool2ei.OutputLevel = INFO

try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = -1

try:
    job.pool2ei.Out = Out
except:
    job.pool2ei.Out = 'pool2ei.ei.pkl'

# 
job.pool2ei.DoProvenanceRef = True
job.pool2ei.DoTriggerInfo = True
