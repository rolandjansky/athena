#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()

topSequence.InDetPriVxFinder.TracksName = "AdjustedTrackParticleCandidate"
topSequence.InDetPriVxFinder.VxCandidatesOutputName = "AdjustedVxPrimaryCandidate"

ToolSvc.NewJetFitterVxFinder.VxPrimaryContainer = "AdjustedVxPrimaryCandidate"
ToolSvc.NewJetFitterVxFinderFlip.VxPrimaryContainer = "AdjustedVxPrimaryCandidate"


JetTagD3PDFlags.PrimaryVertexSGKey="AdjustedVxPrimaryCandidate"
JetTagD3PDFlags.TruthTrackParticleSGKey="AdjustedTrackParticleTruthCandidate"
JetTagD3PDFlags.TrackParticleSGKey="AdjustedTrackParticleCandidate"

from AthenaCommon.AppMgr import ToolSvc
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

if hasattr(ToolSvc, "AtlasExtrapolator"):
    JetTagD3PDAdjustTracks.Extrapolator = ToolSvc.AtlasExtrapolator
else:
    myextrap = AtlasExtrapolator()
    ToolSvc += myextrap
    JetTagD3PDAdjustTracks.Extrapolator = myextrap
