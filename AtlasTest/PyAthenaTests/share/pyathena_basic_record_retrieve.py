## steer basic test of record/retrieve

######################################
if not 'EVTMAX' in dir():
    EVTMAX=1
if not 'OUTPUT' in dir():
    OUTPUT = 'my.pydata.pool'
    pass

######################################

# basic configuration
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr

# job sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from PyAthenaTests.Lib import PyRetrieve, PyRecord
job += PyRecord   ('record_test')
job += PyRetrieve ('retrieve_test')

theApp.EvtMax = EVTMAX

# POOL Persistency
import AthenaPoolCnvSvc.WriteAthenaPool

job += CfgMgr.AthenaOutputStream("OutStream",
                                  WritingTool="AthenaPoolOutputStreamTool")
outStream = job.OutStream
outStream.ItemList = [
    "EventInfo#*",
    "AthExParticles#*",
    "AthExDecay#*",
    "AthExElephantino#*",
    ]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStream.OutputFile = OUTPUT
