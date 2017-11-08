#====================================================================
# BPHY7.py
# 
#  
# Requires the reductionConf flag BPHY7 in Reco_tf.py   
#====================================================================


# Version with extensive use of BPhysics tools...


# Schematic representation of the Derivation code:

# Stream Setup
#
# Preselection (primary kernel for trigger selection, can take DiMuon part if that is faster)
#
# Selection of 2mu vertices
#
# Selection of 3mu/2mu+trk vertices
#
# Skimming based on passing vertices 
#
# ThinningHelper
# 	Duplicate vertex removal
#	Track and vertex thinning + keep tracks in a cone around candidate
#	Keep muon ID tracks (probably obsolete after the previous tool)
#	Truth thinning
#	Jet thinning
#
# Sequence (for kernels)
#	Kernel n1 (now only triggering)
#	Kernel n2 (The main kernel)
#
# SlimmingHelper
#	Statement of all needed variables
#	SmartCollections, AllVariables etc.
#		



# Set up common services and job object. 
# This should appear in ALL derivation job options


# Set to True to deactivate thinning and skimming, and only keep augmentations (to generate a sample with full xAOD plus all the extra)
onlyAugmentations = False
thinTruth = False

from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation


from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY7Stream )

BPHY7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY7Stream.AcceptAlgs(["BPHY7Kernel2"])

## 0/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY7_VertexTools = BPHYVertexTools("BPHY7")


#====================================================================
# TriggerCounting for Kernel1 #Added by Matteo
#====================================================================
#List of trigggers to be counted (high Sig-eff*Lumi ones are in)
triggersToMetadata= ["HLT_2mu10",
                     "HLT_2mu10_bJpsimumu",
                     "HLT_2mu10_bJpsimumu_delayed",
                     "HLT_2mu10_l2msonly",
                     "HLT_2mu10_nomucomb",
                     "HLT_2mu14",
                     "HLT_2mu14_nomucomb",
                     "HLT_2mu4",
                     "HLT_2mu4_bBmumuxv2",
                     "HLT_2mu4_bDimu_noinvm_novtx_ss",
                     "HLT_2mu6",
                     "HLT_2mu6_10invm30_pt2_z10",
                     "HLT_2mu6_bBmumu",
                     "HLT_2mu6_bBmumux_Taumumux",
                     "HLT_2mu6_bBmumuxv2",
                     "HLT_2mu6_bBmumuxv2_delayed",
                     "HLT_2mu6_bDimu_noinvm_novtx_ss",
                     "HLT_2mu6_bJpsimumu",
                     "HLT_2mu6_bJpsimumu_delayed",
                     "HLT_2mu6_bJpsimumu_Lxy0_delayed",
                     "HLT_2mu6_nomucomb_bPhi",
                     "HLT_2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4",
                     "HLT_3mu4",
                     "HLT_3mu4_bDimu",
                     "HLT_3mu4_bDimu2700",
                     "HLT_3mu4_bTau",
                     "HLT_3mu4_l2msonly",
                     "HLT_3mu4_nomucomb_bTau",
                     "HLT_3mu4_nomucomb_delayed",
                     "HLT_3mu6",
                     "HLT_3mu6_bTau",
                     "HLT_3mu6_msonly",
                     "HLT_mu10_mu6_bBmumux_BcmumuDsloose_delayed",
                     "HLT_mu10_mu6_bBmumux_Taumumux",
                     "HLT_mu10_mu6_bBmumux_Taumumux_noL2",
                     "HLT_mu10_mu6_bBmumuxv2",
                     "HLT_mu10_mu6_bBmumuxv2_delayed",
                     "HLT_mu10_mu6_bJpsimumu",
                     "HLT_mu10_mu6_bJpsimumu_Lxy0",
                     "HLT_mu10_mu6_bJpsimumu_Lxy0_delayed",
                     "HLT_mu10_mu6_bUpsimumu",
                     "HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6",
                     "HLT_mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6",
                     "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6",
                     "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6_bTau",
                     "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6",
                     "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau",
                     "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau_delayed",
                     "HLT_mu18_2mu4noL1",
                     "HLT_mu18_mu8noL1",
                     "HLT_mu20_2mu4noL1",
                     "HLT_mu20_l2idonly_mu6noL1_nscan03",
                     "HLT_mu20_l2idonly_mu6noL1_nscan03_bTau",
                     "HLT_mu20_msonly_mu6noL1_msonly_nscan05",
                     "HLT_mu20_mu8noL1",
                     "HLT_mu20_nomucomb_mu6noL1_nscan03",
                     "HLT_mu20_nomucomb_mu6noL1_nscan03_bTau",
                     "HLT_mu22_2mu4noL1",
                     "HLT_mu22_mu8noL1",
                     "HLT_mu24_2mu4noL1",
                     "HLT_mu24_imedium",
                     "HLT_mu24_mu8noL1",
                     "HLT_mu26_ivarmedium",
                     "HLT_mu26i",
                     "HLT_mu50",
                     "HLT_mu6_2mu4",
                     "HLT_mu6_2mu4_bJpsi_delayed",
                     "HLT_mu6_2mu4_bTau_noL2",
                     "HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4",
                     "HLT_mu6_mu4_bBmumuxv2",
                     "HLT_mu6_mu4_bBmumuxv2_delayed",
                     "HLT_mu6_mu4_bDimu_noinvm_novtx_ss",
                     "HLT_mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4",
                     "HLT_mu6_nomucomb_2mu4_nomucomb_delayed_L1MU6_3MU4",
                     "HLT_mu20_mu6noL1_bTau",
                     "HLT_2mu6_mu4_bTau_L12MU6_3MU4",
                     "HLT_mu6_2mu4_bTau_L1MU6_3MU4",
                     "HLT_mu11_2mu4noL1_bTau_L1MU11_2MU6",
                     "HLT_mu11_mu6_bTau",
                     "HLT_mu11_mu6noL1_bTau_L1MU11_2MU6",
                     "HLT_3mu4_bPhi",
                     "HLT_mu11_mu6_bPhi",
                     "HLT_mu11_mu6noL1_bPhi_L1MU11_2MU6",
                     "HLT_mu11_nomucomb_mu6_nomucomb_bPhi",
                     "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bPhi",
                     "HLT_mu11_mu6_bDimu2700" ]

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY7TriggerCountToMetadata = DerivationFramework__TriggerCountToMetadata(name = "BPHY7TriggerCount",
                                                                          TriggerList = triggersToMetadata,
                                                                          FolderName = "BPHY7")

