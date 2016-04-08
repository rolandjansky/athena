#********************************************************************
# EXOT0.py 
# reductionConf flag EXOT0 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

exot0Seq = CfgMgr.AthSequencer("EXOT0Sequence")

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT0MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT0MuonTPThinningTool",
                                                                            ThinningService         = "EXOT0ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ConeSize                =  0.4)
ToolSvc += EXOT0MuonTPThinningTool
thinningTools.append(EXOT0MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT0ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT0ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT0ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        ConeSize                =  0.4)
ToolSvc += EXOT0ElectronTPThinningTool
thinningTools.append(EXOT0ElectronTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT0TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT0TruthTool",
                                                        ThinningService       = "EXOT0ThinningSvc",
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

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
  ToolSvc += EXOT0TruthTool
  thinningTools.append(EXOT0TruthTool)

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT0TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT0TruthTool2",
                                                            ThinningService              = "EXOT0ThinningSvc",
                                                            ParticleSelectionString      = truth_cond_Lepton,
                                                            PreserveDescendants          = False,
                                                            PreserveGeneratorDescendants = True,
                                                            PreserveAncestors            = True)
if globalflags.DataSource()=='geant4':
  ToolSvc += EXOT0TruthTool2
  thinningTools.append(EXOT0TruthTool2)

#====================================================================
# SKIMMING TOOL 
#====================================================================

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask == 1) || (EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad || EF_g35_loose_g25_loose)'
if (beamEnergy > 6.0e+06):
    #expression = '(HLT_2e17_loose1 || HLT_2mu14 || HLT_mu24_mu8noL1)'
    expression = '(count(Electrons.pt > 20*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight|| Electrons.DFCommonElectronsIsEMLoose||Electrons.DFCommonElectronsIsEMMedium||Electrons.DFCommonElectronsIsEMTight)) >= 2) || (count(Muons.pt > 20*GeV && (Muons.DFCommonGoodMuon && Muons.muonType == 0)) >= 2)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT0SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT0SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOT0SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot0Seq
exot0Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT0Kernel_skim", SkimmingTools = [EXOT0SkimmingTool])
exot0Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT0Kernel", ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT0Stream )
EXOT0Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT0Stream.AcceptAlgs(["EXOT0Kernel"])
# Thinning 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT0ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT0ContentList import *
EXOT0SlimmingHelper = SlimmingHelper("EXOT0SlimmingHelper")
EXOT0SlimmingHelper.StaticContent = EXOT0Content
EXOT0SlimmingHelper.AllVariables = EXOT0AllVariables
EXOT0SlimmingHelper.SmartCollections = EXOT0SmartCollections
EXOT0SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT0SlimmingHelper.IncludeMuonTriggerContent = True
EXOT0SlimmingHelper.AppendContentToStream(EXOT0Stream)

