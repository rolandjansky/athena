####################################################
### Setup:
###     sequencer
###     derivation framework / event selection
####################################################

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence() 
DESD_DEDX_Seq = CfgMgr.AthSequencer("DESD_DEDX_Seq")

xeTriggers = ['HLT_xe35',                                   
              'HLT_xe35_tc_lcw',                            
              'HLT_xe35_tc_em',                             
              'HLT_xe35_pueta',                             
              'HLT_xe35_pufit',                             
              'HLT_xe35_mht',                               
              'HLT_xe35_L2FS',                              
              'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu',           
              'HLT_xe50_cell',
              'HLT_xe50_pueta',
              'HLT_xe10',
              'HLT_xe70_pueta',
              'HLT_xe100_cell',
              'HLT_xe100_pueta',
              'HLT_xe70',
              'HLT_xe50']


selection = "count(InDetTrackParticles.pt > 50*GeV) > 0 || " + " || ".join(xeTriggers)

# Event selection tool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DESD_DEDX_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESD_DEDX_SkimmingTool",
                                                                        expression = selection)

ToolSvc += DESD_DEDX_SkimmingTool
print DESD_DEDX_SkimmingTool

# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESD_DEDX_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESD_DEDXKernel", SkimmingTools = [DESD_DEDX_SkimmingTool])
topSequence += DESD_DEDX_Seq 

##################
### Output stream
##################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDESD_DEDX.StreamName
fileName   = buildFileName( primDPD.WriteDESD_DEDX )

StreamDESD_DEDX = MSMgr.NewPoolStream(streamName, fileName)
StreamDESD_DEDX.AddRequireAlgs(["DESD_DEDXKernel"])


#########################################
# Add the containers to the output stream
#########################################
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput


# Take everything from the input, except this very long list:
ExcludeList=[ "xAOD::BTagVertexAuxContainer#*",
              "xAOD::PFOContainer#*",
              "CaloClusterCellLinkContainer#*",
              "xAOD::BTagVertexContainer#*",
              "xAOD::BTagVertexAuxContainer#*",              
              "xAOD::PhotonContainer#*",
              "xAOD::PhotonAuxContainer#*",
              "LArNoisyROSummary#*",
              "xAOD::JetRoIContainer#*",
              "CaloCellContainer#*",
              "xAOD::EmTauRoIContainer#*",
              "xAOD::PFOAuxContainer#*",
              "xAOD::BTaggingAuxContainer#*",
              "xAOD::BTaggingContainer#*",
              "xAOD::TauJetContainer#*",
              "xAOD::TauJetAuxContainer#*",
              "TileCellContainer#*",
              "InDet::TRT_DriftCircleContainer#*",
              "InDet::SCT_ClusterContainer#*",
              "McEventCollection#*",
              "xAOD::CaloClusterAuxContainer#*",
              "xAOD::CaloClusterContainer#*",
              "RpcPadContainer#*"]

# Excluding all but AntiKt4EMTopoJets
JetContainers = 'AntiKt10LCTopoJets, AntiKt10TruthJets, AntiKt10TruthWZJets, AntiKt2PV0TrackJets, AntiKt3PV0TrackJets, AntiKt4EMPFlowJets, AntiKt4LCTopoJets, AntiKt4PV0TrackJets, AntiKt4TruthJets, AntiKt4TruthWZJets, CamKt12LCTopoJets, CamKt12TruthJets, CamKt12TruthWZJets, HLT_xAOD__JetContainer_EFJet, HLT_xAOD__JetContainer_SplitJet, HLT_xAOD__JetContainer_SuperRoi, HLT_xAOD__JetContainer_TrigHLTJetDSSelectorCollection, HLT_xAOD__JetContainer_TrigTauJet, HLT_xAOD__JetContainer_a10TTemnojcalibFS, HLT_xAOD__JetContainer_a10TThadnojcalibFS, HLT_xAOD__JetContainer_a10r_tcemsubjesFS, HLT_xAOD__JetContainer_a10tcemjesFS, HLT_xAOD__JetContainer_a10tcemjesPS, HLT_xAOD__JetContainer_a10tcemnojcalibFS, HLT_xAOD__JetContainer_a10tcemnojcalibPS, HLT_xAOD__JetContainer_a10tcemsubFS, HLT_xAOD__JetContainer_a10tcemsubjesFS, HLT_xAOD__JetContainer_a10tclcwjesFS, HLT_xAOD__JetContainer_a10tclcwjesPS, HLT_xAOD__JetContainer_a10tclcwnojcalibFS, HLT_xAOD__JetContainer_a10tclcwnojcalibPS, HLT_xAOD__JetContainer_a10tclcwsubFS, HLT_xAOD__JetContainer_a10tclcwsubjesFS, HLT_xAOD__JetContainer_a4TTemnojcalibFS, HLT_xAOD__JetContainer_a4TThadnojcalibFS, HLT_xAOD__JetContainer_a4tcemjesFS, HLT_xAOD__JetContainer_a4tcemjesPS, HLT_xAOD__JetContainer_a4tcemnojcalibFS, HLT_xAOD__JetContainer_a4tcemnojcalibPS, HLT_xAOD__JetContainer_a4tcemsubFS, HLT_xAOD__JetContainer_a4tcemsubjesFS, HLT_xAOD__JetContainer_a4tclcwjesFS, HLT_xAOD__JetContainer_a4tclcwjesPS, HLT_xAOD__JetContainer_a4tclcwnojcalibFS, HLT_xAOD__JetContainer_a4tclcwnojcalibPS, HLT_xAOD__JetContainer_a4tclcwsubFS, HLT_xAOD__JetContainer_a4tclcwsubjesFS'

JetAuxContainers = 'AntiKt10LCTopoJetsAux., AntiKt10TruthJetsAux., AntiKt10TruthWZJetsAux., AntiKt2PV0TrackJetsAux., AntiKt3PV0TrackJetsAux., AntiKt4EMPFlowJetsAux., AntiKt4LCTopoJetsAux., AntiKt4PV0TrackJetsAux., AntiKt4TruthJetsAux., AntiKt4TruthWZJetsAux., CamKt12LCTopoJetsAux., CamKt12TruthJetsAux., CamKt12TruthWZJetsAux.'
              
TrackCollections = 'CombinedMuonTracks, ExtrapolatedMuonTracks, GSFTracks, HLT_TrackCollection_InDetTrigTrackSlimmer_Bjet_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_Bphysics_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_CosmicsN_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_Electron_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_FullScan_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_Photon_EFID, HLT_TrackCollection_InDetTrigTrackSlimmer_Tau_EFID, MuonSpectrometerTracks, ResolvedForwardTracks'


ExcludeList += ["xAOD::JetContainer#"+cont for cont in JetContainers.split(", ")]
ExcludeList += ["xAOD::JetAuxContainer#"+cont for cont in JetAuxContainers.split(", ")]
ExcludeList += ["TrackCollection#"+cont for cont in TrackCollections.split(", ")]
    

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
