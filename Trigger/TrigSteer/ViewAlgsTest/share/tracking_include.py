#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="roiData.dat"
#fakeRoI.OutputRoIs="OutputRoIs"
fakeRoI.OutputRoIs="L1RoIs"
setOutput(fakeRoI, "OutputDecisions", "CaloRoIs")
fakeRoI.OutputLevel=DEBUG
topSequence += fakeRoI

# here merge RoIs
from ViewAlgsTest.ViewAlgsTestConf import MergeRoIsAlg
mergeRoIs = MergeRoIsAlg("mergeRoIs")
mergeRoIs.OutputLevel=DEBUG
mergeRoIs.InputRoIsContainer="L1RoIs"
mergeRoIs.OutputRoIsContainer="OutputRoIs"
#mergeRoIs.InputRoIsContainer="OutputRoIs"
#mergeRoIs.OutputRoIsContainer="MergedRoIs"
mergeRoIs.UseSuperRoI=True
topSequence += mergeRoIs

from ViewAlgsTest.ViewAlgsTestConf import TrackHypoAlg, TrackHypoTool, TrackRoIAssocTool
trackHypo = TrackHypoAlg("trackHypoAlg")
trackHypo.OutputLevel=DEBUG
trackHypo.TrackCollection="SiSPSeededTracks"
#trackHypo.RoiCollection="OutputRoIs"
trackHypo.RoiCollection="L1RoIs"

# associates track+roIs in TCs
trackRoIAss=TrackRoIAssocTool("TrackRoIAss")
trackRoIAss.OutputLevel=DEBUG
trackRoIAss.dRSize=0.4
trackHypo += trackRoIAss
trackHypo.TrackRoIAssociationTool=trackRoIAss

# hypo tools
trackCut1 = TrackHypoTool("trackcut1", TrackMultiplicity=1)
trackHypo += trackCut1
trackCut3 = TrackHypoTool("trackcut3", TrackMultiplicity=3)
trackHypo += trackCut3

trackHypo.Hypotheses = [trackCut1, trackCut3]
topSequence += trackHypo
