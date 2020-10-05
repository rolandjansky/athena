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
# Tau Pi0 cluster
#------------------------------------------------------------------------------
#TauAODList += [ "xAOD::CaloClusterContainer#TauPi0Clusters" ]
#TauAODList += [ "xAOD::CaloClusterAuxContainer#TauPi0ClustersAux." ]

TauAODList += [ "xAOD::CaloClusterContainer#TauPi0SubtractedClusters" ]
TauAODList += [ "xAOD::CaloClusterAuxContainer#TauPi0SubtractedClustersAux." ]
TauAODList += [ "CaloClusterCellLinkContainer#TauPi0SubtractedClusters_links" ]

#------------------------------------------------------------------------------
# TauRec main xAOD containers
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauJetContainer#TauJets" ]
TauAODList += [ "xAOD::TauJetAuxContainer#TauJetsAux.-mu.-nVtxPU.-ABS_ETA_LEAD_TRACK.-TAU_ABSDELTAPHI.-TAU_ABSDELTAETA.-absipSigLeadTrk" ]

#------------------------------------------------------------------------------
# TauRec tauTrack xAOD containers
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauTrackContainer#TauTracks" ]
TauAODList += [ "xAOD::TauTrackAuxContainer#TauTracksAux." ]

#------------------------------------------------------------------------------
# Secondary Vertex for Tau Decay
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::VertexContainer#TauSecondaryVertices" ]
TauAODList += [ "xAOD::VertexAuxContainer#TauSecondaryVerticesAux.-vxTrackAtVertex" ]

#------------------------------------------------------------------------------
# Final reconstructed 4-vectors of Pi0s
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::ParticleContainer#finalTauPi0s" ]
TauAODList += [ "xAOD::ParticleAuxContainer#finalTauPi0sAux." ]

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

from DiTauRec.DiTauRecFlags import jobproperties
if jobproperties.DiTauRecFlags.doDiTauRec():
    include("DiTauRec/DiTauAODList.py")
    TauAODList += DiTauAODList


#-------------------------------------------------------------------------
# eflowObjects for tau
#--------------------------------------------------------------------------
#TauAODList += [ "eflowObjectContainer#eflowObjects_tauMode" ]
#TauAODList += [ "xAOD::PFOContainer#neutralTauPFO_eflowRec" ]
#TauAODList += [ "xAOD::PFOAuxContainer#neutralTauPFO_eflowRecAux." ]
#TauAODList += [ "xAOD::PFOContainer#chargedTauPFO_eflowRec" ]
#TauAODList += [ "xAOD::PFOAuxContainer#chargedTauPFO_eflowRecAux." ]

