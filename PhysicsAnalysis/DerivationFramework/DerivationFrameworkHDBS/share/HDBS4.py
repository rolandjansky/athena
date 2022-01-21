#====================================================================
# DAOD_HDBS4.py
# This defines DAOD_HDBS4, a skimmed DAOD format for Run 3.
# It contains the variables and objects needed for the HZa Z->2l, 
# a->2trk physics analysis in ATLAS.
# It requires the reductionConf flag HDBS4 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkJob, DerivationFrameworkHasTruth
from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from AthenaCommon import CfgMgr 
from AthenaCommon.AppMgr import ToolSvc
from DerivationFrameworkInDet.InDetCommon import DFCommonTrackSelection
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HDBS4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HDBS4Stream )
HDBS4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HDBS4Stream.AcceptAlgs(["HDBS4VertexToolKernel"]) 

### Thinning and augmentation tools lists
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HDBS4ThinningHelper = ThinningHelper( "HDBS4ThinningHelper" )
HDBS4ThinningHelper.AppendToStream( HDBS4Stream )
thinningTools       = []
AugmentationTools   = []

# Special sequence 
SeqPHYS = CfgMgr.AthSequencer("SeqPHYS")

#====================================================================
# Truth collections
#====================================================================
if DerivationFrameworkHasTruth:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   import DerivationFrameworkHiggs.TruthCategories
   # Add HF particles
   addHFAndDownstreamParticles(SeqPHYS)
   # Add standard truth
   addStandardTruthContents(SeqPHYS,prefix='PHYS_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   HDBS4TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="HDBS4TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthHFWithDecayParticles"])
   ToolSvc += HDBS4TruthNavigationDecorator
   SeqPHYS.PHYS_MCTruthNavigationDecoratorKernel.AugmentationTools = [HDBS4TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqPHYS)
   addPVCollection(SeqPHYS)

#====================================================================
# TRIGGER CONTENT   
#====================================================================
# See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
# Get single and multi mu, e, photon triggers
# Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
# Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
# Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)

# Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))

# Now reduce the list...
from RecExConfig.InputFilePeeker import inputFileSummary
trigger_names_notau = []
trigger_names_tau = []
for trig_item in inputFileSummary['metadata']['/TRIGGER/HLT/Menu']:
    if not 'ChainName' in trig_item: continue
    if trig_item['ChainName'] in trigger_names_full_notau: trigger_names_notau += [ trig_item['ChainName'] ]
    if trig_item['ChainName'] in trigger_names_full_tau:   trigger_names_tau   += [ trig_item['ChainName'] ]

# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='PHYSTriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='PHYSTriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Special sequence for vertex tool
SeqHDBS4VertexTool = CfgMgr.AthSequencer("SeqHDBS4VertexTool")

#### Inner detector group recommendations for indet tracks in analysis
HDBS4_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && (InDetTrackParticles.pt > 0.5*GeV)" 
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HDBS4TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HDBS4TrackParticleThinningTool",
                                                                           ThinningService         = HDBS4ThinningHelper.ThinningSvc(),
                                                                           SelectionString         = HDBS4_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = True) 

ToolSvc += HDBS4TrackParticleThinningTool
thinningTools.append(HDBS4TrackParticleThinningTool)

#### Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HDBS4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HDBS4MuonTPThinningTool",
                                                                           ThinningService        = HDBS4ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS4MuonTPThinningTool
thinningTools.append(HDBS4MuonTPThinningTool)

####Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HDBS4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HDBS4ElectronTPThinningTool",
                                                                                 ThinningService        = HDBS4ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HDBS4ElectronTPThinningTool
thinningTools.append(HDBS4ElectronTPThinningTool)

#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job, but the tau additions
# below make it such that we need it here
DerivationFrameworkJob += SeqPHYS

#====================================================================
# SKIMMING 
#====================================================================
# COMA integrated luminosities in parentheses
Trigger2L=[
"HLT_mu20_iloose_L1MU15", 
"HLT_mu40",
"HLT_mu26_ivarmedium", #(149,548,085 nb-1)
"HLT_mu50", #(153,445,525 nb-1)
"HLT_mu60_0eta105_msonly", #(153,309,492 nb-1)
"HLT_2mu14", #(152,811,768 nb-1 )
"HLT_mu22_mu8noL1", #(153,249,171 nb-1)
"HLT_mu22_mu8noL1_calotag_0eta010", #(49,341,412 nb-1)
"HLT_e24_lhmedium_L1EM20VH",
"HLT_e26_lhtight_nod0_ivarloose", #(150,196,728 nb-1)
"HLT_e26_lhtight_nod0", #(63,087,620 nb-1)
"HLT_e60_lhmedium_nod0", #(154,256,313 nb-1)
"HLT_e120_lhloose",
"HLT_e140_lhloose_nod0", #(154,249,178 nb-1)
"HLT_e300_etcut", #(150,205,568 nb-1)
"HLT_2e17_lhvloose_nod0_L12EM15VHI", #(111,647,684 nb-1) [2017-2018]
"HLT_2e24_lhvloose_nod0", #(112,460,241 nb-1)[2017-2018]
"HLT_2e15_lhvloose_nod0_L12EM13VH", # (17,770,345 nb-1 ) [2015-2016]
"HLT_2e17_lhvloose_nod0" #(44,207,014 nb-1) mostly 2015-2016 a bit later
]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HDBS4TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "HDBS4TriggerSkimmingTool",
                                                                    TriggerListOR = Trigger2L)
                                                                                                              
