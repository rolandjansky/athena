#********************************************************************
# EGAM1.py
# reductionConf flag EGAM1 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

#====================================================================
# SKIMMING TOOLS
#====================================================================

# SELECTION FOR CALIBRATION

#====================================================================
# Z->ee selection based on single e trigger:
# 1 tight e, central, pT>25 GeV
# 1 medium e, pT>20 GeV
# OS, mee>50 GeV
# it would be nice if we could add a tag-bit to flag
# events passing the T&P selection, perhaps using an augmentation
# tool?
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)

if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 24.5*GeV'
    requirement_probe = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 19.5*GeV'
else :
    requirement_tag = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && Electrons.pt > 24.5*GeV'
    requirement_probe = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 19.5*GeV'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM1_ZEEMassTool1 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool1",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool1
print EGAM1_ZEEMassTool1

#====================================================================
# Z->ee selection based on di-electron trigger
# 2 medium e, central, pT>20 GeV
# OS, mee>50 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors:
    requirement = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 19.5*GeV'
else:
    requirement = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 19.5*GeV'
EGAM1_ZEEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool2",
                                                               Object1Requirements = requirement,
                                                               Object2Requirements = requirement,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass2",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool2
print EGAM1_ZEEMassTool2


# SELECTION FOR T&P

#====================================================================
# Z->ee selection based on single e trigger, for reco (central) and ID SF(central+fwd)
# 1 tight e, central, pT>25 GeV
# 1 e, central, pT>15 GeV
# OS+SS, mee>50 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors :
#    use medium for early data upon electron group request
    requirement_tag = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
else :
    requirement_tag = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
# central electrons: collection = Electrons, pt>14.5 GeV
requirement_probe = 'Electrons.pt > 6.5*GeV'
EGAM1_ZEEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool3",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass3",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool3
print EGAM1_ZEEMassTool3



#====================================================================
# Z->eg selection based on single e trigger, for reco SF (central)
# 1 tight e, central, pT>25 GeV
# 1 gamma, pT>15 GeV, central
# OS+SS, mee>50 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors:
#    use medium for early data upon electron group request
    requirement_tag = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
else:
    requirement_tag = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && Electrons.pt > 24.5*GeV'
requirement_probe = 'DFCommonPhotons_et > 14.5*GeV'
EGAM1_ZEGMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEGMassTool",
                                                              Object1Requirements = requirement_tag,
                                                              Object2Requirements = requirement_probe,
                                                              StoreGateEntryName = "EGAM1_ElectronPhotonMass",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "Photons",
                                                              Pt2BranchName = "DFCommonPhotons_pt",
                                                              Eta2BranchName = "DFCommonPhotons_eta",
                                                              Phi2BranchName = "DFCommonPhotons_phi",
                                                              CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEGMassTool
print EGAM1_ZEGMassTool

# Skimming criteria
#expression = '( ( count(Photons.pt > 10*GeV) > 0 ) || ( count(Electrons.pt > 10*GeV) > 0 ) )'
expression = 'count(EGAM1_DiElectronMass > 50.0*GeV)>=1 || count(EGAM1_DiElectronMass2 > 50.0*GeV)>=1 || count(EGAM1_DiElectronMass3 > 50.0*GeV)>=1 ||  count (EGAM1_ElectronPhotonMass > 50.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM1SkimmingTool",
                                                                   expression = expression)
ToolSvc += EGAM1SkimmingTool
print "EGAM1 skimming tool:", EGAM1SkimmingTool


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM1_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM1_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM1_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM1_MaxCellDecoratorTool


#================
# THINNING
#================
thinningTools=[]

# Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EGAM1JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM1JetLCTPThinningTool",
                                                                          ThinningService         = "EGAM1ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += EGAM1JetLCTPThinningTool
print EGAM1JetLCTPThinningTool
#thinningTools.append(EGAM1JetLCTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EGAM1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM1MuonTPThinningTool",
                                                                          ThinningService         = "EGAM1ThinningSvc",
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM1MuonTPThinningTool
print EGAM1MuonTPThinningTool
#thinningTools.append(EGAM1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM1ElectronTPThinningTool",
                                                                                ThinningService         = "EGAM1ThinningSvc",
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM1ElectronTPThinningTool
print EGAM1ElectronTPThinningTool
#thinningTools.append(EGAM1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM1PhotonTPThinningTool",
                                                                              ThinningService         = "EGAM1ThinningSvc",
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM1PhotonTPThinningTool
print EGAM1PhotonTPThinningTool
#thinningTools.append(EGAM1PhotonTPThinningTool)

# Tracks associated with Taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EGAM1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM1TauTPThinningTool",
                                                                        ThinningService         = "EGAM1ThinningSvc",
                                                                        TauKey                  = "TauJets",
                                                                        ConeSize                = 0.6,
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM1TauTPThinningTool
print EGAM1TauTPThinningTool
#thinningTools.append(EGAM1TauTPThinningTool)

# Tracks from primary vertex
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EGAM1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM1TPThinningTool",
                                                                  ThinningService         = "EGAM1ThinningSvc",
                                                                  SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM1TPThinningTool
print EGAM1TPThinningTool
#thinningTools.append(EGAM1TPThinningTool)



# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EGAM1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM1TruthThinningTool",
                                                                   ThinningService         = "EGAM1ThinningSvc",
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "EGAM1 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += EGAM1TruthThinningTool
    thinningTools.append(EGAM1TruthThinningTool)
print "EGAM1 thinningTools: ", thinningTools


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAM1Kernel",
                                                                       AugmentationTools = [EGAM1_ZEEMassTool1, EGAM1_ZEEMassTool2, EGAM1_ZEEMassTool3, EGAM1_ZEGMassTool, EGAM1_GainDecoratorTool, EGAM1_MaxCellDecoratorTool] + EGAM1_ClusterEnergyPerLayerDecorators,
                                                                       SkimmingTools = [EGAM1SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                       )

#=======================================
# ADD NON-PROMPT LEPTON VETO ALGORITHMS 
#=======================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config
DerivationFrameworkJob += Config.DecoratePromptLepton("Electrons", "AntiKt4PV0TrackJets") 
DerivationFrameworkJob += Config.DecoratePromptLepton("Muons", "AntiKt4PV0TrackJets")


#========================================================================


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM1Stream )
EGAM1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM1Stream.AcceptAlgs(["EGAM1Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EGAM1ThinningSvc", outStreams=[evtStream] )


#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM1SlimmingHelper = SlimmingHelper("EGAM1SlimmingHelper")

from DerivationFrameworkEGamma.EGAM1ExtraContent import *
EGAM1SlimmingHelper.SmartCollections = [
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
EGAM1SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM1SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM1SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM1SlimmingHelper.AllVariables += ExtraContainersTrigger
if globalflags.DataSource()!='geant4':
    EGAM1SlimmingHelper.AllVariables += ExtraContainersTriggerDataOnly

if globalflags.DataSource()=='geant4':
    EGAM1SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM1SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM1_ClusterEnergyPerLayerDecorators:
    EGAM1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM1SlimmingHelper
EGAM1SlimmingHelper.AppendContentToStream(EGAM1Stream)

# Add MET_RefFinalFix
# JRC: COMMENTED TEMPORARILY
#addMETOutputs(EGAM1Stream)

# Add Derived Egamma CellContainer
from DerivationFrameworkEGamma.EGammaCellCommon import CellCommonThinning
CellCommonThinning(EGAM1Stream)
