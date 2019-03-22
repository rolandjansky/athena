#====================================================================
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# BPHY20.py
#
# R_Jpsi analysis (currently muon channel only)
#
#====================================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'ItemListSvc'): svcMgr += CfgMgr.ItemListSvc()
svcMgr.ItemListSvc.OutputLevel = DEBUG

#====================================================================
# FLAGS TO PERSONALIZE THE DERIVATION
#====================================================================

onlyAugmentations = False  # Set to True to deactivate thinning and skimming, and only keep augmentations (to generate a sample with full xAOD plus all the extra)
thinTruth = True
addMuExtrapolationForTrigger = True

from DerivationFrameworkCore.DerivationFrameworkMaster import *
isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY20Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY20Stream )

BPHY20Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY20Stream.AcceptAlgs(["BPHY20Kernel"])

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY20_VertexTools = BPHYVertexTools("BPHY20")

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY20_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY20_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY20_AugOriginalCounts

#====================================================================
# TriggerCounting for Kernel1 (from BPHY7)
#====================================================================
#List of trigggers to be counted (high Sig-eff*Lumi ones are in)
triggersToMetadata= [
### 2018
"HLT_mu11_mu6_bJpsimumu",
"HLT_2mu10_bJpsimumu",
"HLT_mu6_2mu4_bJpsi",
"HLT_3mu4_bJpsi",
"HLT_mu11_mu6_bJpsimumu_Lxy0",
"HLT_3mu6_bJpsi",
"HLT_mu11_mu6_bJpsimumu_L1LFV-MU11",
"HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
"HLT_mu11_mu6_bJpsimumu_Lxy0_L1LFV-MU11",
"HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
"HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_2mu4_bJpsimumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4",
"HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu10_bJpsi_TrkPEB",
"HLT_mu6_bJpsi_TrkPEB",
"HLT_2mu6_bJpsimumu",
"HLT_mu6_mu2noL1_msonly_bJpsimumu_noid_PEB",

### 2017
#HLT_2mu10_bJpsimumu
"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu6_bJpsimumu",
#HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
"HLT_2mu6_bJpsimumu_Lxy0",
#HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
#HLT_3mu4_bJpsi
#HLT_3mu6_bJpsi
#HLT_mu10_bJpsi_TrkPEB
"HLT_mu10_mu6_bJpsimumu",
"HLT_mu10_mu6_bJpsimumu_Lxy0",
#HLT_mu11_mu6_bJpsimumu
#HLT_mu11_mu6_bJpsimumu_Lxy0
"HLT_mu14_bJpsi_Trkloose",
"HLT_mu14_bJpsi_TrkPEB",
"HLT_mu20_2mu2noL1_JpsimumuFS",
"HLT_mu20_2mu4_JpsimumuL2",
"HLT_mu20_bJpsi_Trkloose",
"HLT_mu20_bJpsi_TrkPEB",
#HLT_mu6_bJpsi_TrkPEB
#HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4
#HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4

### 2016
#HLT_2mu10_bJpsimumu
"HLT_2mu10_bJpsimumu_delayed",
"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu4_bJpsimumu_delayed_L1BPH-2M8-2MU4",
"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4",
"HLT_2mu4_bJpsimumu_Lxy0_delayed_L1BPH-2M8-2MU4",
"HLT_2mu6_bJpsimumu",
"HLT_2mu6_bJpsimumu_delayed",
"HLT_2mu6_bJpsimumu_delayed_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
#HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
"HLT_2mu6_bJpsimumu_Lxy0",
"HLT_2mu6_bJpsimumu_Lxy0_delayed",
"HLT_2mu6_bJpsimumu_Lxy0_delayed_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
#HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
#HLT_3mu4_bJpsi
"HLT_3mu4_bJpsi_delayed",
#HLT_3mu6_bJpsi
"HLT_mu10_mu6_bJpsimumu",
"HLT_mu10_mu6_bJpsimumu_delayed",
"HLT_mu10_mu6_bJpsimumu_Lxy0",
"HLT_mu10_mu6_bJpsimumu_Lxy0_delayed",
"HLT_mu18_bJpsi_Trkloose",
"HLT_mu20_2mu0noL1_JpsimumuFS",
"HLT_mu20_2mu4_JpsimumuL2",
#HLT_mu6_2mu4_bJpsi
"HLT_mu6_2mu4_bJpsi_delayed",
"HLT_mu6_mu4_bJpsimumu",
"HLT_mu6_mu4_bJpsimumu_delayed",
"HLT_mu6_mu4_bJpsimumu_delayed_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_delayed_L1MU6MU4-BO",
"HLT_mu6_mu4_bJpsimumu_L12MU4-B",
"HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_L1MU6MU4-BO",
"HLT_mu6_mu4_bJpsimumu_Lxy0",
"HLT_mu6_mu4_bJpsimumu_Lxy0_delayed",
"HLT_mu6_mu4_bJpsimumu_Lxy0_delayed_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",

### 2015
#HLT_2mu10_bJpsimumu
#HLT_2mu10_bJpsimumu_noL2
"HLT_2mu10_l2msonly_bJpsimumu_noL2",
"HLT_2mu4_bJpsimumu",
"HLT_2mu4_bJpsimumu_noL2",
"HLT_2mu4_l2msonly_bJpsimumu_noL2",
#HLT_2mu6_bJpsimumu
"HLT_2mu6_bJpsimumu_noL2",
"HLT_2mu6_l2msonly_bJpsimumu_noL2",
#HLT_3mu4_bJpsi
#HLT_3mu6_bJpsi
"HLT_mu10_mu10_l2msonly_bJpsimumu_noL2",
"HLT_mu18_2mu0noL1_JpsimumuFS",
"HLT_mu18_2mu4_JpsimumuL2",
#HLT_mu18_bJpsi_Trkloose
#HLT_mu20_2mu0noL1_JpsimumuFS
#HLT_mu20_2mu4_JpsimumuL2
"HLT_mu4_mu4_l2msonly_bJpsimumu_noL2",
"HLT_mu6_l2msonly_mu4_bJpsimumu_noL2",
"HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2",
#HLT_mu6_mu4_bJpsimumu
"HLT_mu6_mu4_bJpsimumu_noL2",
"HLT_mu6_mu4_l2msonly_bJpsimumu_noL2",
"HLT_mu6_mu6_l2msonly_bJpsimumu_noL2",

"HLT_mu4","HLT_mu6","HLT_mu10","HLT_mu14","HLT_mu18","HLT_mu24" #5%

 ]

