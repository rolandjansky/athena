#====================================================================
# BPHY18.py
# B0 -> K*ee wheeeee 
# It requires the reductionConf flag BPHY18 in Reco_tf.py   
#====================================================================


#====================================================================
# FLAGS TO PERSONALIZE THE DERIVATION
#====================================================================

onlyAugmentations = False  # Set to True to deactivate thinning and skimming, and only keep augmentations (to generate a sample with full xAOD plus all the extra)
thinTruth = False
skimTruth = False


# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPContent import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print 'is this simulation?', isSimulation

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

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
triggersToMetadata= [
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

triggerList = [
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
]	# BeeX triggers

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY18TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY18TriggerSkim",
                                                            TriggerListOR = triggerList,
                                                            TriggerListAND = [] )

ToolSvc += BPHY18TriggerSkim


#do not know what this does, but let's leave it for now, until we see if it's useful or not!
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY18_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY18_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY18_AugOriginalCounts

#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder_ee
BPHY18DiElectronFinder = Analysis__JpsiFinder_ee(
    name                        = "BPHY18DiElectronFinder",
    OutputLevel                 = DEBUG,
    elAndEl                     = True,
    elAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiElectrons           = True, 
    elThresholdPt               = 4000.0,  # OI was 3000
    invMassUpper                = 7000.0,
    invMassLower                = 1.0,
    Chi2Cut                     = 30.,
    oppChargesOnly	            = False,
    allChargeCombinations       = True,
    useElectronTrackMeasurement = True, #
    electronCollectionKey       = "Electrons",
    TrackParticleCollection     = "GSFTrackParticles",
    useEgammaCuts               = False, #Setting to True makes sure only lhloose electrons are vertexed
    V0VertexFitterTool          = BPHY18_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY18_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY18_VertexTools.InDetTrackSelectorTool,
#OI    ConversionFinderHelperTool  = BPHY18_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY18_VertexTools.VtxPointEstimator
    )

ToolSvc += BPHY18DiElectronFinder
print      BPHY18DiElectronFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

#Reco_mumu - makes vertices from all the JpsiFinder pairs. Has extra information - associates primary vertices to secondary vertices.  
#Makes sure sum of secondary vertex information points back to primary vertex, or tries to. 
#But this isn't the final vertex, so probably not a big deal? Worth remenbering though
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY18DiElectronSelectAndWrite   = DerivationFramework__Reco_mumu(
    name                   = "BPHY18DiElectronSelectAndWrite",
    JpsiFinder             = BPHY18DiElectronFinder,  # OI was JpsiFinder_ee
    OutputVtxContainerName = "BPHY18DiElectronCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    #    RefPVContainerName     = "BPHY18RefJpsiPrimaryVertices",
    #    RefitPV                = True,
    #    MaxPVrefit             = 10000,
    DoVertexType = 7)

ToolSvc += BPHY18DiElectronSelectAndWrite
print BPHY18DiElectronSelectAndWrite



#Decorates the vertex - recalculates some quantities and adds them as aux info to the vertex itself. Input being the step from the reco_mumu.
#  Hypothesis Name? this determines what the output will be called. 
#  Generates a bool pass_Jpsi for each vertex, and if it passes the selections below, it will say "Yes, passed the 'jpsi' selection". 
#  Selection being the MassMin, Max, Chi2Max below. 
#  vertex type = by default was 7. Increases info that goesinto the vertex Generates augmentation for vertex, for different hypothesis of PV choice.
#  (1) take the PV with highest sumPt [not always best for bphys!] (2) smallest A0 - one which all particles of secondary vertices point to,
#  (3) smallest Z0 wrt the secondary vertex. close in the Z0 direction.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
## a/ augment and select di-electron candidates. Keep cuts very loose for now??
BPHY18_Select_DiElectrons = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY18_Select_DiElectrons",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY18DiElectronCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 1.0,
  MassMax               = 7000.0,
  Chi2Max               = 30,
  DoVertexType =7)
  
ToolSvc += BPHY18_Select_DiElectrons
print      BPHY18_Select_DiElectrons

## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BeeKstVertexFit = Trk__TrkVKalVrtFitter(
    name                = "BeeKstVertexFit",
    Extrapolator        = BPHY18_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = True,
    MakeExtendedVertex  = True)

