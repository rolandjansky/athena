if BTaggingFlags.Runmodus == 'analysis':
  # each tagger needs own instance, can't be shared!
  from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
  SV2FlipNewLikelihood = Analysis__NewLikelihoodTool(
                                                  name = "SV2FlipNewLikelihoodTool",
                                                  OutputLevel = BTaggingFlags.OutputLevel,
						  taggerName = "SV2",
						  calibrationTool = BTagCalibrationBrokerTool
                                                )
  ToolSvc += SV2FlipNewLikelihood
  if BTaggingFlags.OutputLevel < 3:
    print SV2FlipNewLikelihood
else:
  SV2FlipNewLikelihood = None

from JetTagTools.JetTagToolsConf import Analysis__SVTag
SV2FlipTagTool = Analysis__SVTag(
                              name = "SV2FlipTag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              SVAlgType = "SV2",
                              SecVxFinderName = "InDetVKalVxNegativeTagInJetTool",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool    = SV2FlipNewLikelihood,
                              jetCollectionList = BTaggingFlags.Jets,
                              writeInfoPlus = False, 
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName
                             )
ToolSvc += SV2FlipTagTool
if BTaggingFlags.OutputLevel < 3:
  print SV2FlipTagTool
