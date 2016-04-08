# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommon.py 
# Schedules all tools needed for muon object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# MCP GROUP TOOLS 
#====================================================================

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
DFCommonMuonToolWrapperTools = []

### IDHits
DFCommonMuonsSelector = CP__MuonSelectionTool(name = "DFCommonMuonsSelector")
DFCommonMuonsSelector.MaxEta = 2.7
DFCommonMuonsSelector.MuQuality = 3
ToolSvc += DFCommonMuonsSelector
print DFCommonMuonsSelector

DFCommonMuonToolWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapper",
                                                                        AsgSelectionTool = DFCommonMuonsSelector,
                                                                        CutType = "IDHits",
                                                                        StoreGateEntryName = "DFCommonGoodMuon",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapper
print DFCommonMuonToolWrapper
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapper)

### Loose
DFCommonMuonsSelectorLoose = CP__MuonSelectionTool(name = "DFCommonMuonsSelectorLoose")
DFCommonMuonsSelectorLoose.MaxEta = 2.7
DFCommonMuonsSelectorLoose.MuQuality = 2
ToolSvc += DFCommonMuonsSelectorLoose
print DFCommonMuonsSelectorLoose

DFCommonMuonToolWrapperLoose = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperLoose",
                                                                        AsgSelectionTool = DFCommonMuonsSelectorLoose,
                                                                        CutType = "Quality",
                                                                        StoreGateEntryName = "DFCommonMuonsLoose",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapperLoose
print DFCommonMuonToolWrapperLoose
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperLoose)

### Medium
DFCommonMuonsSelectorMedium = CP__MuonSelectionTool(name = "DFCommonMuonsSelectorMedium")
DFCommonMuonsSelectorMedium.MaxEta = 2.7
DFCommonMuonsSelectorMedium.MuQuality = 1
ToolSvc += DFCommonMuonsSelectorMedium
print DFCommonMuonsSelectorMedium

DFCommonMuonToolWrapperMedium = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperMedium",
                                                                        AsgSelectionTool = DFCommonMuonsSelectorMedium,
                                                                        CutType = "Quality",
                                                                        StoreGateEntryName = "DFCommonMuonsMedium",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapperMedium
print DFCommonMuonToolWrapperMedium
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperMedium)

### Tight
DFCommonMuonsSelectorTight = CP__MuonSelectionTool(name = "DFCommonMuonsSelectorTight")
DFCommonMuonsSelectorTight.MaxEta = 2.7
DFCommonMuonsSelectorTight.MuQuality = 0
ToolSvc += DFCommonMuonsSelectorTight
print DFCommonMuonsSelectorTight

DFCommonMuonToolWrapperTight = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonMuonToolWrapperTight",
                                                                        AsgSelectionTool = DFCommonMuonsSelectorTight,
                                                                        CutType = "Quality",
                                                                        StoreGateEntryName = "DFCommonMuonsTight",
                                                                        ContainerName = "Muons")
ToolSvc += DFCommonMuonToolWrapperTight
print DFCommonMuonToolWrapperTight
DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperTight)

##====================================================================
## FOR MUON ISOLATION
##====================================================================
##WARNING: duplicate with the EventDensityTool in EGammaCommon.py
##This should be handled in a better way
#run_EDEgamma = False
#if not hasattr (ToolSvc, 'EDEgammaCentralTool'):
#    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
#    from JetRec.JetRecStandard import jtm
#    tc=configEventDensityTool("EDEgammaCentralTool", jtm.emget,
#                              radius          = 0.5,
#                              AbsRapidityMin  = 0.0,
#                              AbsRapidityMax  = 1.5,
#                              OutputContainer = "DensityForEgammaCentral",
#                              OutputLevel     = 3
#                              )
#    ToolSvc += tc
#    tf=configEventDensityTool("EDEgammaForwardTool", jtm.emget,
#                              radius          = 0.5,
#                              AbsRapidityMin  = 1.5,
#                              AbsRapidityMax  = 3.0,
#                              OutputContainer = "DensityForEgammaForward",
#                              OutputLevel     = 3
#                              )
#    ToolSvc += tf
#    run_EDEgamma = True
#
##if not hasattr (ToolSvc, 'DFIsoTrackSelectorTool'):
##bug with this: "NameError: name 'tit' is not defined" ==> NOT USED FOR NOW
##Track selection
#from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
#IsoTrackSelectorTool_Muons = InDet__InDetDetailedTrackSelectorTool(
#    name    = "DFIsoTrackSelectorTool_Muons",
#    pTMin       = 1000.,
#    nHolesPixel = 0,
#    nHitSi      = 9,
#    IPz0Max     = 3.,
#    IPd0Max     = 999999.,
#    etaMax = 2.5,
#    nHitBLayer = 0,
#    nHitPix = 0,
#    nHitSiPhysical = 0,
#    TrackSummaryTool = ToolSvc.InDetTrackSummaryTool,
#    Extrapolator     = ToolSvc.InDetExtrapolator
#    )
#ToolSvc +=IsoTrackSelectorTool_Muons
#
##Track isolation tool
#from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
#tit_Muons = xAOD__TrackIsolationTool(
#    name               = "DFTrackIsolationTool_Muons",
#    TrackSelectionTool = IsoTrackSelectorTool_Muons
#    )
#ToolSvc += tit_Muons
#
#from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__IsolationAugmentationToolMuons
#DFCommonIsolationMuons = DerivationFramework__IsolationAugmentationToolMuons(name               = "DFCommonIsolationMuons",
#                                                                             TrackIsolationTool = tit_Muons,
#                                                                             OutputLevel        = 3
#                                                                             )
#ToolSvc += DFCommonIsolationMuons
#
##=======================================
## CREATE THE DERIVATION KERNEL ALGORITHM   
##=======================================
#if run_EDEgamma:
#    DerivationFrameworkJob += EventDensityAlg("EDEgammaCentralAlg", EventDensityTool = tc)
#    DerivationFrameworkJob += EventDensityAlg("EDEgammaForwardAlg", EventDensityTool = tf)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel",
                                                                         #AugmentationTools = [DFCommonMuonToolWrapper,DFCommonIsolationMuons]
#                                                                          AugmentationTools = [DFCommonMuonToolWrapper]
                                                                         AugmentationTools = DFCommonMuonToolWrapperTools
                                                                        )
