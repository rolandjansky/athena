# muon active/passive geometry setup
from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder

# atlas tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

# atlas tracking geometry builder
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
MuonStandAloneGeometryBuilder = Trk__GeometryBuilder( name = "MuonStandAloneGeometryBuilder",
                                                      MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder,
                                                      InDetTrackingGeometry = False,
                                                      CaloTrackingGeometry = False,
                                                      MuonTrackingGeometry = True )

ToolSvc += MuonStandAloneGeometryBuilder
print MuonStandAloneGeometryBuilder

# combined tracking geometry options
from TrkDetDescrSvc.TrkDetDescrSvcConf import TrackingGeometrySvc
MuonTrackingGeometrySvc = TrackingGeometrySvc( name ="MuonTrackingGeometrySvc",
                                               TrackingGeometryName = "MuonStandaloneTrackingGeometry",
                                               GeometryBuilder = MuonStandAloneGeometryBuilder,
                                               BuildGeometryFromTagInfo = True ,
                                               AssignMaterialFromCOOL = False)

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += MuonTrackingGeometrySvc
 
# the navigator Configurable way
from TrkExTools.TrkExToolsConf import Trk__Navigator
MuonTGNavigator = Trk__Navigator( name = 'MuonTGNavigator',
                                TrackingGeometrySvc = MuonTrackingGeometrySvc
                                )
ToolSvc += MuonTGNavigator
print      MuonTGNavigator
 
# Configurable way
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
MuonTGMaterialUpdator = Trk__MaterialEffectsUpdator( name = "MuonTGMaterialUpdator"
                                                   )
ToolSvc += MuonTGMaterialUpdator
print MuonTGMaterialUpdator
 
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
MuonTGPropagator = Trk__STEP_Propagator( name = 'MuonTGPropagator' ,Tolerance=0.0001, MaterialEffects=True,IncludeBgradients=False)
ToolSvc += MuonTGPropagator
print      MuonTGPropagator

 
# assign it to the Extrapolator
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
MuonTGExtrapolator = Trk__Extrapolator( name = 'MuonTGExtrapolator', 
                                      Propagators = [ MuonTGPropagator ],
                                      Navigator = MuonTGNavigator,
                                      MaterialEffectsUpdators = [ MuonTGMaterialUpdator ]
                                      )
ToolSvc += MuonTGExtrapolator
print MuonTGExtrapolator                          
 
# 


from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonTGMeasurementTool
MuonTGMeasurementTool = Muon__MuonTGMeasurementTool(  name = 'MuonTGMeasurementTool', 
                                                      TrackingGeometryName = 'MuonStandaloneTrackingGeometry' )
ToolSvc += MuonTGMeasurementTool
print MuonTGMeasurementTool 


