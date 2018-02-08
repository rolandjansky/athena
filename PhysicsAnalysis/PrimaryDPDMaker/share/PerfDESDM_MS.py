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

from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  objKeyStore['transient'].list() )
#print "Items in objKeyStore[transient]:", objKeyStore['transient'].list()

# ------------------------------------------------
# String based reduction
# ------------------------------------------------
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool

# ------------------------------------------------
# All 'noalg' muon triggers
# ------------------------------------------------
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
MuonNoAlgTrig_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "MuonNoAlgTrig_TriggerSkimmingTool",
                                                                                TriggerListOR =  ["HLT_noalg_L1MU4", "HLT_noalg_L1MU6", "HLT_noalg_L1MU10",
                                                                                                  "HLT_noalg_L1MU11","HLT_noalg_L1MU15","HLT_noalg_L1MU20",
                                                                                                  "HLT_noalg_L12MU4","HLT_noalg_L12MU6","HLT_noalg_L12MU10"] )
ToolSvc += MuonNoAlgTrig_TriggerSkimmingTool
print MuonNoAlgTrig_TriggerSkimmingTool

MuonNoAlgTrig_PrescaleSkimmingTool = DerivationFramework__PrescaleTool ( name = "MuonNoAlgTrig_PrescaleSkimmingTool",
                                                                         Prescale = 1)

ToolSvc += MuonNoAlgTrig_PrescaleSkimmingTool
print MuonNoAlgTrig_PrescaleSkimmingTool


MuonNoAlgTrig_EventSkimmingTool = DerivationFramework__FilterCombinationAND(name="MuonNoAlgTrig_EventSkimmingTool",FilterList=[MuonNoAlgTrig_TriggerSkimmingTool, MuonNoAlgTrig_PrescaleSkimmingTool])
ToolSvc += MuonNoAlgTrig_EventSkimmingTool
print MuonNoAlgTrig_EventSkimmingTool

# ------------------------------------------------
# Orthogonal triggers (RPC needs)
# ------------------------------------------------
OrthoTrig_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "OrthoTrig_TriggerSkimmingTool",
                                                                            TriggerListOR = ["HLT_xe.*",
                                                                                             "HLT_[0-9]?j[0-9]*",
                                                                                             "HLT_j[0-9]*_(jes|lcw|nojcalib|sub|L1RD0|280eta320|320eta490).*",
                                                                                             "HLT_[0-9]?j[0-9]*_b.*|j[0-9]*_[0-9]j[0-9]*_b.*",
                                                                                             "HLT_tau.*",
                                                                                             "HLT_[0-9]?e[0-9]*_(iloose|loose|medium|lhloose|lhmedium|lhtight|etcut)",
                                                                                             "HLT_[0-9]?e[0-9]*_(iloose|loose|medium|lhloose|lhmedium|lhtight|etcut)_(iloose|nod0|HLTCalo|cu\
td0dphideta|smooth|L1EM[0-9]*VH|L1EM[0-9]*)",
                                                                                             "HLT_[0-9]?e[0-9]*_(iloose|loose|medium|lhloose|lhmedium|lhtight)_(iloose|nod0|HLTCalo|cutd0dph\
ideta|smooth)_(HLTCalo|iloose|L1EM[0-9]*VH|L1EM[0-9]*)",
                                                                                             "HLT_[0-9]?g[0-9]*_(loose|medium|tight|etcut)",
                                                                                             "HLT_g[0-9]*_(loose|etcut)_(L1EM[0-9]*|L1EM[0-9]*VH)",
                                                                                             "HLT_(e|g)[0-9]*_(loose|medium|lhmedium|tight)_g.*",
                                                                                             "HLT_ht.*",
                                                                                             "HLT_te.*",
                                                                                             "HLT_xs.*",
                                                                                             "HLT_mb.*"] )

ToolSvc += OrthoTrig_TriggerSkimmingTool
print OrthoTrig_TriggerSkimmingTool

# ------------------------------------------------
# Offline orthogonal trigger selection (low pT)

OrthoTrig_LowpTMuonEventStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "OrthoTrig_LowpTMuonEventStringSkimmingTool",
                                                                                         expression = "(count(Muons.muonType == 0 && Muons.pt > 4*GeV && Muons.pt < 15*GeV) > 0)")
