

###hack - remove it when the track d3pd is fixed
if not 'JetTagD3PD_correctIPEstimatorConfig' in dir():
  JetTagD3PD_correctIPEstimatorConfig=False


if JetTagD3PD_correctIPEstimatorConfig:
  JetTagD3PDTrackToVertexIPEstimator=None
  from AthenaCommon.AppMgr import ToolSvc
  if hasattr(ToolSvc, 'BTagTrackToVertexIPEstimator'):
    JetTagD3PDTrackToVertexIPEstimator=ToolSvc.BTagTrackToVertexIPEstimator
    print "JetTagD3PD Info: found BTagTrackToVertexIPEstimator (for track d3pd)"
  elif hasattr(ToolSvc, 'JetTagD3PDTrackToVertexIPEstimator'):
    print "JetTagD3PD Info: found JetTagD3PDTrackToVertexIPEstimator (for track d3pd)"
    JetTagD3PDTrackToVertexIPEstimator=ToolSvc.JetTagD3PDTrackToVertexIPEstimator
  else:
    print "JetTagD3PD Info: configure TrackToVertexIPEstimator (for track d3pd)"
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    JetTagD3PDLinTrkFactory = Trk__FullLinearizedTrackFactory(name= "JetTagD3PDFullLinearizedTrackFactory",
                                                                Extrapolator = ToolSvc.AtlasExtrapolator)
    ToolSvc += JetTagD3PDLinTrkFactory
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    JetTagD3PDTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(\
      name="JetTagD3PDTrackToVertexIPEstimator",
      Extrapolator=ToolSvc.AtlasExtrapolator,
      LinearizedTrackFactory=JetTagD3PDLinTrkFactory)
    ToolSvc += JetTagD3PDTrackToVertexIPEstimator

  if hasattr(alg,'trk_Filler'):
    if hasattr(alg.trk_Filler,'trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool'):
      if hasattr(alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool,
                 'trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool_trk_IPEstimate'):
        alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool_trk_IPEstimate.TrackToVertexIPEstimator=JetTagD3PDTrackToVertexIPEstimator
 
 
### hack to change the PV collection for some of the tracking d3pd tools
if hasattr(alg,'trk_Filler'):
  if hasattr(alg.trk_Filler,'trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool'):
    if hasattr(alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool,
               'trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool_trk_IPEstimate'):
      alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool.trk_Filler_D3PD__TrackParticlePerigeeAtOOAssociationTool_trk_IPEstimate.VxKey=JetTagD3PDFlags.PrimaryVertexSGKey()
  if hasattr(alg.trk_Filler,'trk_Filler_D3PD__TrackParticlePerigeeAtPVAssociationTool'):
    if hasattr(alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtPVAssociationTool,
               'Associator'):
      alg.trk_Filler.trk_Filler_D3PD__TrackParticlePerigeeAtPVAssociationTool.Associator.SGKey=JetTagD3PDFlags.PrimaryVertexSGKey()
 