triggersToMetadata_filter = list( set(triggersToMetadata) )

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY20TriggerCountToMetadata = DerivationFramework__TriggerCountToMetadata(name = "BPHY20TriggerCount",
                                                                          TriggerList = triggersToMetadata_filter,
                                                                          FolderName = "BPHY20")

ToolSvc += BPHY20TriggerCountToMetadata

#====================================================================
# PRESELECTION for Kernel1 #Added by Matteo
#====================================================================
## 1/ Setup the skimming based on triggers
##     

triggerList = [ 
## 2018
"HLT_mu11_mu6_bJpsimumu",
"HLT_2mu10_bJpsimumu",
"HLT_mu6_2mu4_bJpsi",
"HLT_3mu4_bJpsi",
"HLT_mu11_mu6_bJpsimumu_Lxy0",
"HLT_3mu6_bJpsi",
"HLT_mu11_mu6_bJpsimumu_L1LFV-MU11",
"HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
"HLT_mu11_mu6_bJpsimumu_Lxy0_L1LFV-MU11",
"HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
"HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_2mu4_bJpsimumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4",
"HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu10_bJpsi_TrkPEB",
"HLT_mu6_bJpsi_TrkPEB",
"HLT_2mu6_bJpsimumu",
"HLT_mu6_mu2noL1_msonly_bJpsimumu_noid_PEB",


"HLT_mu22_mu8noL1_TagandProbe",

### 2017
#HLT_2mu10_bJpsimumu
"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu6_bJpsimumu",
#HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
"HLT_2mu6_bJpsimumu_Lxy0",
#HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
#HLT_3mu4_bJpsi
#HLT_3mu6_bJpsi
#HLT_mu10_bJpsi_TrkPEB
"HLT_mu10_mu6_bJpsimumu",
"HLT_mu10_mu6_bJpsimumu_Lxy0",
#HLT_mu11_mu6_bJpsimumu
#HLT_mu11_mu6_bJpsimumu_Lxy0
"HLT_mu14_bJpsi_Trkloose",
"HLT_mu14_bJpsi_TrkPEB",
"HLT_mu20_2mu2noL1_JpsimumuFS",
"HLT_mu20_2mu4_JpsimumuL2",
"HLT_mu20_bJpsi_Trkloose",
"HLT_mu20_bJpsi_TrkPEB",
#HLT_mu6_bJpsi_TrkPEB
#HLT_mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4
#HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4

### 2016
#HLT_2mu10_bJpsimumu
"HLT_2mu10_bJpsimumu_delayed",
"HLT_2mu10_bJpsimumu_noL2",
"HLT_2mu4_bJpsimumu_delayed_L1BPH-2M8-2MU4",
"HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4",
"HLT_2mu4_bJpsimumu_Lxy0_delayed_L1BPH-2M8-2MU4",
"HLT_2mu6_bJpsimumu",
"HLT_2mu6_bJpsimumu_delayed",
"HLT_2mu6_bJpsimumu_delayed_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
#HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
"HLT_2mu6_bJpsimumu_Lxy0",
"HLT_2mu6_bJpsimumu_Lxy0_delayed",
"HLT_2mu6_bJpsimumu_Lxy0_delayed_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
#HLT_2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6
#HLT_3mu4_bJpsi
"HLT_3mu4_bJpsi_delayed",
#HLT_3mu6_bJpsi
"HLT_mu10_mu6_bJpsimumu",
"HLT_mu10_mu6_bJpsimumu_delayed",
"HLT_mu10_mu6_bJpsimumu_Lxy0",
"HLT_mu10_mu6_bJpsimumu_Lxy0_delayed",
"HLT_mu18_bJpsi_Trkloose",
"HLT_mu20_2mu0noL1_JpsimumuFS",
"HLT_mu20_2mu4_JpsimumuL2",
#HLT_mu6_2mu4_bJpsi
"HLT_mu6_2mu4_bJpsi_delayed",
"HLT_mu6_mu4_bJpsimumu",
"HLT_mu6_mu4_bJpsimumu_delayed",
"HLT_mu6_mu4_bJpsimumu_delayed_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_delayed_L1MU6MU4-BO",
"HLT_mu6_mu4_bJpsimumu_L12MU4-B",
"HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_L1MU6MU4-BO",
"HLT_mu6_mu4_bJpsimumu_Lxy0",
"HLT_mu6_mu4_bJpsimumu_Lxy0_delayed",
"HLT_mu6_mu4_bJpsimumu_Lxy0_delayed_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",
"HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4",

### 2015
#HLT_2mu10_bJpsimumu
#HLT_2mu10_bJpsimumu_noL2
"HLT_2mu10_l2msonly_bJpsimumu_noL2",
"HLT_2mu4_bJpsimumu",
"HLT_2mu4_bJpsimumu_noL2",
"HLT_2mu4_l2msonly_bJpsimumu_noL2",
#HLT_2mu6_bJpsimumu
"HLT_2mu6_bJpsimumu_noL2",
"HLT_2mu6_l2msonly_bJpsimumu_noL2",
#HLT_3mu4_bJpsi
#HLT_3mu6_bJpsi
"HLT_mu10_mu10_l2msonly_bJpsimumu_noL2",
"HLT_mu18_2mu0noL1_JpsimumuFS",
"HLT_mu18_2mu4_JpsimumuL2",
#HLT_mu18_bJpsi_Trkloose
#HLT_mu20_2mu0noL1_JpsimumuFS
#HLT_mu20_2mu4_JpsimumuL2
"HLT_mu4_mu4_l2msonly_bJpsimumu_noL2",
"HLT_mu6_l2msonly_mu4_bJpsimumu_noL2",
"HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2",
#HLT_mu6_mu4_bJpsimumu
"HLT_mu6_mu4_bJpsimumu_noL2",
"HLT_mu6_mu4_l2msonly_bJpsimumu_noL2",
"HLT_mu6_mu6_l2msonly_bJpsimumu_noL2" ,



"HLT_mu4","HLT_mu6","HLT_mu10","HLT_mu14","HLT_mu18","HLT_mu24", #5%
                
             
"HLT_.*mu11_mu6.*",     # Recent triggers
"HLT_.*mu.*imedium.*",	# Trigger with looser isolation selection 
"HLT_.*mu.*iloose.*",
"HLT_.*mu6.*2mu4.*",
"HLT_.*2mu.*",
"HLT_.*mu11.*2mu4noL1.*",
"HLT_.*2mu14_nomucomb.*",
"HLT_.*bTau.*",		# Our tau triggers
"HLT_.*bDimu2700.*",
"HLT_.*bPhi.*"
               ]    
               
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY20TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY20TriggerSkim",
                                                            TriggerListOR = triggerList,
                                                            TriggerListAND = [] )

