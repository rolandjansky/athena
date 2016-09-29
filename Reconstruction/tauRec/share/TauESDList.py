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
# Tau1P3P cell cluster
#------------------------------------------------------------------------------
#TauESDList += [ "CaloClusterContainer#Tau1P3PCellCluster" ]
#TauESDList += [ "CaloCellLinkContainer#Tau1P3PCellCluster_Link" ]
#TauESDList += [ "CaloShowerContainer#Tau1P3PCellCluster_Data" ]

#------------------------------------------------------------------------------
# TauRec cell cluster
#------------------------------------------------------------------------------
#TauESDList += [ "CaloClusterContainer#TauRecCellCluster" ]
#TauESDList += [ "CaloCellLinkContainer#TauRecCellCluster_Link" ]
#TauESDList += [ "CaloShowerContainer#TauRecCellCluster_Data" ]

#------------------------------------------------------------------------------
# Tau1P3P Pi0 cluster
#------------------------------------------------------------------------------
#TauESDList += [ "CaloClusterContainer#TauPi0ClusterContainer" ]
#TauESDList += [ "CaloCellLinkContainer#TauPi0ClusterContainer_Link" ]
#TauESDList += [ "CaloShowerContainer#TauPi0ClusterContainer_Data" ]
#TauESDList += [ "CaloCellContainer#TauCommonPi0CellContainer" ]  # for studies of the cell-based algorithm

#------------------------------------------------------------------------------
# Tau Pi0 cluster
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::CaloClusterContainer#TauPi0Clusters" ]
TauESDList += [ "xAOD::CaloClusterAuxContainer#TauPi0ClustersAux." ]


#------------------------------------------------------------------------------
# Tau Pi0 cluster cell container and links
#------------------------------------------------------------------------------
TauESDList += [ "CaloClusterCellLinkContainer#TauPi0Clusters_links" ]
TauESDList += [ "CaloCellContainer#TauCommonPi0Cells" ]

#------------------------------------------------------------------------------
# TauRec main xAOD containers
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::TauJetContainer#TauJets" ]
TauESDList += [ "xAOD::TauJetAuxContainer#TauJetsAux." ]

#------------------------------------------------------------------------------
# TauRec tauTrack xAOD containers
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::TauTrackContainer#TauTracks" ]
TauESDList += [ "xAOD::TauTrackAuxContainer#TauTracksAux." ]

#------------------------------------------------------------------------------
# Secondary Vertex for Tau Decay
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::VertexContainer#TauSecondaryVertices" ]
TauESDList += [ "xAOD::VertexAuxContainer#TauSecondaryVerticesAux.-vxTrackAtVertex" ]

TauESDList += [ "xAOD::ParticleContainer#finalTauPi0s" ]
TauESDList += [ "xAOD::ParticleAuxContainer#finalTauPi0sAux." ]
##------------------------------------------------------------------------------
## Tau shot clusters
##------------------------------------------------------------------------------
#TauESDList += [ "CaloClusterContainer#TauShotClusters" ]
#TauESDList += [ "CaloCellLinkContainer#TauShotClusters_Link" ]
#TauESDList += [ "CaloShowerContainer#TauShotClusters_Data" ]

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
                    

#-------------------------------------------------------------------------
# eflowObjects for tau
#--------------------------------------------------------------------------
#TauESDList += [ "eflowObjectContainer#eflowObjects_tauMode" ]
#TauESDList += [ "xAOD::PFOContainer#neutralTauPFO_eflowRec" ]
#TauESDList += [ "xAOD::PFOAuxContainer#neutralTauPFO_eflowRecAux." ]
#TauESDList += [ "xAOD::PFOContainer#chargedTauPFO_eflowRec" ]
#TauESDList += [ "xAOD::PFOAuxContainer#chargedTauPFO_eflowRecAux." ]


        
##------------------------------------------------------------------------------
## Tau1P3P cell EM012 cluster
##------------------------------------------------------------------------------
#TauESDList += [ "CaloClusterContainer#Tau1P3PCellEM012ClusterContainer" ]
#TauESDList += [ "CaloCellLinkContainer#Tau1P3PCellEM012ClusterContainer_Link" ]
#TauESDList += [ "CaloShowerContainer#Tau1P3PCellEM012ClusterContainer_Data" ]
                                     

from DiTauRec.DiTauRecFlags import jobproperties
if jobproperties.DiTauRecFlags.doDiTauRec():
    include("DiTauRec/DiTauESDList.py")
    TauESDList += DiTauESDList
    pass
