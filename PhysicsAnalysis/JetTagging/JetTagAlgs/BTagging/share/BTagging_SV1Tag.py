if BTaggingFlags.Runmodus == 'analysis':
  # each tagger needs own instance, can't be shared!
  from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
  SV1NewLikelihood = Analysis__NewLikelihoodTool(
                                                  name = "SV1NewLikelihoodTool",
                                                  OutputLevel = BTaggingFlags.OutputLevel,
						  taggerName = "SV1",
						  calibrationTool = BTagCalibrationBrokerTool
                                                )
  ToolSvc += SV1NewLikelihood
  if BTaggingFlags.OutputLevel < 3:
    print SV1NewLikelihood
else:
  SV1NewLikelihood = None
  
from JetTagTools.JetTagToolsConf import Analysis__SVTag
SV1TagTool = Analysis__SVTag(
                              name = "SV1Tag",
                              Runmodus = BTaggingFlags.Runmodus,
                              referenceType = BTaggingFlags.ReferenceType,
                              SVAlgType = "SV1",
                              xAODBaseName = "SV1",
                              SecVxFinderName = "SV1",
                              OutputLevel = BTaggingFlags.OutputLevel,
                              LikelihoodTool    = SV1NewLikelihood,
                              jetCollectionList = BTaggingFlags.Jets,
                              UseCHypo=True
                             )
ToolSvc += SV1TagTool
if BTaggingFlags.OutputLevel < 3:
  print SV1TagTool