ToolSvc += BPHY20TriggerSkim

#--------------------------------------------------------------------
# 2/ Select J/psi>mu+mu-
#--------------------------------------------------------------------
## a/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY20JpsiFinder = Analysis__JpsiFinder(
    name                       = "BPHY20JpsiFinder",
    OutputLevel                = INFO,
    muAndMu                    = True,
    muAndTrack                 = False,
    TrackAndTrack              = False,
    assumeDiMuons              = True, 
    muonThresholdPt            = 1000,
    invMassUpper               = 4500.0,
    invMassLower               = 2000.0,
    Chi2Cut                    = 20.,
    oppChargesOnly	        = True,
#    allChargeCombinations	   = True,
    combOnly                   = False,
    atLeastOneComb             = True,
    useCombinedMeasurement     = False, # Only takes effect if combOnly=True    
    muonCollectionKey          = "Muons",
    TrackParticleCollection    = "InDetTrackParticles",
    V0VertexFitterTool         = BPHY20_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool        = BPHY20_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool          = BPHY20_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool = BPHY20_VertexTools.InDetConversionHelper,
    VertexPointEstimator       = BPHY20_VertexTools.VtxPointEstimator,
    useMCPCuts                 = False)

ToolSvc += BPHY20JpsiFinder
print      BPHY20JpsiFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY20JpsiSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY20JpsiSelectAndWrite",
    JpsiFinder             = BPHY20JpsiFinder,
    OutputVtxContainerName = "BPHY20JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 1)

