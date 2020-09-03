# *****************************************************
# TAUP5.py 
# reductionConf flag TAUP5 in Reco_tf.py  
# *****************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
#from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

# =============================================
# Private sequence
# =============================================
TAUP5seq = CfgMgr.AthSequencer("TAUP5Sequence")

# =============================================
# Set up stream
# =============================================
streamName      = derivationFlags.WriteDAOD_TAUP5Stream.StreamName
fileName        = buildFileName( derivationFlags.WriteDAOD_TAUP5Stream )
TAUP5Stream     = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP5Stream.AcceptAlgs(["TAUP5Kernel"])

# =============================================
# MC truth
# =============================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents(TAUP5seq,prefix='TAUP5_')

# =============================================
# Jet and Ditau reconstruction
# =============================================

#b-tag AntiKt4PFlowJets
#FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = TAUP5seq)

reducedJetList = ["AntiKt4TruthJets","AntiKt4TruthWZJets","AntiKt4PV0TrackJets", "AntiKt2PV0TrackJets"]
replaceAODReducedJets(reducedJetList,TAUP5seq, "TAUP5")

from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=TAUP5seq)

# =============================================
# Ditau mass decoration
# =============================================
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__DiTauMassDecorator
DiTauMassDecorator = DerivationFramework__DiTauMassDecorator(name               = "DiTauMassDecorator",
                                                             DiTauContainerName = "DiTauJetsLowPt")
ToolSvc += DiTauMassDecorator

# =============================================
# Thinning tool
# =============================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TAUP5ThinningHelper               = ThinningHelper( "TAUP5ThinningHelper" )
TAUP5ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(tau|mu|j|xe|e|b|perf|idperf))(?!HLT_g.*_[0-9]*g.*)(HLT_g.*))'
TAUP5ThinningHelper.AppendToStream( TAUP5Stream )

thinningTools = []

# Tracks thinning (inspired by HIGG1D2)
# might want to update to latest recommendation implemented in DAOD_PHYS:
# "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
TAUP5TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "TAUP5TPThinningTool",
                                                                 StreamName             = streamName,
                                                                 SelectionString        = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0*mm",
                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP5TPThinningTool
thinningTools.append(TAUP5TPThinningTool)

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "TAUP5ElectronTPThinningTool",
                                                                               StreamName             = streamName,
                                                                               SGKey                  = "Electrons",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP5ElectronTPThinningTool
thinningTools.append(TAUP5ElectronTPThinningTool)

# Tracks associated with photons
TAUP5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "TAUP5PhotonTPThinningTool",
                                                                             StreamName             = streamName,
                                                                             SGKey                  = "Photons",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP5PhotonTPThinningTool
thinningTools.append(TAUP5PhotonTPThinningTool)

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "TAUP5MuonTPThinningTool",
                                                                         StreamName             = streamName,
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP5MuonTPThinningTool
thinningTools.append(TAUP5MuonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "TAUP5TauTPThinningTool",
                                                                       StreamName             = streamName,
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       ConeSize               = 0.6)
ToolSvc += TAUP5TauTPThinningTool
thinningTools.append(TAUP5TauTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
TAUP5DiTauTPThinningToolLowPt = DerivationFramework__DiTauTrackParticleThinning(name                   = "TAUP5DiTauLowPtTPThinningTool",
                                                                                StreamName             = streamName,
                                                                                DiTauKey               = "DiTauJetsLowPt",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP5DiTauTPThinningToolLowPt
thinningTools.append(TAUP5DiTauTPThinningToolLowPt)


# truth thinning
#import DerivationFrameworkTau.TAUPThinningHelper 
#TAUP5TruthThinningTools = DerivationFrameworkTau.TAUPThinningHelper.setup("TAUP5",
#                                                                          TAUP5ThinningHelper.ThinningSvc(),
#                                                                          ToolSvc)
#thinningTools += TAUP5TruthThinningTools

# =============================================
# Skimming tool
# =============================================

gReq = '(count (Photons.pt > 150.0*GeV && abs(Photons.eta) < 2.5) >=1 ) '
#eReq = '( Electrons.pt > 10.0*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHTight )'
muReq = '( Muons.pt > 10.0*GeV && abs(Muons.eta) < 2.5 && Muons.DFCommonMuonsPreselection )' 
trigger_ph = '(HLT_g140_loose || HLT_g300_etcut)' #single photon
#noLep = '( (count('+eReq+') == 0) && (count('+muReq+') == 0) )' #Lepton veto (tight ele, loose mu)
noLep = '( (count('+muReq+') == 0) )' #Lepton veto (tight ele, loose mu)
ditau = '(count( (DiTauJetsLowPt.pt > 50.0*GeV) && (DiTauJetsLowPt.nSubjets > 1) && (DiTauJetsLowPt.LSLMass >= 40000) && (DiTauJetsLowPt.LSLMass <= 130000) ) >= 1)'
skim_expression = gReq + "&&" + noLep + "&&" + trigger_ph + "&&" + ditau 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP5SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name       = "TAUP5SkimmingTool",
                                                                expression = skim_expression)
ToolSvc += TAUP5SkimmingTool

# =============================================
# Create derivation Kernel
# =============================================

TAUP5seq += CfgMgr.DerivationFramework__DerivationKernel("TAUP5Kernel",
                                                         AugmentationTools = [DiTauMassDecorator],
                                                         SkimmingTools     = [TAUP5SkimmingTool],
                                                         ThinningTools     = thinningTools)

DerivationFrameworkJob += TAUP5seq

# =============================================
# Add the containers to the output stream (slimming done here)
# =============================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP5SlimmingHelper = SlimmingHelper("TAUP5SlimmingHelper")
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

TAUP5SlimmingHelper.IncludeMuonTriggerContent    = False
TAUP5SlimmingHelper.IncludeTauTriggerContent     = False
TAUP5SlimmingHelper.IncludeEGammaTriggerContent  = True
TAUP5SlimmingHelper.IncludeEtMissTriggerContent  = False
TAUP5SlimmingHelper.IncludeJetTriggerContent     = False
TAUP5SlimmingHelper.IncludeBJetTriggerContent    = False

TAUP5SlimmingHelper.ExtraVariables               = ExtraContentTAUP5
TAUP5SlimmingHelper.AllVariables                 = ExtraContainersTAUP5

addJetOutputs(TAUP5SlimmingHelper, ['TAUP5'], ['AntiKt4TruthJets', 'AntiKt4TruthWZJets'], ['AntiKt4PV0TrackJets','AntiKt2PV0TrackJets','AntiKt10LCTopoJets'])
TAUP5SlimmingHelper.AllVariables += ["DiTauJetsLowPt"] #Add in the ditaus
TAUP5SlimmingHelper.ExtraVariables += ["DiTauJetsLowPt.LSLMass"] #Add ditau lead+sublead subjet system invariant mass decoration because why not

if DerivationFrameworkIsMonteCarlo:
  TAUP5SlimmingHelper.ExtraVariables += ExtraContentTruthTAUP5
  TAUP5SlimmingHelper.AllVariables   += ExtraContainersTruthTAUP5
  TAUP5SlimmingHelper.AllVariables   += ["TruthElectrons", 
                                         "TruthMuons", 
                                         "TruthTaus", 
                                         "TruthPhotons",
                                         "TruthNeutrinos",
                                         "TruthBoson"]

TAUP5SlimmingHelper.AppendContentToStream(TAUP5Stream)

TAUP5Stream.AddItem("xAOD::EventShape#*")
TAUP5Stream.AddItem("xAOD::EventShapeAuxInfo#*")