ToolSvc += HDBS4TriggerSkimmingTool

from DerivationFrameworkHDBS.DerivationFrameworkHDBSConf import DerivationFramework__SkimmingToolHDBS2
SelectionSkimmingTool = DerivationFramework__SkimmingToolHDBS2(name                       = "SkimmingToolHDBS2",
                                                             SkipTriggerRequirement     = True,
                                                             FilterType                 = "2L2TRK", 
                                                             NumberOfLeptons            = 2,
                                                             NumberOfElectrons          = 2,
                                                             NumberOfMuons              = 2,
                                                             NumberOfJets               = 0,
                                                             NumberOfPhotons            = 0,
                                                             NumberOfTracks             = 2,
                                                             MuonQuality                = "inMS", 
                                                             ElectronQuality            = "DFCommonElectronsLHVeryLoose", 
                                                             Trigger2L                  = Trigger2L,
                                                             MuonPtCut                  =  4.0*Units.GeV, 
                                                             LeadingMuonPtCut           = 21.0*Units.GeV, 
                                                             ElectronPtCut              =  6.0*Units.GeV,
                                                             LeadingElectronPtCut       = 25.0*Units.GeV,
                                                             TrackPtCut                 = 0.5*Units.GeV, 
                                                             InvariantMassa0LowCut      =  0.0*Units.GeV,
                                                             InvariantMassa0UpCut       =  2.0*Units.GeV, 
                                                             InvariantMassHLowCut       =  98.0*Units.GeV,
                                                             InvariantMassZLowCut       =  69.0*Units.GeV,
                                                             InvariantMassZUpCut        =  103.0*Units.GeV)
ToolSvc += SelectionSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqHDBS4VertexTool += CfgMgr.DerivationFramework__DerivationKernel("HDBS4Kernel",
                                                        SkimmingTools = [SelectionSkimmingTool, HDBS4TriggerSkimmingTool],
                                                        ThinningTools = thinningTools)

#====================================================================
# AUGMENTATION TOOLS
#====================================================================

## 1/ setup vertexing tools and services
include("JpsiUpsilonTools/configureServices.py")

HDBS4TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(
                                         name                = "HDBS4TrkVKalVrtFitter",
                                         Extrapolator        = InDetExtrapolator,
                                         FirstMeasuredPoint  = False,
                                         MakeExtendedVertex  = True)
ToolSvc += HDBS4TrkVKalVrtFitter


include("DerivationFrameworkHDBS/configure_a0Finder.py")
HDBS4_Tools = HDBSa0FinderTools("HDBS4")

#--------------------------------------------------------------------
# Configure the a0 finder
#--------------------------------------------------------------------
from DerivationFrameworkHDBS.DerivationFrameworkHDBSConf import DerivationFramework__HDBSa0Finder
HDBS4Zmumua0Finder   = DerivationFramework__HDBSa0Finder(
    OutputLevel = INFO, #DEBUG
    name = "HDBS4Zmumua0Finder",
    TrkVertexFitterTool = HDBS4TrkVKalVrtFitter,
    VertexEstimator = HDBS4_Tools.InDetSecVtxPointEstimator,
    InputTrackParticleContainerName = "InDetTrackParticles",
    HCandidateContainerName = "HDBS4HZmumuCandidates",
    TrackPtMin = 500.0,
    deltaz0PVsinthetaMax =  3.0,
    deltaz0PVsignificanceMax =  3.0,
    DitrackMassMax = 1600.0, 
    ZisMuons = True, 
    ZisElectrons = False,
    #d0significanceMax = 2.0, 
    HcandidateMassMin = 98000.0,
    Chi2cut = 50.0, 
    trkZDeltaZ = 0.5, 
    nHitPix = 1, 
    nHitSct = 0, 
    onlyTightPTrk = True,
    onlyLoosePTrk = False,
    onlyLooseTrk = False,
    ZMassMin = 70000.0,
    ZMassMax = 112000.0,
    TrackSelectorTool = InDetTrackSelectorTool,
    muonCollectionKey = "Muons", 
    MuonThresholdPt = 4.0, 
    LeptonTrackThresholdPt = 1.0, 
    ZmumuChi2Cut = 200.0,
    DitrackPtMin = 4000.0, 
    LeadingTrackPt = 2000.0, 
    DeltaPhiTracks = 0.2, 
    DeltaRTracks = 0.2,
    ChiSqProbMin = 1e-6)