ToolSvc += BPHY20JpsiSelectAndWrite
print BPHY20JpsiSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select Jpsi->mumu candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY20_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY20_Select_Jpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = "BPHY20JpsiCandidates",
    VtxMassHypo           = 3096.900,
    MassMin               = 2000.0,
    MassMax               = 4500.0,
    Chi2Max               = 20,
    DoVertexType          = 1)
  
ToolSvc += BPHY20_Select_Jpsi2mumu
print      BPHY20_Select_Jpsi2mumu

#--------------------------------------------------------------------
# 3/ select B_c+->J/psi mu
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiMuVertexFit = Trk__TrkVKalVrtFitter(
    name               = "BcJpsiMuVertexFit",
    Extrapolator       = BPHY20_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint = True,
    MakeExtendedVertex = True)

ToolSvc += BcJpsiMuVertexFit
print      BcJpsiMuVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY20BcJpsiMu = Analysis__JpsiPlus1Track(
    name                    = "BPHY20BcJpsiMu",
    OutputLevel             = INFO, #DEBUG,
    pionHypothesis          = True, #False,
    kaonHypothesis        = False,#True,
    trkThresholdPt        = 1000,
    trkMaxEta               = 3.0,
    BThresholdPt            = 1000.0,
    BMassUpper                = 6900.0,
    BMassLower                = 2000.0,
    JpsiContainerKey        = "BPHY20JpsiCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    MuonsUsedInJpsi         = "NONE",
    ExcludeCrossJpsiTracks		= False,
    TrkVertexFitterTool     = BcJpsiMuVertexFit,
    TrackSelectorTool       = BPHY20_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint       = True, 
    RequireNMuonTracks      = 0,
    Chi2Cut                 = 1000, #5
    TrkTrippletMassUpper    = 6900,
    TrkTrippletMassLower    = 2000.0)
        
ToolSvc += BPHY20BcJpsiMu
print      BPHY20BcJpsiMu    