ToolSvc += BPHY7TriggerCountToMetadata

# Additional metadata output
BPHY7Stream.AddMetaDataItem(["xAOD::FileMetaData#%s*" % BPHY7TriggerCountToMetadata.FolderName,
                             "xAOD::FileMetaDataAuxInfo#%s*Aux." % BPHY7TriggerCountToMetadata.FolderName] )

#BPHY7Stream.AddMetaDataItem([ "TH1D#%s*" %BPHY7TriggerCountToMetadata.HistogramName ] )

#====================================================================
# PRESELECTION for Kernel1 #Added by Matteo
#====================================================================

#--------------------------------------------------------------------
## 1/ Setup the skimming based on triggers
##     

#To inhibit triggers one can remove this tool from Kernel1 (look for "BPHY7TriggerSkim")
#This list is used for the trigger thinning tool
triggerList = [ "HLT_2mu10",
                "HLT_2mu10_l2msonly",
                "HLT_2mu10_nomucomb",
                "HLT_2mu14",
                "HLT_mu50",
                "HLT_2mu14_l2msonly",
                "HLT_2mu14_nomucomb",
                "HLT_2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4",
                "HLT_2mu6_nomucomb_mu4_nomucomb_L12MU6_3MU4",
                "HLT_mu6_2mu4",
                "HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4",
                "HLT_mu6_nomucomb_2mu4_nomucomb_L1MU6_3MU4",
                "HLT_3mu6",
                "HLT_3mu6_msonly",
                "HLT_3mu6_nomucomb",
                "HLT_mu4","HLT_mu6","HLT_mu10","HLT_mu18",
                "HLT_mu14",
                "HLT_mu24",
                "HLT_mu24_L1MU15",
                "HLT_2mu4",
                "HLT_2mu6",
                "HLT_mu20_L1MU15",
                "HLT_mu18_2mu4noL1",
                "HLT_mu18_nomucomb_2mu4noL1",
                "HLT_mu20_2mu4noL1",
                "HLT_mu20_l2idonly_2mu4noL1",
                "HLT_mu20_nomucomb_2mu4noL1",
                "HLT_mu18_mu8noL1",
                "HLT_mu18_nomucomb_mu8noL1",
                "HLT_mu20_mu8noL1",
                "HLT_mu20_l2idonly_2mu4noL1",
                "HLT_mu20_nomucomb_mu8noL1",
                "HLT_mu22_mu8noL1",
                "HLT_mu22_l2idonly_2mu4noL1",
                "HLT_mu22_nomucomb_mu8noL1",
                "HLT_mu22_2mu4noL1",
                "HLT_mu22_nomucomb_2mu4noL1",
                "HLT_mu20_2mu4noL1", "HLT_mu20_mu8noL1",
                "HLT_mu14_tau25_medium1_tracktwo",
                "HLT_mu14_tau35_medium1_tracktwo",
                "HLT_mu14_tau25_medium1_tracktwo_xe50",
                "HLT_mu14_tau35_medium1_tracktwo_L1TAU20",
                "HLT_mu24_mu8noL1", #Triggers with high Lumi*Efficiency values (missing in the wildcards)
                "HLT_mu6_nomucomb_2mu4_nomucomb_delayed_L1MU6_3MU4", 
                "HLT_2mu6_bBmumuxv2_delayed", 
                "HLT_2mu4_bDimu_noinvm_novtx_ss", 
                "HLT_2mu6_bDimu_noinvm_novtx_ss", 
                "HLT_mu24_2mu4noL1", 
                "HLT_mu10_mu6_bUpsimumu", 
                "HLT_mu10_mu6_bBmumuxv2", 
                "HLT_mu10_mu6_bJpsimumu", 
                "HLT_mu6_mu4_bBmumuxv2_delayed", 
                "HLT_2mu6_10invm30_pt2_z10", 
                "HLT_2mu6_nomucomb_bPhi",
                "HLT_mu6_mu4_bDimu_noinvm_novtx_ss",
                "HLT_mu11_mu6_bDimu2700",
                "HLT_.*mu11_mu6.*", # Lazy addition to keep the latest of interest
                "HLT_.*3mu4.*",
                "HLT_.*mu.*imedium.*",	# Trigger with looser isolation selection 
                "HLT_.*mu.*iloose.*",
                "HLT_.*mu6.*2mu4.*",
                "HLT_.*mu11.*2mu4noL1.*",
                "HLT_.*2mu14_nomucomb.*",
                "HLT_.*bTau.*",		# Our tau triggers
                "HLT_.*bDimu2700.*",
                "HLT_.*bPhi.*",
                "HLT_.*bBmumuxv2.*",
                "HLT_.*nscan.*"  ]	# Narrow scan triggers

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY7TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY7TriggerSkim",
                                                            TriggerListOR = triggerList,
                                                            TriggerListAND = [] )

