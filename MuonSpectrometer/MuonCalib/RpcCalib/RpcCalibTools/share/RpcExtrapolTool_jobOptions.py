# the geometry builder alg tool
#from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder

#GeometryBuilder = Trk__GeometryBuilder(name = 'AtlasGeometryBuilder')
# register it to tool svc
#ToolSvc += AtlasGeometryBuilder

#AtlasGeometryBuilder.MuonTrackingGeometry    = True
#from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder

#MuonTrackingGeometryBuilder.BuildActiveMaterial = True
#AtlasGeometryBuilder.MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder


#from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP_Propagator
#STEP_Propagator = STEP_Propagator(name = 'STEP_Propagator')
#ToolSvc += STEP_Propagator

# the Extrapolator
#from TrkExTools.TrkExToolsConf import Trk__Extrapolator
#Extrapolator = Trk__Extrapolator( name = 'Extrapolator',
#                                 Propagators = [ STEP_Propagator])
#ToolSvc += Extrapolator

from RpcCalibTools.RpcCalibToolsConf import RpcExtrapolationTool
RpcExtrapolationTool = RpcExtrapolationTool('RpcExtrapolationTool')
ToolSvc+=RpcExtrapolationTool

from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonTGMeasurementTool
MuonTGMeasurementTool = Muon__MuonTGMeasurementTool(  name = 'MuonTGMeasurementTool',
                                                      TrackingGeometryName = 'AtlasTrackingGeometry' )
ToolSvc += MuonTGMeasurementTool

