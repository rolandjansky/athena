##-----------------------------------------------------------------------------
## Name: PerfDPD_Jet.py
##
## Author: Christopher Young (CERN)
## Email:  christopher.young@cern.ch
## Modified: Quentin Buat (Adding tau part)
## Email: quentin.buat@cern.ch
##
## Description: This defines the content of the CalJet DPD output stream.
##
##-----------------------------------------------------------------------------

# Sequence
from AthenaCommon.AlgSequence import AlgSequence 
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
topSequence = AlgSequence() 
DESDM_CALJET_Seq = CfgMgr.AthSequencer("DESDM_CALJET_Seq")


##============================================================================
## Define the skimming (event selection) for the DESDM_CALJET output stream
##============================================================================
# Object selection strings
sel_jet600 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 600.*GeV'
sel_jet800 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 800.*GeV'
sel_jet1000 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 1000.*GeV'

# Event selection string
desd_jetA = '(HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300)'
desd_jetB = '( HLT_j320 )'
desd_jetC = '( HLT_j360 ||  HLT_j380 || HLT_j400 )'
desd_jetD = '( HLT_j420 && !HLT_j460 )'
desd_jetE = '( HLT_j460 )'
desd_jetF = '( HLT_j460 && count('+sel_jet600+')>0 && count('+sel_jet800+')==0 )'
desd_jetG = '( HLT_j460 && count('+sel_jet800+')>0 && count('+sel_jet1000+')==0 )'
desd_jetH = '( HLT_j460 && count('+sel_jet1000+')>0 )'

prescaleA = 20
prescaleB = 10
prescaleC = 40
prescaleD = 30
prescaleE = 20
prescaleF = 10
prescaleG = 5
prescaleH = 1


# Event selection tool

DESDM_CALJET_SkimmingToolA = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolA", expression = desd_jetA)
ToolSvc += DESDM_CALJET_SkimmingToolA
DESDM_CALJET_PrescaleToolA = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolA",Prescale=prescaleA)
ToolSvc += DESDM_CALJET_PrescaleToolA
DESDM_CALJET_ANDToolA = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolA",FilterList=[DESDM_CALJET_SkimmingToolA,DESDM_CALJET_PrescaleToolA] )
ToolSvc += DESDM_CALJET_ANDToolA

DESDM_CALJET_SkimmingToolB = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolB", expression = desd_jetB)
ToolSvc += DESDM_CALJET_SkimmingToolB
DESDM_CALJET_PrescaleToolB = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolB",Prescale=prescaleB)
ToolSvc += DESDM_CALJET_PrescaleToolB
DESDM_CALJET_ANDToolB = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolB",FilterList=[DESDM_CALJET_SkimmingToolB,DESDM_CALJET_PrescaleToolB] )
ToolSvc += DESDM_CALJET_ANDToolB

DESDM_CALJET_SkimmingToolC = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolC", expression = desd_jetC)
ToolSvc += DESDM_CALJET_SkimmingToolC
DESDM_CALJET_PrescaleToolC = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolC",Prescale=prescaleC)
ToolSvc += DESDM_CALJET_PrescaleToolC
DESDM_CALJET_ANDToolC = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolC",FilterList=[DESDM_CALJET_SkimmingToolC,DESDM_CALJET_PrescaleToolC] )
ToolSvc += DESDM_CALJET_ANDToolC

DESDM_CALJET_SkimmingToolD = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolD", expression = desd_jetD)
ToolSvc += DESDM_CALJET_SkimmingToolD
DESDM_CALJET_PrescaleToolD = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolD",Prescale=prescaleD)
ToolSvc += DESDM_CALJET_PrescaleToolD
DESDM_CALJET_ANDToolD = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolD",FilterList=[DESDM_CALJET_SkimmingToolD,DESDM_CALJET_PrescaleToolD] )
ToolSvc += DESDM_CALJET_ANDToolD

DESDM_CALJET_SkimmingToolE = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolE", expression = desd_jetE)
ToolSvc += DESDM_CALJET_SkimmingToolE
DESDM_CALJET_PrescaleToolE = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolE",Prescale=prescaleE)
ToolSvc += DESDM_CALJET_PrescaleToolE
DESDM_CALJET_ANDToolE = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolE",FilterList=[DESDM_CALJET_SkimmingToolE,DESDM_CALJET_PrescaleToolE] )
ToolSvc += DESDM_CALJET_ANDToolE

DESDM_CALJET_SkimmingToolF = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolF", expression = desd_jetF)
ToolSvc += DESDM_CALJET_SkimmingToolF
DESDM_CALJET_PrescaleToolF = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolF",Prescale=prescaleF)
ToolSvc += DESDM_CALJET_PrescaleToolF
DESDM_CALJET_ANDToolF = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolF",FilterList=[DESDM_CALJET_SkimmingToolF,DESDM_CALJET_PrescaleToolF] )
ToolSvc += DESDM_CALJET_ANDToolF

DESDM_CALJET_SkimmingToolG = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolG", expression = desd_jetG)
ToolSvc += DESDM_CALJET_SkimmingToolG
DESDM_CALJET_PrescaleToolG = DerivationFramework__PrescaleTool(name="DESDM_CALJET_PrescaleToolG",Prescale=prescaleG)
ToolSvc += DESDM_CALJET_PrescaleToolG
DESDM_CALJET_ANDToolG = DerivationFramework__FilterCombinationAND(name="DESDM_CALJET_ANDToolG",FilterList=[DESDM_CALJET_SkimmingToolG,DESDM_CALJET_PrescaleToolG] )
ToolSvc += DESDM_CALJET_ANDToolG

