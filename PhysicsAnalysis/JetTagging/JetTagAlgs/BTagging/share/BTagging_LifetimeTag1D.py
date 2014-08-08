if BTaggingFlags.Runmodus == 'analysis':
  lhVariablesForAnalysis = [ "significance1D" ]
# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__LikelihoodTool
myBTagLikelihoodTool1D = Analysis__LikelihoodTool(
                                                  name                = "myBTagLikelihoodTool1D",
                                                  allLhVariables      = lhVariablesForAnalysis,
                                                  useTheseLhVariables = lhVariablesForAnalysis,
                                                  OutputLevel = BTaggingFlags.OutputLevel
                                                 )
ToolSvc += myBTagLikelihoodTool1D
if BTaggingFlags.OutputLevel < 3:
  print myBTagLikelihoodTool1D

from JetTagTools.JetTagToolsConf import Analysis__LifetimeTag
Z0LifetimeTagTool = Analysis__LifetimeTag( 
                                      name = "LifetimeTag1D",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      LifetimeModus = "1D",
                                      OutputLevel     = BTaggingFlags.OutputLevel,
                                      TrackToVertexTool = BTagTrackToVertexTool,
                                      LikelihoodTool    = myBTagLikelihoodTool1D,
                                      useVariables      = lhVariablesForAnalysis
                                     )

# make sure to create all histos when in ref mode (for now there is only one variable in this tagger ...)
if BTaggingFlags.Runmodus == 'reference':
  Z0LifetimeTagTool.useVariables = [ "significance1D" ]

ToolSvc += Z0LifetimeTagTool
if BTaggingFlags.OutputLevel < 3:
  print Z0LifetimeTagTool
