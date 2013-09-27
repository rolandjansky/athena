#
# Job Options file for TrigInDetTrackTruthMaker
# /Trigger/TrigAnalysis/TrigInDetTruthAlgs/share
# RG 28/3/2006
#
# the DLL:
#theApp.Dlls += [ "TrigInDetTruthAlgs" ]
#theApp.TopAlg += [ "TrigInDetTrackTruthMaker" ]

# the algorithm derived class
#TrigInDetTrackTruthMaker = Algorithm( "TrigInDetTrackTruthMaker" )

# set properties
#TrigInDetTrackTruthMaker.TrigIDTrackTruthMapName = "MyTrigInDetTrackTruthMap"
#TrigInDetTrackTruthMaker.MinNrMatchingHits = 2
#TrigInDetTrackTruthMaker.OutputLevel = DEBUG

from TrigInDetTruthAlgs.TrigInDetTruthAlgsConfig import TrigIDTruthMaker

myTrigIDTruthMaker = TrigIDTruthMaker()
#myTrigIDTruthMaker.OutputLevel=DEBUG

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += myTrigIDTruthMaker



