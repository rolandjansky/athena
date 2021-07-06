#==================================================================== 
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#==================================================================== 

#====================================================================
# DAOD_HIGG1D1.py
# This defines DAOD_HIGG1D1, an Diphoton skimmed DAOD format for Run 3.
# It requires the reductionConf flag HIGG1D1 in Reco_tf.py   
#====================================================================

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr
from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkMuons import MuonsCommon
# Common augmentations ("DFCommon")
InDetCommon.makeInDetDFCommon()
EGammaCommon.makeEGammaDFCommon()
MuonsCommon.makeMuonsDFCommon()
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool, getPFlowfJVT
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
import re



#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D1Stream )
HIGG1D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D1Stream.AcceptAlgs(["HIGG1D1Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
augmentationTools   = []

# Special sequence 
SeqHIGG1D1 = CfgMgr.AthSequencer("SeqHIGG1D1")


#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   HIGG1D1TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "HIGG1D1TruthCharmTool",
                                                                        NewCollectionName       = "TruthCharm",
                                                                        KeepNavigationInfo      = False,
                                                                        ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                        Do_Compress             = True)
   ToolSvc += HIGG1D1TruthCharmTool 
   SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("HIGG1D1TruthCharmKernel",AugmentationTools=[HIGG1D1TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqHIGG1D1)
   # Add standard truth
   addStandardTruthContents(SeqHIGG1D1,prefix="DFCommon_")
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   HIGG1D1TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="HIGG1D1TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += HIGG1D1TruthNavigationDecorator
   SeqHIGG1D1.DFCommon_MCTruthNavigationDecoratorKernel.AugmentationTools = [HIGG1D1TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqHIGG1D1)
   addPVCollection(SeqHIGG1D1)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"




#====================================================================
# TRIGGER CONTENT   
#====================================================================
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)
#
## Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))
#
## Now reduce the list...
trigger_names_notau = []
trigger_names_tau = []
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.EDMVersion == 3:
   r_tau = re.compile("HLT_.*tau.*")
   r_notau = re.compile("HLT_[1-9]*(e|mu|g).*") 
   r_veto = re.compile("HLT_.*(LRT).*")   
   for chain_name in GetFileMD(ConfigFlags.Input.Files)["TriggerMenu"]["HLTChains"]:
      result_tau = r_tau.match(chain_name)
      result_notau = r_notau.match(chain_name)
      result_veto = r_veto.match(chain_name)
      if result_tau is not None and result_veto is None: trigger_names_tau.append(chain_name)
      if result_notau is not None and result_veto is None: trigger_names_notau.append(chain_name)
   trigger_names_notau = set(trigger_names_notau) - set(trigger_names_tau)
   trigger_names_notau = list(trigger_names_notau)
else:
   for chain_name in GetFileMD(ConfigFlags.Input.Files)["TriggerMenu"]["HLTChains"]:
      if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
      if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name) 
# Create trigger matching decorations
HIGG1D1_trigmatching_helper_notau = TriggerMatchingHelper(name="HIGG1D1TriggerMatchingToolNoTau",
        trigger_list = trigger_names_notau, add_to_df_job=True)
HIGG1D1_trigmatching_helper_tau = TriggerMatchingHelper(name="HIGG1D1TriggerMatchingToolTau",
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)



#====================================================================
# PV refitting after removing Z->ee tracks, for vertex studies
#====================================================================

# Creates a vertex container (ZeeRefittedPrimaryVertices) where the type=1 vertex is refitted
# after removing tracks that are associated with Z->ee decay candidates
# Tool runs only for data and Zee MC samples (must be defined in the MCSamples list)
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__ZeeVertexRefittingTool
HIGG1D1_ZeeVertexRefitterTool = DerivationFramework__ZeeVertexRefittingTool( name = "HIGG1D1_ZeeVertexRefitterTool",
                                    ObjectRequirements="(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.*GeV)",
                                    LowMassCut=50*Units.GeV,
                                    RefittedPVContainerName="ZeeRefittedPrimaryVertices",                                    
                                    ElectronContainerName="Electrons",
                                    PVContainerName="PrimaryVertices",
                                    MCSamples = [361106] )