ToolSvc += BPHY7TriggerSkim


#====================================================================
# 2mu vertex for Kernel2 #Added by Matteo
#====================================================================

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY7DiMuon_Finder = Analysis__JpsiFinder(name                         = "BPHY7DiMuon_Finder",
                                          #    OutputLevel                 = DEBUG,
                                          muAndMu                     = True,
                                          muAndTrack                  = False,
                                          TrackAndTrack               = False,
                                          assumeDiMuons               = True, 
                                          invMassUpper                = 2900.0, # Cut just below the J/psi
                                          invMassLower                = 0.0,
                                          Chi2Cut                     = 110., #CHANGED! Was 200
                                          oppChargesOnly	            = False,
                                          allChargeCombinations	    = True,
                                          combOnly		    = False,
                                          atLeastOneComb		    = True,
                                          useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                          muonCollectionKey           = "Muons",
                                          TrackParticleCollection     = "InDetTrackParticles",
                                          V0VertexFitterTool          = BPHY7_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                          useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                          TrkVertexFitterTool         = BPHY7_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                          TrackSelectorTool           = BPHY7_VertexTools.InDetTrackSelectorTool,
                                          ConversionFinderHelperTool  = BPHY7_VertexTools.InDetConversionHelper,
                                          VertexPointEstimator        = BPHY7_VertexTools.VtxPointEstimator,
                                          useMCPCuts                  = False)
