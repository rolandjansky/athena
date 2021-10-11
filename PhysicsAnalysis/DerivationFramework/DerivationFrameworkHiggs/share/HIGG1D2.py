#====================================================================
# DAOD_HIGG1D2.py
# This defines DAOD_HIGG1D2, an Diphoton skimmed DAOD format for Run 3.
# It requires the reductionConf flag HIGG1D2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkMuons import MuonsCommon
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool, getPFlowfJVT
from DerivationFrameworkJetEtMiss import METCommon
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D2Stream )
HIGG1D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D2Stream.AcceptAlgs(["HIGG1D2Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
augmentationTools   = []

# Special sequence 
SeqHIGG1D2 = CfgMgr.AthSequencer("SeqHIGG1D2")

#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   HIGG1D2TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "HIGG1D2TruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += HIGG1D2TruthCharmTool
   #from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqHIGG1D2 += CfgMgr.DerivationFramework__CommonAugmentation("HIGG1D2TruthCharmKernel",AugmentationTools=[HIGG1D2TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqHIGG1D2)
   # Add standard truth
   addStandardTruthContents(SeqHIGG1D2,prefix='HIGG1D2_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   HIGG1D2TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="HIGG1D2TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += HIGG1D2TruthNavigationDecorator
   SeqHIGG1D2.HIGG1D2_MCTruthNavigationDecoratorKernel.AugmentationTools = [HIGG1D2TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqHIGG1D2)
   addPVCollection(SeqHIGG1D2)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # Add sumOfWeights metadata for LHE3 multiweights =======
   from DerivationFrameworkCore.LHE3WeightMetadata import *



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
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.EDMVersion == 3:
   r_tau = re.compile("HLT_.*tau.*")
   r_notau = re.compile("HLT_[1-9]*(e|mu|g).*") 
   r_veto = re.compile("HLT_.*(LRT).*")   
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      result_tau = r_tau.match(chain_name)
      result_notau = r_notau.match(chain_name)
      result_veto = r_veto.match(chain_name)
      if result_tau is not None and result_veto is None: trigger_names_tau.append(chain_name)
      if result_notau is not None and result_veto is None: trigger_names_notau.append(chain_name)
   trigger_names_notau = set(trigger_names_notau) - set(trigger_names_tau)
   trigger_names_notau = list(trigger_names_notau)
else:
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
      if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name) 
# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='HIGG1D2TriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='HIGG1D2TriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)




#====================================================================
# SKIMMING TOOLS
#====================================================================


SkipTriggerRequirement=((globalflags.DataSource()=='geant4'))
print( "HIGG1D2.py SkipTriggerRequirement: ",  SkipTriggerRequirement )
TriggerExp = []
TriggerMerged = []
if not SkipTriggerRequirement:

   try:
     year=int(rec.projectName()[4:6])
     if year > 18 :
       year=18
     print( "HIGG1D2.py: Project tag: " + rec.projectName() +  " Year: " +  str(year) )
   except:
     print( "HIGG1D2.py: Failed to extract year from project tag "+ rec.projectName() +". Guessing 2017" )
     year=17
   print( "HIGG1D2.py: Setting up trigger requirement for year 20" + str(year) )
   if year==17 or year==18:
     TriggerMerged          = ["HLT_e25_mergedtight_g35_medium_Heg","HLT_e30_mergedtight_g35_medium_Heg","HLT_2g50_loose_L12EM20VH","HLT_e24_lhmedium_nod0_L1EM20VH_g25_medium","HLT_g35_medium_g25_medium_L12EM20VH"]
     TriggerExp             = [
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_g35_loose_L1EM24VHI_mu15_mu2noL1", # photon dimuon
                               "HLT_g35_loose_L1EM24VHI_mu18",  #photon muon
                               "HLT_g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1", # photon dimuon
                               "HLT_g35_tight_icalotight_L1EM24VHI_mu18noL1", # photon muon
                               "HLT_2mu14", #dimuon
                               "HLT_mu22_mu8noL1", #dimuon
                               "HLT_mu26_ivarmedium", #single muon
                               "HLT_mu50",
                               "HLT_mu60_0eta105_msonly",
                               "HLT_e26_lhtight_nod0_ivarloose", #single electron
                               "HLT_e60_lhmedium_nod0",
                               "HLT_e140_lhloose_nod0",
                               "HLT_2e17_lhvloose_nod0_L12EM15VHI", #dielectron
                               "HLT_2e24_lhvloose_nod0",
                               "HLT_e25_mergedtight_g35_medium_Heg", #merged electron
                               "HLT_e30_mergedtight_g35_medium_Heg",
                               "HLT_e24_lhmedium_nod0_L1EM20VH_g25_medium",
                               "HLT_g35_medium_g25_medium_L12EM20VH",
                               "HLT_2g50_loose_L12EM20VH", #di photon
                               "HLT_g140_loose", #single photon
                               "HLT_g200_loose",
                               "HLT_g300_etcut"  #single photon
                              ]
   elif year==16:
     #2016
     TriggerMerged          = ["HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_e20_lhmedium_nod0_g35_loose"]
     TriggerExp             = [
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1", # photon dimuon
                               "HLT_g35_loose_L1EM22VHI_mu18noL1",  #photon muon
                               "HLT_2mu10",
                               "HLT_2mu14", #dimuon
                               "HLT_mu22_mu8noL1", #dimuon
                               "HLT_mu20_mu8noL1",
                               "HLT_mu24_ivarmedium", #single muon
                               "HLT_mu26_ivarmedium",
                               "HLT_mu50",
                               "HLT_e24_lhtight_nod0_ivarloose", #single electron
                               "HLT_e26_lhtight_nod0_ivarloose",
                               "HLT_e60_lhmedium_nod0",
                               "HLT_e60_medium",
                               "HLT_e140_lhloose_nod0",
                               "HLT_2e15_lhvloose_nod0_L12EM13VHI", #dielectron
                               "HLT_2e17_lhvloose_nod0",
                               "HLT_g35_loose_g25_loose", #di photon
                               "HLT_g35_medium_g25_medium",
                               "HLT_e20_lhmedium_nod0_g35_loose", # electron  +  photon
                               "HLT_g140_loose", #single photon
                               "HLT_g300_etcut"  #single photon
                              ]
   elif year ==15:
     #2015
     TriggerMerged          = ["HLT_g35_loose_g25_loose","HLT_e20_lhmedium_g35_loose"]
     TriggerExp             = [
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_2mu10", #dimuon
                               "HLT_mu18_mu8noL1", #dimuon
                               "HLT_mu20_iloose_L1MU15", #single muon
                               "HLT_mu40",
                               "HLT_mu60_0eta105_msonly",
                               "HLT_e24_lhmedium_L1EM20VH", #single electron
                               "HLT_e60_lhmedium",
                               "HLT_e120_lhloose",
                               "HLT_2e12_lhloose_L12EM10VH", #dielectron
                               "HLT_e20_lhmedium_g35_loose", #electron + photon
                               "HLT_g35_loose_g25_loose", #di photon
                               "HLT_g120_loose", #single photon
                               "HLT_g200_etcut"  #single photon
                              ]
   else:
     print( "HIGG1D2.py : Year not supported -- results might be ugly. Year : ", year )
     SkipTriggerRequirement =  True


print( "HIGG1D2.py TriggerExp", TriggerExp )


RequireTrigger=not SkipTriggerRequirement

print( "HIGG1D2.py RequireTrigger", RequireTrigger )


from ROOT import egammaPID
MergedElectronIsEM = CfgMgr.AsgElectronIsEMSelector("MergedElectronIsEM",
                                             isEMMask=egammaPID.ElectronTightHLT,
                                             ConfigFile="ElectronPhotonSelectorTools/trigger/rel21_20161021/ElectronIsEMMergedTightSelectorCutDefs.conf")
ToolSvc += MergedElectronIsEM


from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
HIGG1D2SkimmingTool = DerivationFramework__SkimmingToolHIGG1(
                                 name = "HIGG1D2SkimmingTool",
                                 RequireGRL = False,
                                 ReqireLArError = True,
                                 RequireTrigger = RequireTrigger,
                                 IncludeDoublePhotonPreselection = False,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 MergedElectronTriggers = TriggerMerged,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False,
                                 IncludePhotonDoubleElectronPreselection = True,
                                 IncludeDoubleMuonPreselection = True,
                                 IncludePhotonMergedElectronPreselection = True,
                                 IncludeHighPtPhotonElectronPreselection = True,
                                 MinimumPhotonPt = 9.9*GeV,
                                 MinimumElectronPt = 4.4*GeV,
                                 MinimumMergedElectronPt = 18*GeV,
                                 MinimumMuonPt = 2.9*GeV,
                                 MaxMuonEta = 2.7,
                                 RemoveCrack = False,
                                 MaxEta = 2.5,
                                 MergedElectronCutTool = MergedElectronIsEM
                                 )

ToolSvc += HIGG1D2SkimmingTool
print( HIGG1D2SkimmingTool )



#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
# Inner detector group recommendations for indet tracks in analysis
HIGG1D2_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D2TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG1D2TrackParticleThinningTool",
                                                                           StreamName              = HIGG1D2Stream.Name, 
                                                                           SelectionString         = HIGG1D2_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D2TrackParticleThinningTool
thinningTools.append(HIGG1D2TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG1D2MuonTPThinningTool",
                                                                        StreamName              = HIGG1D2Stream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D2MuonTPThinningTool
thinningTools.append(HIGG1D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2ElectronTPThinningTool",
                                                                                  StreamName              = HIGG1D2Stream.Name,
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2ElectronTPThinningTool
print( HIGG1D2ElectronTPThinningTool )
#thinningTools.append(HIGG1D2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2PhotonTPThinningTool",
                                                                                  StreamName              = HIGG1D2Stream.Name,
                                                                                  SGKey                   = "Photons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2PhotonTPThinningTool
thinningTools.append(HIGG1D2PhotonTPThinningTool)


# Truth thinning
truth_cond_1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3 = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D2TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D2TruthThinningTool",
                                                                      StreamName              = HIGG1D2Stream.Name,
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print( "HIGG1D2.py globalflags.DataSource()", globalflags.DataSource() )

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D2TruthThinningTool
    thinningTools.append(HIGG1D2TruthThinningTool)
print( "HIGG1D2.py thinningTools", thinningTools )



#====================================================================
# AugmentationTools
#====================================================================

from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
egammaExtrapolationTool = EMExtrapolationTools(name = 'egammaExtrapolationTool',
                                               NarrowDeltaEta = 0.5,
                                               NarrowDeltaPhi = 1.0)

ToolSvc += egammaExtrapolationTool

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
AnExtrapolator = AtlasExtrapolator()
ToolSvc += AnExtrapolator

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(name = "TrkVKalVrtFitter",
                                         Extrapolator = AnExtrapolator,
                                         FirstMeasuredPoint = False)
ToolSvc += TrkVKalVrtFitter


from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__MergedElectronDetailsDecorator
HIGG1D2MergedElectronDetailsDecorator = DerivationFramework__MergedElectronDetailsDecorator(
                                 name = "HIGG1D2MergedElectronDetailsDecorator",
                                 EMExtrapolationTool = egammaExtrapolationTool,
                                 VertexFitterTool = TrkVKalVrtFitter
                                 )

ToolSvc += HIGG1D2MergedElectronDetailsDecorator
print( HIGG1D2MergedElectronDetailsDecorator )





#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D2Kernel",
                                                                       AugmentationTools = [HIGG1D2MergedElectronDetailsDecorator],
                                                                       SkimmingTools = [HIGG1D2SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                      )

# Before any custom jet reconstruction, it's good to set up the output list
OutputJets["HIGG1D2Jets"] = []

#====================================================================
# JET/MET   
#====================================================================

# TODO: UFO jets to be added in the future
largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" #, "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]

OutputJets["HIGG1D2"] = largeRJetCollections
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets", "AntiKtVR30Rmax4Rmin02PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["HIGG1D2"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqHIGG1D2,"HIGG1D2")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqHIGG1D2,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(SeqHIGG1D2,"HIGG1D2",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqHIGG1D2,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqHIGG1D2,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqHIGG1D2,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqHIGG1D2, algname='PHYSJetForwardPFlowJvtToolAlg')




#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D2SlimmingHelper = SlimmingHelper("HIGG1D2SlimmingHelper")
HIGG1D2Stream.AddItem("xAOD::EventShape#*")
HIGG1D2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

HIGG1D2SlimmingHelper.AppendToDictionary = {'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPrimaryVertices': 'xAOD::VertexContainer','TruthPrimaryVerticesAux': 'xAOD::VertexAuxContainer'}

HIGG1D2SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "MET_Reference_AntiKt4EMPFlow",
                                          "AntiKt4EMPFlowJets",
                                          "AntiKt4EMTopoJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201903",
                                          "BTagging_AntiKt4EMTopo_201810",
                                          "BTagging_AntiKt4EMPFlow_201810",
                                          "BTagging_AntiKt4EMPFlow_201903",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D2SlimmingHelper.AllVariables = ["Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthElectrons","TruthPhotons","TruthMuons","TruthBoson","TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","PrimaryVertices","MET_Truth", "MET_Track", "egammaTruthParticles","CaloCalTopoClusters"]

HIGG1D2SlimmingHelper.ExtraVariables = ["PhotonsAux.DFCommonPhotonsIsEMTightPtIncl.DFCommonPhotonsIsEMTightPtInclIsEMValue",
                                        "Muons.quality.EnergyLoss.energyLossType.etcone20.ptconecoreTrackPtrCorrection",
                                        "MuonClusterCollection.eta_sampl.phi_sampl",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.PartonTruthLabelID.Jvt.JVFCorr.JvtRpt.ConstituentScale",
                                        "AntiKt4EMPFlowJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.PartonTruthLabelID.Jvt.JVFCorr.JvtRpt.ConstituentScale",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "BTagging_AntiKt4EMTopo.MV1_discriminant",
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta",
                                        "DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1",
                                        "NumTrkPt500PV.PartonTruthLabelID",
                                        "MuonTruthParticles.truthOrigin.truthType",
                                        "HLT_xAOD__PhotonContainer_egamma_Iso_Photons.eta.phi.pt"]

from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
HIGG1D2SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HIGG1D2SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HIGG1D2SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
HIGG1D2SlimmingHelper.ExtraVariables += ["GSFTrackParticles.numberOfInnermostPixelLayerSharedHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSplitHits"]

# Add AODCellContainer (have to find how to keep only cells belonging to e/gamma objects)
# Usage of AddItem is forbidden
# HIGG1D2Stream.AddItem("CaloCellContainer#AODCellContainer")

HIGG1D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG1D2SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D2SlimmingHelper.AppendContentToStream(HIGG1D2Stream)
