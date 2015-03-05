# Tools needed for vertexing with VKalVert:
#
# Magnetic field tool
#
from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
InDetMagField = Trk__MagneticFieldTool('InDetMagField')
ToolSvc += InDetMagField
print      InDetMagField
#
# set up extrapolator
#
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
InDetExtrapolator = AtlasExtrapolator()
ToolSvc += InDetExtrapolator
print      InDetExtrapolator 

# Vertex point estimator
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
VtxPointEstimator = InDet__VertexPointEstimator(name                   = "VtxPointEstimator",
                                                MinDeltaR              = [-10000.,-10000.,-10000.],
                                                MaxDeltaR              = [10000.,10000.,10000.],
                                                MaxPhi                 = [10000., 10000., 10000.],
                                                MaxChi2OfVtxEstimation = 2000.)
ToolSvc += VtxPointEstimator

# Helper Tool
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
InDetConversionHelper = InDet__ConversionFinderUtils(name = "InDetConversionFinderUtils")
ToolSvc += InDetConversionHelper

# configure vertex fitters
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(name                = "VKalVrtFitterName",
                                         Extrapolator        = InDetExtrapolator,
                                         MagFieldSvc         = InDetMagField,
                                         FirstMeasuredPoint  = True,
                                         #FirstMeasuredPointLimit = True,
                                         MakeExtendedVertex  = True)
ToolSvc += TrkVKalVrtFitter
print      TrkVKalVrtFitter

# V0 tools
from TrkVertexAnalysisUtils.TrkVertexAnalysisUtilsConf import Trk__V0Tools
InDetV0Tools = Trk__V0Tools(name = "InDetV0Tools")
ToolSvc += InDetV0Tools
print InDetV0Tools

# TrigEffJpsiTools - trigger matching tool
from TrigEffJpsiTools.TrigEffJpsiToolsConf import TrigEffJpsiTools
myTool = TrigEffJpsiTools("TrigEffJpsiTools")
ToolSvc += myTool

