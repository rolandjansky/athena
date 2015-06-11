#********************************************************************
# EGAM6.py
# reductionConf flag EGAM6 in Reco_tf.py
# identical to EGAM1 but with loose electron cuts for start of run with poor ID/ECAL alignment
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
# OS, mee>60 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)

if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 24.5*GeV'
    requirement_probe = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'
else :
    requirement_tag = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && DFCommonElectrons_pt > 24.5*GeV'
    requirement_probe = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'
#requirement_tag = '(DFCommonElectronsLHTight) && DFCommonElectrons_pt > 24.5*GeV'
#requirement_probe = '(DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM6_ZEEMassTool1 = DerivationFramework__EGInvariantMassTool( name = "EGAM6_ZEEMassTool1",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM6_DiElectronMass",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               Pt1BranchName = "DFCommonElectrons_pt",
                                                               Eta1BranchName = "DFCommonElectrons_eta",
                                                               Phi1BranchName = "DFCommonElectrons_phi",
                                                               Pt2BranchName = "DFCommonElectrons_pt",
                                                               Eta2BranchName = "DFCommonElectrons_eta",
                                                               Phi2BranchName = "DFCommonElectrons_phi",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM6_ZEEMassTool1
print EGAM6_ZEEMassTool1

#====================================================================
# Z->ee selection based on di-electron trigger
# 2 medium e, central, pT>20 GeV
# OS, mee>60 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors:
    requirement = '(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'
else:
    requirement = '(Electrons.Loose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'
#requirement = '(DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 19.5*GeV'
EGAM6_ZEEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM6_ZEEMassTool2",
                                                               Object1Requirements = requirement,
                                                               Object2Requirements = requirement,
                                                               StoreGateEntryName = "EGAM6_DiElectronMass2",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               Pt1BranchName = "DFCommonElectrons_pt",
                                                               Eta1BranchName = "DFCommonElectrons_eta",
                                                               Phi1BranchName = "DFCommonElectrons_phi",
                                                               Pt2BranchName = "DFCommonElectrons_pt",
                                                               Eta2BranchName = "DFCommonElectrons_eta",
                                                               Phi2BranchName = "DFCommonElectrons_phi",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM6_ZEEMassTool2
print EGAM6_ZEEMassTool2


# SELECTION FOR T&P

#====================================================================
# Z->ee selection based on single e trigger, for reco (central) and ID SF(central+fwd)
# 1 tight e, central, pT>25 GeV
# 1 e, pT>15 GeV if central, >20 GeV if forward
# OS+SS, mee>60 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 24.5*GeV'
else :
    requirement_tag = '(Electrons.Loose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 24.5*GeV'

# central electrons: collection = Electrons, pt>14.5 GeV
requirement_probe = 'DFCommonElectrons_pt > 14.5*GeV'
EGAM6_ZEEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM6_ZEEMassTool3",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM6_DiElectronMass3",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               Pt1BranchName = "DFCommonElectrons_pt",
                                                               Eta1BranchName = "DFCommonElectrons_eta",
                                                               Phi1BranchName = "DFCommonElectrons_phi",
                                                               Pt2BranchName = "DFCommonElectrons_pt",
                                                               Eta2BranchName = "DFCommonElectrons_eta",
                                                               Phi2BranchName = "DFCommonElectrons_phi",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM6_ZEEMassTool3
print EGAM6_ZEEMassTool3

