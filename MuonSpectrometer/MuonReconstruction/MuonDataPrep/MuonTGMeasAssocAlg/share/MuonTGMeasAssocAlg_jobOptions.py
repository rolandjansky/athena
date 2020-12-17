# muon active/passive geometry setup
from MuonTrackingGeometry.ConfigMuonTrackingGeometry import MuonTrackingGeometryBuilder
ToolSvc += MuonTrackingGeometryBuilder

# atlas tracking geometry builder
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
MuonStandAloneGeometryBuilder = Trk__GeometryBuilder( name = "MuonStandAloneGeometryBuilder",
                                        MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder )
ToolSvc += MuonStandAloneGeometryBuilder
ToolSvc.MuonStandAloneGeometryBuilder.InDetTrackingGeometry = False
ToolSvc.MuonStandAloneGeometryBuilder.CaloTrackingGeometry = False
ToolSvc.MuonStandAloneGeometryBuilder.MuonTrackingGeometry = True
print MuonStandAloneGeometryBuilder

# combined tracking geometry options
from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg
MuonTrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg(name = "MuonTrackingGeometryCondAlg",
                                                  GeometryBuilder = MuonStandAloneGeometryBuilder,
                                                  BuildGeometryFromTagInfo = False,
                                                  TrackingGeometryWriteKey = 'MuonStandaloneTrackingGeometry')

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq+= TrkGeoCondAlg

# navigator
from TrkExTools.TrkExToolsConf import Trk__Navigator
MuonNavigator = Trk__Navigator(name = "MuonNavigator" )
ToolSvc += MuonNavigator
ToolSvc.MuonNavigator.TrackingGeometrySvc = "MuonTrackingGeometrySvc"
ToolSvc.MuonNavigator.OutputLevel = 5
print MuonNavigator

# material effects updator
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
MuonMatUpdator = Trk__MaterialEffectsUpdator( name = "MuonMatUpdator" )
ToolSvc += MuonMatUpdator
print MuonMatUpdator

# propagator
from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
MuonPropagator = Trk__STEP_Propagator(name = 'MuonPropagator')
ToolSvc += MuonPropagator
ToolSvc.MuonPropagator.MaterialEffects = True
print MuonPropagator

# extrapolator
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
MuonExtrapolator = Trk__Extrapolator(  name = 'MuonExtrapolator',
                                   Propagators = [ MuonPropagator ] ,
                                   Navigator = MuonNavigator,
                                   MaterialEffectsUpdators = [ MuonMatUpdator ]
                                       )
ToolSvc += MuonExtrapolator
#ToolSvc.MuonExtrapolator.ConsiderMuonStationOverlaps = False
ToolSvc.MuonExtrapolator.OutputLevel = 5
print MuonExtrapolator

#
from MuonTGMeasAssocAlg.MuonTGMeasAssocAlgConf import Muon__MuonTGMeasAssocAlg
MuonTGMeasAssocAlg = Muon__MuonTGMeasAssocAlg(  name = 'MuonTGMeasAssocAlg',
                                                Extrapolator = MuonExtrapolator,
                                                TrackingGeometry = 'MuonStandaloneTrackingGeometry',
                                                ProcessSegments = True )
topSequence += MuonTGMeasAssocAlg
print MuonTGMeasAssocAlg