ToolSvc += BPHY7DiMuon_Finder

#--------------------------------------------------------------------
##Comment from BPHY2...
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY7DiMuon_SelectAndWrite = DerivationFramework__Reco_mumu(name              = "BPHY7DiMuon_SelectAndWrite",
                                                            JpsiFinder    	      = BPHY7DiMuon_Finder,
                                                            OutputVtxContainerName = "BPHY7TwoMuCandidates",
                                                            PVContainerName        = "PrimaryVertices",
                                                            RefPVContainerName     = "SHOULDNOTBEUSED_DiMuonRefittedPV")
ToolSvc += BPHY7DiMuon_SelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY7DiMuon_Decorator = DerivationFramework__Select_onia2mumu(name                  = "BPHY7DiMuon_Decorator",
                                                              HypothesisName        = "Jpsi",
                                                              InputVtxContainerName = "BPHY7TwoMuCandidates",
                                                              VtxMassHypo           = 1230,   # used to determine time-of-flight and thus lifetime (deviations and sigmas are also added to the vertex)
                                                              MassMin               = 0.0,
                                                              MassMax               = 2900.0,
                                                              Chi2Max               = 200,
                                                              DoVertexType =1)              #	1 = Pt, 2 = A0, 4 = Z0
  
ToolSvc += BPHY7DiMuon_Decorator
#====================================================================
# 3mu/2mu+trk vertex for Kernel2 #Added by Matteo
#====================================================================
## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BpmVertexFit = Trk__TrkVKalVrtFitter(name                = "BpmVertexFit",
                                     Extrapolator        = BPHY7_VertexTools.InDetExtrapolator,
                                     FirstMeasuredPoint  = True,
                                     MakeExtendedVertex  = True)
ToolSvc += BpmVertexFit

## 5/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY7ThreeMuon_Finder = Analysis__JpsiPlus1Track(name 				= "BPHY7ThreeMuon_Finder",
                                                 OutputLevel 			= INFO,
                                                 pionHypothesis			= True,
                                                 kaonHypothesis			= False,
                                                 trkThresholdPt			= 1000.0,
                                                 #trkMaxEta			= 2.5, # is this value fine?? default would be 102.5
                                                 BThresholdPt			= 1000.0,
                                                 BMassUpper			= 5000.0, # What is this??
                                                 BMassLower			= 0.0,
                                                 JpsiContainerKey		= "BPHY7TwoMuCandidates",
                                                 TrackParticleCollection		= "InDetTrackParticles",
                                                 MuonsUsedInJpsi			= "NONE", #cannnot allow, would kill 3muons
                                                 ExcludeCrossJpsiTracks		= False,
                                                 TrkVertexFitterTool		= BpmVertexFit,
                                                 TrackSelectorTool		= BPHY7_VertexTools.InDetTrackSelectorTool,
                                                 UseMassConstraint		= False, 
                                                 Chi2Cut 			= 150) #Cut on chi2/Ndeg_of_freedom, so is very loose
												
        
ToolSvc += BPHY7ThreeMuon_Finder

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY7ThreeMuon_SelectAndWrite = DerivationFramework__Reco_dimuTrk(name                     = "BPHY7ThreeMuon_SelectAndWrite",
                                                                  OutputLevel              = INFO,
                                                                  Jpsi1PlusTrackName       = BPHY7ThreeMuon_Finder,
                                                                  OutputVtxContainerName   = "BPHY7Tau3MuCandidates",
                                                                  PVContainerName          = "PrimaryVertices",
                                                                  RefPVContainerName       = "BPHY7RefittedPrimaryVertices",
                                                                  RefitPV                  = True,
                                                                  MaxPVrefit = 1000)
ToolSvc += BPHY7ThreeMuon_SelectAndWrite 

## b/ augment and select Bplus->JpsiKplus candidates
BPHY7ThreeMuon_Decorator = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY7ThreeMuon_Decorator",
  OutputLevel                = INFO,
  HypothesisName             = "Tau3MuLoose",
  InputVtxContainerName      = "BPHY7Tau3MuCandidates",
  TrkMasses                  = [105.658, 105.658, 105.658],
  VtxMassHypo                = 1777.,
  MassMin                    = 0.0,
  MassMax                    = 5000.,  # If the two selections start differing one might have to check that the tools below still run on the right vertices
  Chi2Max                    = 100.)

