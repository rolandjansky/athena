# *****************************************************
# TAUP5.py 
# reductionConf flag TAUP5 in Reco_tf.py  
DAOD_StreamID = 'TAUP5' 
# *****************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from AthenaCommon.GlobalFlags import globalflags
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()


# =============================================
# Private sequence here
# =============================================
TAUP5seq = CfgMgr.AthSequencer("TAUP5Sequence")

#b-tag AntiKt4PFlowJets
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = TAUP5seq)

# =============================================
# Set up stream
# =============================================
streamName      = derivationFlags.WriteDAOD_TAUP5Stream.StreamName
fileName        = buildFileName( derivationFlags.WriteDAOD_TAUP5Stream )
TAUP5Stream     = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP5Stream.AcceptAlgs(["TAUP5Kernel"])

# =============================================
# Ditau mass decoration
# =============================================
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__DiTauMassDecorator
DiTauMassDecorator = DerivationFramework__DiTauMassDecorator(
    name              = "DiTauMassDecorator",
    DiTauContainerName  = "DiTauJetsLowPt",
    )
ToolSvc += DiTauMassDecorator





# =============================================
# Thinning tool
# =============================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TAUP5ThinningHelper                              = ThinningHelper( "TAUP5ThinningHelper" )
TAUP5ThinningHelper.TriggerChains                = '(^(?!.*_[0-9]*(tau|mu|j|xe|e|b|perf|idperf))(?!HLT_g.*_[0-9]*g.*)(HLT_g.*))'
TAUP5ThinningHelper.AppendToStream( TAUP5Stream )

thinningTools = []

# MET/Jet tracks
thinning_expression     = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
TAUP5MetTPThinningTool    = DerivationFramework__TrackParticleThinning(
  name                      = "TAUP5MetTPThinningTool",
  ThinningService           = TAUP5ThinningHelper.ThinningSvc(),
  SelectionString           = thinning_expression,
  InDetTrackParticlesKey    = "InDetTrackParticles",
  ApplyAnd                  = True)


