##-----------------------------------------------------------------------------
## Name: PerfDESDM_MS.py
##
## Author: Saminder Dhaliwal 
## Email:  s.dhaliwal@cern.ch
##
## Description: This defines the content of the commissioning stream for
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDESDM_MS.py")


## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PerfDESDM_MSStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec
## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDESDMFlags_PerfMS import primDPDAlignTrigMu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= DEBUG:
    muonDPDStream_msg.info('Values of all PerfDESDM_MS flags:')
    print primDPDAlignTrigMu
    pass


# ============================================================================
# Check if xAOD muon collection is available
# ============================================================================
from RecExConfig.ObjKeyStore import objKeyStore
hasMuons = False

if objKeyStore.isInInput("xAOD::MuonContainer", "Muons") \
       or objKeyStore['transient'].has_item("xAOD::MuonContainer#Muons"):
    hasMuons = True
    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdAlignmentTriggerMuonSequence = CfgMgr.AthSequencer( 'DESDM_MSPERF_Sequence',
                                                        StopOverride = True )
topSequence += desdAlignmentTriggerMuonSequence

##============================================================================
## Define the skimming (event selection) for the DESDM_MSPERF output stream
##============================================================================
#include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
        
# Define the some lists
commandstring = ""
algsToBookkeep = []


from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  objKeyStore['transient'].list() )
#print "Items in objKeyStore[transient]:", objKeyStore['transient'].list()

# ------------------------
# String based reduction
# ------------------------

#Retain events with atleast one muon (this is neccessary for orthoganal triggers, not for mu triggers)
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool

EventStringSkimmingTool1_sub = DerivationFramework__xAODStringSkimmingTool(name = "EventStringSkimmingTool1_sub",
                                                                expression = "(count(Muons.muonType == 0 && Muons.pt > 4*GeV && Muons.pt < 15*GeV) > 0)")
ToolSvc += EventStringSkimmingTool1_sub
print EventStringSkimmingTool1_sub

LowpTMuonPrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "LowpTMuonPrescaleSkimmingTool",
                                                                    Prescale = 10)
ToolSvc += LowpTMuonPrescaleSkimmingTool
print LowpTMuonPrescaleSkimmingTool

EventStringSkimmingTool1 = DerivationFramework__FilterCombinationAND(name="EventStringSkimmingTool1",FilterList=[EventStringSkimmingTool1_sub, LowpTMuonPrescaleSkimmingTool])
ToolSvc += EventStringSkimmingTool1
print EventStringSkimmingTool1

EventStringSkimmingTool2 = DerivationFramework__xAODStringSkimmingTool(name = "EventStringSkimmingTool2",
                                                                expression = "(count(Muons.muonType == 0 && Muons.pt >= 15*GeV) > 0)")
ToolSvc += EventStringSkimmingTool2
print EventStringSkimmingTool2

EventStringSkimmingTool=DerivationFramework__FilterCombinationOR(name="EventStringSkimmingTool",FilterList=[EventStringSkimmingTool1, EventStringSkimmingTool2])
ToolSvc+=EventStringSkimmingTool
print EventStringSkimmingTool


#Trigger selection
#Muon triggers:
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
MuonTrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "MuonTrigSkimmingTool",
                                                                   TriggerListOR =  ["HLT_noalg_L1MU4", "HLT_noalg_L1MU6", "HLT_noalg_L1MU10", 
                                                                                     "HLT_noalg_L1MU15", "HLT_noalg_L1MU20", "HLT_noalg_L12MU4", 
                                                                                     "HLT_noalg_L12MU6", "HLT_noalg_L12MU10"] )
ToolSvc += MuonTrigSkimmingTool
print MuonTrigSkimmingTool

JPsiTrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "JPsiTrigSkimmingTool",
                                                                   TriggerListOR = ["HLT_mu20_2mu0noL1_JpsimumuFS", "HLT_mu18_2mu0noL1_JpsimumuFS", 
                                                                                    "HLT_mu20_2mu4_JpsimumuL2","HLT_mu18_2mu4_JpsimumuL2", 
                                                                                    "HLT_mu6_bJpsi_Trkloose", "HLT_mu18_bJpsi_Trkloose", 
                                                                                    "HLT_mu4_bJpsi_Trkloose", "HLT_mu20_msonly_mu6noL1_msonly_nscan05"] )
ToolSvc += JPsiTrigSkimmingTool
print JPsiTrigSkimmingTool

#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
JPsiTrigPrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "JPsiTrigPrescaleSkimmingTool",
                                                                    Prescale = 1)
ToolSvc += JPsiTrigPrescaleSkimmingTool

#Orthogonal triggers
OrthoTrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "OrthoTrigSkimmingTool",
                                                                    TriggerListOR = ["HLT_j.*", "HLT_[0-9]j.*",
                                                                                     "HLT_ht.*", "HLT_xe.*",
                                                                                     "HLT_te.*", "HLT_tau.*",
                                                                                     "HLT_xs.*", "HLT_g.*",
                                                                                     "HLT_[0-9]g.*","HLT_[0-9]e.*",
                                                                                     "HLT_e.*", "HLT_b.*",
                                                                                     "HLT_mb.*", "HLT_noalg_mb.*",
                                                                                     "HLT_noalg_L1J30.0ETA49_2J20.0ETA49",
                                                                                     "HLT_noalg_L1MJJ-100"] )

ToolSvc += OrthoTrigSkimmingTool
print OrthoTrigSkimmingTool

JPsiFilterTool = DerivationFramework__FilterCombinationAND(name="JPsiFilterTool",FilterList=[JPsiTrigPrescaleSkimmingTool, JPsiTrigSkimmingTool])
ToolSvc += JPsiFilterTool
print JPsiFilterTool

EventFilterTool_sub=DerivationFramework__FilterCombinationAND(name="EventFilterTool_sub",FilterList=[EventStringSkimmingTool, OrthoTrigSkimmingTool])
ToolSvc+=EventFilterTool_sub
print EventFilterTool_sub

EventFilterTool=DerivationFramework__FilterCombinationOR(name="EventFilterTool",FilterList=[EventFilterTool_sub, JPsiFilterTool, MuonTrigSkimmingTool])
ToolSvc+=EventFilterTool
print EventFilterTool

desdAlignmentTriggerMuonSequence+=CfgMgr.DerivationFramework__DerivationKernel("EventSkimmingKernel", SkimmingTools=[EventFilterTool])


# ------------------------
# Create Track filters
# ------------------------
if hasMuons:
    from PrimaryDPDMaker.PrimaryDPDMakerConf import muonTrkTrackThinTool
    filAlg_MS=muonTrkTrackThinTool("muonTrkTrackThinTool_MS", TrackCollectionKey = "MuonSpectrometerTracks")
    desdAlignmentTriggerMuonSequence+=filAlg_MS

    filAlg_CT=muonTrkTrackThinTool("muonTrkTrackThinTool_CT", TrackCollectionKey = "CombinedMuonTracks")
    desdAlignmentTriggerMuonSequence+=filAlg_CT

    print topSequence
        
##====================================================================
## Define this Muon DPD output stream
##====================================================================
#WriteMuonAlignmentTriggerStream
streamName = primDPD.WriteDESDM_MSPerfStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_MSPerfStream )
if primDPD.WriteDESDM_MSPerfStream.isVirtual or primDPD.isVirtual() :
    AlignmentTriggerMuonStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    AlignmentTriggerMuonStream = MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    AlignmentTriggerMuonStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPDAlignTrigMu.ApplyThinning():
    AlignmentTriggerMuonStream.AcceptAlgs(["muonTrkTrackThinTool_MS"])
#    AlignmentTriggerMuonStream.AcceptAlgs(["muonTrkTrackThinTool_CT"])
    pass

