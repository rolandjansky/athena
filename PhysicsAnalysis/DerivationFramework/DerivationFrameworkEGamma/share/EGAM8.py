#********************************************************************
# EGAM8.py
# Z->ee reduction for forward e T&P
# reductionConf flag EGAM8 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM8ExtraContent import *


#====================================================================
# read common DFEGamma settings from egammaDFFlags
#====================================================================
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")

# this could also go in egammaDFFlags
RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False


#====================================================================
# check if we run on data or MC (DataSource = geant4)
#====================================================================
from AthenaCommon.GlobalFlags import globalflags
print("EGAM8 globalflags.DataSource(): ", globalflags.DataSource())
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly


#====================================================================
# Set up sequence for this format and add to the top sequence
#====================================================================
EGAM8Sequence = CfgMgr.AthSequencer("EGAM8Sequence")
DerivationFrameworkJob += EGAM8Sequence


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM8Stream )
EGAM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM8Stream.AcceptAlgs(["EGAM8Kernel"])


### Thinning and augmentation tools lists
augmentationTools = []
thinningTools=[]


#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# 1. di-electron invariant mass for events passing the Z->ee
#    selection based on single e trigger, for reco (central) and ID
#    SF(central+fwd)
#
#    1 medium e, central, pT>25 GeV
#    1 forward e, pT>20 GeV
#    OS+SS, mee>50 GeV
#====================================================================

if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
else :
    requirement_tag = '(Electrons.LHMedium) && Electrons.pt > 24.5*GeV'
requirement_probe = 'ForwardElectrons.pt > 19.5*GeV'
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM8_ZEEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM8_ZEEMassTool",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM8_DiElectronMass",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "ForwardElectrons",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM8_ZEEMassTool
augmentationTools += [EGAM8_ZEEMassTool]
print(EGAM8_ZEEMassTool)


#====================================================================
# 2. mue invariant amss for events passing the Z->mue selection based
#    on single muon trigger, for ID SF(central+fwd) background studies
#
#    1 medium muon, central, pT>25 GeV
#    1 forward e, pT>20 GeV
#    OS+SS, mmue>50 GeV
#====================================================================

requirement_tag_muon = 'Muons.pt>24.5*GeV && abs(Muons.eta)<2.7 && Muons.DFCommonMuonPassPreselection'
#requirement_probe = 'ForwardElectrons.pt > 19.5*GeV'
EGAM8_ZMuEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM8_ZMuEMassTool",
                                                               Object1Requirements = requirement_tag_muon,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM8_MuonElectronMass",
                                                               Mass1Hypothesis = 105*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Muons",
                                                               Container2Name = "ForwardElectrons",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM8_ZMuEMassTool
augmentationTools += [EGAM8_ZMuEMassTool]
print(EGAM8_ZMuEMassTool)


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM8_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM8_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons")
ToolSvc += EGAM8_MaxCellDecoratorTool
augmentationTools += [EGAM8_MaxCellDecoratorTool]


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM8_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM8_GainDecoratorTool
augmentationTools += [EGAM8_GainDecoratorTool]

cluster_sizes = (3,7), (5,5), (7,11)
EGAM8_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
augmentationTools += EGAM8_ClusterEnergyPerLayerDecorators


#====================================================================
# SET UP THINNING
#====================================================================

print('WARNING, Thinning of trigger navigation has to be properly implemented in R22')
#from DerivationFrameworkCore.ThinningHelper import ThinningHelper
#EGAM8ThinningHelper = ThinningHelper( "EGAM8ThinningHelper" )
#EGAM8ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))|HLT_e.*_Zee.*'
#EGAM8ThinningHelper.AppendToStream( EGAM8Stream, ExtraContainersTrigger )

