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
TauESDList += [ "xAOD::TauJetAuxContainer#TauJetsAux." ]

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
# Pi0 cells
#------------------------------------------------------------------------------
TauESDList += [ "CaloCellContainer#TauCommonPi0Cells" ]
TauESDList += [ "CaloClusterCellLinkContainer#TauInitialPi0Clusters_links" ]

#------------------------------------------------------------------------------
# Pi0 initial clusters
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::CaloClusterContainer#TauInitialPi0Clusters" ]
TauESDList += [ "xAOD::CaloClusterAuxContainer#TauInitialPi0ClustersAux." ]

#------------------------------------------------------------------------------
# Shot clusters
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::CaloClusterContainer#TauShotClusters"]
TauESDList += [ "xAOD::CaloClusterAuxContainer#TauShotClustersAux."]

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
