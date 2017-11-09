#********************************************************************
# EXOT0.py 
# reductionConf flag EXOT0 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT0Stream )
EXOT0Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT0Stream.AcceptAlgs(["EXOT0KernelSkim"])

# Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT0ThinningHelper = ThinningHelper( "EXOT0ThinningHelper" )
thinningTools = []
AugmentationTools = []

# stream-specific sequence for on-the-fly jet building
SeqEXOT0 = CfgMgr.AthSequencer("EXOT0Sequence")
DerivationFrameworkJob += SeqEXOT0

EXOT0ThinningHelper.AppendToStream( EXOT0Stream )


#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT0MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT0MuonTPThinningTool",
                                                                         ThinningService         = EXOT0ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         ConeSize                =  0.4)
ToolSvc += EXOT0MuonTPThinningTool
thinningTools.append(EXOT0MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT0ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT0ElectronTPThinningTool",
                                                                               ThinningService         = EXOT0ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               ConeSize                =  0.4)
ToolSvc += EXOT0ElectronTPThinningTool
thinningTools.append(EXOT0ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT0PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT0PhotonTPThinningTool",
                                                                             ThinningService         = EXOT0ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ConeSize                =  0.4)
ToolSvc += EXOT0PhotonTPThinningTool
thinningTools.append(EXOT0PhotonTPThinningTool)

# track thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT0TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "EXOT0TPThinningTool",
								 ThinningService        = EXOT0ThinningHelper.ThinningSvc(),
								 SelectionString        = "InDetTrackParticles.pt > 5*GeV",
								 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT0TPThinningTool
thinningTools.append(EXOT0TPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT0TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "EXOT0TauTPThinningTool",
                                                                       ThinningService        = EXOT0ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT0TauTPThinningTool
thinningTools.append(EXOT0TauTPThinningTool)

# Photons
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT0PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "EXOT0PhotonThinningTool",
                                                                      ThinningService  = EXOT0ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += EXOT0PhotonThinningTool
thinningTools.append(EXOT0PhotonThinningTool)

# truth thinning
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  EXOT0TruthTool = DerivationFramework__MenuTruthThinning(name                         = "EXOT0TruthTool",
                                                          ThinningService              = EXOT0ThinningHelper.ThinningSvc(),
                                                          WritePartons                 = False,
                                                          WriteHadrons                 = False,
                                                          WriteBHadrons                = True,
                                                          WriteGeant                   = False,
                                                          GeantPhotonPtThresh          = 20000,
                                                          WriteTauHad                  = True,
                                                          PartonPtThresh               = -1.0,
                                                          WriteBSM                     = True,
                                                          WriteBosons                  = True,
                                                          WriteBSMProducts             = True,
                                                          WriteBosonProducts           = True,
                                                          WriteTopAndDecays            = True,
                                                          WriteEverything              = False,
                                                          WriteAllLeptons              = False,
                                                          WriteLeptonsNotFromHadrons   = False,
                                                          WriteStatus3                 = False,
                                                          WriteFirstN                  = -1,
                                                          PreserveAncestors            = True,
                                                          PreserveGeneratorDescendants = False,
                                                          SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += EXOT0TruthTool
  thinningTools.append(EXOT0TruthTool)


  truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  EXOT0TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT0TruthTool2",
                                                              ThinningService              = EXOT0ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString      = truth_cond_Lepton,
                                                              PreserveDescendants          = False,
                                                              PreserveGeneratorDescendants = True,
                                                              PreserveAncestors            = True)
  ToolSvc += EXOT0TruthTool2
  thinningTools.append(EXOT0TruthTool2)

  
#====================================================================
# SKIMMING TOOL 
#====================================================================

#expression = '(count(Electrons.pt > 20*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight)) >= 2) || (count(Muons.pt > 20*GeV && (Muons.DFCommonGoodMuon && Muons.muonType == 0)) >= 2)'
#expression = '(count(Electrons.pt > 20*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight)) >= 2) || (count(Muons.pt > 20*GeV && (Muons.DFCommonGoodMuon && (Muons.muonType==0 || Muons.muonType==1 || Muons.muonType==2))) >= 2)'
goodelectron = "Electrons.pt>20*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight)"
goodmuon     = "Muons.pt>20*GeV && (Muons.DFCommonGoodMuon && (Muons.muonType==0 || Muons.muonType==1 || Muons.muonType==2))"
expression   = '(count('+goodelectron+')>=2)  ||  (count('+goodmuon+')>=2)  ||  ((count('+goodelectron+')>=1) && (count('+goodmuon+')>=1))'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT0SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT0SkimmingTool1", expression = expression)
ToolSvc += EXOT0SkimmingTool


#====================================================================
# ISOLATION TOOL 
#====================================================================

#Track selection
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool500.TrackSelectionTool.minPt= 500.
TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool500

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt1000_')
Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool500,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt500_')
ToolSvc += Pt1000IsoTrackDecorator
ToolSvc += Pt500IsoTrackDecorator

AugmentationTools.append(Pt1000IsoTrackDecorator)
AugmentationTools.append(Pt500IsoTrackDecorator)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqEXOT0 += CfgMgr.DerivationFramework__DerivationKernel("EXOT0KernelSkim", SkimmingTools = [EXOT0SkimmingTool])
SeqEXOT0 += CfgMgr.DerivationFramework__DerivationKernel("EXOT0KernelAug", AugmentationTools = AugmentationTools, ThinningTools = thinningTools)

# Add sumOfWeights metadata for LHE3 multiweights
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# JETS
#==============================================================================
#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT0"] = []
reducedJetList = [
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,SeqEXOT0,"EXOT0")


#==============================================================================
# Tau truth building/matching
#==============================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
  addTruthTaus(AugmentationTools)


#==============================================================================
# background generator filters
#==============================================================================
if DerivationFrameworkIsMonteCarlo:
  ToolSvc += CfgMgr.DerivationFramework__GenFilterTool("EXOT0GenFilt",SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset)
  AugmentationTools.append(ToolSvc.EXOT0GenFilt)

  
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT0ContentList import *
EXOT0SlimmingHelper = SlimmingHelper("EXOT0SlimmingHelper")
EXOT0SlimmingHelper.AllVariables = EXOT0AllVariables
EXOT0SlimmingHelper.ExtraVariables = EXOT0ExtraVariables
EXOT0SlimmingHelper.SmartCollections = EXOT0SmartCollections
EXOT0SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT0SlimmingHelper.IncludeMuonTriggerContent   = True
EXOT0SlimmingHelper.IncludeBPhysTriggerContent  = False
EXOT0SlimmingHelper.IncludeJetTriggerContent    = False
EXOT0SlimmingHelper.IncludeTauTriggerContent    = False
EXOT0SlimmingHelper.IncludeEtMissTriggerContent = False
EXOT0SlimmingHelper.IncludeBJetTriggerContent   = False

if DerivationFrameworkIsMonteCarlo:
  # Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
  EXOT0SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer'}
  EXOT0SlimmingHelper.AppendToDictionary = {'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer'}
  EXOT0SlimmingHelper.AppendToDictionary = {'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}  
  # All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py) and are added to the AllVariables list in EXOT0ContentList.py

EXOT0SlimmingHelper.AppendContentToStream(EXOT0Stream)
