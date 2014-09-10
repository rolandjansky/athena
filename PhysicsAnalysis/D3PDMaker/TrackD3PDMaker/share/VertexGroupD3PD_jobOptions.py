#
## @file TrackD3PDMaker/share/VertexGroupD3PD_jobOptions.py
## @brief Setup D3PD Maker algorithm for vertex group D3PD production
## @author Simone Pagan Griso
## @date Mar, 2012
##

"""
Setup D3PD Maker algorithm to produce Vertex Group D3PD
"""

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc
from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
from TrackD3PDMaker.VertexD3PDAnalysisKeys import VertexD3PDAnalysisKeys

### Pre-configure options (if you want your custom options, set and lock them before including this file)
include("TrackD3PDMaker/VertexGroupD3PD_loadConfig.py")

### Now semi-automatic settings -- do manually in your JO!
#if VertexD3PDAnalysisFlags.useAllVertexCollections:
#    #Need to enable algorithms in IDRE
#    from InDetRecExample.InDetJobProperties import InDetFlags
#    InDetFlags.doVertexFindingForMonitoring.set_Value_and_Lock(True)
#    #Include IDRE job options
#    include("InDetRecExample/InDetRec_all.py")

### Now setup D3PD Maker algorithm
import D3PDMakerCoreComps
VertexGroupD3PDMaker = D3PDMakerCoreComps.MakerAlg('VertexGroupD3PDMaker',
                                                   topSequence,
                                                   file=VertexD3PDAnalysisKeys.D3PDFileName(),
                                                   tuplename=VertexD3PDAnalysisKeys.D3PDTreeName())

### Add objects to the algorithm
from TrackD3PDMaker.VertexGroupD3PD import VertexGroupD3PD
VertexGroupD3PD(VertexGroupD3PDMaker, **(VertexD3PDAnalysisFlags.D3PDMakerExtraDict()))
