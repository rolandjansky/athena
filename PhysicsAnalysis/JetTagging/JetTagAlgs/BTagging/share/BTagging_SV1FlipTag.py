if BTaggingFlags.Runmodus == 'analysis':
  # each tagger needs own instance, can't be shared!
  from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
  SV1FlipNewLikelihood = Analysis__NewLikelihoodTool(
                                                  name = "SV1FlipNewLikelihoodTool",
                                                  OutputLevel = BTaggingFlags.OutputLevel,
						  taggerName = "SV1",
						  calibrationTool = BTagCalibrationBrokerTool
                                                )
  ToolSvc += SV1FlipNewLikelihood
  if BTaggingFlags.OutputLevel < 3:
    print SV1FlipNewLikelihood
else:
  SV1FlipNewLikelihood = None
  
from JetTagTools.JetTagToolsConf import Analysis__SVTag
SV1FlipTagTool = Analysis__SVTag(
                              name = "SV1FlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              SVAlgType = "SV1",
                              SecVxFinderName = "InDetVKalVxNegativeTagInJetTool",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool    = SV1FlipNewLikelihood,
                              jetCollectionList = BTaggingFlags.Jets,
                              writeInfoPlus = False,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName
                             )
ToolSvc += SV1FlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print SV1FlipTagTool
