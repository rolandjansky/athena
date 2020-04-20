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

# TODO: event bookkeeping?

### NTUP->NTUP
ntupFlags=jobproperties.NTUPtoNTUPProdFlags
printfunc ("Content of rec.NTUPScripts = %s", ntupFlags.NTUPScripts)
if ntupFlags.NTUPScripts()!=[]:
  for ntup in ntupFlags.NTUPScripts():
      NTUPName = str(ntup)
      printfunc ("Including %s...",NTUPName)
      include(ntup)
      pass
else:
  printfunc ("no scripts in NTUPScripts...")