ToolSvc += HDBS4Zmumua0Finder

HDBS4Zeea0Finder   = DerivationFramework__HDBSa0Finder(
    OutputLevel = INFO, #DEBUG
    name = "HDBS4Zeea0Finder",
    TrkVertexFitterTool = HDBS4TrkVKalVrtFitter, 
    VertexEstimator = HDBS4_Tools.InDetSecVtxPointEstimator,
    InputTrackParticleContainerName = "InDetTrackParticles",
    HCandidateContainerName = "HDBS4HZeeCandidates",
    TrackPtMin = 500.0, 
    deltaz0PVsinthetaMax = 3.0,
    deltaz0PVsignificanceMax = 3.0, 
    DitrackMassMax = 1600.0, 
    ZisMuons = False, 
    ZisElectrons = True,
    #d0significanceMax = 2.0, 
    HcandidateMassMin = 98000.0, 
    Chi2cut =  50.0, 
    trkZDeltaZ =  0.5,
    nHitPix = 1, 
    nHitSct = 0, 
    onlyTightPTrk = True, 
    onlyLoosePTrk = False,
    onlyLooseTrk = False,
    UseGSFTrackIndices = [0,1],
    electronCollectionKey = "Electrons", 
    TrackSelectorTool = InDetTrackSelectorTool,
    ElectronThresholdPt = 6.0, 
    LeptonTrackThresholdPt = 1.0, 
    ZMassMin = 70000.0, 
    ZMassMax = 112000.0, 
    ZeeChi2Cut = 200.0,
    DitrackPtMin = 4000.0, 
    LeadingTrackPt = 2000.0,
    DeltaPhiTracks = 0.2, 
    DeltaRTracks = 0.2,
    ChiSqProbMin = 1e-6)

ToolSvc += HDBS4Zeea0Finder

StaticContent = [] 

StaticContent += ["xAOD::VertexContainer#%s"        % "HDBS4HZmumuCandidates"]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % "HDBS4HZmumuCandidates"]
# we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % "HDBS4HZmumuCandidates"]

StaticContent += ["xAOD::VertexContainer#%s"        % "HDBS4HZeeCandidates"]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % "HDBS4HZeeCandidates"]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % "HDBS4HZeeCandidates"]

SeqHDBS4VertexTool += CfgMgr.DerivationFramework__DerivationKernel("HDBS4VertexToolKernel", AugmentationTools = [HDBS4Zmumua0Finder, HDBS4Zeea0Finder])
DerivationFrameworkJob += SeqHDBS4VertexTool

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HDBS4SlimmingHelper = SlimmingHelper("HDBS4SlimmingHelper")

HDBS4SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets"
                                      ]

HDBS4SlimmingHelper.StaticContent = StaticContent

# Trigger content
HDBS4SlimmingHelper.IncludeTriggerNavigation = True
HDBS4SlimmingHelper.IncludeJetTriggerContent = False
HDBS4SlimmingHelper.IncludeMuonTriggerContent = True
HDBS4SlimmingHelper.IncludeEGammaTriggerContent = True
HDBS4SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
HDBS4SlimmingHelper.IncludeTauTriggerContent = False
HDBS4SlimmingHelper.IncludeEtMissTriggerContent = False
HDBS4SlimmingHelper.IncludeBJetTriggerContent = False
HDBS4SlimmingHelper.IncludeBPhysTriggerContent = False
HDBS4SlimmingHelper.IncludeMinBiasTriggerContent = False

# # Add the jet containers to the stream (defined in JetCommon if import needed)
# addJetOutputs(PHYSSlimmingHelper,["PHYS"])

# Truth containers
if DerivationFrameworkHasTruth:
   HDBS4SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                            'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer'
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(HDBS4SlimmingHelper)
   HDBS4SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices']

HDBS4SlimmingHelper.ExtraVariables += [
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "InDetTrackParticles.TruthLink",    
                                      "TruthPrimaryVertices.t.x.y.z", 
                                      "PrimaryVertices.nTrackParticles",
                                      "PrimaryVertices.chiSquared", 
                                      "PrimaryVertices.numberDoF"]

# Add trigger matching
trigmatching_helper_notau.add_to_slimming(HDBS4SlimmingHelper)
trigmatching_helper_tau.add_to_slimming(HDBS4SlimmingHelper)

# Final construction of output stream
HDBS4SlimmingHelper.AppendContentToStream(HDBS4Stream)

