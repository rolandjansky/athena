from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
athenaCommonFlags.FilesInput.set_Value_and_Lock(['/home/emoyse/master/q431/myESD.pool.root'])
athenaCommonFlags.AllowIgnoreConfigError = False

rec.AutoConfiguration=['everything']
rec.readESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(False)

import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

import AthenaPoolCnvSvc.ReadAthenaPool  
svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if not hasattr(topSequence, "SGInputLoader"):
  from SGComps.SGCompsConf import SGInputLoader
  topSequence+=SGInputLoader(OutputLevel=DEBUG, ShowEventDump=False)

# from IOVSvc.IOVSvcConf import CondInputLoader
# topSequence += CondInputLoader( "CondInputLoader", OutputLevel=DEBUG,  )
#
# import StoreGate.StoreGateConf as StoreGateConf
# svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")
#
# from IOVSvc.IOVSvcConf import CondSvc
# svcMgr += CondSvc()

doExtrap = True

ExtrapolationEngine=""
if (doExtrap):
  import MagFieldServices.SetupField
  
  from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
  VP1ExtraPolatorName='VP1Extrapolator';
  VP1Extrapolator = AtlasExtrapolator(name=VP1ExtraPolatorName)
  
  from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
  ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=INFO)
  print ExtrapolationEngine
  ToolSvc += ExtrapolationEngine  

from DumpEventDataToJson.DumpEventDataToJsonConf import DumpEventDataToJsonAlg
event_info_key = "EventInfo"
topSequence += DumpEventDataToJsonAlg(OutputLevel=VERBOSE, EventInfoKey = event_info_key, ExtrapolateTracks=doExtrap, Extrapolator = ExtrapolationEngine)

ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%80W%S%7W%R%T %0W%M"

theApp.EvtMax = 2
theApp.OutputLevel=VERBOSE