ToolSvc += HIGG1D1_ZeeVertexRefitterTool
SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("ZeeVertexRefitKernel", AugmentationTools = [HIGG1D1_ZeeVertexRefitterTool])

#====================================================================
# Diphoton vertex decoration tool
#====================================================================

# Creates a shallow copy of PrimaryVertices (HggPrimaryVertices) for diphoton events
# Must be created before the jetalg in the sequence as it is input to the modified PFlow jets

import PhotonVertexSelection.PhotonVertexSelectionConf as PVS 
PhotonPointingTool = PVS.CP__PhotonPointingTool( name = "yyVtxPhotonPointingTool", isSimulation = rec.doTruth() )
ToolSvc += PhotonPointingTool
PhotonVertexSelectionTool = PVS.CP__PhotonVertexSelectionTool( name = "PhotonVertexSelectionTool", PhotonPointingTool = PhotonPointingTool)
ToolSvc += PhotonVertexSelectionTool

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__DiphotonVertexDecorator
DiphotonVertexDecorator = DerivationFramework__DiphotonVertexDecorator(name = "DiphotonVertexDecorator",  
                                                                     PhotonVertexSelectionTool = PhotonVertexSelectionTool)
ToolSvc += DiphotonVertexDecorator
SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("DiphotonVertexKernel", AugmentationTools = [DiphotonVertexDecorator])


isMC = False
if globalflags.DataSource()=="geant4":
    isMC = True
    from DerivationFrameworkMCTruth.MCTruthCommon import addPVCollection, addStandardTruthContents
    addStandardTruthContents(SeqHIGG1D1,prefix="HIGG1D1_")
    addPVCollection()
    print( "HIGG1D1.py Applying MCTruthCommon")


#====================================================================
# SKIMMING TOOLS 
#====================================================================

