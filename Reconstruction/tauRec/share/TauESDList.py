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
TauESDList += [ "CaloClusterContainer#Tau1P3PCellCluster" ]
TauESDList += [ "CaloCellLinkContainer#Tau1P3PCellCluster_Link" ]
TauESDList += [ "CaloShowerContainer#Tau1P3PCellCluster_Data" ]

#------------------------------------------------------------------------------
# TauRec cell cluster
#------------------------------------------------------------------------------
TauESDList += [ "CaloClusterContainer#TauRecCellCluster" ]
TauESDList += [ "CaloCellLinkContainer#TauRecCellCluster_Link" ]
TauESDList += [ "CaloShowerContainer#TauRecCellCluster_Data" ]

#------------------------------------------------------------------------------
# Tau1P3P Pi0 cluster
#------------------------------------------------------------------------------
TauESDList += [ "CaloClusterContainer#TauPi0ClusterContainer" ]
TauESDList += [ "CaloCellLinkContainer#TauPi0ClusterContainer_Link" ]
TauESDList += [ "CaloShowerContainer#TauPi0ClusterContainer_Data" ]
#TauESDList += [ "CaloCellContainer#TauCommonPi0CellContainer" ]  # for studies of the cell-based algorithm

#------------------------------------------------------------------------------
# Tau shot clusters
#------------------------------------------------------------------------------
TauESDList += [ "CaloClusterContainer#TauShotClusterContainer" ]
TauESDList += [ "CaloCellLinkContainer#TauShotClusterContainer_Link" ]
TauESDList += [ "CaloShowerContainer#TauShotClusterContainer_Data" ]

#------------------------------------------------------------------------------
# Shot PFOs
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer_v1#TauShotPFOContainer" ]
TauESDList += [ "xAOD::PFOAuxContainer_v1#TauShotPFOContainerAux." ]

#------------------------------------------------------------------------------
# Cell-based charged PFOs
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer_v1#TauPi0ChargedPFOContainer" ]
TauESDList += [ "xAOD::PFOAuxContainer_v1#TauPi0ChargedPFOContainerAux." ]

#------------------------------------------------------------------------------
# Cell-based neutral PFOs
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::PFOContainer_v1#TauPi0NeutralPFOContainer" ]
TauESDList += [ "xAOD::PFOAuxContainer_v1#TauPi0NeutralPFOContainerAux." ]

#------------------------------------------------------------------------------
# Secondary Vertex for Tau Decay
#------------------------------------------------------------------------------
TauESDList += [ "xAOD::VertexContainer_v1#TauSecondaryVertexContainer" ]
TauESDList += [ "xAOD::VertexAuxContainer_v1#TauSecondaryVertexContainerAux.-vxTrackAtVertex" ]
                                     
#------------------------------------------------------------------------------
# Tau1P3P cell EM012 cluster
#------------------------------------------------------------------------------
TauESDList += [ "CaloClusterContainer#Tau1P3PCellEM012ClusterContainer" ]
TauESDList += [ "CaloCellLinkContainer#Tau1P3PCellEM012ClusterContainer_Link" ]
TauESDList += [ "CaloShowerContainer#Tau1P3PCellEM012ClusterContainer_Data" ]
                                     

#------------------------------------------------------------------------------
# Tau1P3P main containers
#------------------------------------------------------------------------------
TauESDList += [ "Analysis::TauJetContainer#Tau1P3PContainer" ]
TauESDList += [ "Analysis::TauDetailsContainer#Tau1P3PDetailsContainer" ]
TauESDList += [ "Analysis::TauDetailsContainer#Tau1P3PExtraDetailsContainer" ]

#------------------------------------------------------------------------------
# TauRec main containers
#------------------------------------------------------------------------------
TauESDList += [ "Analysis::TauJetContainer#TauRecContainer" ]
TauESDList += [ "Analysis::TauDetailsContainer#TauRecDetailsContainer" ]
TauESDList += [ "Analysis::TauDetailsContainer#TauRecExtraDetailsContainer" ]
TauESDList += [ "Analysis::TauDetailsContainer#TauPi0CandidateDetailsContainer" ]

#-------------------------------------------------------------------------
# eflowObjects for tau
#--------------------------------------------------------------------------
TauESDList += [ "eflowObjectContainer#eflowObjects_tauMode" ]
