if BTaggingFlags.Runmodus == 'analysis':
  # each tagger needs own instance, can't be shared!
  from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
  SV2NewLikelihood = Analysis__NewLikelihoodTool(
                                                  name = "SV2NewLikelihoodTool",
                                                  OutputLevel = BTaggingFlags.OutputLevel,
						  taggerName = "SV2",
						  calibrationTool = BTagCalibrationBrokerTool
                                                )
  ToolSvc += SV2NewLikelihood
  if BTaggingFlags.OutputLevel < 3:
    print SV2NewLikelihood
else:
  SV2NewLikelihood = None

from JetTagTools.JetTagToolsConf import Analysis__SVTag
SV2TagTool = Analysis__SVTag(
                              name = "SV2Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              SVAlgType = "SV2",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool    = SV2NewLikelihood,
                              jetCollectionList = BTaggingFlags.Jets,
                              jetWithInfoPlus   = BTaggingFlags.JetsWithInfoPlus,
                              originalTPCollectionName = BTaggingFlags.TrackParticleCollectionName
                             )
ToolSvc += SV2TagTool
if BTaggingFlags.OutputLevel < 3:
  print SV2TagTool