#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the Bc+>J/psi mu
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY20BcJpsiMuSelectAndWrite = DerivationFramework__Reco_dimuTrk(
    name                   = "BPHY20BcJpsiMuSelectAndWrite",
    OutputLevel            = INFO,
    Jpsi1PlusTrackName     = BPHY20BcJpsiMu,
    OutputVtxContainerName = "BPHY20BcJpsiMuCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY20RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit           = 1000)

ToolSvc += BPHY20BcJpsiMuSelectAndWrite 
print      BPHY20BcJpsiMuSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select B_c+>Jpsi mu candidates
BPHY20_Select_Bc2JpsiMu = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY20_Select_Bc2JpsiMu",
    HypothesisName        = "Bc",
    InputVtxContainerName = "BPHY20BcJpsiMuCandidates",
    TrkMasses             = [105.658, 105.658, 105.658],
    VtxMassHypo           = 6274.9,
    MassMin               = 2000.0,
    MassMax               = 6900.0,
    Chi2Max               = 1000)

ToolSvc += BPHY20_Select_Bc2JpsiMu
print      BPHY20_Select_Bc2JpsiMu



#====================================================================
# Isolation
#====================================================================


#Track isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexTrackIsolation
BPHY20TrackIsolationDecorator = DerivationFramework__VertexTrackIsolation(
  name                            = "BPHY20TrackIsolationDecorator",
  OutputLevel                     = INFO,
  TrackIsoTool                       = "xAOD::TrackIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY20BcJpsiMuCandidates",
  PassFlags                       = ["passed_Bc"] )

ToolSvc += BPHY20TrackIsolationDecorator

#CaloIsolationTool explicitly declared to avoid pointless warnings (it works!!!)
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
BPHY20CaloIsolationTool = xAOD__CaloIsolationTool(
  name                            = "BPHY20CaloIsolationTool",
  OutputLevel                     = WARNING,                  
  saveOnlyRequestedCorrections    = True,
  IsoLeakCorrectionTool           = "" ) #Workaround for a bug in older versions

ToolSvc += BPHY20CaloIsolationTool

#Calo isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexCaloIsolation
BPHY20CaloIsolationDecorator = DerivationFramework__VertexCaloIsolation(
  name                            = "BPHY20CaloIsolationDecorator",
  OutputLevel                     = INFO,                  
  CaloIsoTool                     = BPHY20CaloIsolationTool,  #"xAOD::CaloIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY20BcJpsiMuCandidates",
  CaloClusterContainer            = "CaloCalTopoClusters",
  ParticleCaloExtensionTool       = "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool",
  PassFlags                       = ["passed_Bc"] )

ToolSvc += BPHY20CaloIsolationDecorator



#====================================================================
# Skimming tool to select only events with the correct vertices
#====================================================================

#--------------------------------------------------------------------
## 9/ select the event. We only want to keep events that contain certain three-mu vertices which passed certain selection.
##    Exactly like in the preselection, where only 2mu vertices are treated.

#
#expression = "count(BPHY20JpsiCandidates.x > -999.0)+count(BPHY20BcJpsiMuCandidates.x > -999.0)+ count(BPHY20BcJpsiMuCandidates.passed_Bc) >0 "

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY20_SelectBcJpsiMuEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY20_SelectBcJpsiMuEvent",
     expression = "count(BPHY20BcJpsiMuCandidates.passed_Bc) >= 1 ")
   
ToolSvc += BPHY20_SelectBcJpsiMuEvent
print      BPHY20_SelectBcJpsiMuEvent

  #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
      
#   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
#   BPHY20SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
#       "BPHY20SkimmingOR",
#       FilterList = [BPHY20_SelectBcJpsiMuEvent] )
#   ToolSvc += BPHY20SkimmingOR
#   print      BPHY20SkimmingOR   
   

#====================================================================
# Add Extrapolation of muons to trigger layers
#====================================================================

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__MuonExtrapolationTool 
BPHY20_Extrap_Tool = DerivationFramework__MuonExtrapolationTool(   name = "BPHY20_ExtrapolationTool",   OutputLevel = INFO ) 

ToolSvc += BPHY20_Extrap_Tool





#====================================================================
# Thinning Helper and various thinning tools
#====================================================================

