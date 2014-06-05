include.block( "JetTagD3PDMaker/MuonScatteringSigToolConfig.py" )

from AthenaCommon import CfgMgr
        #if not hasattr(self.ToolSvc,"iPatFitter"):
        #    include("MuidExample/iPatFitter.py")
        # extrapolator configuration taken from MuonRecExample
from AthenaCommon.AppMgr    import ServiceMgr
from AthenaCommon.AppMgr    import ToolSvc
from MuonRecExample import MuonRecTools
atlasExtrapolator             = MuonRecTools.getPublicTool('AtlasExtrapolator')
muonExtrapolator              = MuonRecTools.getPublicTool('MuonExtrapolator')

# material allocation (uses TrackingGeometry)
from TrkiPatFitter.TrkiPatFitterConf import Trk__MaterialAllocator
muidMaterialAllocator = Trk__MaterialAllocator(
    name                      = 'MuidMaterialAllocator',
    AggregateMaterial         = True,
    Extrapolator              = atlasExtrapolator,
    SpectrometerExtrapolator  = muonExtrapolator,
    TrackingGeometrySvc       = ServiceMgr.AtlasTrackingGeometrySvc)
ToolSvc += muidMaterialAllocator

# and the fitter
from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatFitter
iPatFitter = Trk__iPatFitter(
    name                      = 'iPatFitter',
    AggregateMaterial         = True,
    FullCombinedFit           = True,
    MaterialAllocator         = muidMaterialAllocator)

ToolSvc += iPatFitter
from MuonCombinedEvaluationTools.MuonCombinedEvaluationToolsConf import Rec__MuonScatteringAngleSignificanceTool
MuonScatteringSigTool = Rec__MuonScatteringAngleSignificanceTool(name="MuonScatteringSigTool",
                                                                 TrackFitter = ToolSvc.iPatFitter)
ToolSvc +=  MuonScatteringSigTool