ToolSvc += BPHY7ThreeMuon_Decorator

## b/ augment and select Bplus->JpsiKplus candidates
BPHY7ThreeMuon_Decorator2 = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY7ThreeMuon_Decorator2",
  OutputLevel                = INFO,
  HypothesisName             = "Ds2MuPi",
  InputVtxContainerName      = "BPHY7Tau3MuCandidates",
  TrkMasses                  = [105.658, 105.658, 139.57],
  VtxMassHypo                = 1968.3,
  MassMin                    = 0.0,
  MassMax                    = 5000.,  # If the two selections start differing one might have to check that the tools below still run on the right vertices
  Chi2Max                    = 100.)

ToolSvc += BPHY7ThreeMuon_Decorator2

#Track isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexTrackIsolation
BPHY7TrackIsolationDecorator = DerivationFramework__VertexTrackIsolation(
  name                            = "BPHY7TrackIsolationDecorator",
  OutputLevel                     = INFO,
  TrackIsoTool 	                  = "xAOD::TrackIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY7Tau3MuCandidates",
  PassFlags                       = ["passed_Tau3MuLoose", "passed_Ds2MuPi"] )

ToolSvc += BPHY7TrackIsolationDecorator

#CaloIsolationTool explicitly declared to avoid pointless warnings (it works!!!)
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
BPHY7CaloIsolationTool = xAOD__CaloIsolationTool(
  name                            = "BPHY7CaloIsolationTool",
  OutputLevel                     = WARNING,                  
  saveOnlyRequestedCorrections    = True,
  IsoLeakCorrectionTool           = "" ) #Workaround for a bug in older versions

ToolSvc += BPHY7CaloIsolationTool

#Calo isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexCaloIsolation
BPHY7CaloIsolationDecorator = DerivationFramework__VertexCaloIsolation(
  name                            = "BPHY7CaloIsolationDecorator",
  OutputLevel                     = INFO,                  
  CaloIsoTool                     = BPHY7CaloIsolationTool,  #"xAOD::CaloIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY7Tau3MuCandidates",
  CaloClusterContainer            = "CaloCalTopoClusters",
  ParticleCaloExtensionTool       = "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool",
  PassFlags                       = ["passed_Tau3MuLoose", "passed_Ds2MuPi"] )

ToolSvc += BPHY7CaloIsolationDecorator

#====================================================================
# Skimming tool to select only events with the correct vertices
#====================================================================

#--------------------------------------------------------------------
## 9/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    Exactli like in the preselection, where only 2mu vertices are treated.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

expression = "count(BPHY7Tau3MuCandidates.passed_Tau3MuLoose) > 0 || count(BPHY7Tau3MuCandidates.passed_Ds2MuPi) > 0"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY7_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name 		= "BPHY7_SelectEvent",
                                                                OutputLevel   	= INFO,
                                                                expression 	= expression)

ToolSvc += BPHY7_SelectEvent
print BPHY7_SelectEvent

#====================================================================
# Thinning Helper and various thinning tools
#====================================================================

#--------------------------------------------------------------------
## 10/ Setup the thinning helper, only tool able to perform thinning of trigger navigation information

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY7ThinningHelper = ThinningHelper( "BPHY7ThinningHelper" )
BPHY7ThinningHelper.TriggerChains = 'HLT_.*mu.*' #triggerList	# . = any character; * = 0 or more times; + = 1 or more times; ? 0 or 1 times  "Regular_Expression"
BPHY7ThinningHelper.AppendToStream( BPHY7Stream )

# Use the following line for the ThinningTools which follow
#  ThinningService = BPHY7ThinningHelper.ThinningSvc()  


#--------------------------------------------------------------------
## 11/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which haven't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the 
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## 12/ Cleans up, removing duplicate vertices. An issue caused by the logic of Jpsi+1 track in the case of 3-muon candidates

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxDuplicates
BPHY7Thin_vtxDuplicates = DerivationFramework__Thin_vtxDuplicates(name                       = "BPHY7Thin_vtxDuplicates",
                                                                  OutputLevel                = INFO,
                                                                  ThinningService            = BPHY7ThinningHelper.ThinningSvc(),
                                                                  TrackParticleContainerName = "InDetTrackParticles",
                                                                  VertexContainerNames       = ["BPHY7Tau3MuCandidates"],
                                                                  PassFlags                  = ["passed_Tau3MuLoose", "passed_Ds2MuPi"])

