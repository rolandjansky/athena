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
    job.pool2ei.Out = 'pool2ei.{:08i}.pkl'.format(os.getpid())


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
    job.pool2ei.DoTriggerInfo = True

#--------------------------------------------------------------
# Send to Broker flag
#--------------------------------------------------------------
try: 
    job.pool2ei.SendToBroker = SendToBroker
except:
    job.pool2ei.SendToBroker = True

#--------------------------------------------------------------
# Input dataset name. Overrrides value read for job options
#--------------------------------------------------------------
try: 
    job.pool2ei.EiDsName = EiDsName
except:
    job.pool2ei.EiDsName = None

#--------------------------------------------------------------
# Tier0 job parameters
#--------------------------------------------------------------
try:
  job.pool2ei.TaskID = TaskID
except:
  job.pool2ei.TaskID = None
  
try:
  job.pool2ei.JobID = JobID
except:
  job.pool2ei.JobID = None

try:
  job.pool2ei.AttemptNumber = AttemptNumber
except:
  job.pool2ei.AttemptNumber = None


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput = In

#from RecExConfig.RecFlags import rec
#rec.AutoConfiguration=['everything']
rec.readRDO=False
rec.readESD=False
rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False


from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.GlobalFlags  import globalflags
globalflags.InputFormat = 'pool'
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
rec.projectName = inputFileSummary['tag_info']['project_name']

if "/TRIGGER/HLT/HltConfigKeys" in inputFileSummary['metadata'].keys():

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']
    #TriggerFlags.configurationSourceList = []
    ##TriggerFlags.configForStartup = 'HLTonline'
    TriggerFlags.configForStartup = 'HLToffline'
    ##TriggerFlags.dataTakingConditions = 'FullTrigger'

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter("ReadPool")

    from AthenaCommon.AppMgr import ToolSvc
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt = Trig__TrigDecisionTool("TrigDecisionTool")
    ToolSvc += tdt

