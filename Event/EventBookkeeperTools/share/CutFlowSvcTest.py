#For the moment, this requires:
# cmt co -r AthenaPython-00-03-27 Control/AthenaPython

from AthenaCommon.AlgSequence import AlgSequence
from EventBookkeeperTools.FilterTester import FilterTester
topSequence=AlgSequence()
topSequence+= FilterTester('myFilter')

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from EventBookkeeperTools.CutFlowSvc import cutFlowSvc
if not hasattr(svcMgr, 'CutFlowSvc'):
    svcMgr += cutFlowSvc

theApp.EvtMax=10

from EventBookkeeperTools.CutFlowSvc import cutFlowSvc
