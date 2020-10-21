#====================================================================
# BPHY18.py
# B0 -> K*ee 
# It requires the reductionConf flag BPHY18 in Reco_tf.py   
#====================================================================


#====================================================================
# FLAGS TO PERSONALIZE THE DERIVATION
#====================================================================

onlyAugmentations = False  
thinTruth = False
skimTruth = False


# Set up common services and job object. 
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPContent import *

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
isSimulation = DerivationFrameworkHasTruth

print isSimulation

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY18_VertexTools = BPHYVertexTools("BPHY18")


print '********************** VERTEX TOOLS ***********************'
print BPHY18_VertexTools
print BPHY18_VertexTools.TrkV0Fitter
print '********************** END VERTEX TOOLS ***********************'

#====================================================================
# TriggerCounting for Kernel1 #Added by Matteo
#====================================================================
#List of trigggers to be counted (high Sig-eff*Lumi ones are in)
triggersToMetadata = [
"HLT_2e5_lhvloose_nod0_bBeexM6000t",  #37,143,877  inb
"HLT_e5_lhvloose_nod0_bBeexM6000t",  #37,143,877
"HLT_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #37,312,506
"HLT_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #27,041,892
"HLT_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #149,100	
"HLT_e9_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #2,681,764
"HLT_e9_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #1,979,362
"HLT_e9_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #3,359,105
"HLT_e9_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #2,426,663
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #2,950,935
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #2,928,030
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #3,647,507
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #3,605,371
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #40,169,436
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #37,312,506
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #677,340

'HLT_3mu4_bDimu2700',  'HLT_3mu6_bDimu',  'HLT_mu6_2mu4_bDimu2700','HLT_mu11_mu6_bJpsimumu',  'HLT_3mu4_bJpsi', 'HLT_mu11_mu6_bDimu', 'HLT_2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6','HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',   'HLT_2mu10_bBmumuxv2',  'HLT_mu6_mu4_bDimu',   'HLT_2mu6_bBmumuxv2_L1LFV-MU6', 'HLT_mu11_mu6_bJpsimumu_Lxy0',  'HLT_mu11_mu6_bDimu2700', 'HLT_mu11_mu6_bBmumux_BpmumuKp', 'HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',   'HLT_mu11_mu6_bBmumuxv2', 'HLT_mu6_2mu4_bJpsi', 'HLT_2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'HLT_3mu6_bJpsi', 'HLT_mu11_mu6_bBmumu']

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY18TriggerCountToMetadata = DerivationFramework__TriggerCountToMetadata(name = "BPHY18TriggerCount",
                                                                           TriggerList = triggersToMetadata,
                                                                           FolderName = "BPHY18")

ToolSvc += BPHY18TriggerCountToMetadata

#====================================================================
# PRESELECTION for Kernel1 #Added by Matteo
#====================================================================
## 1/ Setup the skimming based on triggers
##     

triggerList = ["HLT_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #37,312,506
"HLT_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #27,041,892
"HLT_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #149,100	
"HLT_e9_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #2,681,764
"HLT_e9_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #1,979,362
"HLT_e9_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #3,359,105
"HLT_e9_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #2,426,663
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #2,950,935
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #2,928,030
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #3,647,507
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #3,605,371
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #40,169,436
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #37,312,506
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #677,340
"HLT_2e5_lhvloose_nod0_bBeexM6000t",  #37,143,877  inb
"HLT_e5_lhvloose_nod0_bBeexM6000t"  #37,143,877
]	# Seeded + Unseeded BeeX triggers

triggerList_unseeded = ["HLT_2e5_lhvloose_nod0_bBeexM6000t",  #37,143,877  inb
"HLT_e5_lhvloose_nod0_bBeexM6000t"  #37,143,877
]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY18TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY18TriggerSkim",
                                                             TriggerListOR = triggerList,
							                                 TriggerListORHLTOnly = triggerList_unseeded )

ToolSvc += BPHY18TriggerSkim
print BPHY18TriggerSkim