ToolSvc += OrthoTrig_LowpTMuonEventStringSkimmingTool
print OrthoTrig_LowpTMuonEventStringSkimmingTool

OrthoTrig_LowpTMuonPrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "OrthoTrig_LowpTMuonPrescaleSkimmingTool",
                                                                               Prescale = 10)
ToolSvc += OrthoTrig_LowpTMuonPrescaleSkimmingTool
print OrthoTrig_LowpTMuonPrescaleSkimmingTool

OrthoTrig_LowpTMuonEventSkimmingTool = DerivationFramework__FilterCombinationAND(name="OrthoTrig_LowpTMuonEventSkimmingTool",FilterList=[OrthoTrig_LowpTMuonEventStringSkimmingTool, OrthoTrig_LowpTMuonPrescaleSkimmingTool])

ToolSvc += OrthoTrig_LowpTMuonEventSkimmingTool
print OrthoTrig_LowpTMuonEventSkimmingTool

# ------------------------------------------------
# Offline orthogonal trigger selection (mid pT)

OrthoTrig_MidpTMuonEventStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "OrthoTrig_MidpTMuonEventStringSkimmingTool",
                                                                                         expression = "(count(Muons.muonType == 0 && Muons.pt >= 15*GeV) > 0)")

ToolSvc += OrthoTrig_MidpTMuonEventStringSkimmingTool
print OrthoTrig_MidpTMuonEventStringSkimmingTool

OrthoTrig_MidpTMuonPrescaleSkimmingTool = DerivationFramework__PrescaleTool ( name = "OrthoTrig_MidpTMuonPrescaleSkimmingTool",
                                                                              Prescale = 1)

ToolSvc += OrthoTrig_MidpTMuonPrescaleSkimmingTool
print OrthoTrig_MidpTMuonPrescaleSkimmingTool

OrthoTrig_MidpTMuonEventSkimmingTool = DerivationFramework__FilterCombinationAND(name="OrthoTrig_MidpTMuonEventSkimmingTool",FilterList=[OrthoTrig_MidpTMuonEventStringSkimmingTool, OrthoTrig_MidpTMuonPrescaleSkimmingTool])
ToolSvc += OrthoTrig_MidpTMuonEventSkimmingTool
print OrthoTrig_MidpTMuonEventSkimmingTool

# ------------------------------------------------
# Ortho trig events accepted if there is: 
# [[a low pt (+ prescale) muon] OR [mid pt muon]] 
# && [orthogonal trigger] event

OrthoTrig_pTMuonEventSkimmingTool=DerivationFramework__FilterCombinationOR(name="OrthoTrig_pTMuonEventSkimmingTool",FilterList=[OrthoTrig_LowpTMuonEventSkimmingTool, OrthoTrig_MidpTMuonEventSkimmingTool])
ToolSvc += OrthoTrig_pTMuonEventSkimmingTool
print OrthoTrig_pTMuonEventSkimmingTool

OrthoTrig_EventSkimmingTool=DerivationFramework__FilterCombinationAND(name="OrthoTrig_EventSkimmingTool",FilterList=[OrthoTrig_pTMuonEventSkimmingTool, OrthoTrig_TriggerSkimmingTool])

ToolSvc += OrthoTrig_EventSkimmingTool
print OrthoTrig_EventSkimmingTool

# ------------------------------------------------
# JPsi Trigger selection
# ------------------------------------------------
JPsiTrig_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "JPsiTrig_TriggerSkimmingTool",
                                                                           TriggerListOR = ["HLT_mu20_2mu0noL1_JpsimumuFS", "HLT_mu18_2mu0noL1_JpsimumuFS", 
                                                                                            "HLT_mu20_2mu4_JpsimumuL2","HLT_mu18_2mu4_JpsimumuL2", 
                                                                                            "HLT_mu6_bJpsi_Trkloose", "HLT_mu18_bJpsi_Trkloose", 
                                                                                            "HLT_mu4_bJpsi_Trkloose", "HLT_mu20_msonly_mu6noL1_msonly_nscan05"] )
