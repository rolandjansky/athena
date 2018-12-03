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

# QGTaggerTool ### 
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=STDM6Sequence, algname="QGTaggerToolAlg")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
"""
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
"""

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDM9ContentList import *

STDM6SlimmingHelper = SlimmingHelper("STDM6SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
STDM6SlimmingHelper.SmartCollections = STDM9SmartContent
STDM6SlimmingHelper.ExtraVariables = STDM9ExtraVariables
if isMC:
  STDM6SlimmingHelper.ExtraVariables = STDM9ExtraVariablesTruth

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
STDM6SlimmingHelper.AllVariables = STDM9AllVariablesContent

# Add jet collections created by derivation job
STDM6SlimmingHelper.StaticContent = STDM9StaticContent

STDM6SlimmingHelper.AppendToDictionary = {}

addJetOutputs(STDM6SlimmingHelper, ["STDM6","STDM6Jets"])

listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']
if globalflags.DataSource()=='geant4':
  listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
for i in listJets:
  STDM6SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'

# (Dont) Add jet triger content
#STDM9SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
STDM6SlimmingHelper.IncludeJetTriggerContent = True
STDM6SlimmingHelper.IncludeEGammaTriggerContent = True

addOriginCorrectedClusters(STDM6SlimmingHelper, writeLC=True, writeEM=True)

STDM6SlimmingHelper.AppendContentToStream(STDM6Stream)