#do not know what this does, but let's leave it for now, until we see if it's useful or not!
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY18_AugOriginalCounts = DerivationFramework__AugOriginalCounts(name = "BPHY18_AugOriginalCounts",
                                                                  VertexContainer = "PrimaryVertices",
                                                                  TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY18_AugOriginalCounts

#lhvloose_nod0
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronLikelihoodTool
ElectronLHSelectorLHvloose_nod0 = AsgElectronLikelihoodTool("ElectronLHSelectorLHvloosenod0", 
ConfigFile="ElectronPhotonSelectorTools/offline/mc16_20190328_nod0/ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth_nod0.conf")
ElectronLHSelectorLHvloose_nod0.primaryVertexContainer = "PrimaryVertices"
ToolSvc += ElectronLHSelectorLHvloose_nod0
print ElectronLHSelectorLHvloose_nod0

# decorate electrons with the output of LH vloose nod0
ElectronPassLHvloosenod0 = DerivationFramework__EGSelectionToolWrapper(name = "ElectronPassLHvloosenod0",
                                                                       EGammaSelectionTool = ElectronLHSelectorLHvloose_nod0,
                                                                       EGammaFudgeMCTool = "",
                                                                       CutType = "",
                                                                       StoreGateEntryName = "DFCommonElectronsLHVeryLoosenod0",
                                                                       ContainerName = "Electrons")
ToolSvc += ElectronPassLHvloosenod0
print ElectronPassLHvloosenod0

#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder_ee
BPHY18DiElectronFinder = Analysis__JpsiFinder_ee(
    name                        = "BPHY18DiElectronFinder",
    OutputLevel                 = INFO,
    elAndEl                     = True,
    elAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiElectrons           = True, 
    elThresholdPt               = 4000.0,
    invMassUpper                = 7000.0,
    invMassLower                = 1.0,
    Chi2Cut                     = 30.,
    oppChargesOnly	            = False,
    allChargeCombinations       = True,
    useElectronTrackMeasurement = True, 
    electronCollectionKey       = "Electrons",
    TrackParticleCollection     = "GSFTrackParticles",
    useEgammaCuts               = True, 
    V0VertexFitterTool          = BPHY18_VertexTools.TrkV0Fitter,            
    useV0Fitter                 = False,                  
    TrkVertexFitterTool         = BPHY18_VertexTools.TrkVKalVrtFitter,      
    TrackSelectorTool           = BPHY18_VertexTools.InDetTrackSelectorTool,
    VertexPointEstimator        = BPHY18_VertexTools.VtxPointEstimator,
    ElectronSelection 		      = "d0_or_nod0"
    )

ToolSvc += BPHY18DiElectronFinder
print      BPHY18DiElectronFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s).
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY18DiElectronSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY18DiElectronSelectAndWrite",
    JpsiFinder             = BPHY18DiElectronFinder,
    OutputVtxContainerName = "BPHY18DiElectronCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 7
    )

ToolSvc += BPHY18DiElectronSelectAndWrite
print BPHY18DiElectronSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY18_Select_DiElectrons = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY18_Select_DiElectrons",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = "BPHY18DiElectronCandidates",
    VtxMassHypo           = 3096.916,
    MassMin               = 1.0,
    MassMax               = 7000.0,
    Chi2Max               = 30,
    DoVertexType          = 7
    )
  
ToolSvc += BPHY18_Select_DiElectrons
print      BPHY18_Select_DiElectrons

## 4/ setup a new vertexing tool (necessary due to use of mass constraint)
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BeeKstVertexFit = Trk__TrkVKalVrtFitter(
    name                = "BeeKstVertexFit",
    Extrapolator        = BPHY18_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = True,
    MakeExtendedVertex  = True
    )

ToolSvc += BeeKstVertexFit
print      BeeKstVertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY18BeeKst = Analysis__JpsiPlus2Tracks(
    name                    = "BPHY18BeeKstFinder",
    OutputLevel             = INFO,
    kaonkaonHypothesis	    = False,
    pionpionHypothesis      = False,
    kaonpionHypothesis      = True,
    oppChargesOnly          = False,
    SameChargesOnly         = False,
    trkThresholdPt          = 500.0,
    trkMaxEta		        = 3.0, 
    BThresholdPt            = 1000.,
    BMassLower              = 3000.0,
    BMassUpper		        = 6500.0,
    JpsiContainerKey	    = "BPHY18DiElectronCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    ExcludeCrossJpsiTracks  = False,   
    TrkVertexFitterTool	    = BeeKstVertexFit,
    TrackSelectorTool	    = BPHY18_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint	    = False, 
    DiTrackMassUpper        = 1110., 
    DiTrackMassLower        = 690.,  
    Chi2Cut                 = 15.0, 
    DiTrackPt               = 500.,
    TrkQuadrupletMassLower  = 1000.0, 
    TrkQuadrupletMassUpper  = 10000.0, 
    FinalDiTrackPt          = 500.,
    UseGSFTrackIndices      = [0,1]
    )

ToolSvc += BPHY18BeeKst
print      BPHY18BeeKst

## 6/ setup the combined augmentation/skimming tool for the BeeKst
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY18BeeKstSelectAndWrite  = DerivationFramework__Reco_dimuTrkTrk(
    name                   = "BPHY18BeeKstSelectAndWrite",
    Jpsi2PlusTrackName     = BPHY18BeeKst,
    OutputVtxContainerName = "BeeKstCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY18RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit             = 10000,
    DoVertexType           = 7
    )