if primDPDAlignTrigMu.ApplySkimming():
    AlignmentTriggerMuonStream.AcceptAlgs(["EventSkimmingKernel"])

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#old way
#AlignmentTriggerMuonStream.Stream.TakeItemsFromInput = True
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

#General
AlignmentTriggerMuonStream.AddItem(["xAOD::EventInfo#*"])
AlignmentTriggerMuonStream.AddItem(["xAOD::EventAuxInfo#*"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonContainer#Muons"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonAuxContainer#MuonsAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonSegmentContainer#MuonSegments"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonSegmentAuxContainer#MuonSegmentsAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#CombinedMuonTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux."])
#Trigger General
AlignmentTriggerMuonStream.AddItem(["xAOD::TrigNavigation#TrigNavigation"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrigNavigationAuxInfo#TrigNavigationAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrigDecision#xTrigDecision"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrigDecisionAuxInfo#xTrigDecisionAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonRoIContainer#LVL1MuonRoIs"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux."])
AlignmentTriggerMuonStream.AddItem(["Muon::RpcPrepDataContainer#*"])
AlignmentTriggerMuonStream.AddItem(["Muon::TgcPrepDataContainer#*"])
AlignmentTriggerMuonStream.AddItem(["Muon::CscPrepDataContainer#*"])
AlignmentTriggerMuonStream.AddItem(["Muon::MdtPrepDataContainer#*"])
#Alignment
AlignmentTriggerMuonStream.AddItem(["Trk::SegmentCollection#MuonSegments"])
AlignmentTriggerMuonStream.AddItem(["xAOD::VertexContainer#PrimaryVertices"])
AlignmentTriggerMuonStream.AddItem(["xAOD::VertexAuxContainer#PrimaryVerticesAux."])
AlignmentTriggerMuonStream.AddItem(["TrackCollection#MuonSpectrometerTracks"])
AlignmentTriggerMuonStream.AddItem(["TrackCollection#CombinedMuonTracks"])
#AlignmentTriggerMuonStream.AddItem(["TrackCollection#Tracks"])
#RPC
AlignmentTriggerMuonStream.AddItem(["MuCTPI_RDO#MUCTPI_RDO"])
AlignmentTriggerMuonStream.AddItem(["RpcPadContainer#RPCPAD"])
AlignmentTriggerMuonStream.AddItem(["Muon::RpcPrepDataContainer#RPC_Measurements"])
AlignmentTriggerMuonStream.AddItem(["Muon::RpcCoinDataContainer#RPC_triggerHits"])
AlignmentTriggerMuonStream.AddItem(["RpcSectorLogicContainer#RPC_SECTORLOGIC"])
#TGC
AlignmentTriggerMuonStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC"])
AlignmentTriggerMuonStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC"])
AlignmentTriggerMuonStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollection"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TruthParticleContainer#MuonTruthParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TruthParticleAuxContainer#MuonTruthParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#InDetTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."])
#HLT
AlignmentTriggerMuonStream.AddItem(["CosmicMuonCollection#HLT_CosmicMuons"])
AlignmentTriggerMuonStream.AddItem(["xAOD::L2StandAloneMuonContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo"])
AlignmentTriggerMuonStream.AddItem(["xAOD::L2CombinedMuonContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo"])
AlignmentTriggerMuonStream.AddItem(["xAOD::L2IsoMuonContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuTagIMO_EF"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonContainer#HLT_xAOD__MuonContainer_eMuonEFInfo"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonRoIContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuon"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTF"])
AlignmentTriggerMuonStream.AddItem(["xAOD::L2StandAloneMuonAuxContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::L2CombinedMuonAuxContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux."])
#AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux."]) #No Aux container?
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuTagIMO_EFAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_eMuonEFInfoAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonRoIAuxContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuonAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticlesAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTFAux."])
