#********************************************************************
# STDM6.py 
# reductionConf flag STDM6 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkEGamma.EGammaCommon import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


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
                                                                 ThinningTools = [])

# JET REBUILDING
reducedJetList = ["AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, STDM6Sequence, "STDM6Jets")

# ADD MBTS Container
from xAODForwardCnv.xAODMBTSModuleCreator import xAODMaker__MBTSModuleCnvAlg
STDM6Sequence +=  xAODMaker__MBTSModuleCnvAlg()


# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM6Sequence


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
                                        "InDetTrackParticles",
                                        "PrimaryVertices"  ]

STDM6SlimmingHelper.AllVariables=[ "CaloCalTopoClusters","ALFADataContainer","MBTSModules","AFPTrackContainer", "AFPSiHitContainer", "AFPTrackContainerAux", "AFPSiHitContainerAux"]
STDM6SlimmingHelper.AppendToDictionary = {"MBTSModules":"xAOD::MBTSModuleContainer","MBTSModulesAux":"xAOD::MBTSModuleAuxContainer"}

STDM6SlimmingHelper.IncludeEGammaTriggerContent = True
STDM6SlimmingHelper.IncludeMuonTriggerContent = True
STDM6SlimmingHelper.IncludeMinBiasTriggerContent = True


STDM6SlimmingHelper.ExtraVariables = ExtraContentAll
STDM6SlimmingHelper.ExtraVariables += ["InDetTrackParticles.pixeldEdx.numberOfUsedHitsdEdx.numberOfIBLOverflowsdEdx"]
STDM6SlimmingHelper.AllVariables += ExtraContainersAll
if globalflags.DataSource()=='geant4':
    STDM6SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM6SlimmingHelper.AllVariables += ExtraContainersTruth
    STDM6SlimmingHelper.AppendToDictionary = ExtraDictionary

addJetOutputs(STDM6SlimmingHelper,["STDM6","STDM6Jets"])

STDM6SlimmingHelper.AppendContentToStream(STDM6Stream)