DESDM_CALJET_SkimmingToolH = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_CALJET_SkimmingToolH", expression = desd_jetH)
ToolSvc += DESDM_CALJET_SkimmingToolH



##============================================================================
## Define the skimming for the DESDM_CALJET output stream for the taus
##============================================================================

# Object selection strings
sel_tau = '(TauJets.pt > 20.0*GeV) && (abs(TauJets.eta) < 2.6) && (TauJets.nTracks == 1 || TauJets.nTracks == 3)'

prescale_rules = {
    'HLT_j15': 1,
    'HLT_j25': 1,
    'HLT_j35': 3,
    'HLT_j45': 4,
    'HLT_j55': 4,
    'HLT_j85': 8,
    'HLT_j110': 20,
    'HLT_j175': 20,
    'HLT_j260': 20,
    'HLT_j360': 400,
}


# Define skimming and prescale tool for each trigger
TauCombTools = []
for trigger in prescale_rules.keys():
    expression = '{0} && count({1}) >= 1'.format(
        trigger, sel_tau)

    # skimming tool
    skimming_tool_name = "DESDM_CALJET_TauSkimmingTool_{0}".format(trigger)
    TauSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
        name=skimming_tool_name, expression=expression)
    ToolSvc += TauSkimmingTool

    # prescale tool
    prescale_tool_name = "DESDM_CALJET_TauPrescaleTool_{0}".format(trigger)
    TauPrescaleTool = DerivationFramework__PrescaleTool(
        name=prescale_tool_name, Prescale=prescale_rules[trigger])
    ToolSvc += TauPrescaleTool

    # Skimming AND Prescale
    TauCombTool = DerivationFramework__FilterCombinationAND(
        name="DESDM_CALJET_Tau_{0}".format(trigger),
        FilterList=[TauSkimmingTool, TauPrescaleTool])
    TauCombTools.append(TauCombTool)
    ToolSvc += TauCombTool


filter_list = [
    DESDM_CALJET_ANDToolA,
    DESDM_CALJET_ANDToolB,
    DESDM_CALJET_ANDToolC,
    DESDM_CALJET_ANDToolD,
    DESDM_CALJET_ANDToolE,
    DESDM_CALJET_ANDToolF,
    DESDM_CALJET_ANDToolG,
    DESDM_CALJET_SkimmingToolH
]

filter_list += TauCombTools
DESDM_CALJET_ORTool = DerivationFramework__FilterCombinationOR(
    name="TauCalTauTool", FilterList=filter_list)
ToolSvc += DESDM_CALJET_ORTool
print DESDM_CALJET_ORTool





# Kernel algorithm
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DESDM_CALJET_Seq += CfgMgr.DerivationFramework__DerivationKernel("DESDM_CALJETKernel",
                                                               SkimmingTools = [DESDM_CALJET_ORTool]
                                                               )
topSequence += DESDM_CALJET_Seq 

##====================================================================
## Define the Jet DPD output stream
##====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteDESDM_CALJETStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_CALJETStream )
if primDPD.WriteDESDM_CALJETStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_CALJET=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_CALJET=MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_CALJET.AddRequireAlgs(["DESDM_CALJETKernel"])

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#JetStream.Stream.TakeItemsFromInput = True
ExcludeList=[ "TrackCollection#Tracks",
              "InDet::PixelClusterContainer#PixelClusters",
              "InDet::SCT_ClusterContainer#SCT_Clusters",
              "InDet::TRT_DriftCircleContainer#TRT_DriftCircles",
              "PRD_MultiTruthCollection#PRD_MultiTruthTRT",
              "PRD_MultiTruthCollection#CSC_TruthMap",
              "PRD_MultiTruthCollection#MDT_TruthMap",
              "PRD_MultiTruthCollection#PRD_MultiTruthPixel",
              "PRD_MultiTruthCollection#PRD_MultiTruthSCT",
              "PRD_MultiTruthCollection#RPC_TruthMap",
              "PRD_MultiTruthCollection#TGC_TruthMap",
              "Muon::CscStripPrepDataContainer#CSC_Measurements",
              "Muon::RpcPrepDataContainer#RPC_Measurements",
              "Muon::TgcPrepDataContainer#TGC_Measurements",
              "Muon::TgcPrepDataContainer#TGC_MeasurementsNextBC",
              "Muon::TgcPrepDataContainer#TGC_MeasurementsPriorBC",
              "Muon::MdtPrepDataContainer#MDT_DriftCircles",
              "Muon::RpcCoinDataContainer#RPC_triggerHits",
              "MuonSimDataCollection#MDT_SDO",
              "MuonSimDataCollection#RPC_SDO",
              "MuonSimDataCollection#TGC_SDO",
              "CscSimDataCollection#CSC_SDO",
              "CscRawDataContainer#CSCRDO",
              "Muon::CscPrepDataContainer#CSC_Clusters",
              "InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap",
              "InDet::PixelGangedClusterAmbiguities#SplitClusterAmbiguityMap",
              "MdtCsmContainer#MDTCSM",
              # not sure if the detailed track truth will be useful?
              "DetailedTrackTruthCollection#ConvertedMBoyMuonSpectroOnlyTracksTruth",
              "DetailedTrackTruthCollection#ConvertedMBoyTracksTruth",
              "DetailedTrackTruthCollection#DetailedTrackTruth",
              "DetailedTrackTruthCollection#MooreTracksTruth",
              "DetailedTrackTruthCollection#MuonSpectrometerTracksTruth",
              "DetailedTrackTruthCollection#ResolvedForwardTracksDetailedTruth",
              ]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)


