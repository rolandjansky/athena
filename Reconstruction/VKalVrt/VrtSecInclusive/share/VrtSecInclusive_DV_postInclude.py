try:
	StreamESD.ItemList+=["xAOD::TrackParticleContainer#VrtSecInclusive*"]
	StreamESD.ItemList+=["xAOD::VertexContainer#VrtSecInclusive*"]
	StreamESD.ItemList+=["xAOD::VertexAuxContainer#VrtSecInclusive*"]
except:
	print "StreamESD undefined"

try:
	StreamAOD.ItemList+=["xAOD::TrackParticleContainer#VrtSecInclusive*"]
	StreamAOD.ItemList+=["xAOD::VertexContainer#VrtSecInclusive*"]
	StreamAOD.ItemList+=["xAOD::VertexAuxContainer#VrtSecInclusive*"]
except:
	print "StreamAOD undefined"


ToolSvc.EGammaAmbiguityTool.minNoSiHits = 2
ToolSvc.EGammaAmbiguityTool.minNoPixHits = 0
ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.IPd0Max = 9999.
ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.nHitPix = 0
ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.nHitSct = 2
ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.nHitSi = 2
ToolSvc.CaloTrkMuIdAlgTrackSelectorTool.IPd0Max = 9999.
ToolSvc.CaloTrkMuIdAlgTrackSelectorTool.nHitPix = 0
ToolSvc.CaloTrkMuIdAlgTrackSelectorTool.nHitSct = 2
ToolSvc.CaloTrkMuIdAlgTrackSelectorTool.nHitSi = 2
