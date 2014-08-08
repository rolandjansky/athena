
# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
SoftMuonTagNewLikelihood = Analysis__NewLikelihoodTool( 
  name = "SoftMuonTagNewLikelihoodTool",
  OutputLevel = BTaggingFlags.OutputLevel,
  taggerName = "SoftMu",
  calibrationTool = BTagCalibrationBrokerTool,
  smoothNTimes = 0,
  normalizedProb = True,
  interpolate = True
  )
ToolSvc += SoftMuonTagNewLikelihood
if BTaggingFlags.OutputLevel < 3:
  print SoftMuonTagNewLikelihood

from JetTagTools.JetTagToolsConf import Analysis__SoftMuonTag

SoftMuonTagTool = Analysis__SoftMuonTag(
  name = "SoftMuonTag",
  Runmodus = BTaggingFlags.Runmodus,
  OutputLevel = BTaggingFlags.OutputLevel,
  LikelihoodTool    = SoftMuonTagNewLikelihood,
  TrackToVertexTool = BTagTrackToVertexTool,
  jetCollectionList = BTaggingFlags.Jets,
  originalMuCollectionName = BTaggingFlags.MuonCollectionName
  )
if BTaggingFlags.Runmodus == 'reference':
  SoftMuonTagTool.BTagJetEtamin = 2.5
  SoftMuonTagNewLikelihood.smoothNTimes = 1
  
ToolSvc += SoftMuonTagTool
if BTaggingFlags.OutputLevel < 3:
  print SoftMuonTagTool



