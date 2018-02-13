# FTK
from RecExConfig.RecFlags import rec
FtkESDList = []
if rec.doFTK:
    FtkESDList = [
        "TrackCollection#FTK_TrackCollection",
        "TrackCollection#FTK_TrackCollectionRefit",
        "VxContainer#FTK_VxContainer",
        "VxContainer#FTK_VxContainerRefit",
        "xAOD::TrackParticleContainer#FTK_TrackParticleContainer",
        "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerAux.",
        "xAOD::TrackParticleContainer#FTK_TrackParticleContainerRefit",
        "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerRefitAux.",
        "xAOD::VertexContainer#FTK_VertexContainer",
        "xAOD::VertexAuxContainer#FTK_VertexContainerAux.-vxTrackAtVertex",
        "xAOD::VertexContainer#FTK_VertexContainerRefit",
        "xAOD::VertexAuxContainer#FTK_VertexContainerRefitAux.-vxTrackAtVertex",
        "InDet::PixelClusterContainer#FTK_PixelClusterContainer",
        "InDet::SCT_ClusterContainer#FTK_SCT_ClusterContainer",
        ]


    if rec.doTruth():
        FtkESDList += [
            "xAOD::TrackParticleContainer#Converted_FTKTrackParticleContainer",
            "xAOD::TrackParticleAuxContainer#Converted_FTKTrackParticleContainerAux.",
            "DetailedTrackTruthCollection#FTK_Tracks_DetailedTruthCollection",
            "DetailedTrackTruthCollection#FTK_RefitTracks_DetailedTruthCollection",
            "xAOD::TrackParticleContainer#Converted_FTKTrackParticleContainerRefit",
            "xAOD::TrackParticleAuxContainer#Converted_FTKTrackParticleContainerRefitAux.",
            "PRD_MultiTruthCollection#FTK_PRD_MultiTruthPixel",
            "PRD_MultiTruthCollection#FTK_PRD_MultiTruthSCT"
            ]


