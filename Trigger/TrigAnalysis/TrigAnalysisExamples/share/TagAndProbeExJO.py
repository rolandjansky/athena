if not ('EvtMax' in dir()):
  EvtMax = 10

if not ('PoolAODInput' in dir()):
  PoolAODInput = ['/afs/cern.ch/user/b/bensmith/public/zmumu.AOD.pool.root']

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=PoolAODInput
ServiceMgr.OutputLevel=ERROR
for i in ServiceMgr:  i.OutputLevel=ERROR

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(PoolAODInput)

##############################
# Add the TagAndProbeExAlg
##############################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigAnalysisExamples.TrigAnalysisExamplesConf import TagAndProbeExAlg
TagAndProbeExAlg = TagAndProbeExAlg()
TagAndProbeExAlg.OutputLevel = INFO
TagAndProbeExAlg.MuonContainerKey = "StacoMuonCollection"
TagAndProbeExAlg.L1ChainName = "L1_MU10"
TagAndProbeExAlg.L2ChainName = "L2_mu10"
TagAndProbeExAlg.EFChainName = "EF_mu10"
topSequence += TagAndProbeExAlg

###############################
# Configure the job
###############################

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

#
# Trigger Configuration
#
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter("ReadPool")
