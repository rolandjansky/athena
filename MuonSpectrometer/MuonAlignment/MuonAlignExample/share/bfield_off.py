from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

mualign.trackCollectionProvider  = 'MuonTrackCollectionProvider'
mualign.MuonContainerName        = "MuidMuonCollection"
align.inputTrkCol                = "MooreTracks"
mualign.inputIDTrackCol          = "Tracks"
mualign.useStandaloneTracks      = True
mualign.inputTrackCollectionType = 1 # MS standalone with ID track match
mualign.momentumCut              = 4 
