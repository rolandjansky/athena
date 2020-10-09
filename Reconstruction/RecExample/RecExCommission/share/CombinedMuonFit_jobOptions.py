from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
GlobalChi2FitterComb=Trk__GlobalChi2Fitter(name='GlobalChi2FitterComb',
                                       StraightLine=not jobproperties.BField.solenoidOn(),
                                       ExtrapolationTool=InDetExtrapolator,
                                       NavigatorTool = InDetNavigator,
                                       PropagatorTool = InDetPropagator,
                                       RotCreatorTool = InDetRotCreator,
                                       OutlierCut = 5.,
                                       ExternalMat = True,
                                       sortingReferencePoint = [0,25000,0])

ToolSvc += GlobalChi2FitterComb

print(GlobalChi2FitterComb)

from CombinedMuonRefit.CombinedMuonRefitConf import ReFitMuon
MuonReFitTrack2 = ReFitMuon (name = "CombMuonRefit",
                             FitterTool     = GlobalChi2FitterComb,
                             Cosmics=True,
                             IDTrackName="Tracks",
                             MSTrackName="MooreTracks",
                             matEffects=0,
                             runOutlier=False,
                             NewTrackName="Combined_Tracks",
                             OutputLevel=3)

topSequence += MuonReFitTrack2
print(MuonReFitTrack2)

#GlobalChi2FitterBarrel.OutputLevel=1
#GlobalChi2FitterBarrel.NumericalDerivs=True
#MdtDriftCircleOnTrackCreator = Service( "ToolSvc.Muon::MdtDriftCircleOnTrackCreator" )
#MdtDriftCircleOnTrackCreator.CreateTubeHit = True
#MdtDriftCircleOnTrackCreator.OutputLevel=1
#MdtTubeHitOnTrackCreator = Service( "ToolSvc.Muon::MdtTubeHitOnTrackCreator" )
#MdtTubeHitOnTrackCreator.CreateTubeHit = True
#MdtTubeHitOnTrackCreator.OutputLevel=1


#mooretotracktool=Service("ToolSvc.Muon::MooreToTrackTool")
#mooretotracktool.OutputLevel=1

#ToolSvc.TrackFitter.OutputLevel=1
#ToolSvc.GlobalChi2FitterBarrel.LayerCreatorTool="Trk::MuidDynamicLayerCreator/MuidDynamicLayerCreator"
#ToolSvc.GlobalChi2FitterBarrel.ExternalMat=True
#ToolSvc.GlobalChi2FitterBarrel.Momentum=5000
#ToolSvc.MuidDynamicLayerCreator.Cosmics=True
#MuonReFitTrack2.OutputLevel=1

