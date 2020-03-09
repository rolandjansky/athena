import sys

#needs to run from athena.py 

from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.TestDriveDummies import dummyService, dummyTool
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG, INFO
log.setLevel(DEBUG)

def newConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
    acc=ComponentAccumulator()
    acc.addService(dummyService("dummyService1",AString="bla",
                                AList=["l1","l2"],
                                SomeTools=[dummyTool("tool1",BList=["lt1","lt2"]),])
                   )
    
    acc.addPublicTool(dummyTool("dummyTool1"))
    return acc




#Here starts the old Config

TheDummyService=dummyService("oldDummyService")
TheDummyService.AList=["l41",]
svcMgr+=TheDummyService
TheNewDummyService=dummyService("dummyService1",AList=["l0",])
svcMgr+=TheNewDummyService


CAtoGlobalWrapper(newConfig,None)

allServiceNames=[s.getFullName() for s in svcMgr.getChildren()]
allToolNames=[t.getFullName() for t in ToolSvc.getChildren()]


assert("dummyService/oldDummyService" in allServiceNames)

assert(svcMgr.oldDummyService.AList[0] is "l41")

assert("dummyTool/dummyTool1" in allToolNames)

#tests deduplication
for l in ("l0","l1","l2"):
    assert(l in svcMgr.dummyService1.AList)

sys.exit(0)

