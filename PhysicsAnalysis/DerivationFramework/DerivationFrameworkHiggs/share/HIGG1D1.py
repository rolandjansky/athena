#====================================================================
# HIGG1D1.py
# This requires the reductionConf flag HIGG1D1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

#====================================================================
# SKIMMING TOOLS 
#====================================================================

print "HIGG1D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement
TriggerExp = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerExp               = ["EF_g35_loose_g25_loose"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerExp               = ["HLT_g35_medium_g25_medium","HLT_2g20_tight"]
print "HIGG1D1.py TriggerExp", TriggerExp

#KeepTrigger=not SkipTriggerRequirement
KeepTrigger=False

print "HIGG1D1.py KeepTrigger", KeepTrigger

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
SkimmingToolHIGG1D1 = DerivationFramework__SkimmingToolHIGG1(
                                 name = "SkimmingToolHIGG1D1",
                                 RequireGRL = True,
                                 ReqireLArError = True,
                                 RequireTrigger = KeepTrigger,
                                 RequirePreselection = True,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 IncludeSingleElectronPreselection = True,
                                 IncludeDoubleElectronPreselection = True,
                                 IncludeSingleMuonPreselection = True)

ToolSvc += SkimmingToolHIGG1D1
print SkimmingToolHIGG1D1

#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D1MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1MetTPThinningTool",
                                                                       ThinningService         = "HIGG1D1ThinningSvc",
                                                                       SelectionString         = thinning_expression,
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       ApplyAnd                = True)
ToolSvc += HIGG1D1MetTPThinningTool
print HIGG1D1MetTPThinningTool
#thinningTools.append(HIGG1D1MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetLCTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetLCTPThinningTool
print HIGG1D1JetLCTPThinningTool
#thinningTools.append(HIGG1D1JetLCTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D1JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D1JetEMTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D1JetEMTPThinningTool
print HIGG1D1JetEMTPThinningTool
#thinningTools.append(HIGG1D1JetEMTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "HIGG1D1MuonTPThinningTool",
                                                                            ThinningService         = "HIGG1D1ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1MuonTPThinningTool
print HIGG1D1MuonTPThinningTool
#thinningTools.append(HIGG1D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1ElectronTPThinningTool",
                                                                                  ThinningService         = "HIGG1D1ThinningSvc",
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1ElectronTPThinningTool
print HIGG1D1ElectronTPThinningTool
#thinningTools.append(HIGG1D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D1PhotonTPThinningTool",
                                                                                  ThinningService         = "HIGG1D1ThinningSvc",
                                                                                  SGKey                   = "Photons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1PhotonTPThinningTool
print HIGG1D1PhotonTPThinningTool
#thinningTools.append(HIGG1D1PhotonTPThinningTool)

# Tracks themselves
HIGG1D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D1TPThinningTool",
                                                                    ThinningService         = "HIGG1D1ThinningSvc",
                                                                    SelectionString             = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1TPThinningTool
print HIGG1D1TPThinningTool
#thinningTools.append(HIGG1D1TPThinningTool)

# Truth thinning
truth_cond_1    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3  = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D1TruthThinningTool", 
                                                                      ThinningService         = "HIGG1D1ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG1D1.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D1TruthThinningTool
    thinningTools.append(HIGG1D1TruthThinningTool)
print "HIGG1D1.py thinningTools", thinningTools


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HIGG1D1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "HIGG1D1_MaxCellDecoratorTool",
                                                                      SGKey_electrons = "Electrons",
                                                                      SGKey_photons   = "Photons"
                                                                      )
ToolSvc += HIGG1D1_MaxCellDecoratorTool

#====================================================================
# Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
HIGG1D1_GainDecoratorTool = GainDecorator()

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D1Kernel",
                                                                        SkimmingTools = [SkimmingToolHIGG1D1],
                                                                        AugmentationTools = [HIGG1D1_MaxCellDecoratorTool,HIGG1D1_GainDecoratorTool],
                                                                        ThinningTools = thinningTools
                                                                      )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D1Stream )
HIGG1D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D1Stream.AcceptAlgs(["HIGG1D1Kernel"])

## Do not use this variable at the derivation stage
#HIGG1D1Stream.AddItem("std::vector<int>#leadingV")

# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG1D1ThinningSvc", outStreams=[evtStream] )

 #====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper")


HIGG1D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D1SlimmingHelper.AllVariables = ["Electrons","Photons","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","PrimaryVertices","MET_Truth", "egammaTruthParticles","CaloCalTopoClusters"]

HIGG1D1SlimmingHelper.ExtraVariables = ["Muons.quality.EnergyLoss.energyLossType",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "BTagging_AntiKt4EMTopo.MV1_discriminant",                                        
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "MuonTruthParticles.truthOrigin.truthType",
                                        "Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z",
                                        "Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]

HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(HIGG1D1_GainDecoratorTool) )

#addMETOutputs(HIGG1D1SlimmingHelper)

HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D1SlimmingHelper.AppendContentToStream(HIGG1D1Stream)

