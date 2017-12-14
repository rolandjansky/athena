#********************************************************************
# EGAM8.py: Z->ee, one forward e
# reductionConf flag EGAM8 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM8ExtraContent import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM8Stream )
EGAM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )


#====================================================================
# SET UP SKIMMING
#====================================================================

# SELECTION FOR T&P

#====================================================================
# Z->ee selection based on single e trigger, for reco (central) and ID SF(central+fwd)
# 1 tight e, central, pT>25 GeV
# 1 forward e, pT>20 GeV
# OS+SS, mee>50 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors :
#    use medium for early data upon electron group request
    requirement_tag = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
else :
    requirement_tag = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
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
print EGAM8_ZEEMassTool


#====================================================================
# Z->mue selection based on single muon trigger, for ID SF(central+fwd) background studies
# 1 medium muon, central, pT>25 GeV
# 1 forward e, pT>20 GeV
# OS+SS, mmue>50 GeV
#====================================================================

requirement_tag_muon = 'Muons.pt>24.5*GeV && abs(Muons.eta)<2.7 && Muons.DFCommonMuonsPreselection'
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
print EGAM8_ZMuEMassTool


# Skimming criteria
expression = 'count(EGAM8_DiElectronMass > 50.0*GeV)>=1 || count(EGAM8_MuonElectronMass > 50.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM8_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM8_SkimmingTool",
                                                                   expression = expression)
ToolSvc += EGAM8_SkimmingTool
print "EGAM8 skimming tool:", EGAM8_SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM8_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM8_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM8_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================                                                                              
# Max Cell sum decoration tool
#====================================================================                                                        

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM8_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM8_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM8_MaxCellDecoratorTool


#====================================================================
# SET UP THINNING
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM8ThinningHelper = ThinningHelper( "EGAM8ThinningHelper" )
EGAM8ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))|HLT_e.*_Zee.*'
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly
EGAM8ThinningHelper.AppendToStream( EGAM8Stream, ExtraContainersTrigger )

thinningTools=[]

# Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EGAM8JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM8JetLCTPThinningTool",
                                                                          ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += EGAM8JetLCTPThinningTool
print EGAM8JetLCTPThinningTool
#thinningTools.append(EGAM8JetLCTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EGAM8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM8MuonTPThinningTool",
                                                                          ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM8MuonTPThinningTool
print EGAM8MuonTPThinningTool
#thinningTools.append(EGAM8MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM8ElectronTPThinningTool",
                                                                                ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM8ElectronTPThinningTool
print EGAM8ElectronTPThinningTool
#thinningTools.append(EGAM8ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM8PhotonTPThinningTool",
                                                                              ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM8PhotonTPThinningTool
print EGAM8PhotonTPThinningTool
#thinningTools.append(EGAM8PhotonTPThinningTool)

# Tracks associated with Taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EGAM8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM8TauTPThinningTool",
                                                                        ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        ConeSize                = 0.6,
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM8TauTPThinningTool
print EGAM8TauTPThinningTool
#thinningTools.append(EGAM8TauTPThinningTool)

# Tracks from primary vertex
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EGAM8TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM8TPThinningTool",
                                                                  ThinningService         = EGAM8ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM8TPThinningTool
print EGAM8TPThinningTool
#thinningTools.append(EGAM8TPThinningTool)



# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EGAM8TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM8TruthThinningTool",
                                                                   ThinningService         = "EGAM8ThinningSvc",
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "EGAM8 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += EGAM8TruthThinningTool
    thinningTools.append(EGAM8TruthThinningTool)
print "EGAM8 thinningTools: ", thinningTools


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam8Seq = CfgMgr.AthSequencer("EGAM8Sequence")
DerivationFrameworkJob += egam8Seq



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam8Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM8Kernel",
                                                         AugmentationTools = [EGAM8_ZEEMassTool, EGAM8_ZMuEMassTool, EGAM8_GainDecoratorTool, EGAM8_MaxCellDecoratorTool] + EGAM8_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM8_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )


#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam8Seq,"EGAM8")


#====================================================================
# SET UP STREAM SELECTION
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM8Stream.AcceptAlgs(["EGAM8Kernel"])


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM8SlimmingHelper = SlimmingHelper("EGAM8SlimmingHelper")

EGAM8SlimmingHelper.SmartCollections = [
                                        "Electrons",
					"Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"
                                        ]

# Add egamma trigger objects
EGAM8SlimmingHelper.IncludeEGammaTriggerContent = True
# Add muon trigger objects
EGAM8SlimmingHelper.IncludeMuonTriggerContent = True

# Extra variables
EGAM8SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM8SlimmingHelper.AllVariables = ExtraContainersElectrons

if globalflags.DataSource()=='geant4':
    EGAM8SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM8SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM8_ClusterEnergyPerLayerDecorators:
    EGAM8SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM8SlimmingHelper
EGAM8SlimmingHelper.AppendContentToStream(EGAM8Stream)