# Track thinning
if jobproperties.egammaDFFlags.doEGammaDAODTrackThinning:

    TrackThinningKeepElectronTracks = True
    TrackThinningKeepPhotonTracks = True
    TrackThinningKeepJetTracks = False
    TrackThinningKeepMuonTracks = True
    TrackThinningKeepTauTracks = False
    TrackThinningKeepPVTracks = False

    # Tracks associated with Electrons
    if (TrackThinningKeepElectronTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM8ElectronTPThinningTool",
                                                                                        StreamName              = streamName,
                                                                                        SGKey                   = "Electrons",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 0*GeV",
                                                                                        BestMatchOnly = True,
                                                                                        ConeSize = 0.3)
        ToolSvc += EGAM8ElectronTPThinningTool
        print(EGAM8ElectronTPThinningTool)
        thinningTools.append(EGAM8ElectronTPThinningTool)
        
    # Tracks associated with Photons
    if (TrackThinningKeepPhotonTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM8PhotonTPThinningTool",
                                                                                      StreamName              = streamName,
                                                                                      SGKey                   = "Photons",
                                                                                      GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      SelectionString         = "Photons.pt > 0*GeV",
                                                                                      BestMatchOnly = True,
                                                                                      ConeSize = 0.3)
        
        ToolSvc += EGAM8PhotonTPThinningTool
        print(EGAM8PhotonTPThinningTool)
        thinningTools.append(EGAM8PhotonTPThinningTool)
        
    # Tracks associated with Jets
    if (TrackThinningKeepJetTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
        EGAM8JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM8JetTPThinningTool",
                                                                                StreamName              = streamName,
                                                                                JetKey                  = "AntiKt4EMPFlowJets",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM8JetTPThinningTool
        print(EGAM8JetTPThinningTool)
        thinningTools.append(EGAM8JetTPThinningTool)

    # Tracks associated with Muons
    if (TrackThinningKeepMuonTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
        EGAM8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM8MuonTPThinningTool",
                                                                                  StreamName              = streamName,
                                                                                  MuonKey                 = "Muons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM8MuonTPThinningTool
        print(EGAM8MuonTPThinningTool)
        thinningTools.append(EGAM8MuonTPThinningTool)

    # Tracks associated with Taus
    if (TrackThinningKeepTauTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
        EGAM8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM8TauTPThinningTool",
                                                                                StreamName              = streamName,
                                                                                TauKey                  = "TauJets",
                                                                                ConeSize                = 0.6,
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM8TauTPThinningTool
        print(EGAM8TauTPThinningTool)
        thinningTools.append(EGAM8TauTPThinningTool)

    # Tracks from primary vertex
    thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
    if (TrackThinningKeepPVTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
        EGAM8TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM8TPThinningTool",
                                                                          StreamName              = streamName,
                                                                          SelectionString         = thinning_expression,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM8TPThinningTool
        print(EGAM8TPThinningTool)
        thinningTools.append(EGAM8TPThinningTool)


# Truth thinning
if globalflags.DataSource()=='geant4':
    truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
    truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
    truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    EGAM8TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM8TruthThinningTool",
                                                                       StreamName              = streamName,
                                                                       ParticleSelectionString = truth_expression,
                                                                       PreserveDescendants     = False,
                                                                       PreserveGeneratorDescendants     = True,
                                                                       PreserveAncestors      = True)
    
    
    ToolSvc += EGAM8TruthThinningTool
    thinningTools.append(EGAM8TruthThinningTool)


#====================================================================
# Setup the skimming criteria
#====================================================================
expression = 'count(EGAM8_DiElectronMass > 50.0*GeV)>=1 || count(EGAM8_MuonElectronMass > 50.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM8_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM8_SkimmingTool",
																  expression = expression)
ToolSvc += EGAM8_SkimmingTool
print("EGAM8 skimming tool:", EGAM8_SkimmingTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
print("EGAM8 skimming tools: ", [EGAM8_SkimmingTool])
print("EGAM8 thinning tools: ", thinningTools)
print("EGAM8 augmentation tools: ", augmentationTools)
EGAM8Sequence += CfgMgr.DerivationFramework__DerivationKernel("EGAM8Kernel",
                                                              AugmentationTools = augmentationTools,
                                                              SkimmingTools = [EGAM8_SkimmingTool],
                                                              ThinningTools = thinningTools
                                                              )


#====================================================================
# JET/MET
#====================================================================
from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
from JetRecConfig.StandardSmallRJets import AntiKt4Truth
addDAODJets([AntiKt4Truth], EGAM8Sequence)


#========================================
# ENERGY DENSITY
#========================================
if (DerivationFrameworkIsMonteCarlo):
    # Schedule the two energy density tools for running after the pseudojets are created.
    for alg in ['EDTruthCentralAlg', 'EDTruthForwardAlg']:
        if hasattr(topSequence, alg):
            edtalg = getattr(topSequence, alg)
            delattr(topSequence, alg)
            EGAM8Sequence += edtalg


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM8SlimmingHelper = SlimmingHelper("EGAM8SlimmingHelper")

EGAM8SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Baseline_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM8SlimmingHelper.IncludeEGammaTriggerContent = True
# Add muon trigger objects
EGAM8SlimmingHelper.IncludeMuonTriggerContent = True

# Extra variables
EGAM8SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM8SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM8SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM8SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM8SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM8SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM8_ClusterEnergyPerLayerDecorators:
    EGAM8SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# Add event info
if jobproperties.egammaDFFlags.doEGammaEventInfoSlimming:
    EGAM8SlimmingHelper.SmartCollections.append("EventInfo")
else:
    EGAM8SlimmingHelper.AllVariables += ["EventInfo"]

# Add detailed shower shape variables
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
EGAM8SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EGAM8SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
EGAM8SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# This line must come after we have finished configuring EGAM8SlimmingHelper
EGAM8SlimmingHelper.AppendContentToStream(EGAM8Stream)