ToolSvc += BeeKstVertexFit
print      BeeKstVertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY18BeeKst = Analysis__JpsiPlus2Tracks(
    name                    = "BPHY18BeeKstFinder",
    OutputLevel             = DEBUG, #can also be DEBUG, WARNING, VERBOSE
    kaonkaonHypothesis	    = False,
    pionpionHypothesis      = False,
    kaonpionHypothesis      = True,
    trkThresholdPt          = 500.0, #minimum track pT in MeV
    trkMaxEta		    = 3.0, 
    BThresholdPt            = 1000.,
    BMassLower              = 3000.0, #OI makes no sense below Jpsi mass #same values as BPHY18 (original) - Bs->JpsiKK
    BMassUpper		        = 6500.0,
    JpsiContainerKey	    = "BPHY18DiElectronCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    #MuonsUsedInJpsi	    = "Muons", #Don't remove all muons, just those in J/psi candidate (see the following cut)
    ExcludeCrossJpsiTracks  = False,   #setting this to False rejects the muons from J/psi candidate
    TrkVertexFitterTool	    = BeeKstVertexFit,
    TrackSelectorTool	    = BPHY18_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint	    = False, #Set to True, according to Bs->JpsiKK DAOD
    DiTrackMassUpper        = 1110., #OI was 1500. Can eventually set these to be the K* mass?
    DiTrackMassLower        = 690.,  #OI was 500
    Chi2Cut                 = 15.0, #THIS IS CHI2/NDOF, checked the code!!!
    DiTrackPt               = 500.,
    TrkQuadrupletMassLower  = 1000.0, #Two electrons + two tracks (one K, one pi)
    TrkQuadrupletMassUpper  = 10000.0, # same as BPHY18, original
    #FinalDiTrackMassUpper   = 1000.,
    #FinalDiTrackMassLower   = 800.,
    #TrkDeltaZ               = 20., #Normally, this cut should not be used since it is lifetime-dependent
    FinalDiTrackPt          = 500.,
#OI    DoElectrons = True,
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
    DoVertexType = 7)

ToolSvc += BPHY18BeeKstSelectAndWrite 
print      BPHY18BeeKstSelectAndWrite

## b/ augment and select B->eeKst candidates
#  set mass hypothesis (K pi)
BPHY18_Select_BeeKst = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY18_Select_BeeKst",
    HypothesisName             = "Bd", #creates output variable pass_Bd
    InputVtxContainerName      = "BeeKstCandidates",
    TrkMasses                  = [0.511, 0.511, 493.677, 139.570],
    VtxMassHypo                = 5279.6, #mass of B
    MassMin                    = 1.0,      #no mass cuts here
    MassMax                    = 10000.0,   #no mass cuts here
    Chi2Max                    = 30.0) #THIS IS CHI2! NOT CHI2/NDOF! Careful!

ToolSvc += BPHY18_Select_BeeKst
print      BPHY18_Select_BeeKst

## c/ augment and select Bdbar->eeKstbar candidates
# set mass hypothesis (pi K)
BPHY18_Select_BeeKstbar = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY18_Select_Bd2JpsiKstbar",
    HypothesisName             = "Bdbar", #creates pass_Bdbar in the output vtx container
    InputVtxContainerName      = "BeeKstCandidates",
    TrkMasses                  = [0.511, 0.511, 139.570, 493.677],
    VtxMassHypo                = 5279.6,
    MassMin                    = 1.0,      #no mass cuts here
    MassMax                    = 10000.0,   #no mass cuts here
    Chi2Max                    = 30.0)

ToolSvc += BPHY18_Select_BeeKstbar
print      BPHY18_Select_BeeKstbar

"""from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track_ee
BPHY18BdJpsiK = Analysis__JpsiPlus1Track_ee(name = "BPHY18BdJpsiK",
OutputLevel             = INFO,#DEBUG,
pionHypothesis                  = True, #CHECK ALL THE INPUT CUTS, need to take Dielectron candidates 
kaonHypothesis                  = True,
trkThresholdPt                  = 500.0,
trkMaxEta                       = 3.0,
BThresholdPt                    = 4000.0,
BMassUpper                      = 6500.0,
BMassLower                      = 1000.0,
Chi2Cut                         = 15.0,
TrkTrippletMassUpper            = 1000, # Check these cuts
TrkTrippletMassLower            = 10000,
JpsiContainerKey                = "BPHY18DiElectronCandidates",
TrackParticleCollection         = "InDetTrackParticles",
#MuonsUsedInJpsi                 = "Muons",
TrkVertexFitterTool             = BdKVertexFit,  # What should this be ? This is a RANDOM name
TrackSelectorTool               = BPHY18_VertexTools.InDetTrackSelectorTool,
# m_doElectrons ??
UseMassConstraint               = False)

# What names here ? Depends on the targetted processes 
ToolSvc += BPHY18BdlJpsiK
print      BPHY18BdJpsiK
"""

if skimTruth or not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY18_SelectBeeKstEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY18_SelectBeeKstEvent",
     expression = "(count(BeeKstCandidates.passed_Bd > 0) + count(BeeKstCandidates.passed_Bdbar > 0)  + count(BPHY18DiElectronCandidates.passed_Jpsi > 0) ) > 0") 
     #just make sure there is at least one vertex in the event - either Bd/Bdbar or just a jpsi (of course every Bd has a Jpsi already...)
   ToolSvc += BPHY18_SelectBeeKstEvent
   print BPHY18_SelectBeeKstEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools (though it seems we only have one here!)
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY18SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY18SkimmingOR",
       FilterList = [BPHY18_SelectBeeKstEvent, BPHY18TriggerSkim],) #OR of all your different filters
   ToolSvc += BPHY18SkimmingOR
   print      BPHY18SkimmingOR

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

