#********************************************************************
# STDM6.py 
# reductionConf flag STDM6 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

# Add Truth MetaData
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import *

#===========================================================================================\
# AUGMENTATION  TOOL                                                                         
#===========================================================================================\
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__PFlowAugmentationTool
STDM6_PFlowAugmentationTool = DerivationFramework__PFlowAugmentationTool(name = "STDM6_PFlowAugmentationTool")
ToolSvc += STDM6_PFlowAugmentationTool

#====================================================================                                               
# SET UP STREAM 
#====================================================================                                               

streamName = derivationFlags.WriteDAOD_STDM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM6Stream )
STDM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM6Stream.AcceptAlgs(["STDM6Kernel"])

isMC = globalflags.DataSource()=='geant4'

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM6Sequence = CfgMgr.AthSequencer("STDM6Sequence")

# ADD KERNEL 
STDM6Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM6Kernel",
                                                                 SkimmingTools = [],
                                                                 ThinningTools = [],
                                                                 AugmentationTools=[STDM6_PFlowAugmentationTool])

# JET REBUILDING
reducedJetList = ["AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets"]
replaceAODReducedJets(reducedJetList, STDM6Sequence, "STDM6Jets")

# ADD MBTS Container
from xAODForwardCnv.xAODMBTSModuleCreator import xAODMaker__MBTSModuleCnvAlg
STDM6Sequence +=  xAODMaker__MBTSModuleCnvAlg()

# # FAKE LEPTON TAGGER
# import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
# STDM6Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM6Sequence


# PFlow augmentation
applyPFOAugmentation(STDM6Sequence)

# add map with modified association method and make MET:
addHadRecoilMETMap(STDM6Sequence, STDM6Stream, "STDM6")

#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM6Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM6Stream )
#STDM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM6Stream.AcceptAlgs(["STDM6Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM6ThinningSvc", outStreams=[evtStream] )


# QGTaggerTool ### 
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=STDM6Sequence, algname="QGTaggerToolAlg")

#====================================================================
# Jet reconstruction/retagging
#====================================================================

#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = STDM6Sequence)

#improved fJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation,getPFlowfJVT
# MVfJvt #
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=STDM6Sequence, algname='JetForwardJvtToolBDTAlg')
# PFlow fJvt #
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=STDM6Sequence, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM6SlimmingHelper = SlimmingHelper("STDM6SlimmingHelper")
STDM6SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4LCTopoJets",
                                        "AntiKt4EMTopoJets",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"  ]

STDM6SlimmingHelper.AllVariables=[ "CaloCalTopoClusters","ALFADataContainer","MBTSModules","AFPTrackContainer", "AFPSiHitContainer", "AFPTrackContainerAux", "AFPSiHitContainerAux"]
STDM6SlimmingHelper.AppendToDictionary = {"MBTSModules":"xAOD::MBTSModuleContainer","MBTSModulesAux":"xAOD::MBTSModuleAuxContainer"}

STDM6SlimmingHelper.IncludeEGammaTriggerContent = True
STDM6SlimmingHelper.IncludeMuonTriggerContent = True
STDM6SlimmingHelper.IncludeMinBiasTriggerContent = True


STDM6SlimmingHelper.ExtraVariables = ExtraContentAll
STDM6SlimmingHelper.ExtraVariables += [
    "InDetTrackParticles.pixeldEdx.numberOfUsedHitsdEdx.numberOfIBLOverflowsdEdx",
    "Electrons.UEcorr_Pt", "Muons.UEcorr_Pt",
    "Electrons.Reta.Rphi.Rhad1.Rhad.weta2.Eratio.f3.deltaEta1.deltaPhiRescaled2.wtots1.e277.f1.weta1.fracs1.DeltaE",
    "Photons.Reta.Rphi.Rhad1.Rhad.weta2.Eratio.deltaEta1.deltaPhiRescaled2.wtots1.e277.f1.weta1.fracs1.DeltaE"   
]
# STDM6SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
STDM6SlimmingHelper.AllVariables += ExtraContainersAll
if globalflags.DataSource()=='geant4':
    STDM6SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM6SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM6SlimmingHelper.AppendToDictionary = ExtraDictionary

addJetOutputs(STDM6SlimmingHelper,["STDM6","STDM6Jets"])

addMETOutputs(STDM6SlimmingHelper,["AntiKt4EMPFlow"])

STDM6SlimmingHelper.AppendContentToStream(STDM6Stream)
