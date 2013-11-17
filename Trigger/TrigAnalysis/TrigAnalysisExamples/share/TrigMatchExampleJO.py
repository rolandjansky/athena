if not ('EvtMax' in dir()):
  theApp.EvtMax = 3

if not ('PoolAODInput' in dir()):
    PoolAODInput = ['/afs/cern.ch/user/b/bensmith/public/AOD.pool.root']

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=PoolAODInput
ServiceMgr.OutputLevel=ERROR
for i in ServiceMgr:  i.OutputLevel=ERROR

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(PoolAODInput)

##############################
# Add the TrigMatchExAlg
##############################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigMatchExAlg
TrigMatchExAlg = TrigMatchExAlg()
TrigMatchExAlg.OutputLevel = INFO
TrigMatchExAlg.MatchMuons = True
TrigMatchExAlg.MatchElectrons = True
TrigMatchExAlg.MatchPhotons = True
TrigMatchExAlg.MatchJets = True
TrigMatchExAlg.MatchTaus = True
TrigMatchExAlg.MatchClusterElectrons = False
TrigMatchExAlg.MatchL1 = True
TrigMatchExAlg.MatchL2 = True
TrigMatchExAlg.MatchEF = True
topSequence += TrigMatchExAlg

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