ToolSvc += BPHY18BeeKstSelectAndWrite 
print      BPHY18BeeKstSelectAndWrite

## b/ augment and select B->eeKst candidates
BPHY18_Select_BeeKst = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY18_Select_BeeKst",
    HypothesisName        = "Bd", 
    InputVtxContainerName = "BeeKstCandidates",
    TrkMasses             = [0.511, 0.511, 493.677, 139.570],
    VtxMassHypo           = 5279.6, 
    MassMin               = 1.0,     
    MassMax               = 10000.0,  
    Chi2Max               = 30.0
    ) 

ToolSvc += BPHY18_Select_BeeKst
print      BPHY18_Select_BeeKst

## c/ augment and select Bdbar->eeKstbar candidates
BPHY18_Select_BeeKstbar = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY18_Select_Bd2JpsiKstbar",
    HypothesisName        = "Bdbar", 
    InputVtxContainerName = "BeeKstCandidates",
    TrkMasses             = [0.511, 0.511, 139.570, 493.677],
    VtxMassHypo           = 5279.6,
    MassMin               = 1.0,      
    MassMax               = 10000.0,   
    Chi2Max               = 30.0
    )

ToolSvc += BPHY18_Select_BeeKstbar
print      BPHY18_Select_BeeKstbar

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex
BPHY18_diMeson_revertex = DerivationFramework__ReVertex(
    name                   = "BPHY18_diMeson_revertex",
    InputVtxContainerName  = "BeeKstCandidates",
    TrackIndices           = [ 2, 3 ],
    TrkVertexFitterTool    = BeeKstVertexFit,
    OutputVtxContainerName = "BPHY18DiMeson"
    )

ToolSvc += BPHY18_diMeson_revertex
print      BPHY18_diMeson_revertex

BPHY18_Select_Kpi = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY18_Select_Kpi",
    HypothesisName        = "Kpi", 
    InputVtxContainerName = "BPHY18DiMeson",
    TrkMasses             = [ 493.677, 139.570 ],
    VtxMassHypo           = 891.66, 
    MassMin               = 1.0,     
    MassMax               = 100000.0,  
    Chi2Max               = 100.0
    ) 

ToolSvc += BPHY18_Select_Kpi
print      BPHY18_Select_Kpi

BPHY18_Select_piK = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY18_Select_piK",
    HypothesisName        = "piK", 
    InputVtxContainerName = "BPHY18DiMeson",
    TrkMasses             = [ 139.570, 493.677 ],
    VtxMassHypo           = 891.66, 
    MassMin               = 1.0,     
    MassMax               = 100000.0,  
    Chi2Max               = 100.0
    ) 

ToolSvc += BPHY18_Select_piK
print      BPHY18_Select_piK

if True:
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    BPHY18_SelectBeeKstEvent = DerivationFramework__xAODStringSkimmingTool(
        name = "BPHY18_SelectBeeKstEvent",
        expression = "(count(BeeKstCandidates.passed_Bd > 0) + count(BeeKstCandidates.passed_Bdbar > 0)) > 0") 
    ToolSvc += BPHY18_SelectBeeKstEvent
    print BPHY18_SelectBeeKstEvent

    #====================================================================
    # Make event selection based on an OR of the input skimming tools
    #====================================================================
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
    BPHY18SkimmingAND = CfgMgr.DerivationFramework__FilterCombinationAND(
        "BPHY18SkimmingAND",
        FilterList = [BPHY18_SelectBeeKstEvent, BPHY18TriggerSkim]) 
    ToolSvc += BPHY18SkimmingAND
    print      BPHY18SkimmingAND

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY18_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
    name                       = "BPHY18_thinningTool_Tracks",
    ThinningService            = "BPHY18ThinningSvc",
    TrackParticleContainerName = "InDetTrackParticles",
    VertexContainerNames       = ["BeeKstCandidates"],
    PassFlags                  = ["passed_Bd", "passed_Bdbar"] )

BPHY18_thinningTool_GSFTracks = DerivationFramework__Thin_vtxTrk(
    name                       = "BPHY18_thinningTool_GSFTracks",
    ThinningService            = "BPHY18ThinningSvc",
    TrackParticleContainerName = "GSFTrackParticles",
    VertexContainerNames       = ["BeeKstCandidates"],
    PassFlags                  = ["passed_Bd", "passed_Bdbar"] )

ToolSvc += BPHY18_thinningTool_Tracks
ToolSvc += BPHY18_thinningTool_GSFTracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY18_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
    name                 = "BPHY18_thinningTool_PV",
    ThinningService      = "BPHY18ThinningSvc",
    CandidateCollections = ["BeeKstCandidates"],
    KeepPVTracks         = True
    )
 