# forward electrons: collection = ForwardElectrons, pt>19.5 GeV
requirement_probe = 'ForwardElectrons.pt > 19.5*GeV'
EGAM6_ZEEMassTool4 = DerivationFramework__EGInvariantMassTool( name = "EGAM6_ZEEMassTool4",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM6_DiElectronMass4",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "ForwardElectrons",
                                                               Pt1BranchName = "DFCommonElectrons_pt",
                                                               Eta1BranchName = "DFCommonElectrons_eta",
                                                               Phi1BranchName = "DFCommonElectrons_phi",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM6_ZEEMassTool4
print EGAM6_ZEEMassTool4


#====================================================================
# Z->eg selection based on single e trigger, for reco SF (central)
# 1 tight e, central, pT>25 GeV
# 1 gamma, pT>15 GeV, central
# OS+SS, mee>60 GeV
#====================================================================

# switch to likelihood selectors only as soon as they're commissioned (and used in trigger)
if RecomputeElectronSelectors:
    requirement_tag = '(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 24.5*GeV'
else:
    requirement_tag = '(Electrons.Loose || Electrons.DFCommonElectronsLHLoose || Electrons.DFCommonElectronsLHMedium) && DFCommonElectrons_pt > 24.5*GeV'
#requirement_tag = '(DFCommonElectronsLHLoose) && DFCommonElectrons_pt > 24.5*GeV'
requirement_probe = 'DFCommonPhotons_et > 14.5*GeV'
EGAM6_ZEGMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM6_ZEGMassTool",
                                                              Object1Requirements = requirement_tag,
                                                              Object2Requirements = requirement_probe,
                                                              StoreGateEntryName = "EGAM6_ElectronPhotonMass",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "Photons",
                                                              Pt1BranchName = "DFCommonElectrons_pt",
                                                              Eta1BranchName = "DFCommonElectrons_eta",
                                                              Phi1BranchName = "DFCommonElectrons_phi",
                                                              Pt2BranchName = "DFCommonPhotons_pt",
                                                              Eta2BranchName = "DFCommonPhotons_eta",
                                                              Phi2BranchName = "DFCommonPhotons_phi",
                                                              CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)
ToolSvc += EGAM6_ZEGMassTool
print EGAM6_ZEGMassTool

# Skimming criteria
#expression = '( ( count(Photons.pt > 10*GeV) > 0 ) || ( count(Electrons.pt > 10*GeV) > 0 ) )'
expression = 'count(EGAM6_DiElectronMass > 60.0*GeV)>=1 || count(EGAM6_DiElectronMass2 > 60.0*GeV)>=1 || count(EGAM6_DiElectronMass3 > 60.0*GeV)>=1 || count(EGAM6_DiElectronMass4 > 60.0*GeV)>=1 ||  count (EGAM6_ElectronPhotonMass > 60.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM6SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM6SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM6SkimmingTool
print "EGAM6 skimming tool:", EGAM6SkimmingTool

#====================================================================
# Cell sum decoration tool
#====================================================================

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CellDecorator
EGAM6_CellDecoratorTool = DerivationFramework__CellDecorator( name                    = "EGAM6_CellDecoratorTool",
                                                              SGKey_electrons         = "Electrons",
                                                              SGKey_photons           = "Photons",
                                                              CaloFillRectangularTool_5x5  = EGAMCOM_caloFillRect55,
                                                              CaloFillRectangularTool_3x5  = EGAMCOM_caloFillRect35,
                                                              CaloFillRectangularTool_3x7  = EGAMCOM_caloFillRect37,
                                                              CaloFillRectangularTool_7x11  = EGAMCOM_caloFillRect711
                                                              )
ToolSvc += EGAM6_CellDecoratorTool

#====================================================================                                                                              
# Max Cell sum decoration tool
#====================================================================                                                        

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__MaxCellDecorator
EGAM6_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM6_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons",
                                                                    )
ToolSvc += EGAM6_MaxCellDecoratorTool


#================
# THINNING
#================
thinningTools=[]

# Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EGAM6JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM6JetLCTPThinningTool",
                                                                          ThinningService         = "EGAM6ThinningSvc",
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ApplyAnd                = True)
ToolSvc += EGAM6JetLCTPThinningTool
print EGAM6JetLCTPThinningTool
#thinningTools.append(EGAM6JetLCTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EGAM6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM6MuonTPThinningTool",
                                                                          ThinningService         = "EGAM6ThinningSvc",
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM6MuonTPThinningTool
print EGAM6MuonTPThinningTool
#thinningTools.append(EGAM6MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM6ElectronTPThinningTool",
                                                                                ThinningService         = "EGAM6ThinningSvc",
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM6ElectronTPThinningTool
print EGAM6ElectronTPThinningTool
#thinningTools.append(EGAM6ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM6PhotonTPThinningTool",
                                                                              ThinningService         = "EGAM6ThinningSvc",
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM6PhotonTPThinningTool
print EGAM6PhotonTPThinningTool
#thinningTools.append(EGAM6PhotonTPThinningTool)

