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
# Tau1P3P Pi0 cluster
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::CaloClusterContainer_v1#TauPi0ClusterContainer" ]
TauAODList += [ "xAOD::CaloClusterAuxContainer_v1#TauPi0ClusterContainerAux." ]

#------------------------------------------------------------------------------
# Tau1P3P cell EM012 cluster
#------------------------------------------------------------------------------
#TauAODList += [ "CaloClusterContainer#Tau1P3PCellEM012ClusterContainer" ]

#------------------------------------------------------------------------------
# Tau1P3P main containers
#------------------------------------------------------------------------------
#TauAODList += [ "Analysis::TauJetContainer#Tau1P3PContainer" ]
#TauAODList += [ "Analysis::TauDetailsContainer#Tau1P3PDetailsContainer" ]

#------------------------------------------------------------------------------
# TauRec main containers
#------------------------------------------------------------------------------
#TauAODList += [ "Analysis::TauJetContainer#TauRecContainer" ]
#TauAODList += [ "Analysis::TauDetailsContainer#TauRecDetailsContainer" ]
#TauAODList += [ "Analysis::TauDetailsContainer#TauPi0CandidateDetailsContainer" ]

#------------------------------------------------------------------------------
# TauRec main xAOD containers
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::TauJetContainer_v1#TauRecContainer" ]
TauAODList += [ "xAOD::TauJetAuxContainer_v1#TauRecContainerAux." ]

#------------------------------------------------------------------------------
# Secondary Vertex for Tau Decay
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::VertexContainer_v1#TauSecondaryVertexContainer" ]
TauAODList += [ "xAOD::VertexAuxContainer_v1#TauSecondaryVertexContainerAux.-vxTrackAtVertex" ]

#------------------------------------------------------------------------------
# Shot PFOs
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer_v1#TauShotPFOContainer" ]
TauAODList += [ "xAOD::PFOAuxContainer_v1#TauShotPFOContainerAux." ]

#------------------------------------------------------------------------------
# Cell-based charged PFOs
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer_v1#TauPi0ChargedPFOContainer" ]
TauAODList += [ "xAOD::PFOAuxContainer_v1#TauPi0ChargedPFOContainerAux." ]

#------------------------------------------------------------------------------
# Cell-based neutral PFOs
#------------------------------------------------------------------------------
TauAODList += [ "xAOD::PFOContainer_v1#TauPi0NeutralPFOContainer" ]
TauAODList += [ "xAOD::PFOAuxContainer_v1#TauPi0NeutralPFOContainerAux." ]

#-------------------------------------------------------------------------
# eflowObjects for tau
#--------------------------------------------------------------------------
#TauAODList += [ "eflowObjectContainer#eflowObjects_tauMode" ]
TauAODList += [ "xAOD::PFOContainer_v1#neutralTauPFO_eflowRec" ]
TauAODList += [ "xAOD::PFOAuxContainer_v1#neutralTauPFO_eflowRecAux." ]
TauAODList += [ "xAOD::PFOContainer_v1#chargedTauPFO_eflowRec" ]
TauAODList += [ "xAOD::PFOAuxContainer_v1#chargedTauPFO_eflowRecAux." ]