print( "HIGG1D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy() )
SkipTriggerRequirement=((globalflags.DataSource()=="geant4") and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print( "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement)
TriggerExp = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerExp               = ["EF_g35_loose_g25_loose"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerExp               = ["HLT_2g50_loose_L12EM20VH","HLT_2g25_loose_g15_loose","HLT_g35_medium_g25_medium_L12EM20VH","HLT_2g25_tight_L12EM20VH","HLT_2g22_tight_L12EM15VHI","HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_2g50_loose","HLT_2g20_tight","HLT_2g22_tight","HLT_2g20_tight_icalovloose_L12EM15VHI","HLT_2g20_tight_icalotight_L12EM15VHI","HLT_2g22_tight_L12EM15VHI","HLT_2g22_tight_icalovloose_L12EM15VHI","HLT_2g22_tight_icalotight_L12EM15VHI","HLT_2g22_tight_icalovloose","HLT_2g25_tight_L12EM20VH","HLT_2g20_loose","HLT_2g20_loose_L12EM15","HLT_g35_medium_g25_medium","HLT_g35_medium_g25_medium_L12EM15VH","HLT_g35_loose_g25_loose","HLT_g35_loose_g25_loose_L12EM15VH", "HLT_2g20_loose_g15_loose", "HLT_3g20_loose", "HLT_3g15_loose", "HLT_2g6_tight_icalotight_L1J100", "HLT_2g6_loose_L1J100", "HLT_2g6_tight_icalotight_L1J50", "HLT_2g6_loose_L1J50","HLT_g120_loose","HLT_g140_loose"]


print( "HIGG1D1.py TriggerExp", TriggerExp )

KeepTrigger=not SkipTriggerRequirement
#KeepTrigger=False

print( "HIGG1D1.py KeepTrigger", KeepTrigger )

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
SkimmingToolHIGG1D1 = DerivationFramework__SkimmingToolHIGG1(
                                 name = "SkimmingToolHIGG1D1",
                                 RequireGRL = True,
                                 ReqireLArError = True,
                                 RequireTrigger = KeepTrigger,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False)

ToolSvc += SkimmingToolHIGG1D1


#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
# TODO correct this so it keeps tracks from the selected PV
HIGG1D1_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D1TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG1D1TrackParticleThinningTool",
                                                                           StreamName              = HIGG1D1Stream.Name, 
                                                                           SelectionString         = HIGG1D1_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D1TrackParticleThinningTool
thinningTools.append(HIGG1D1TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG1D1MuonTPThinningTool",
                                                                        StreamName              = HIGG1D1Stream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D1MuonTPThinningTool
thinningTools.append(HIGG1D1MuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
HIGG1D1TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "HIGG1D1TauJetsThinningTool",
                                                                     StreamName      = HIGG1D1Stream.Name,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += HIGG1D1TauJetsThinningTool
thinningTools.append(HIGG1D1TauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG1D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG1D1TauTPThinningTool",
                                                                      StreamName             = HIGG1D1Stream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += HIGG1D1TauTPThinningTool
thinningTools.append(HIGG1D1TauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
HIGG1D1DiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "HIGG1D1DiTauTPThinningTool",
                                                                          StreamName              = HIGG1D1Stream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1DiTauTPThinningTool
thinningTools.append(HIGG1D1DiTauTPThinningTool)





from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=="geant4":
    # Truth thinning
    truth_cond_1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_3 = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
    truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_cond_5 = "(abs(TruthParticles.pdgId) >=  1000000)" # BSM
    truth_cond_finalState = "(TruthParticles.status == 1 && TruthParticles.barcode < 200000)" # stable particles
    truth_expression = "("+truth_cond_1+" || "+truth_cond_2 +" || "+truth_cond_3 +" || "+truth_cond_4+ " || "+truth_cond_5+") || ("+truth_cond_finalState+")"

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG1D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D1TruthThinningTool", 
                                                                        StreamName              = HIGG1D1Stream.Name,
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = False,
                                                                        PreserveGeneratorDescendants  = True,
                                                                        PreserveAncestors             = True)
    ToolSvc += HIGG1D1TruthThinningTool
    thinningTools.append(HIGG1D1TruthThinningTool)

print( "HIGG1D1.py thinningTools", thinningTools )

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from LArCabling.LArCablingAccess import LArOnOffIdMapping
#LArOnOffIdMapping()



from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HIGG1D1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "HIGG1D1_MaxCellDecoratorTool",
                                                                      SGKey_electrons = "Electrons",
                                                                      SGKey_photons   = "Photons"
                                                                      )
ToolSvc += HIGG1D1_MaxCellDecoratorTool
augmentationTools.append(HIGG1D1_MaxCellDecoratorTool)
#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
HIGG1D1_GainDecoratorTool = GainDecorator()
ToolSvc += HIGG1D1_GainDecoratorTool
augmentationTools.append(HIGG1D1_GainDecoratorTool)


cluster_sizes = (3,5), (5,7), (7,7), (7,11)
HIGG1D1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
augmentationTools += HIGG1D1_ClusterEnergyPerLayerDecorators

#====================================================================
# JET/MET   
#====================================================================

# TODO: UFO jets to be added in the future
largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" #, "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]

OutputJets["HIGG1D1"] = largeRJetCollections
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets", "AntiKtVR30Rmax4Rmin02PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["HIGG1D1"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqHIGG1D1,"HIGG1D1")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqHIGG1D1,"jetalgAntiKt10TruthTrimmedPtFrac5SmallR20")
addDefaultTrimmedJets(SeqHIGG1D1,"HIGG1D1",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqHIGG1D1,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqHIGG1D1,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqHIGG1D1,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg="AntiKt4EMPFlow",sequence=SeqHIGG1D1, algname="PHYSJetForwardPFlowJvtToolAlg")




OutputJets["HIGG1D1"] = []
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

replaceAODReducedJets(reducedJetList,SeqHIGG1D1,"HIGG1D1")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqHIGG1D1,"jetalgAntiKt10TruthTrimmedPtFrac5SmallR20")
addDefaultTrimmedJets(SeqHIGG1D1,"HIGG1D1",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
#if (DerivationFrameworkIsMonteCarlo):
#   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqHIGG1D1,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqHIGG1D1,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqHIGG1D1,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg="AntiKt4EMPFlow",sequence=SeqHIGG1D1, algname="HIGG1D1JetForwardPFlowJvtToolAlg")




#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
#from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqHIGG1D1 += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D1Kernel",
                                                            SkimmingTools = [SkimmingToolHIGG1D1],
                                                            AugmentationTools = augmentationTools,
                                                            ThinningTools = thinningTools)




#====================================================================
# Custom JETS
#====================================================================

PrefixForHggCollection   = "Hgg"
HggVertexContainerName   = PrefixForHggCollection+"PrimaryVertices"
CustomPFJetContainerName = "AntiKt4PFlowCustomVtxHgg"

# we need to import the jet config helpers :
from ROOT import xAODType
from JetRec.JetRecStandardToolManager import jtm
from JetRecTools.ConstitToolManager import ctm
from JetRecTools.JetRecToolsConf import  JetConstituentModSequence, SoftKillerWeightTool, ConstituentSubtractorTool, ChargedHadronSubtractionTool, CorrectPFOTool
from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets, defineEDAlg

# Tools to correct PFO"s
ctm.add(CorrectPFOTool("correctPFOCustomVtx",   WeightPFOTool = jtm.pflowweighter, InputIsEM=True, CalibratePFO=True, CorrectNeutral=True,InputType = xAODType.ParticleFlow,VertexContainerKey=HggVertexContainerName ), alias="correctPFOCustomVtx")
ctm.add(ChargedHadronSubtractionTool("pfoCHSCustomVtx", InputType = xAODType.ParticleFlow, VertexContainerKey=HggVertexContainerName), alias="pfoCHSCustomVtx")

# here we prepare a tool building our unified PFlow container. The prefix of the output collection is "PlainLC"
jtm += ctm.buildConstitModifSequence( "JetConstitSeq_PFlowCustomVtx",
                                    OutputContainer = "CustomVtx", #"ParticleFlowObjects" will be appended to the end
                                    InputContainer= "JetETMiss",
                                    InputType = xAODType.ParticleFlow,
                                    modList = [ "correctPFOCustomVtx", "pfoCHSCustomVtx" ] )

# Run the FPO correction tools
ctools = [jtm.JetConstitSeq_PFlowCustomVtx]

from JetRec.JetRecConf import JetToolRunner
from JetRec.JetRecConf import JetAlgorithm
jtm += JetToolRunner("jetconstitPFlowCustomVtx",
                         EventShapeTools=[],
                         Tools=ctools,
                         Timer=jetFlags.timeJetToolRunner()
                         )
SeqHIGG1D1 += JetAlgorithm("jetalgConstituentsPFlowCustomVtx",
                        Tools=[jtm.jetconstitPFlowCustomVtx])


if not hasattr(SeqHIGG1D1, jtm.pflowcustomvtxget.name()):
    SeqHIGG1D1 += jtm.pflowcustomvtxget

#EventShape (needed for calibration)
if not hasattr(SeqHIGG1D1, "EventDensityAlgEDTool4PFlowCustomVtx"):
    SeqHIGG1D1 += defineEDAlg(R=0.4, inputtype="PFlowCustomVtx")

# Get empflow_reduced getters but replace empflowget with pflowcustomvtxge
myGetters=[jtm.pflowcustomvtxget if i==jtm.empflowget else i for i in jtm.gettersMap["empflow_reduced"]]  
for getter in myGetters:
    print( "MyGetter: ", getter.name()) 

# Finally add the new jet collection
finderArgs = dict()
finderArgs["ptmin"] = 5000
finderArgs["ptminFilter"] = 10000
finderArgs["ghostArea"] = 0.01
finderArgs["mods"] = "pflow_ungroomed"
finderArgs["calibOpt"] = "a:pflow"
finderArgs["algseq"] = SeqHIGG1D1
finderArgs["outputGroup"] = CustomPFJetContainerName
finderArgs["customGetters"] = myGetters
addStandardJets("AntiKt", 0.4, "PFlowCustomVtx", **finderArgs)

# Add QG tagger
addQGTaggerTool(jetalg="AntiKt4PFlowCustomVtx",sequence=SeqHIGG1D1,algname="QGTaggerToolPFCVAlg")

# fJVT
# TODO fails becasue of residual pileup correction file name issue
getPFlowfJVT(jetalg="AntiKt4PFlowCustomVtx",sequence=SeqHIGG1D1, algname="JetForwardPFlowCustomVtxHggJvtToolAlg", primaryVertexCont=HggVertexContainerName,overlapLabel="")




#MET associated to HggPrimaryVertices
from DerivationFrameworkJetEtMiss import METCommon
#TODO  Understand if DerivationFrameworkJetEtMiss/METCommon.py" need to be updated 
METCommon.scheduleMETCustomVertex ( vxColl="Hgg", jetcoll="AntiKt4PFlowCustomVtx", outputlist = "CustomMET" )
METCommon.scheduleMETAssocAlg(SeqHIGG1D1)




#====================================================================
# FLAVOUR TAGGING   
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
FtagJetCollections(["AntiKt4PFlowCustomVtxJets","AntiKt4EMPFlowJets"], SeqHIGG1D1, pvCols=[HggVertexContainerName,"PrimaryVertices"])


#====================================================================
# ED
#====================================================================
if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ["EDTruthCentralAlg", "EDTruthForwardAlg"]:
      if hasattr(topSequence, alg):
         edtalg = getattr(topSequence, alg)
         delattr(topSequence, alg)
         SeqHIGG1D1 += edtalg

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper")

HIGG1D1SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

# Trigger content
HIGG1D1SlimmingHelper.IncludeTriggerNavigation = False
HIGG1D1SlimmingHelper.IncludeJetTriggerContent = False
HIGG1D1SlimmingHelper.IncludeMuonTriggerContent = False
HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = False
HIGG1D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
HIGG1D1SlimmingHelper.IncludeTauTriggerContent = False
HIGG1D1SlimmingHelper.IncludeEtMissTriggerContent = False
HIGG1D1SlimmingHelper.IncludeBJetTriggerContent = False
HIGG1D1SlimmingHelper.IncludeBPhysTriggerContent = False
HIGG1D1SlimmingHelper.IncludeMinBiasTriggerContent = False
HIGG1D1SlimmingHelper.AllVariables = ["HLT_xAOD__PhotonContainer_egamma_Iso_Photons","Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","TruthElectrons","TruthPhotons","TruthMuons","TruthTaus","TruthBoson","PrimaryVertices","MET_Truth", "MET_Track","egammaTruthParticles","CaloCalTopoClusters","HggPrimaryVertices","ZeeRefittedPrimaryVertices"]


# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(HIGG1D1SlimmingHelper,["HIGG1D1"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   HIGG1D1SlimmingHelper.AppendToDictionary = {"TruthEvents":"xAOD::TruthEventContainer","TruthEventsAux":"xAOD::TruthEventAuxContainer",
                                            "MET_Truth":"xAOD::MissingETContainer","MET_TruthAux":"xAOD::MissingETAuxContainer",
                                            "TruthElectrons":"xAOD::TruthParticleContainer","TruthElectronsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthMuons":"xAOD::TruthParticleContainer","TruthMuonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthPhotons":"xAOD::TruthParticleContainer","TruthPhotonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthTaus":"xAOD::TruthParticleContainer","TruthTausAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthNeutrinos":"xAOD::TruthParticleContainer","TruthNeutrinosAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBSM":"xAOD::TruthParticleContainer","TruthBSMAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBoson":"xAOD::TruthParticleContainer","TruthBosonAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthTop":"xAOD::TruthParticleContainer","TruthTopAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthForwardProtons":"xAOD::TruthParticleContainer","TruthForwardProtonsAux":"xAOD::TruthParticleAuxContainer",
                                            "BornLeptons":"xAOD::TruthParticleContainer","BornLeptonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBosonsWithDecayParticles":"xAOD::TruthParticleContainer","TruthBosonsWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBosonsWithDecayVertices":"xAOD::TruthVertexContainer","TruthBosonsWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthBSMWithDecayParticles":"xAOD::TruthParticleContainer","TruthBSMWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBSMWithDecayVertices":"xAOD::TruthVertexContainer","TruthBSMWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "HardScatterParticles":"xAOD::TruthParticleContainer","HardScatterParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "HardScatterVertices":"xAOD::TruthVertexContainer","HardScatterVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthHFWithDecayParticles":"xAOD::TruthParticleContainer","TruthHFWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthHFWithDecayVertices":"xAOD::TruthVertexContainer","TruthHFWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthCharm":"xAOD::TruthParticleContainer","TruthCharmAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthPrimaryVertices":"xAOD::TruthVertexContainer","TruthPrimaryVerticesAux":"xAOD::TruthVertexAuxContainer",
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(HIGG1D1SlimmingHelper)
   HIGG1D1SlimmingHelper.AllVariables += ["TruthHFWithDecayParticles","TruthHFWithDecayVertices","TruthCharm"]

HIGG1D1SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "AntiKt4PFlowCustomVtxJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z",
                                      "InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights"]

#Custom Jet Varibles

HIGG1D1SlimmingHelper.AppendToDictionary = {  "AntiKt4PFlowCustomVtxJets": "xAOD::JetContainer", "AntiKt4PFlowCustomVtxJetsAux":"xAOD::JetAuxContainer",
        "METAssoc_AntiKt4PFlowCustomVtxHgg":"xAOD::MissingETAssociationMap", "METAssoc_AntiKt4PFlowCustomVtxHggAux":"xAOD::MissingETAuxAssociationMap",
        "MET_Core_AntiKt4PFlowCustomVtxHgg":"xAOD::MissingETContainer", "MET_Core_AntiKt4PFlowCustomVtxHggAux":"xAOD::MissingETAuxContainer",
        HggVertexContainerName:"xAOD::VertexContainer", HggVertexContainerName+"Aux":"xAOD::ShallowAuxContainer",
        "Kt4PFlowCustomVtxEventShape":"xAOD::EventShape", "Kt4PFlowCustomVtxEventShapeAux":"xAOD::EventShapeAuxInfo"
     }

HIGG1D1SlimmingHelper.AllVariables += [HggVertexContainerName,"ZeeRefittedPrimaryVertices","AntiKt4PFlowCustomVtxJets","Kt4PFlowCustomVtxEventShape"]


from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent, BTaggingXbbContent
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4PFlowCustomVtxJets")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowJets")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4PFlowCustomVtxJets")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4EMPFlowJets")



