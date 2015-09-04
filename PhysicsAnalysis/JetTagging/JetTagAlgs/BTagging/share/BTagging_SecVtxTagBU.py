# in analysis mode take only your favourite histos
if BTaggingFlags.Runmodus == 'analysis':
  lhVariablesForAnalysis = [ "vtxmassBU", "vtxmultBU", "vtxdistBU", "vtxenergyFractionBU", "vtxNGTVtxBU" ]

# each tagger needs own instance, can't be shared!
from JetTagTools.JetTagToolsConf import Analysis__LikelihoodTool
myBTagLikelihoodToolBU = Analysis__LikelihoodTool(
                                                  name                = "myBTagLikelihoodToolBU",
                                                  allLhVariables      = lhVariablesForAnalysis,
                                                  useTheseLhVariables = lhVariablesForAnalysis,
                                                  OutputLevel = BTaggingFlags.OutputLevel
                                                 )
ToolSvc += myBTagLikelihoodToolBU
if BTaggingFlags.OutputLevel < 3:
  print myBTagLikelihoodToolBU

from JetTagTools.JetTagToolsConf import Analysis__SecVtxTag
BUSecVtxTagTool = Analysis__SecVtxTag( 
                                      name = "SecVtxTagBU",
                                      Runmodus = BTaggingFlags.Runmodus,
                                      VertexFindingMode = "BU",
                                      OutputLevel     = BTaggingFlags.OutputLevel,
                                      #VertexFitterTool  = BTagVxFastFitterTool,
                                      TrackToVertexTool = BTagTrackToVertexTool,
                                      LikelihoodTool    = myBTagLikelihoodToolBU,
                                      useVariables      = lhVariablesForAnalysis
                                     )
# make sure to create all histos when in ref mode
if BTaggingFlags.Runmodus == 'reference':
  BUSecVtxTagTool.useVariables = [ "vtxmassBU", "vtxmultBU", "vtxdistBU", "vtxenergyFractionBU", "vtxNGTVtxBU" ]

ToolSvc += BUSecVtxTagTool
if BTaggingFlags.OutputLevel < 3:
  print BUSecVtxTagTool

