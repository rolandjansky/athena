include.block( "UserAnalysisUtils/UserAnalysisOverlapRemovalTool_jobOptions.py" )

# the user selection tools to use
# they are only used here to check if a jet is a b-jet or not  
include ( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )

# the user Overlap Checking tools to use
include ( "UserAnalysisUtils/UserAnalysisOverlapCheckingTool_jobOptions.py" )

from UserAnalysisUtils.UserAnalysisUtilsConf import \
     UserAnalysisOverlapRemovalTool as ConfiguredUserAnalysisOverlapRemovalTool
UserAnalysisOverlapRemovalTool = ConfiguredUserAnalysisOverlapRemovalTool(

  # define the pre-selection tools - used here only to check if a jet is a b-jet
  UserSelectionTool = UserAnalysisSelectionTool,

  # Whether to check overlap in same container or not. 
  # For example, muon overlapping with muon?
  # Currently when set to False, it applies to all contianers. 
  RemoveOverlapInSameContainer = True,

  # define the overlap checking tools
  UserOverlapCheckingTool = UserAnalysisOverlapCheckingTool,

  # thelist of the input container keys - the order is important: the overlap removing will be done in that order
  
  InputContainerKeys=[  "SelectedElectronCollection",
                        "SelectedStacoMuonCollection",
                        "SelectedTauRecContainer",
                        "SelectedAntiKt4TowerJets",
                        "SelectedPhotonAODCollection",
                        "SelectedCaloCalTopoCluster",
                        "SelectedTrackParticleCandidate"
                       ],


  IsAtlfastData=False, # set this to true if running on Atlfast AOD 
                       # Only deltaR overlap removal is done for Atlfast, no cluster/TrackParticle overlap
                       # and Cell/Hit overlap not done in the case of Atlfast 

  # the list of the output container keys - 
  OuputObjectKey         = "FinalStateObjectCollection",
  OutputLeptonKey        = "FinalStateLeptonCollection",
  OutputPhotonKey        = "FinalStatePhotonCollection",
  OutputElectronKey      = "FinalStateElectronCollection",
  OutputMuonKey          = "FinalStateMuonCollection",
  OutputTauJetKey        = "FinalStateTauJetCollection",
  OutputCalloClusterKey  = "FinalStateCaloClusterCollection",
  OutputTrackParticleKey = "FinalStateTrackParticleCollection",
  OutputJetKey           = "FinalStateJetCollection",
  OutputBJetKey          = "FinalStateBJetCollection",
  OutputLightJetKey      = "FinalStateLightJetCollection"

    )

ToolSvc += UserAnalysisOverlapRemovalTool
print      UserAnalysisOverlapRemovalTool
