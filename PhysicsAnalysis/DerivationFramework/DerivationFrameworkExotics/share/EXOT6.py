#********************************************************************
# EXOT6.py 
# reductionConf flag EXOT6 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

exot6Seq = CfgMgr.AthSequencer("EXOT6Sequence")

augTools = []

triggers = ['HLT_g60_loose',
            'HLT_g140_loose',
            'HLT_g160_loose',
            'HLT_g75_tight_3j50noL1_L1EM22VHI',
            'HLT_g75_tight_3j25noL1_L1EM22VHI',
            'HLT_g85_tight_L1EM22VHI_3j50noL1',
            ]

from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="EXOT6",
                                                 PhotonTriggers=triggers)

augTools.append(TrigMatchAug)

# using now TauTruthCommon, so we use a central Python setup and it is not imported twice
# Tau truth common is a part of the standard truth tools
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()

    # Redo ghost association
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=exot6Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=exot6Seq, algname="JetPtAssociationAlg")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT6Stream )
EXOT6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT6Stream.AcceptAlgs(["EXOT6Kernel"])

#====================================================================
# THINNING TOOL 
#====================================================================

#thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT6ThinningHelper = ThinningHelper( "EXOT6ThinningHelper" )
EXOT6ThinningHelper.AppendToStream( EXOT6Stream )
    
# Track thinning   
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT6TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT6TPThinningTool",
                                                                 ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6TPThinningTool

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT6MuonTPThinningTool",
                                                                         ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT6ElectronTPThinningTool",
                                                                               ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                               SGKey             	   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6ElectronTPThinningTool

# Tracks associated with Photons
EXOT6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT6PhotonTPThinningTool",
                                                                             ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                             SGKey             	     = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT6PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT6PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(name = "EXOT6PhotonCCThinningTool",
                                                                     ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                     SGKey                   = "Photons",
                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                     #SelectionString         = "Photons.pt > 30*GeV",
                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                     ConeSize                = 0.6)
ToolSvc += EXOT6PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT6ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name = "EXOT6ElectronCCThinningTool",
                                                                       ThinningService         = EXOT6ThinningHelper.ThinningSvc(),
                                                                       SGKey                   = "Electrons",
                                                                       CaloClCollectionSGKey   = "egammaClusters",
                                                                       #SelectionString         = "Electrons.pt > 15*GeV",
                                                                       #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                       ConeSize                = 0)
ToolSvc += EXOT6ElectronCCThinningTool

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = '(HLT_g60_loose && (count(Photons.pt > 60*GeV) > 0))  ||  ((HLT_g140_loose || HLT_g160_loose) && (count(Photons.pt > 130*GeV) > 0 || count(Electrons.pt > 100*GeV) > 0))  ||  ((HLT_xe90_tc_lcw_L1XE50||HLT_g75_tight_3j50noL1_L1EM22VHI||HLT_g75_tight_3j25noL1_L1EM22VHI || HLT_g85_tight_L1EM22VHI_3j50noL1 || HLT_xe90_mht_L1XE50||HLT_xe100_mht_L1XE50||HLT_xe110_mht_L1XE50||HLT_xe130_mht_L1XE50) && (count(Photons.pt > 70*GeV) > 0))'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT6SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT6SkimmingTool1", expression = expression)
ToolSvc += EXOT6SkimmingTool

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EXOT6_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator(name                    = "EXOT6_MaxCellDecoratorTool",
                                                                   SGKey_electrons         = "Electrons",
                                                                   SGKey_photons           = "Photons")
ToolSvc += EXOT6_MaxCellDecoratorTool
augTools.append(EXOT6_MaxCellDecoratorTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot6Seq
exot6Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT6Kernel_skim", SkimmingTools = [EXOT6SkimmingTool])
exot6Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT6Kernel",
                                                         AugmentationTools = augTools,
                                                         ThinningTools = [EXOT6TPThinningTool,EXOT6MuonTPThinningTool,EXOT6ElectronTPThinningTool, EXOT6PhotonTPThinningTool, EXOT6PhotonCCThinningTool, EXOT6ElectronCCThinningTool])

#=======================================
# JETS
#=======================================

#Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'],Sequencer = exot6Seq)

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT6"] = []
reducedJetList = [
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot6Seq,"EXOT6")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT6ContentList import EXOT6SmartContent, EXOT6AllVariablesContent, EXOT6ExtraVariables, EXOT6UnslimmedContent

EXOT6SlimmingHelper = SlimmingHelper("EXOT6SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT6SlimmingHelper.SmartCollections = EXOT6SmartContent

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT6SlimmingHelper.AllVariables = EXOT6AllVariablesContent

# Adding extra missing variables from the smart slimming
EXOT6ExtraVariables.append("Photons."+NewTrigVars["Photons"])
EXOT6SlimmingHelper.ExtraVariables = EXOT6ExtraVariables
EXOT6SlimmingHelper.StaticContent = EXOT6UnslimmedContent
EXOT6SlimmingHelper.AppendContentToStream(EXOT6Stream)
