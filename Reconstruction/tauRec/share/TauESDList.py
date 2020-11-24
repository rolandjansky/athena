################################################################################
##
#@file TauESDList.py
#
#@brief List ESD output containers. 
################################################################################

#------------------------------------------------------------------------------
# ESD output list
#------------------------------------------------------------------------------
TauESDList = []

#------------------------------------------------------------------------------
# Taus
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::TauJetContainer#TauJets" ]
TauESDList += [ "xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters" ]

#------------------------------------------------------------------------------
# Tau tracks
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::TauTrackContainer#TauTracks" ]
TauESDList += [ "xAOD::TauTrackAuxContainer#TauTracksAux." ]

#------------------------------------------------------------------------------
# Secondary vertex
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::VertexContainer#TauSecondaryVertices" ]
TauESDList += [ "xAOD::VertexAuxContainer#TauSecondaryVerticesAux.-vxTrackAtVertex" ]

#------------------------------------------------------------------------------
# Pi0 clusters and links to cells
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::CaloClusterContainer#TauPi0Clusters" ]
TauESDList += [ "xAOD::CaloClusterAuxContainer#TauPi0ClustersAux." ]
TauESDList += [ "CaloClusterCellLinkContainer#TauPi0Clusters_links" ]

#------------------------------------------------------------------------------
# Shot clusters and cell links
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::CaloClusterContainer#TauShotClusters"]
TauESDList += [ "xAOD::CaloClusterAuxContainer#TauShotClustersAux."]
TauESDList += [ "CaloClusterCellLinkContainer#TauShotClusters_links" ]

#------------------------------------------------------------------------------
# Final reconstructed 4-vectors of Pi0s
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::ParticleContainer#TauFinalPi0s" ]
TauESDList += [ "xAOD::ParticleAuxContainer#TauFinalPi0sAux." ]

#------------------------------------------------------------------------------
# Shot ParticleFlowObjects
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer#TauShotParticleFlowObjects" ]
TauESDList += [ "xAOD::PFOAuxContainer#TauShotParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based charged ParticleFlowObjects
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer#TauChargedParticleFlowObjects" ]
TauESDList += [ "xAOD::PFOAuxContainer#TauChargedParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based neutral ParticleFlowObjects
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer#TauNeutralParticleFlowObjects" ]
TauESDList += [ "xAOD::PFOAuxContainer#TauNeutralParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based hadronic cluster ParticleFlowObjects
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer#TauHadronicParticleFlowObjects" ]
TauESDList += [ "xAOD::PFOAuxContainer#TauHadronicParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Di-tau AOD content
#------------------------------------------------------------------------------
from DiTauRec.DiTauRecFlags import jobproperties
if jobproperties.DiTauRecFlags.doDiTauRec():
    include("DiTauRec/DiTauESDList.py")
    TauESDList += DiTauESDList
