#********************************************************************
# EXOT9.py 
# reductionConf flag EXOT9 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

exot9Seq = CfgMgr.AthSequencer("EXOT9Sequence")

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT9MuonTPThinningTool",
                                                                            ThinningService         = "EXOT9ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ConeSize                =  0.4)
ToolSvc += EXOT9MuonTPThinningTool
thinningTools.append(EXOT9MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT9ElectronTPThinningTool",
                                                                               ThinningService         = "EXOT9ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               ConeSize                =  0.4)
ToolSvc += EXOT9ElectronTPThinningTool
thinningTools.append(EXOT9ElectronTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT9TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT9TruthTool",
                                                        ThinningService       = "EXOT9ThinningSvc",
                                                        WritePartons          = False,
                                                        WriteHadrons          = False,
                                                        WriteBHadrons         = False,
                                                        WriteGeant            = False,
                                                        GeantPhotonPtThresh   = -1.0,
                                                        WriteTauHad           = False,
                                                        PartonPtThresh        = -1.0,
                                                        WriteBSM              = True,
                                                        WriteBosons           = True,
                                                        WriteBSMProducts      = True,
                                                        WriteBosonProducts    = True,
                                                        WriteTopAndDecays     = True,
                                                        WriteEverything       = False,
                                                        WriteAllLeptons       = False,
                                                        WriteStatus3          = False,
                                                        PreserveGeneratorDescendants  = False,
                                                        PreserveAncestors     = True,
                                                        WriteFirstN           = -1)
if isMC:
  ToolSvc += EXOT9TruthTool
  thinningTools.append(EXOT9TruthTool)

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT9TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT9TruthTool2",
                                                            ThinningService              = "EXOT9ThinningSvc",
                                                            ParticleSelectionString      = truth_cond_Lepton,
                                                            PreserveDescendants          = False,
                                                            PreserveGeneratorDescendants = True,
                                                            PreserveAncestors            = True)
if isMC:
  ToolSvc += EXOT9TruthTool2
  thinningTools.append(EXOT9TruthTool2)

#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    triggerStrategy = '(EventInfo.eventTypeBitmask==1) || (EF_xe80_tclcw || EF_xe80_tclcw_loose || EF_e24vhi_medium1 || EF_e60_medium1 || EF_mu24i_tight || EF_mu36_tight || EF_g120_loose)'
if (beamEnergy > 6.0e+06):
    #triggerStrategy = '(EventInfo.eventTypeBitmask==1) || (HLT_mu26_imedium || HLT_mu50 || HLT_mu60_msonly_0eta105 || HLT_e28_tight_iloose || HLT_e60_medium || HLT_g140_loose || HLT_xe100 || HLT_g60_loose_xe60 || HLT_e140_loose)'
    triggerStrategy = '(count(Electrons.pt > 50*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight||Electrons.DFCommonElectronsIsEMLoose||Electrons.DFCommonElectronsIsEMMedium||Electrons.DFCommonElectronsIsEMTight)) >= 1) || (count(Muons.pt > 50*GeV && (Muons.DFCommonGoodMuon && Muons.muonType == 0)) >= 1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT9SkimmingTool = DerivationFramework__xAODStringSkimmingTool(	name = "EXOT9SkimmingTool1", 
									expression = triggerStrategy)
ToolSvc += EXOT9SkimmingTool
print EXOT9SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot9Seq
exot9Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT9Kernel_skim", SkimmingTools = [EXOT9SkimmingTool])
exot9Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT9Kernel", ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT9Stream )
EXOT9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT9Stream.AcceptAlgs(["EXOT9Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT9ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT9ContentList import *
EXOT9SlimmingHelper = SlimmingHelper("EXOT9SlimmingHelper")
EXOT9SlimmingHelper.StaticContent = EXOT9Content
EXOT9SlimmingHelper.AllVariables = EXOT9AllVariables
EXOT9SlimmingHelper.SmartCollections = EXOT9SmartCollections
EXOT9SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT9SlimmingHelper.IncludeMuonTriggerContent = True
EXOT9SlimmingHelper.AppendContentToStream(EXOT9Stream)