# is this really needed given Photons are in the AllVariables list ?
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import PhotonsCPDetailedContent
HIGG1D1SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(HIGG1D1_GainDecoratorTool) )
for tool in HIGG1D1_ClusterEnergyPerLayerDecorators:
    HIGG1D1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

from DerivationFrameworkJetEtMiss.METCommon import addMETOutputs
addMETOutputs(HIGG1D1SlimmingHelper,["AntiKt4PFlowCustomVtxHgg"])
addMETOutputs(HIGG1D1SlimmingHelper,["AntiKt4EMPFlow"])
HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = True


HIGG1D1Stream.AddItem("xAOD::CutBookkeeperContainer#CutBookkeepers")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperAuxContainer#CutBookkeepersAux.")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepersAux.")
HIGG1D1Stream.AddItem("ByteStreamMetadataContainer#ByteStreamMetadata")

# Add trigger matching
HIGG1D1_trigmatching_helper_notau.add_to_slimming(HIGG1D1SlimmingHelper)
HIGG1D1_trigmatching_helper_tau.add_to_slimming(HIGG1D1SlimmingHelper)

# Final construction of output stream
HIGG1D1SlimmingHelper.AppendContentToStream(HIGG1D1Stream)
    

#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job
DerivationFrameworkJob += SeqHIGG1D1
