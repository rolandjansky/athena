################################################################################
##
#@file TauAODList.py
#
#@brief List AOD output containers. 
################################################################################

#------------------------------------------------------------------------------
# AOD output list
#------------------------------------------------------------------------------
TauAODList = []

#------------------------------------------------------------------------------
# Taus
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauJetContainer#TauJets" ]
TauAODList += [ "xAOD::TauJetAuxContainer#TauJetsAux.-mu.-nVtxPU.-ABS_ETA_LEAD_TRACK.-TAU_ABSDELTAPHI.-TAU_ABSDELTAETA.-absipSigLeadTrk" ]

#------------------------------------------------------------------------------
# Tau tracks
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauTrackContainer#TauTracks" ]
TauAODList += [ "xAOD::TauTrackAuxContainer#TauTracksAux." ]

#------------------------------------------------------------------------------
# Secondary vertex
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::VertexContainer#TauSecondaryVertices" ]
TauAODList += [ "xAOD::VertexAuxContainer#TauSecondaryVerticesAux.-vxTrackAtVertex" ]

#------------------------------------------------------------------------------
# Pi0 cells
#------------------------------------------------------------------------------
TauAODList += [ "CaloCellContainer#TauCommonPi0Cells" ]
TauAODList += [ "CaloClusterCellLinkContainer#TauInitialPi0Clusters_links" ]

#------------------------------------------------------------------------------
# Pi0 initial clusters
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::CaloClusterContainer#TauInitialPi0Clusters" ]
TauAODList += [ "xAOD::CaloClusterAuxContainer#TauInitialPi0ClustersAux." ]

#------------------------------------------------------------------------------
# Shot clusters
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::CaloClusterContainer#TauShotClusters"]
TauAODList += [ "xAOD::CaloClusterAuxContainer#TauShotClustersAux."]

#------------------------------------------------------------------------------
# Final reconstructed 4-vectors of Pi0s
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::ParticleContainer#TauFinalPi0s" ]
TauAODList += [ "xAOD::ParticleAuxContainer#TauFinalPi0sAux." ]

#------------------------------------------------------------------------------
# Shot ParticleFlowObjects
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer#TauShotParticleFlowObjects" ]
TauAODList += [ "xAOD::PFOAuxContainer#TauShotParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based charged ParticleFlowObjects
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer#TauChargedParticleFlowObjects" ]
TauAODList += [ "xAOD::PFOAuxContainer#TauChargedParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based neutral ParticleFlowObjects
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer#TauNeutralParticleFlowObjects" ]
TauAODList += [ "xAOD::PFOAuxContainer#TauNeutralParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Cell-based hadronic cluster ParticleFlowObjects
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer#TauHadronicParticleFlowObjects" ]
TauAODList += [ "xAOD::PFOAuxContainer#TauHadronicParticleFlowObjectsAux." ]

#------------------------------------------------------------------------------
# Di-tau AOD content
#------------------------------------------------------------------------------
from DiTauRec.DiTauRecFlags import jobproperties
if jobproperties.DiTauRecFlags.doDiTauRec():
    include("DiTauRec/DiTauAODList.py")
    TauAODList += DiTauAODList
