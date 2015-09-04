# in analysis mode take only your favourite histos
if BTaggingFlags.Runmodus == 'analysis':
  lhVariablesForAnalysis = [ "vtxprobTD", "vtxmassTD", "vtxmultTD", "vtxdistTD", "vtxenergyFractionTD" ]

# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__LikelihoodTool
myBTagLikelihoodToolTD = Analysis__LikelihoodTool(
                                                  name                = "myBTagLikelihoodToolTD",
                                                  allLhVariables      = lhVariablesForAnalysis,
                                                  useTheseLhVariables = lhVariablesForAnalysis,
                                                  OutputLevel = BTaggingFlags.OutputLevel
                                                 )
ToolSvc += myBTagLikelihoodToolTD
if BTaggingFlags.OutputLevel < 3:
  print myBTagLikelihoodToolTD

from JetTagTools.JetTagToolsConf import Analysis__SecVtxTag
TDSecVtxTagTool = Analysis__SecVtxTag( 
                                      name = "SecVtxTagTD",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      VertexFindingMode = "TD",
                                      OutputLevel     = BTaggingFlags.OutputLevel,
                                      #VertexFitterTool  = BTagVxFastFitterTool,
                                      TrackToVertexTool = BTagTrackToVertexTool,
                                      LikelihoodTool   = myBTagLikelihoodToolTD,
                                      useVariables = lhVariablesForAnalysis
                                     )
# make sure to create all histos when in ref mode
if BTaggingFlags.Runmodus == 'reference':
  TDSecVtxTagTool.useVariables = [ "vtxprobTD", "vtxmassTD", "vtxmultTD", "vtxdistTD", "vtxenergyFractionTD" ]

ToolSvc += TDSecVtxTagTool
if BTaggingFlags.OutputLevel < 3:
  print TDSecVtxTagTool