ToolSvc += BPHY18_thinningTool_PV

## b) thinning out tracks that are not attached to muons.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY18MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = "BPHY18MuonTPThinningTool",
    ThinningService         = "BPHY18ThinningSvc",
    MuonKey                 = "Muons",
    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY18MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY18EgammaTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                   = "BPHY18EgammaTPThinningTool",
    ThinningService        = "BPHY18ThinningSvc",
    SGKey                  = "Electrons",
    InDetTrackParticlesKey = "InDetTrackParticles")  
ToolSvc += BPHY18EgammaTPThinningTool

# Only save truth informtion directly associated with: mu Ds+ D+ D*+ Ds*+ D0 D*0 B+ B*+ B0 B*0 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY18TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY18TruthThinTool",
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 10311 || abs(TruthParticles.pdgId) == 521 || abs(TruthParticles.pdgId) == 523 || TruthParticles.pdgId == 511 || TruthParticles.pdgId == 513",
                                                                PreserveDescendants     = True,
                                                                PreserveAncestors       = True)
ToolSvc += BPHY18TruthThinTool

# Only save truth neutrino and b/c quarks information
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY18TruthThinNoChainTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY18TruthThinNoChainTool",
                                                                       ParticleSelectionString = "abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14",
                                                                       PreserveDescendants     = False,
                                                                       PreserveAncestors       = False)
ToolSvc += BPHY18TruthThinNoChainTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thinningCollection = [ BPHY18_thinningTool_Tracks,  BPHY18_thinningTool_GSFTracks,
                       BPHY18_thinningTool_PV, #BPHY18_thinningTool_PV_GSF, 
                       BPHY18EgammaTPThinningTool, BPHY18MuonTPThinningTool
                     ]

#if we're doing truth, add these [BPHY18TruthThinTool,BPHY18TruthThinNoChainTool] 
if isSimulation:
    thinningCollection += [BPHY18TruthThinTool,BPHY18TruthThinNoChainTool]

print thinningCollection

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY18Kernel",

    AugmentationTools = [ ElectronPassLHvloosenod0,BPHY18DiElectronSelectAndWrite,  
                          BPHY18_Select_DiElectrons,
                          BPHY18BeeKstSelectAndWrite, BPHY18_Select_BeeKst, BPHY18_Select_BeeKstbar,
                          BPHY18_diMeson_revertex, BPHY18_Select_Kpi, BPHY18_Select_piK ],

    #Only skim if not MC
    SkimmingTools     = [BPHY18SkimmingAND],
    ThinningTools     = thinningCollection
    )


#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY18Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY18Stream )
BPHY18Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY18Stream.AcceptAlgs(["BPHY18Kernel"])

# Special lines for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY18ThinningSvc = createThinningSvc( svcName="BPHY18ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY18ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY18SlimmingHelper = SlimmingHelper("BPHY18SlimmingHelper")
AllVariables   = []
StaticContent  = []
ExtraVariables = []
BPHY18SlimmingHelper.SmartCollections = ["Electrons", "Muons", "InDetTrackParticles" ] 

# Needed for trigger objects
BPHY18SlimmingHelper.IncludeMuonTriggerContent   = False
BPHY18SlimmingHelper.IncludeBPhysTriggerContent  = False
BPHY18SlimmingHelper.IncludeEGammaTriggerContent = True

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY18RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY18RefittedPrimaryVerticesAux."]

ExtraVariables += ["Muons.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Hits.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits",
                   "Muons.numberOfTriggerEtaLayers.numberOfPhiLayers",
                   "InDetTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits.vx.vy.vz",
                   "PrimaryVertices.chiSquared.covariance", "Electrons.deltaEta1.DFCommonElectronsLHVeryLoosenod0","egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME","HLT_xAOD__ElectronContainer_egamma_ElectronsAuxDyn.charge"]

## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY18DiElectronSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY18DiElectronSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY18BeeKstSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY18BeeKstSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        % BPHY18_diMeson_revertex.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY18_diMeson_revertex.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY18_diMeson_revertex.OutputVtxContainerName]

AllVariables += [ "GSFTrackParticles"] 


# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices", "ElectronTruthParticles"]

AllVariables = list(set(AllVariables)) # remove duplicates

BPHY18SlimmingHelper.AllVariables = AllVariables
BPHY18SlimmingHelper.ExtraVariables = ExtraVariables

BPHY18SlimmingHelper.StaticContent = StaticContent

from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
BPHY18SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
BPHY18SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

BPHY18SlimmingHelper.AppendContentToStream(BPHY18Stream)