ToolSvc += JPsiTrig_TriggerSkimmingTool
print JPsiTrig_TriggerSkimmingTool

JPsiTrig_PrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "JPsiTrig_PrescaleSkimmingTool",
                                                                     Prescale = 2)
ToolSvc += JPsiTrig_PrescaleSkimmingTool
print JPsiTrig_PrescaleSkimmingTool

# ------------------------------------------------
# Jpsi trig events accepted if there is:
# Jpsi trigger (+ prescale)
JpsiTrig_EventSkimmingTool = DerivationFramework__FilterCombinationAND(name="JpsiTrig_EventSkimmingTool",FilterList=[JPsiTrig_PrescaleSkimmingTool, JPsiTrig_TriggerSkimmingTool])

ToolSvc += JpsiTrig_EventSkimmingTool
print JpsiTrig_EventSkimmingTool

# ------------------------------------------------
#For TGC, Muon HLT triggers:
# ------------------------------------------------
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
MuonHLTTrig_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "MuonHLTTrig_TriggerSkimmingTool",
                                                                              TriggerListOR =  ["HLT_mu.*"] )

ToolSvc += MuonHLTTrig_TriggerSkimmingTool
print MuonHLTTrig_TriggerSkimmingTool

# ------------------------------------------------
# Dimuon selection
MuonHLTTrig_DiMuonEventStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "MuonHLTTrig_DiMuonEventStringSkimmingTool",
                                                                                        expression = "( (count(Muons.muonType == 0 && Muons.pt > 25*GeV)  > 0) && (count(Muons.muonType == 0 && Muons.pt > 10*GeV)  > 1) )")
ToolSvc += MuonHLTTrig_DiMuonEventStringSkimmingTool
print MuonHLTTrig_DiMuonEventStringSkimmingTool

MuonHLTTrig_DiMuonEventPrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "MuonHLTTrig_DiMuonEventPrescaleSkimmingTool",
                                                                                   Prescale = 1)
ToolSvc += MuonHLTTrig_DiMuonEventPrescaleSkimmingTool
print MuonHLTTrig_DiMuonEventPrescaleSkimmingTool

MuonHLTTrig_DiMuonEventSkimmingTool = DerivationFramework__FilterCombinationAND(name="MuonHLTTrig_DiMuonEventSkimmingTool",FilterList=[MuonHLTTrig_DiMuonEventStringSkimmingTool,\
                                                                                                                                           MuonHLTTrig_DiMuonEventPrescaleSkimmingTool])
ToolSvc += MuonHLTTrig_DiMuonEventSkimmingTool
print MuonHLTTrig_DiMuonEventSkimmingTool

# ------------------------------------------------
# Mid pT muon selection
MuonHLTTrig_MidpTEventStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "MuonHLTTrig_MidpTEventStringSkimmingTool",
                                                                                       expression = "(count(Muons.muonType == 0 && Muons.pt > 25*GeV) > 0)")
ToolSvc += MuonHLTTrig_MidpTEventStringSkimmingTool
print MuonHLTTrig_MidpTEventStringSkimmingTool

MuonHLTTrig_MidpTMuonPrescaleSkimmingTool = DerivationFramework__PrescaleTool(   name  = "MuonHLTTrig_MidpTMuonPrescaleSkimmingTool",
                                                                                 Prescale = 2) #okay with 2
ToolSvc += MuonHLTTrig_MidpTMuonPrescaleSkimmingTool
print MuonHLTTrig_MidpTMuonPrescaleSkimmingTool

MuonHLTTrig_MidpTMuonEventSkimmingTool=DerivationFramework__FilterCombinationAND(name="MuonHLTTrig_MidpTMuonEventSkimmingTool",FilterList=[MuonHLTTrig_MidpTEventStringSkimmingTool, MuonHLTTrig_MidpTMuonPrescaleSkimmingTool])
ToolSvc += MuonHLTTrig_MidpTMuonEventSkimmingTool
print MuonHLTTrig_MidpTMuonEventSkimmingTool

# ------------------------------------------------
# Muon HLT selection with combined logic

