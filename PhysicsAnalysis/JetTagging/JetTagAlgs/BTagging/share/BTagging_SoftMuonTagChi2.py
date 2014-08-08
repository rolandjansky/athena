
# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
SoftMuonTagChi2NewLikelihood = Analysis__NewLikelihoodTool( 
  name = "SoftMuonTagChi2NewLikelihoodTool",
  OutputLevel = BTaggingFlags.OutputLevel,
  taggerName = "SoftMuChi2",
  calibrationTool = BTagCalibrationBrokerTool,
  smoothNTimes = 0,
  normalizedProb = True,
  interpolate = True
  )
ToolSvc += SoftMuonTagChi2NewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print SoftMuonTagChi2NewLikelihood

from JetTagTools.JetTagToolsConf import Analysis__SoftMuonTag

SoftMuonTagChi2Tool = Analysis__SoftMuonTag(
  name = "SoftMuonTagChi2",
  Runmodus = BTaggingFlags.Runmodus,
  OutputLevel = BTaggingFlags.OutputLevel,
  LikelihoodTool    = SoftMuonTagChi2NewLikelihood,
  TrackToVertexTool = BTagTrackToVertexTool,
  MuonAssociationName="Muons",
  jetCollectionList = BTaggingFlags.Jets,
  originalMuCollectionName = BTaggingFlags.MuonCollectionName
  )
SoftMuonTagChi2Tool.TaggingAlgType = "CHI2" # this is what switches the chi2 tagger ON
SoftMuonTagChi2Tool.RecAlgorithm = 0 # use only combined muons
SoftMuonTagChi2Tool.writeInfoPlus = 1 # turn InfoPlus off because the L1D tagger takes care of it (albeit cuts on the chi2...) --> # Turn-on (for rel 17.2)
SoftMuonTagChi2Tool.CutMatchChi2 = 10000 # no match-chi2 cut 

ToolSvc += SoftMuonTagChi2Tool
if BTaggingFlags.OutputLevel < 3:
  print SoftMuonTagChi2Tool



