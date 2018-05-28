# FTK
from RecExConfig.RecFlags import rec
FtkAODList = []

FtkAODList += [
        "xAOD::VertexContainer#FTK_VertexContainer",
        "xAOD::VertexAuxContainer#FTK_VertexContainerAux.-vxTrackAtVertex",
        "xAOD::VertexContainer#FTK_VertexContainerRefit",
        "xAOD::VertexAuxContainer#FTK_VertexContainerRefitAux.-vxTrackAtVertex"
    ]

if rec.doTruth():
    FtkAODList += [
        "xAOD::TrackParticleContainer#Converted_FTKTrackParticleContainer",
        "xAOD::TrackParticleAuxContainer#Converted_FTKTrackParticleContainerAux.",
        "xAOD::TrackParticleContainer#Converted_FTKTrackParticleContainerRefit",
        "xAOD::TrackParticleAuxContainer#Converted_FTKTrackParticleContainerRefitAux.",
    ]
    
FtkAODList += [
    "xAOD::TrackParticleContainer#FTK_TrackParticleContainer",
    "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerAux.",
    "xAOD::TrackParticleContainer#FTK_TrackParticleContainerRefit",
    "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerRefitAux.",
    ]


