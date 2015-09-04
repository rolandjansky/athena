if BTaggingFlags.Runmodus == 'analysis':
  lhVariablesForAnalysis = [ "significance2D" ]
# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__LikelihoodTool
myBTagLikelihoodTool2D = Analysis__LikelihoodTool(
                                                  name                = "myBTagLikelihoodTool2D",
                                                  allLhVariables      = lhVariablesForAnalysis,
                                                  useTheseLhVariables = lhVariablesForAnalysis,
                                                  OutputLevel = BTaggingFlags.OutputLevel
                                                 )
ToolSvc += myBTagLikelihoodTool2D
if BTaggingFlags.OutputLevel < 3:
  print myBTagLikelihoodTool2D

from JetTagTools.JetTagToolsConf import Analysis__LifetimeTag
D0LifetimeTagTool = Analysis__LifetimeTag( 
                                      name = "LifetimeTag2D",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      LifetimeModus = "2D",
                                      OutputLevel     = BTaggingFlags.OutputLevel,
                                      TrackToVertexTool = BTagTrackToVertexTool,
                                      LikelihoodTool    = myBTagLikelihoodTool2D,
                                      useVariables      = lhVariablesForAnalysis
                                     )

# make sure to create all histos when in ref mode (for now there is only one variable in this tagger ...)
if BTaggingFlags.Runmodus == 'reference':
  D0LifetimeTagTool.useVariables = [ "significance2D" ]

ToolSvc += D0LifetimeTagTool
if BTaggingFlags.OutputLevel < 3:
  print D0LifetimeTagTool