ToolSvc += BPHY7Thin_vtxDuplicates

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY7Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY7Thin_vtxTrk",
  OutputLevel                = INFO,
  ThinningService            = BPHY7ThinningHelper.ThinningSvc(),
  TrackParticleContainerName = "InDetTrackParticles",
  AcceptanceRadius	     = 1.,
  VertexContainerNames       = ["BPHY7Tau3MuCandidates"],
  PassFlags                  = ["passed_Tau3MuLoose", "passed_Ds2MuPi"],
  ApplyAnd                   = True )  # "and" requirement for Vertices

ToolSvc += BPHY7Thin_vtxTrk


## 13/ thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##     between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY7MuonTPThinningTool",
                                                                         ThinningService         = BPHY7ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY7MuonTPThinningTool

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY7_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(name                       = "BPHY7_thinningTool_PV",
                                                                 ThinningService            = BPHY7ThinningHelper.ThinningSvc(),
                                                                 CandidateCollections       = ["BPHY7Tau3MuCandidates"],
                                                                 KeepPVTracks  =True)

ToolSvc += BPHY7_thinningTool_PV

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
BPHY7TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "BPHY7TauTPThinningTool",
                                                                       ThinningService         = BPHY7ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY7TauTPThinningTool

# Only save truth informtion directly associated with muons
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY7TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY7TruthThinTool",
                                                               ThinningService         = BPHY7ThinningHelper.ThinningSvc(),
                                                               ParticleSelectionString = "abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 431",
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors      = True)
ToolSvc += BPHY7TruthThinTool

# Only save truth informtion directly associated with muons
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY7TruthMetTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY7TruthMetTool",
                                                              ThinningService         = BPHY7ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString = "abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16",
                                                              PreserveDescendants     = False,
                                                              PreserveAncestors      = False)
ToolSvc += BPHY7TruthMetTool


#--------------------------------------------------------------------
## 14/ Jet thinning based on generic object thinning tool
selection = "(AntiKt4EMTopoJets.pt > 20*GeV)"

#### Thinning of jets using the generic object thinning tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
BPHY7JetThinningTool = DerivationFramework__GenericObjectThinning( name		          = "BPHY7JetThinningTool",
								   ThinningService	  = BPHY7ThinningHelper.ThinningSvc(),
                                                                   ContainerName          = "AntiKt4EMTopoJets",
								   SelectionString        = selection,
								   ApplyAnd               = False)

ToolSvc += BPHY7JetThinningTool;

#==================================================================================
# Add everything which is needed to calibrate objects and to followingly make MET
#==================================================================================

#Erase if not re-activated

#from DerivationFrameworkTau.TruthObjectTools import *  # should give me the definition of TERUTH1TauTool

if isSimulation:
    from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
    scheduleTauTruthTools()
#also added the TruthTau containers to the output

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# !! DUAL Kernel to allow PRESELECTION     #Added by Matteo  
#====================================================================
## 15/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##     be executed!

BPHY7ThinningTools = [ BPHY7MuonTPThinningTool, BPHY7Thin_vtxDuplicates, BPHY7Thin_vtxTrk, BPHY7_thinningTool_PV,  BPHY7TauTPThinningTool] #  BPHY7Thin_refittedVtxTrk (removed to eliminate redundancy)

BPHY7SkimmingTools = [BPHY7_SelectEvent]

BPHY7AugmentationTools = [BPHY7DiMuon_SelectAndWrite, BPHY7DiMuon_Decorator, BPHY7ThreeMuon_SelectAndWrite, BPHY7ThreeMuon_Decorator, BPHY7ThreeMuon_Decorator2, BPHY7TrackIsolationDecorator, BPHY7CaloIsolationDecorator]

Kernel1Tools = [BPHY7TriggerSkim]

if isSimulation:
    BPHY7AugmentationTools.append(DFCommonTauTruthMatchingWrapper)
    if thinTruth:
       BPHY7ThinningTools.append(BPHY7TruthThinTool)
       BPHY7ThinningTools.append(BPHY7TruthMetTool)