#--------------------------------------------------------------------
## 10/ Setup the thinning helper, only tool able to perform thinning of trigger navigation information

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY20ThinningHelper = ThinningHelper( "BPHY20ThinningHelper" )
BPHY20ThinningHelper.TriggerChains = 'HLT_.*mu.*' #triggerList    # . = any character; * = 0 or more times; + = 1 or more times; ? 0 or 1 times  "Regular_Expression"
BPHY20ThinningHelper.AppendToStream( BPHY20Stream )


#--------------------------------------------------------------------
## 11/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which haven't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the 
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## 12/ Cleans up, removing duplicate vertices. An issue caused by the logic of Jpsi+1 track in the case of 3-muon candidates

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxDuplicates
BPHY20Thin_vtxDuplicates = DerivationFramework__Thin_vtxDuplicates(name                       = "BPHY20Thin_vtxDuplicates",
                                                                  OutputLevel                = INFO,
                                                                  ThinningService            = BPHY20ThinningHelper.ThinningSvc(),
                                                                  TrackParticleContainerName = "InDetTrackParticles",
                                                                  VertexContainerNames       = ["BPHY20BcJpsiMuCandidates"],
                                                                  PassFlags                  = ["passed_Bc"])

ToolSvc += BPHY20Thin_vtxDuplicates

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY20Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY20Thin_vtxTrk",
  OutputLevel                = INFO,
  ThinningService            = BPHY20ThinningHelper.ThinningSvc(),
  TrackParticleContainerName = "InDetTrackParticles",
  AcceptanceRadius         = 1.,
  VertexContainerNames       = ["BPHY20BcJpsiMuCandidates"],
  PassFlags                  = ["passed_Bc"],
  ApplyAnd                   = True )  # "and" requirement for Vertices

ToolSvc += BPHY20Thin_vtxTrk


## 13/ thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##     between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY20MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY20MuonTPThinningTool",
                                                                         ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY20MuonTPThinningTool

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY20_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(name                       = "BPHY20_thinningTool_PV",
                                                                 ThinningService            = BPHY20ThinningHelper.ThinningSvc(),
                                                                 CandidateCollections       = ["BPHY20BcJpsiMuCandidates"],
                                                                 KeepPVTracks  =True)

ToolSvc += BPHY20_thinningTool_PV

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
BPHY20TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "BPHY20TauTPThinningTool",
                                                                       ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY20TauTPThinningTool

# Only save truth informtion directly associated with: mu Ds+ D+ D*+ Ds*+ D0 D*0 B+ B*+ B0 B*0 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY20TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY20TruthThinTool",
                                                               ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                               ParticleSelectionString = "abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 541 || abs(TruthParticles.pdgId) == 431 || abs(TruthParticles.pdgId) == 411 || abs(TruthParticles.pdgId) == 413 || abs(TruthParticles.pdgId) == 433 || TruthParticles.pdgId == 421 || TruthParticles.pdgId == 423 || abs(TruthParticles.pdgId) == 521 || abs(TruthParticles.pdgId) == 523 || TruthParticles.pdgId == 511 || TruthParticles.pdgId == 513",
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors      = True)
ToolSvc += BPHY20TruthThinTool

# Only save truth neutrino and b/c quarks information
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY20TruthThinNoChainTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY20TruthThinNoChainTool",
                                                              ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString = "abs(TruthParticles.pdgId) == 4 || abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16",
                                                              PreserveDescendants     = False,
                                                              PreserveAncestors      = False)
ToolSvc += BPHY20TruthThinNoChainTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

BPHY20ThinningTools = [ BPHY20MuonTPThinningTool, BPHY20Thin_vtxDuplicates, 
                        BPHY20Thin_vtxTrk, BPHY20_thinningTool_PV,  
                        BPHY20TauTPThinningTool]

BPHY20SkimmingTools = [BPHY20_SelectBcJpsiMuEvent]

BPHY20AugmentationTools = [BPHY20JpsiSelectAndWrite, BPHY20_Select_Jpsi2mumu,
                           BPHY20BcJpsiMuSelectAndWrite, BPHY20_Select_Bc2JpsiMu,
                           BPHY20_AugOriginalCounts,
                           BPHY20TrackIsolationDecorator, BPHY20CaloIsolationDecorator]

if addMuExtrapolationForTrigger:
    BPHY20AugmentationTools.append(BPHY20_Extrap_Tool)

Kernel1Tools = [BPHY20TriggerSkim]

