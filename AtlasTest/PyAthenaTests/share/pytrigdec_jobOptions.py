## testing trig-decision tool

##############################
if not 'INPUT' in dir():
    INPUT = [ # a "reference" file from FCT
        'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
        '/AllBasicSamples.AOD.pool.root'
        ]
    #INPUT = ['AOD.pool.root']
if not 'EVTMAX' in dir():
    EVTMAX=-1
if not 'OUTPUT' in dir():
    OUTPUT='trig_filtered.pool'
if not 'ASCIILOG' in dir():
    ASCIILOG='trig_passed_evts.ascii'
##############################

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp, ToolSvc as toolSvc, ServiceMgr as svcMgr

## detector description version
DetDescrVersion="ATLAS-CSC-02-00-00"

# configure application for reading POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = INPUT
theApp.EvtMax = EVTMAX

# sequence of top algs
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from PyAthenaTests.Lib import TrigDecTestAlg
job += TrigDecTestAlg(OutputLevel=Lvl.DEBUG)
job.TrigDecTestAlg.filename = ASCIILOG

## trigger configuration service
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
import IOVDbSvc.IOVDb 
from IOVDbSvc.CondDB import conddb
conddb.addFolder("TRIGGER","/TRIGGER/HLT/Menu <tag>HEAD</tag>") 
conddb.addFolder("TRIGGER","/TRIGGER/HLT/HltConfigKeys <tag>HEAD</tag>")
conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>")
conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Menu <tag>HEAD</tag>")
conddb.addFolder("TRIGGER","/TRIGGER/LVL1/Prescales <tag>HEAD</tag>")

## set up trigger decision tool
tdt = CfgMgr.Trig__TrigDecisionTool(OutputLevel=Lvl.INFO)
toolSvc += tdt

from RecExConfig.RecFlags  import rec
rec.readAOD=True

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly = True

## setup configuration service
from TrigConfigSvc.TrigConfigSvcConfig import DSConfigSvc, SetupTrigConfigSvc
dscfg = DSConfigSvc()
dscfg.OutputLevel=Lvl.VERBOSE
svcMgr += dscfg

trigcfg = SetupTrigConfigSvc()
trigcfg.OutputLevel=Lvl.VERBOSE
trigcfg.SetStates('ds')
trigcfg.InitialiseSvc()
#svcMgr += trigcfg

#theApp.ReflexPluginDebugLevel = 1000


## configure the job to write out POOL files
import AthenaPoolCnvSvc.WriteAthenaPool
## create an output stream
job += CfgMgr.AthenaOutputStream(
    'OutStream',
    WritingTool = "AthenaPoolOutputStreamTool"
    )
# Copy everything from the input and must force reading of all input
# objects
job.OutStream.OutputFile = OUTPUT
job.OutStream.ItemList   = ['EventInfo#*']
job.OutStream.AcceptAlgs = [job.TrigDecTestAlg.getName()]
