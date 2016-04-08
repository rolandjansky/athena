include.block ("NTUPtoNTUP_topOptions.py")

from AthenaCommon.Logging import logging
logNTUPtoNTUP_topOptions = logging.getLogger( 'NTUPtoNTUP_topOptions' )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr

### Set Input
import AthenaRootComps.ReadAthenaRoot
from AthenaCommon.AthenaCommonFlags import jobproperties
ntupFlags=jobproperties.NTUPtoNTUPProdFlags
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
ServiceMgr.EventSelector.TupleName = jobproperties.NTUPtoNTUPProdFlags.TreeName()

# Number of events to be processed, skip event
theApp.EvtMax = athenaCommonFlags.EvtMax()
try:
    ServiceMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
except Exception:
    treatException("Could not set EventSelector.SkipEvents")


#### Schedule EventCounterAlg
#from EventBookkeeperTools.EventCounterAlg import EventCounterAlg
#topSequence+=EventCounterAlg("AllExecutedEvents")
#
####EventBookkeepers
#if not hasattr(ServiceMgr,"CutFlowSvc"):
#    from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
#    ServiceMgr+=CutFlowSvc()
#    theApp.CreateSvc+=['CutFlowSvc']
#    pass

### NTUP->NTUP
ntupFlags=jobproperties.NTUPtoNTUPProdFlags
print "Content of rec.NTUPScripts = %s", ntupFlags.NTUPScripts
if ntupFlags.NTUPScripts()!=[]:
  for ntup in ntupFlags.NTUPScripts():
      NTUPName = str(ntup)
      print "Including %s...",NTUPName
      include(ntup)
      pass
else:
  print "no scripts in NTUPScripts..."
