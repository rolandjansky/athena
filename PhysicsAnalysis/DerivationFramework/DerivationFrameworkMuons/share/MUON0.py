#====================================================================
# MUON0.py for events passed muon triggers
# This requires the reductionConf flag MUON0 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
# include("DerivationFrameworkCore/DerivationFrameworkMaster.py")
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
# from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON0Stream )
MUON0Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON0Stream.AcceptAlgs(["MUON0Kernel"])

# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()

triggerList1 = ['HLT_.*mu\d+.*', 'L1_.*MU\d+.*', 'HLT_noalg_L1.*MU\d+.*']
#triggerList1 = []
triggerList = ['HLT_.*mu.*', 'L1_.*MU.*','HLT_noalg_L1.*MU.*']

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
MUON0ThinningHelper = ThinningHelper( "MUON0ThinningHelper" )
MUON0ThinningHelper.TriggerChains = '|'.join(triggerList1)
printfunc (MUON0ThinningHelper.TriggerChains)
MUON0ThinningHelper.AppendToStream( MUON0Stream )
thinningHelperTool = getattr( ToolSvc, "MUON0ThinningHelperSlim" )
thinningHelperTool.FeatureInclusionList += ['HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo','HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux.','HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo','HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux.','HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo','HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux.','HLT_TrigRoiDescriptorCollection_forMS','HLT_TrigRoiDescriptorCollection_forMSAux.','HLT_TrigRoiDescriptorCollection_forID','HLT_TrigRoiDescriptorCollection_forIDAux.']



#====================================================================
# AUGMENTATION TOOLS 
#====================================================================

#====================================================================
# STRING BASED SKIMMING TOOL 
#====================================================================
MUON0_skimming_tools = []

### trigger seleciton
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
MUON0SkimmingTool0a = DerivationFramework__TriggerSkimmingTool(name = "MUON0SkimmingTool0a",
                                                              TriggerListOR = triggerList,
                                                              TriggerListAND = [])
ToolSvc += MUON0SkimmingTool0a

### muon selection
expression = 'count(Muons.pt>30*GeV)>0'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON0SkimmingTool0b = DerivationFramework__xAODStringSkimmingTool(name = "MUON0SkimmingTool0b",
                                                                 expression = expression)
ToolSvc += MUON0SkimmingTool0b

### OR combination
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
MUON0SkimmingTool0 = DerivationFramework__FilterCombinationOR(name="MUON0SkimmingTool0",
                                                              FilterList=[MUON0SkimmingTool0a, MUON0SkimmingTool0b])
ToolSvc += MUON0SkimmingTool0

### adding the combined tool
MUON0_skimming_tools.append(MUON0SkimmingTool0)

#====================================================================
# THINNING TOOL 
#====================================================================
# Tracks associated with Muons
MUON0_thinning_tools = []

## keep only tracks near muon
thinning_expression2 = "Muons.pt > 4*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON0ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON0ThinningTool2",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.4,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON0ThinningTool2
MUON0_thinning_tools.append(MUON0ThinningTool2)

### also for forward tracks
thinning_expression3 = "Muons.muonType==4"
MUON0ThinningTool2f = DerivationFramework__MuonTrackParticleThinning(name                   = "MUON0ThinningTool2f",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression3,
                                                                    ConeSize                = 0.5,
                                                                    InDetTrackParticlesKey  = "InDetForwardTrackParticles")
ToolSvc += MUON0ThinningTool2f
MUON0_thinning_tools.append(MUON0ThinningTool2f)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON0Kernel",
# 									AugmentationTools = [],
                                                                        ThinningTools = MUON0_thinning_tools,
                                                                        SkimmingTools = MUON0_skimming_tools
                                                                      )

#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON0Stream, 'MUON0')
