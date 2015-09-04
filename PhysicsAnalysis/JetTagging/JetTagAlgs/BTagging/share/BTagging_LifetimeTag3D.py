if BTaggingFlags.Runmodus == 'analysis':
  lhVariablesForAnalysis = [ "significance3D" ]
# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__LikelihoodTool
myBTagLikelihoodTool3D = Analysis__LikelihoodTool(
                                                  name                = "myBTagLikelihoodTool3D",
                                                  allLhVariables      = lhVariablesForAnalysis,
                                                  useTheseLhVariables = lhVariablesForAnalysis,
                                                  OutputLevel = BTaggingFlags.OutputLevel
                                                 )
ToolSvc += myBTagLikelihoodTool3D
if BTaggingFlags.OutputLevel < 3:
  print myBTagLikelihoodTool3D

from JetTagTools.JetTagToolsConf import Analysis__LifetimeTag
A0LifetimeTagTool = Analysis__LifetimeTag( 
                                      name = "LifetimeTag3D",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      LifetimeModus = "3D",
                                      OutputLevel     = BTaggingFlags.OutputLevel,
                                      TrackToVertexTool = BTagTrackToVertexTool,
                                      LikelihoodTool    = myBTagLikelihoodTool3D,
                                      useVariables      = lhVariablesForAnalysis
                                     )

# make sure to create all histos when in ref mode (for now there is only one variable in this tagger ...)
if BTaggingFlags.Runmodus == 'reference':
  A0LifetimeTagTool.useVariables = [ "significance3D" ]

ToolSvc += A0LifetimeTagTool
if BTaggingFlags.OutputLevel < 3:
  print A0LifetimeTagTool
