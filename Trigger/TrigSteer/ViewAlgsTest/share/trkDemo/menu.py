from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO
from ViewAlgsTest.Menu import *

from ViewAlgsTest.ViewAlgsTestConf import TrackHypoAlg, TrackHypoTool, TrackRoIAssocTool, FilterTracksInRoIs
trackHypo = TrackHypoAlg("trackHypoAlg")
trackHypo.OutputLevel=DEBUG
connectAlgorithmsIO(consumer=(trackHypo, "TrackCollection"), producer=(topSequence.InDetSiSpTrackFinder, "TracksLocation" ))
connectAlgorithmsIO(consumer=(trackHypo, "RoiCollection"), producer=(topSequence.fakeRoI, "OutputRoIs" )) # change to use merged RoI

# associates track+roIs in TCs
trackRoIAss=TrackRoIAssocTool("TrackRoIAss")
trackRoIAss.OutputLevel=DEBUG
trackRoIAss.dRSize=0.4
trackHypo += trackRoIAss
trackHypo.TrackRoIAssociationTool=trackRoIAss

# hypo tools
trackCut1 = TrackHypoTool("trackcut1", TrackMultiplicity=1)
trackHypo += trackCut1
trackCut3 = TrackHypoTool("trackcut3", TrackMultiplicity=30)
trackHypo += trackCut3

trackHypo.Hypotheses = [trackCut1, trackCut3]
topSequence += trackHypo

TrackingMenuStep = genMenuAlg("TrackingMenuStep")

connectAlgorithmsIO(consumer=(TrackingMenuStep, "InputChainDecisions"), producer=(topSequence.FakeCTP, "OutputDecisions"))
connectAlgorithmsIO(consumer=(TrackingMenuStep, "InputHypoDecisions"), producer=(trackHypo, "OutputDecisions"))
topSequence += TrackingMenuStep


FilterTracks = FilterTracksInRoIs("FilterTracks")
connectAlgorithmsIO(consumer=(FilterTracks, "CompContainer"), producer=(TrackingMenuStep, "OutputHypoDecisions"))
FilterTracks.OutputTracksContainer = "TracksFilteredByRoI"
FilterTracks.OutputLevel=DEBUG
topSequence += FilterTracks



trackHypo2 = TrackHypoAlg("trackHypoAlg2")
connectAlgorithmsIO(consumer=(trackHypo2, "TrackCollection"), producer=(FilterTracks ,"OutputTracksContainer"))
connectAlgorithmsIO(consumer=(trackHypo2, "RoiCollection"),   producer=(topSequence.fakeRoI, "OutputRoIs" )) # just want to see if Const tracks are digestable
setOutput(trackHypo2, "OutputDecisions", "TighterTrackingCuts")
trackHypo2.OutputLevel=DEBUG
topSequence += trackHypo2


chains = ChainsList()
chains + Chain("HLT_singleTrack",        "L1_whatever", 1, {"TrackingMenuStep": "trackcut1 x 1"})
chains + Chain("HLT_tracksInTwoRoIs",    "L1_whatever", 1, {"TrackingMenuStep": "trackcut1 x 2"})
chains + Chain("HLT_threeTracksInRoi",   "L1_whatever", 1, {"TrackingMenuStep": "trackcut3 x 1"})

configureMenu(chains, topSequence)

