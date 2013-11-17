
if not ('PoolESDInput' in dir()):
    PoolESDInput=["/afs/cern.ch/user/t/trigcomm/w0/T0Cos/newData/myESD-express.pool.root"]

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=PoolESDInput
ServiceMgr.OutputLevel=ERROR
ServiceMgr.MessageSvc.infoLimit=200000

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(PoolESDInput)

from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys(['everything'])


#
# Example Code
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigExpressStreamAlg
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
topSequence += TrigExpressStreamAlg()

###############################
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

theApp.EvtMax=-1
theApp.OutputLevel=ERROR