ToolSvc  += TAUP5MetTPThinningTool
thinningTools.append(TAUP5MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
TAUP5JetTPThinningTool    = DerivationFramework__JetTrackParticleThinning(
  name                      = "TAUP5JetTPThinningTool",
  ThinningService           = TAUP5ThinningHelper.ThinningSvc(),
  JetKey                    = "AntiKt4EMPFlowJets",
  InDetTrackParticlesKey    = "InDetTrackParticles",
  ApplyAnd                  = True)


ToolSvc   += TAUP5JetTPThinningTool
thinningTools.append(TAUP5JetTPThinningTool)

# Tracks themselves (inspired by HIGG1D2)
TAUP5TPThinningTool    = DerivationFramework__TrackParticleThinning(
  name                      = "TAUP5TPThinningTool",
  ThinningService           = TAUP5ThinningHelper.ThinningSvc(),
  SelectionString           = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
  InDetTrackParticlesKey    = "InDetTrackParticles")


ToolSvc  += TAUP5TPThinningTool
thinningTools.append(TAUP5TPThinningTool)

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name                      = "TAUP5ElectronTPThinningTool",
  ThinningService           = TAUP5ThinningHelper.ThinningSvc(),
  SGKey                     = "Electrons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc += TAUP5ElectronTPThinningTool
thinningTools.append(TAUP5ElectronTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name                    = "TAUP5PhotonTPThinningTool",
  ThinningService         = TAUP5ThinningHelper.ThinningSvc(),
  SGKey                   = "Photons",
  SelectionString 	  = "",
  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += TAUP5PhotonTPThinningTool
thinningTools.append(TAUP5PhotonTPThinningTool)

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP5MuonTPThinningTool   = DerivationFramework__MuonTrackParticleThinning(
  name                      = "TAUP5MuonTPThinningTool",
  ThinningService           = TAUP5ThinningHelper.ThinningSvc(),
  MuonKey                   = "Muons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc += TAUP5MuonTPThinningTool
thinningTools.append(TAUP5MuonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP5TauTPThinningTool  = DerivationFramework__TauTrackParticleThinning(
  name                    = "TAUP5TauTPThinningTool",
  ThinningService         = TAUP5ThinningHelper.ThinningSvc(),
  TauKey                  = "TauJets",
  InDetTrackParticlesKey  = "InDetTrackParticles",
  ConeSize                = 0.6)

ToolSvc += TAUP5TauTPThinningTool
thinningTools.append(TAUP5TauTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
TAUP5DiTauTPThinningToolLowPt = DerivationFramework__DiTauTrackParticleThinning(
  name               = "TAUP5DiTauLowPtTPThinningTool",
  ThinningService         = TAUP5ThinningHelper.ThinningSvc(),
  DiTauKey                = "DiTauJetsLowPt",
  SelectionString         = "",
  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += TAUP5DiTauTPThinningToolLowPt
thinningTools.append(TAUP5DiTauTPThinningToolLowPt)


# truth thinning here:
import DerivationFrameworkTau.TAUPThinningHelper 
TAUP5TruthThinningTools = DerivationFrameworkTau.TAUPThinningHelper.setup("TAUP5",
                                                                          TAUP5ThinningHelper.ThinningSvc(),
                                                                          ToolSvc)

thinningTools += TAUP5TruthThinningTools
# =============================================
# Skimming tool
# =============================================


gReq = '(count (Photons.pt > 150.0*GeV && abs(Photons.eta) < 2.5) >=1 ) '
eReq = '( Electrons.pt > 10.0*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHTight )'
muReq = '( Muons.pt > 10.0*GeV && abs(Muons.eta) < 2.5 && Muons.DFCommonMuonsPreselection )' 
trigger_ph = '(HLT_g140_loose || HLT_g300_etcut)' #single photon
noLep = '( (count('+eReq+') == 0) && (count('+muReq+') == 0) )' #Lepton veto (tight ele, loose mu)
ditau = '(count( (DiTauJetsLowPt.pt > 50.0*GeV) && (DiTauJetsLowPt.nSubjets > 1) && (DiTauJetsLowPt.LSLMass >= 40000) && (DiTauJetsLowPt.LSLMass <= 130000) ) >= 1)'
skim_expression = gReq + "&&" + noLep + "&&" + trigger_ph + "&&" + ditau 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP5SkimmingTool = DerivationFramework__xAODStringSkimmingTool(
  name          = "TAUP5SkimmingTool",
  expression    = skim_expression)

ToolSvc += TAUP5SkimmingTool

# =============================================
# Jet and Ditau reconstruction
# =============================================

reducedJetList = ["AntiKt4TruthJets","AntiKt4TruthWZJets","AntiKt4PV0TrackJets", "AntiKt2PV0TrackJets"]
replaceAODReducedJets(reducedJetList,TAUP5seq, "TAUP5")
from DerivationFrameworkTau.TauTruthCommon import *
addCHSPFlowObjects()
addStandardJets("AntiKt", 1.0, "EMPFlow", ptmin=40000, ptminFilter=50000, mods="pflow_ungroomed", calibOpt="none", algseq=TAUP5seq, outputGroup=DAOD_StreamID)
import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs
from DiTauRec.DiTauRecConf import DiTauBuilder

ditauTools = []
ditauTools.append(DiTauAlgs.getSeedJetBuilder("AntiKt10EMPFlowJets"))
ditauTools.append(DiTauAlgs.getElMuFinder())
ditauTools.append(DiTauAlgs.getSubjetBuilder())
ditauTools.append(DiTauAlgs.getVertexFinder())
ditauTools.append(DiTauAlgs.getDiTauTrackFinder())
ditauTools.append(DiTauAlgs.getIDVarCalculator(False))

DiTauBuilderBase = DiTauBuilder(
    name="DiTauBuilderLowPt",
    DiTauContainer="DiTauJetsLowPt",
    DiTauAuxContainer="DiTauJetsLowPtAux.",
    Tools=ditauTools,
    SeedJetName="AntiKt10EMPFlowJets",
    minPt=50000,
    maxEta=2.5,
    OutputLevel=2,
    Rjet=1.0,
    Rsubjet=0.2,
    Rcore=0.1)

TAUP5seq += DiTauBuilderBase


# =============================================
# Create derivation Kernel
# =============================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
TAUP5seq += CfgMgr.DerivationFramework__DerivationKernel(
  "TAUP5Kernel",
  AugmentationTools         = [DiTauMassDecorator],
  SkimmingTools             = [TAUP5SkimmingTool],
  ThinningTools             = thinningTools
  )
DerivationFrameworkJob += TAUP5seq
# =============================================
# Add the containers to the output stream (slimming done here)
# =============================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP5SlimmingHelper       = SlimmingHelper("TAUP5SlimmingHelper")
TAUP5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMPFlow",  
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMTopoJets_BTagging201810", 
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "AntiKt4EMPFlowJets",                                        
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]

if DerivationFrameworkHasTruth:
  TAUP5SlimmingHelper.StaticContent  = ["xAOD::TruthParticleContainer#TruthBoson",
                                        "xAOD::TruthParticleAuxContainer#TruthBosonAux.",
                                        "xAOD::TruthParticleContainer#TruthElectrons",
                                        "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                        "xAOD::TruthParticleContainer#TruthMuons",
                                        "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                        "xAOD::TruthParticleContainer#TruthTaus",
                                        "xAOD::TruthParticleAuxContainer#TruthTausAux.",
                                        "xAOD::TruthParticleContainer#TruthPhotons",
                                        "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
                                        "xAOD::TruthParticleContainer#TruthNeutrinos",
                                        "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux."]

TAUP5SlimmingHelper.IncludeMuonTriggerContent    = False
TAUP5SlimmingHelper.IncludeTauTriggerContent     = False
TAUP5SlimmingHelper.IncludeEGammaTriggerContent  = True
TAUP5SlimmingHelper.IncludeEtMissTriggerContent  = False
TAUP5SlimmingHelper.IncludeJetTriggerContent     = False
TAUP5SlimmingHelper.IncludeBJetTriggerContent    = False

TAUP5SlimmingHelper.ExtraVariables               = ExtraContentTAUP5
TAUP5SlimmingHelper.AllVariables                 = ExtraContainersTAUP5

addJetOutputs(TAUP5SlimmingHelper, [DAOD_StreamID], ['AntiKt4TruthJets', 'AntiKt4TruthWZJets'], ['AntiKt4PV0TrackJets','AntiKt2PV0TrackJets','AntiKt10LCTopoJets'])
TAUP5SlimmingHelper.AppendToDictionary["DiTauJetsLowPt"] = 'xAOD::DiTauJetContainer' #Add in the ditaus
TAUP5SlimmingHelper.AppendToDictionary["DiTauJetsLowPtAux"] = 'xAOD::DiTauJetAuxContainer' #Add in the ditaus
TAUP5SlimmingHelper.AllVariables += ["DiTauJetsLowPt"] #Add in the ditaus
TAUP5SlimmingHelper.ExtraVariables += ["DiTauJetsLowPt.LSLMass"] #Add ditau lead+sublead subjet system invariant mass decoration because why not

if DerivationFrameworkHasTruth:
  TAUP5SlimmingHelper.ExtraVariables            += ExtraContentTruthTAUP5
  TAUP5SlimmingHelper.AllVariables              += ExtraContainersTruthTAUP5

TAUP5SlimmingHelper.AppendContentToStream(TAUP5Stream)

TAUP5Stream.AddItem("xAOD::EventShape#*")
TAUP5Stream.AddItem("xAOD::EventShapeAuxInfo#*")