# Tracks associated with Taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EGAM6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM6TauTPThinningTool",
                                                                        ThinningService         = "EGAM6ThinningSvc",
                                                                        TauKey                  = "TauJets",
                                                                        ConeSize                = 0.6,
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM6TauTPThinningTool
print EGAM6TauTPThinningTool
#thinningTools.append(EGAM6TauTPThinningTool)

# Tracks from primary vertex
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EGAM6TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM6TPThinningTool",
                                                                  ThinningService         = "EGAM6ThinningSvc",
                                                                  SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EGAM6TPThinningTool
print EGAM6TPThinningTool
#thinningTools.append(EGAM6TPThinningTool)



# Truth thinning
truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EGAM6TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM6TruthThinningTool",
                                                                   ThinningService         = "EGAM6ThinningSvc",
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = False,
                                                                   PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "EGAM6 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()=='geant4':
    ToolSvc += EGAM6TruthThinningTool
    thinningTools.append(EGAM6TruthThinningTool)
print "EGAM6 thinningTools: ", thinningTools


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAM6Kernel",
                                                                       AugmentationTools = [EGAM6_ZEEMassTool1, EGAM6_ZEEMassTool2, EGAM6_ZEEMassTool3, EGAM6_ZEEMassTool4, EGAM6_ZEGMassTool, EGAM6_CellDecoratorTool, EGAM6_MaxCellDecoratorTool],
                                                                       SkimmingTools = [EGAM6SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                       )


#============ Create Derivation EGAM6 cell collection ==================

# Keep only calo cells associated with the egammaClusters collection
from DerivationFrameworkEGamma.CaloCellDFGetter import CaloCellDFGetter
theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
                                       outputCellKey="DFEGAM6CellContainer")

#========================================================================


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM6Stream )
EGAM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM6Stream.AcceptAlgs(["EGAM6Kernel"])


#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EGAM6ThinningSvc", outStreams=[evtStream] )

#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM6SlimmingHelper = SlimmingHelper("EGAM6SlimmingHelper")

EGAM6SlimmingHelper.SmartCollections = [
				        #"Electrons",
					"Photons",
					"Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"
                                        ]

# read list of extra content from EGAM1 file (output of EGAM6 and EGAM1 is the same)
from DerivationFrameworkEGamma.EGAM1ExtraContent import *
EGAM6SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM6SlimmingHelper.AllVariables = ExtraContainersElectrons

if globalflags.DataSource()=='geant4':
    EGAM6SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM6SlimmingHelper.AllVariables += ExtraContainersTruth
#    EGAM6SlimmingHelper.StaticContent = ["TruthIsoCentralEventShape","TruthIsoForwardEventShape"]

EGAM6SlimmingHelper.AppendContentToStream(EGAM6Stream)

# Add EventShape info
#EGAM6Stream.AddItem("xAOD::EventShape#*")
#EGAM6Stream.AddItem("xAOD::EventShapeAuxInfo#*")

#if globalflags.DataSource()=='geant4':
#    EGAM6SlimmingHelper.StaticContent = ["TruthIsoCentralEventShape","TruthIsoForwardEventShape"]
#    EGAM6Stream.AddItem("TruthIsoCentralEventShape#*")
#    EGAM6Stream.AddItem("TruthIsoForwardEventShape#*")

# Add MET_RefFinalFix
# JRC: COMMENTED TEMPORARILY
#addMETOutputs(EGAM6Stream)

# Add AODCellContainer (thinned)
EGAM6Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
EGAM6Stream.AddItem("CaloCellContainer#DFEGAM6CellContainer")

# Add egamma trigger objects
EGAM6Stream.IncludeEGammaTriggerContent = True