MuonHLTTrig_pTMuonEventSkimmingTool=DerivationFramework__FilterCombinationOR(name="MuonHLTTrig_pTMuonEventSkimmingTool",FilterList=[MuonHLTTrig_MidpTMuonEventSkimmingTool, MuonHLTTrig_DiMuonEventSkimmingTool])
ToolSvc += MuonHLTTrig_pTMuonEventSkimmingTool
print MuonHLTTrig_pTMuonEventSkimmingTool

MuonHLTTrig_EventSkimmingTool=DerivationFramework__FilterCombinationAND(name="MuonHLTTrig_EventSkimmingTool",FilterList=[MuonHLTTrig_pTMuonEventSkimmingTool, MuonHLTTrig_TriggerSkimmingTool])
ToolSvc += MuonHLTTrig_EventSkimmingTool
print MuonHLTTrig_EventSkimmingTool

# ================================================
# ------------------------------------------------
# Final logical selection
# ------------------------------------------------
# ================================================
if primDPDAlignTrigMu.doAlignmentFormat():
    # If running alignment format, want following selction:
    AlignmentEventStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "AlignmentEventStringSkimmingTool",
                                                                                   expression = "(count(Muons.muonType == 0 && Muons.pt > 5*GeV) > 0)")
    
    ToolSvc += AlignmentEventStringSkimmingTool
    print AlignmentEventStringSkimmingTool
    desdAlignmentTriggerMuonSequence+=CfgMgr.DerivationFramework__DerivationKernel("EventSkimmingKernel", SkimmingTools=[AlignmentEventStringSkimmingTool])
    
else:
    EventFilterTool=DerivationFramework__FilterCombinationOR(name="EventFilterTool",FilterList=[MuonNoAlgTrig_EventSkimmingTool, OrthoTrig_EventSkimmingTool, JpsiTrig_EventSkimmingTool, MuonHLTTrig_EventSkimmingTool])
    ToolSvc+=EventFilterTool
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
    pass
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
    pass

if primDPDAlignTrigMu.ApplyThinning():
    AlignmentTriggerMuonStream.AcceptAlgs(["muonTrkTrackThinTool_MS"])
    AlignmentTriggerMuonStream.AcceptAlgs(["muonTrkTrackThinTool_CT"])
    pass

if primDPDAlignTrigMu.ApplySkimming():
    AlignmentTriggerMuonStream.AcceptAlgs(["EventSkimmingKernel"])
    pass
#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#old way
#AlignmentTriggerMuonStream.Stream.TakeItemsFromInput = True
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

trackParticleAuxExclusions="-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

if primDPDAlignTrigMu.doAlignmentFormat():
		trackParticleAuxExclusions=""

#General
AlignmentTriggerMuonStream.AddItem(["xAOD::EventInfo#*"])
AlignmentTriggerMuonStream.AddItem(["xAOD::EventAuxInfo#*"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonContainer#Muons"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonAuxContainer#MuonsAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonSegmentContainer#MuonSegments"])
AlignmentTriggerMuonStream.AddItem(["xAOD::MuonSegmentAuxContainer#MuonSegmentsAux."])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux."+trackParticleAuxExclusions])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#CombinedMuonTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux."+trackParticleAuxExclusions])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#ExtrapolatedMuonTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#ExtrapolatedMuonTrackParticlesAux."+trackParticleAuxExclusions])

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
AlignmentTriggerMuonStream.AddItem(["xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex"])
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
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#InDetTrackParticles"]) #Really want to skim/slim this guy //for ID Alignment
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."]) #Really want to skim/slim this guy
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleContainer#MSOnlyExtrapolatedMuonTrackParticles"])
AlignmentTriggerMuonStream.AddItem(["xAOD::TrackParticleAuxContainer#MSOnlyExtrapolatedMuonTrackParticlesAux."])

AlignmentTriggerMuonStream.AddItem(["TileDigitsContainer#MuRcvDigitsCnt"])
AlignmentTriggerMuonStream.AddItem(["TileRawChannelContainer#MuRcvRawChCnt"])
AlignmentTriggerMuonStream.AddItem(["TileMuonReceiverContainer#TileMuRcvCnt"])
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
AlignmentTriggerMuonStream.AddItem(["Muon::CscStripPrepDataContainer#CSC_Measurements"])
