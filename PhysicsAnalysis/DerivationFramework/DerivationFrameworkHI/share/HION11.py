#====================================================================
# HION11.py
# authors: Soumya Mohapatra <soumya@cern.ch>,
#          Jakub Kremer <jakub.kremer@cern.ch>
# Application: Dimuon analysis
# Triggers: All dimuon triggers
#====================================================================



#====================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags



#====================================================================
# SET UP OUTPUT STREAM  
#====================================================================
streamName = derivationFlags.WriteDAOD_HION11Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION11Stream )
HION11Stream = MSMgr.NewPoolRootStream( streamName, fileName )



#====================================================================
#Check to see if it is MC Derivation
#====================================================================
GetConditionsFromMetaData()
isSimulation = HIDerivationFlags.isSimulation()



#====================================================================
#SKIMMING TOOL
#====================================================================
#Triggers
triggers=['HLT_2mu4', 'HLT_mu4_mu4noL1', 'HLT_2mu3', 'HLT_2mu6', 'HLT_2mu8']

triggers='(' + ' || '.join(triggers) + ')'
#Other cuts
vertex_cut='count(abs(PrimaryVertices.z)<300)>1'
#All cuts
req_total =triggers+'&&('+vertex_cut+')'
#disable triggers for MC
if isSimulation:
    triggers=""
    req_total='('+vertex_cut+')'
print "******************************************************"
print req_total
print "******************************************************"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION11SkimmingTriggerTool = DerivationFramework__xAODStringSkimmingTool( name = "HION11SkimmingTriggerTool", expression = req_total )
ToolSvc+=HION11SkimmingTriggerTool



#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HION11ThinningHelper = ThinningHelper( "HION11ThinningHelper" )
HION11ThinningHelper.AppendToStream( HION11Stream )

from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
HITrackSelector = InDet__InDetTrackSelectionTool("HION11InDetTrackSelectionTool") 
HITrackSelector.CutLevel = "Loose"
HITrackSelector.maxNSiSharedModules = 100
HITrackSelector.minPt = 1900
ToolSvc += HITrackSelector

from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HITrackParticleThinningTool
HION11ThinningTracksTool = DerivationFramework__HITrackParticleThinningTool( name = 'HION11ThinningTracksTool',
                                                                            ThinningService = HION11ThinningHelper.ThinningSvc(),
                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
                                                                            PrimaryVertexKey = "PrimaryVertices",
                                                                            PrimaryVertexSelection = "sumPt2",
                                                                            TrackSelectionTool = HITrackSelector
                                                                          )

ToolSvc += HION11ThinningTracksTool
thinningTools.append(HION11ThinningTracksTool)



#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION11Kernel",
                          SkimmingTools     = [HION11SkimmingTriggerTool],
                          ThinningTools     = thinningTools)
HION11Stream.AcceptAlgs(["HION11Kernel"])




from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION11SlimmingHelper = SlimmingHelper("HION11SlimmingHelper")

HION11SlimmingHelper.SmartCollections = ["InDetTrackParticles", "PrimaryVertices", "Muons"]
HION11SlimmingHelper.AllVariables = ["CaloSums", "MSOnlyExtrapolatedMuonTrackParticles"]

if isSimulation:
    HION11SlimmingHelper.AllVariables += ["TruthParticles", "TruthVertices"]
else:
    HION11SlimmingHelper.AllVariables += ["HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScan",
                                           "HLT_CombinedMuonFeatureContainer",
                                           "HLT_MuonFeatureDetailsContainer",
                                           "HLT_xAOD__MuonContainer_MuTagIMO_EF",
                                           "HLT_xAOD__MuonContainer_MuonEFInfo",
                                           "HLT_xAOD__MuonContainer_MuonEFInfo_FullScan",
                                           "HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScan"] 
    HION11SlimmingHelper.IncludeMuonTriggerContent = True

HION11SlimmingHelper.AppendContentToStream(HION11Stream)