if isSimulation:
    #BPHY20AugmentationTools.append(DFCommonTauTruthMatchingWrapper)
    if thinTruth:
       BPHY20ThinningTools.append(BPHY20TruthThinTool)
       BPHY20ThinningTools.append(BPHY20TruthThinNoChainTool)

#The sequence object. Is in principle just a wrapper which allows to run two kernels in sequence
BPHY20_Sequence = CfgMgr.AthSequencer("BPHY20_Sequence")


#onlyAugmentations implementation
if onlyAugmentations:
    Kernel1Tools = []
    BPHY20SkimmingTools = []
    BPHY20ThinningTools = []

# Kernel n1 PRESELECTION
# The name of the kernel (BPHY20Kernel1 in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY20_Sequence += CfgMgr.DerivationFramework__DerivationKernel("BPHY20Kernel_trigPresel",
                                                               AugmentationTools = [BPHY20TriggerCountToMetadata] ,
                                                               SkimmingTools     = Kernel1Tools)
# Kernel n2 deep Derivation
# The name of the kernel (BPHY20Kernel2 in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY20_Sequence += CfgMgr.DerivationFramework__DerivationKernel("BPHY20Kernel",
                                                               AugmentationTools = BPHY20AugmentationTools,
                                                               SkimmingTools     = BPHY20SkimmingTools, 
                                                               ThinningTools     = BPHY20ThinningTools)

#Vital, replaces the adding of kernels directly
DerivationFrameworkJob += BPHY20_Sequence

#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY20SlimmingHelper = SlimmingHelper("BPHY20SlimmingHelper")
AllVariables  = []
StaticContent = []


SmartCollections = [
                    "Photons", 
                    "TauJets", 
                    "AntiKt4EMTopoJets", 
                    "BTagging_AntiKt4EMTopo", 
                    "PrimaryVertices", 
                    "Muons", 
                    "InDetTrackParticles", 
                    "MET_Reference_AntiKt4EMTopo"
                    ]


AllVariables = ["METAssoc_AntiKt4EMTopo",
                 "MET_Core_AntiKt4EMTopo",
                 "MET_Truth",
                 "MET_Track",
                 "MET_LocHadTopo"]

AllVariables += ["Kt4EMTopoOriginEventShape",
                 "Kt4EMTopoEventShape"]

AllVariables += ["CombinedMuonTrackParticles",
                 "ExtrapolatedMuonTrackParticles",
                 "MuonSpectrometerTrackParticles"]


ExtraVariables = ["Photons.pt.eta.phi.m",
                  "Electrons.pt.eta.phi.m","TauJets.pt.eta.phi.m.IsTruthMatched.truthJetLink.truthParticleLink",
                  "AntiKt4EMTopoJets.JetPileupScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_m", 
                  "AntiKt4EMTopoJets.JvtJvfcorr.HECFrac.LArQuality.HECQuality.NegativeE.AverageLArQF", 
                  "AntiKt4EMTopoJets.JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m"]

ExtraVariables += ["Muons.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Hits.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits",
                   "Muons.numberOfTriggerEtaLayers.numberOfPhiLayers",
                   "CombinedMuonTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits", 
                   "InDetTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits.vx.vy.vz",
                   "PrimaryVertices.chiSquared.covariance"]


StaticContent =  ["xAOD::VertexContainer#BPHY20RefittedPrimaryVertices",
                  "xAOD::VertexAuxContainer#BPHY20RefittedPrimaryVerticesAux."]


## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20JpsiSelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi Mu+ candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20BcJpsiMuSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20BcJpsiMuSelectAndWrite.OutputVtxContainerName]


# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "METMap_Truth"]
    SmartCollections += ["AntiKt4TruthJets"] 

# Needed for trigger objects
BPHY20SlimmingHelper.IncludeMuonTriggerContent = True
BPHY20SlimmingHelper.IncludeBPhysTriggerContent = True

# Pass all lists to the SlimmingHelper
BPHY20SlimmingHelper.ExtraVariables = ExtraVariables
BPHY20SlimmingHelper.AllVariables = AllVariables
BPHY20SlimmingHelper.StaticContent = StaticContent
BPHY20SlimmingHelper.SmartCollections = SmartCollections
BPHY20SlimmingHelper.AppendContentToStream(BPHY20Stream)

