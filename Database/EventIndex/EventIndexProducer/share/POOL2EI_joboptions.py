## @file: POOL2EI_joboptions.py
## @purpose: simple joboptions to convert AOD to EI
## @date Feb 2014
## @author Javier Sanchez <Javier.Sanchez@ific.uv.es>

__version__ = "$Revision$"
__author__  = "Javier Sanchez <Javier.Sanchez@ific.uv.es>"


from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# algorithm
from EventIndexProducer.POOL2EI_Lib import POOL2EI
pool2ei = POOL2EI('pool2ei', OutputLevel=Lvl.INFO)
job += pool2ei

# service
from EventIndexProducer.POOL2EI_Lib import  POOL2EISvc
pool2eisvc = POOL2EISvc(algo=pool2ei)
svcMgr += pool2eisvc
theApp.CreateSvc += [pool2eisvc.getFullJobOptName()]

 
#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
try:
  svcMgr.MessageSvc.OutputLevel = Level
except:
  svcMgr.MessageSvc.OutputLevel = INFO


#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = -1


#--------------------------------------------------------------
# Input collection name
#--------------------------------------------------------------
try:
  svcMgr.EventSelector.InputCollections  = In 
except:
  svcMgr.EventSelector.InputCollections  = [ "test.root" ]

#--------------------------------------------------------------
# Output Event Index file name
#--------------------------------------------------------------
try:
    job.pool2ei.Out = Out
except:
    import os
    job.pool2ei.Out = 'pool2ei.%08i.pkl' % os.getpid()


#--------------------------------------------------------------
# Output Provenance references flag
#--------------------------------------------------------------
try: 
    job.pool2ei.DoProvenanceRef = DoProvenanceRef
except:
    job.pool2ei.DoProvenanceRef = False

#--------------------------------------------------------------
# Output Trigger Info flag
#--------------------------------------------------------------
try: 
    job.pool2ei.DoTriggerInfo = DoTriggerInfo
except:
    job.pool2ei.DoTriggerInfo = False

