include.block( "UserAnalysisUtils/UserAnalysisPreparationTool_jobOptions.py" )

# the user selection tools to use
include ( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )

from UserAnalysisUtils.UserAnalysisUtilsConf import \
     UserAnalysisPreparationTool as ConfiguredUserAnalysisPreparationTool
UserAnalysisPreparationTool = ConfiguredUserAnalysisPreparationTool(

  # define the pre-selection tools
  UserSelectionTool = UserAnalysisSelectionTool,

  # thelist of the input container keys - the order does not matter
  InputContainerKeys=[ "ElectronAODCollection",
                       "StacoMuonCollection",
                       "TauRecContainer",
                       "AntiKt4TowerJets",
                       "PhotonAODCollection",
                       "CaloCalTopoCluster",
                       "TrackParticleCandidate"
                       ],


  IsAtlfastData=False, # set this to true if running on Atlfast AOD - some of the cuts are not used in this case

  # the list of the output container keys - these containers container the selected objects
  # The order matter::Should follow the same order as the input container keys above
  OutputContainerKeys=[ "SelectedElectronCollection",
                        "SelectedStacoMuonCollection",
                        "SelectedTauRecContainer",
                        "SelectedAntiKt4TowerJets",
                        "SelectedPhotonAODCollection",
                        "SelectedCaloCalTopoCluster",
                        "SelectedTrackParticleCandidate"
                        ]

    )

ToolSvc += UserAnalysisPreparationTool
print      UserAnalysisPreparationTool