#this part gives very very few PVs in the PV container... 
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY18_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY18_thinningTool_PV",
  ThinningService            = "BPHY18ThinningSvc",
  CandidateCollections       = ["BeeKstCandidates"],
#  TrackParticleContainerName = "InDetTrackParticles",
  KeepPVTracks  =True
 )

"""BPHY18_thinningTool_PV_GSF = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY18_thinningTool_PV_GSF",
  ThinningService            = "BPHY18ThinningSvc",
  CandidateCollections       = ["BeeKstCandidates"],
  TrackParticleContainerName = "GSFTrackParticles",
  KeepPVTracks  =True
 )
"""
ToolSvc += BPHY18_thinningTool_PV
#ToolSvc += BPHY18_thinningTool_PV_GSF

#muons are funny, but we (might) need them because of the trigger!
## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY18MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = "BPHY18MuonTPThinningTool",
    ThinningService         = "BPHY18ThinningSvc",
    MuonKey                 = "Muons",
    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY18MuonTPThinningTool

#also do it for egamma particles
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY18EgammaTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                   = "BPHY18EgammaTPThinningTool",
    ThinningService        = "BPHY18ThinningSvc",
    SGKey                  = "Electrons",
    InDetTrackParticlesKey = "InDetTrackParticles")  # OI, weird, it does not like GSF, but electrons do not use InDetTrackParticles
ToolSvc += BPHY18EgammaTPThinningTool

# Only save truth informtion directly associated with: mu Ds+ D+ D*+ Ds*+ D0 D*0 B+ B*+ B0 B*0 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY18TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY18TruthThinTool",
                                                               ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 10311 || abs(TruthParticles.pdgId) == 521 || abs(TruthParticles.pdgId) == 523 || TruthParticles.pdgId == 511 || TruthParticles.pdgId == 513",
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors      = True)
ToolSvc += BPHY18TruthThinTool

# Only save truth neutrino and b/c quarks information
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY18TruthThinNoChainTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY18TruthThinNoChainTool",
                                                              ParticleSelectionString = "abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14",
                                                              PreserveDescendants     = False,
                                                              PreserveAncestors      = False)
ToolSvc += BPHY18TruthThinNoChainTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

#remove BPHY18_thinningTool_PV for now 
thinningCollection = [   BPHY18_thinningTool_Tracks,  BPHY18_thinningTool_GSFTracks,
                         BPHY18_thinningTool_PV, #BPHY18_thinningTool_PV_GSF, 
                         BPHY18EgammaTPThinningTool, BPHY18MuonTPThinningTool
                            ]

#if we're doing truth, add these [BPHY18TruthThinTool,BPHY18TruthThinNoChainTool] 
if isSimulation:
    thinningCollection += [BPHY18TruthThinTool,BPHY18TruthThinNoChainTool]

print thinningCollection

# The name of the kernel (BPHY18Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY18Kernel",
    AugmentationTools = [BPHY18DiElectronSelectAndWrite,  BPHY18_Select_DiElectrons,
                         BPHY18BeeKstSelectAndWrite, BPHY18_Select_BeeKst, BPHY18_Select_BeeKstbar ],
    #Only skim if not MC
    SkimmingTools     = [BPHY18SkimmingOR] if skimTruth or not isSimulation else [],
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
# Thinning service name must match the one passed to the thinning tools
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
AllVariables  = []
StaticContent = []
ExtraVariables = []
BPHY18SlimmingHelper.SmartCollections = ["Electrons", "Muons", "InDetTrackParticles" ] #, "GSFTrackParticles"]

# Needed for trigger objects
BPHY18SlimmingHelper.IncludeMuonTriggerContent  = False
BPHY18SlimmingHelper.IncludeBPhysTriggerContent = False
BPHY18SlimmingHelper.IncludeEGammaTriggerContent  = True

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY18RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY18RefittedPrimaryVerticesAux."]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)

#AllVariables += ["CombinedMuonTrackParticles",
#                 "ExtrapolatedMuonTrackParticles",
#                 "MuonSpectrometerTrackParticles"]

ExtraVariables += ["Muons.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Hits.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits",
                   "Muons.numberOfTriggerEtaLayers.numberOfPhiLayers",
                   #"CombinedMuonTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits", 
                   "InDetTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits.vx.vy.vz",
                   "PrimaryVertices.chiSquared.covariance", "Electrons.deltaEta1","egammaClusters.calE.calEta.calPhi.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME","HLT_xAOD__ElectronContainer_egamma_ElectronsAuxDyn.charge"]

## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY18DiElectronSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY18DiElectronSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY18BeeKstSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY18BeeKstSelectAndWrite.OutputVtxContainerName]

# Tagging information (in addition to that already requested by usual algorithms)
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