#The sequence object. Is in principle just a wrapper which allows to run two kernels in sequence
BPHY7_Sequence = CfgMgr.AthSequencer("BPHY7_Sequence")


#onlyAugmentations implementation
if onlyAugmentations:
    Kernel1Tools = []
    BPHY7SkimmingTools = []
    BPHY7ThinningTools = []


#  REMOVED THE PRE-SELECTION!!

#  ////// k 1 \\\\\\
# Kernel n1 PRESELECTION
# The name of the kernel (BPHY7Kernel1 in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY7_Sequence += CfgMgr.DerivationFramework__DerivationKernel("BPHY7Kernel1",
                                                               AugmentationTools = [BPHY7TriggerCountToMetadata] ,
                                                               SkimmingTools     = Kernel1Tools)


#  ////// k 2 \\\\\\
# Kernel n2 deep Derivation
# The name of the kernel (BPHY7Kernel2 in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY7_Sequence += CfgMgr.DerivationFramework__DerivationKernel("BPHY7Kernel2",
                                                               AugmentationTools = BPHY7AugmentationTools,
                                                               SkimmingTools     = BPHY7SkimmingTools, 
                                                               ThinningTools     = BPHY7ThinningTools)

#Vital, replaces the adding of kernels directly
DerivationFrameworkJob += BPHY7_Sequence

#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY7SlimmingHelper = SlimmingHelper("BPHY7SlimmingHelper")


# Smart collection will add all extra info needed to have the selecte object fully functional
# Includes loads of variables. TODO: can we reduce this information???
SmartCollections = ["MET_Reference_AntiKt4EMTopo"] # , "MET_Reference_AntiKt4LCTopo"]

#for calculation of MET
#if isSimulation:
SmartCollections += ["Electrons", "Photons", "TauJets", "AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo"]

AllVariables = []

AllVariables += ['MET_Reference_AntiKt4EMTopo',
                 'METAssoc_AntiKt4EMTopo',
                 'MET_Core_AntiKt4EMTopo',
                 'MET_Truth',
                 'MET_Track',
                 'MET_LocHadTopo']

AllVariables += ["Kt4EMTopoOriginEventShape"]
AllVariables += ["Kt4EMTopoEventShape"]

StaticContent = [] 

# Needed for trigger objects
BPHY7SlimmingHelper.IncludeMuonTriggerContent = True
BPHY7SlimmingHelper.IncludeBPhysTriggerContent = True

BPHY7SlimmingHelper.ExtraVariables = ["Photons.pt.eta.phi.m","Electrons.pt.eta.phi.m","TauJets.pt.eta.phi.m.IsTruthMatched.truthJetLink.truthParticleLink", "AntiKt4EMTopoJets.JetPileupScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_m", "AntiKt4EMTopoJets.JvtJvfcorr", "AntiKt4EMTopoJets.JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m"]

AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY7RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY7RefittedPrimaryVerticesAux."]

## TruthTaus (needed to calibrate taus, then used for MET)

if isSimulation:
    StaticContent += ["xAOD::TruthParticleContainer#TruthTaus"]
    StaticContent += ["xAOD::TruthParticleAuxContainer#TruthTausAux."]
    AllVariables += ["AntiKt4TruthJets"] 

AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]
AllVariables += ["MuonSpectrometerTrackParticles"]

## muon container
AllVariables += ["Muons"]

#print "Something's off, since I do not have the desired Aux info for my verteces"
## Jpsi candidates 
#StaticContent += ["xAOD::VertexContainer#%s"        % BPHY7DiMuon_SelectAndWrite.OutputVtxContainerName]
#StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY7DiMuon_SelectAndWrite.OutputVtxContainerName]
#StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY7DiMuon_SelectAndWrite.OutputVtxContainerName]

# ThreeBody candidates (vertices)
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY7ThreeMuon_SelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY7ThreeMuon_SelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY7ThreeMuon_SelectAndWrite.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "METMap_Truth"]

BPHY7SlimmingHelper.AllVariables = AllVariables
BPHY7SlimmingHelper.StaticContent = StaticContent
BPHY7SlimmingHelper.SmartCollections = SmartCollections
BPHY7SlimmingHelper.AppendContentToStream(BPHY7Stream)

