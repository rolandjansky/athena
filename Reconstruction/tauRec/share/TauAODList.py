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
# Tau1P3P cell cluster
#------------------------------------------------------------------------------
#TauAODList += [ "CaloClusterContainer#Tau1P3PCellCluster" ]

#------------------------------------------------------------------------------
# TauRec cell cluster
#------------------------------------------------------------------------------
#TauAODList += [ "CaloClusterContainer#TauRecCellCluster" ]

#------------------------------------------------------------------------------
# Tau Pi0 cluster
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::CaloClusterContainer#TauPi0Clusters" ]
TauAODList += [ "xAOD::CaloClusterAuxContainer#TauPi0ClustersAux." ]

#------------------------------------------------------------------------------
# Tau1P3P cell EM012 cluster
#------------------------------------------------------------------------------
#TauAODList += [ "CaloClusterContainer#Tau1P3PCellEM012ClusterContainer" ]

#------------------------------------------------------------------------------
# TauRec main xAOD containers
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauJetContainer#TauJets" ]
#TauAODList += [ "xAOD::TauJetAuxContainer#TauJetsAux.-ABS_ETA_LEAD_TRACK.-CORRCENTFRAC.-CORRFTRK.-EMFRACTIONATEMSCALE_MOVEE3.-HADLEAKET.-NUMTRACK.-TAU_ABSDELTAETA.-TAU_ABSDELTAPHI.-TAU_SEEDTRK_SECMAXSTRIPETOVERPT.-TAU_TRT_NHT_OVER_NLT.-NUMVERTICES" ]
TauAODList += [ "xAOD::TauJetAuxContainer#TauJetsAux.-mu.-nVtx.-NUMTRACK.-NUMVERTICES.-nVtxPU.-MU" ]

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
    pass


#-------------------------------------------------------------------------
# eflowObjects for tau
#--------------------------------------------------------------------------
#TauAODList += [ "eflowObjectContainer#eflowObjects_tauMode" ]
#TauAODList += [ "xAOD::PFOContainer#neutralTauPFO_eflowRec" ]
#TauAODList += [ "xAOD::PFOAuxContainer#neutralTauPFO_eflowRecAux." ]
#TauAODList += [ "xAOD::PFOContainer#chargedTauPFO_eflowRec" ]
#TauAODList += [ "xAOD::PFOAuxContainer#chargedTauPFO_eflowRecAux." ]

