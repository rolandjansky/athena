# Configure athena common flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

athenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/RDO/mc12_8TeV.CurrentRef.RDO.pool.root']
athenaCommonFlags.EvtMax = 5
athenaCommonFlags.SkipEvents = 0
#athenaCommonFlags.OutputLevel = 2
#athenaCommonFlags.AllowIgnoreConfigError = False


from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = True
jetFlags.useTracks = True
jetFlags.useMuonSegments = True
#jetFlags.usePflow = True 
jetFlags.separateJetAlgs = True
#jetFlags.applyCalibration = False

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

from RecExConfig.RecFlags import rec
rec.doTrigger=False
rec.doTau = False
#rec.doEgamma=False
## rec.doInDet = False
## rec.doMuon = False
## rec.doMuonCombined=False
## rec.doTruth = False
#rec.doCalo = False

#rec.doFloatingPointException.set_Value_and_Lock(True)


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


