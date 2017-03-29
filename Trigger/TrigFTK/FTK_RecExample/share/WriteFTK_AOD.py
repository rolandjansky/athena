# FTK
from RecExConfig.RecFlags import rec
FtkAODList = []
if rec.doFTK:
    FtkAODList = [
        #"VxContainer#FTK_VxContainer",
        #"VxContainer#FTK_VxContainerRefit",
        #"TrackCollection#FTK_TrackCollection",
        #"TrackCollection#FTK_TrackCollectionRefit",
        "xAOD::TrackParticleContainer#FTK_TrackParticleContainer",
        "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerAux.",
        "xAOD::TrackParticleContainer#FTK_TrackParticleContainerRefit",
        "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerRefitAux.",
        "xAOD::VertexContainer#FTK_VertexContainer",
        "xAOD::VertexAuxContainer#FTK_VertexContainerAux.-vxTrackAtVertex",
        "xAOD::VertexContainer#FTK_VertexContainerRefit",
        "xAOD::VertexAuxContainer#FTK_VertexContainerRefitAux.-vxTrackAtVertex",
        ]
