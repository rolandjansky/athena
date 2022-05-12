# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkVKalVrtFitter.VKalVrtFitterConfig import TrkVKalVrtFitterCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
 
from AthenaCommon.Logging import logging
mlog = logging.getLogger('Rec__NewVrtSecInclusiveConfig')

################################################################### 
# Search for low-pt (soft) B-hadron vertices. 
#------------------------------------
def SoftBFinderToolCfg(flags, name="SoftBFinderTool", **myargs):
 
    mlog.info("entering SoftBFinderTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["CutPt"]        = 500.
    myargs["CutBLayHits"]  = 1
    myargs["CutPixelHits"] = 3
    myargs["CutSiHits"]    = 8
    myargs["CutTRTHits"]   = 10
    myargs["useVertexCleaning"]  = True
    myargs["MultiWithOneTrkVrt"] = True
    myargs["removeTrkMatSignif"] = -1.    # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 2.0
    myargs["TrkSigCut"]      = 2.0
    myargs["SelVrtSigCut"]   = 3.0
    myargs["v2tIniBDTCut"]   =-0.7
    myargs["v2tFinBDTCut"]   = 0.0
    myargs["cosSVPVCut"]     = 0.4
    myargs["FastZSVCut"]     = 5.
    myargs["VertexMergeCut"] = 4.
    myargs["MaxSVRadiusCut"] = 50.
    SoftBFinder = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(SoftBFinder)
    mlog.info("SoftBFinderTool created")
 
    return acc

################################################################### 
# Configuration for B-hadron search in the ttbar phase space
#------------------------------------
def InclusiveBFinderToolCfg(flags, name="InclusiveBFinderTool", **myargs):

    mlog.info("entering InclusiveBFinderTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["CutPt"]        = 500.
    myargs["CutBLayHits"]  = 0
    myargs["CutPixelHits"] = 2
    myargs["CutSiHits"]    = 8
    myargs["CutTRTHits"]   = 10
    myargs["useVertexCleaning"]  = True
    myargs["MultiWithOneTrkVrt"] = True
    myargs["removeTrkMatSignif"] = -1.      # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 2.0
    myargs["TrkSigCut"]      = 2.0
    myargs["SelVrtSigCut"]   = 3.0
    myargs["v2tIniBDTCut"]   =-0.7
    myargs["v2tFinBDTCut"]   =-0.2
    myargs["cosSVPVCut"]     = 0.5
    myargs["FastZSVCut"]     = 8.

    InclusiveBFinder = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(InclusiveBFinder)
    mlog.info("InclusiveBFinderTool created")
 
    return acc


################################################################### 
# Configuration for B-hadron search in the high-pt phase space
#------------------------------------
def HighPtBFinderToolCfg(flags, name="HighPtBFinderTool", **myargs):

    mlog.info("entering HighPtBFinderTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["CutPt"]        = 1000.
    myargs["CutBLayHits"]  = 0
    myargs["CutPixelHits"] = 2
    myargs["CutSiHits"]    = 8
    myargs["CutTRTHits"]   = 10
    myargs["useVertexCleaning"]  = True
    myargs["MultiWithOneTrkVrt"] = True
    myargs["removeTrkMatSignif"] = -1.     # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 2.0
    myargs["TrkSigCut"]      = 2.0
    myargs["SelVrtSigCut"]   = 3.0
    myargs["v2tIniBDTCut"]   =-0.6
    myargs["v2tFinBDTCut"]   = 0.2
    myargs["cosSVPVCut"]     = 0.7
    myargs["FastZSVCut"]     = 8.

    HighPtBFinder = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(HighPtBFinder)
    mlog.info("HighPtBFinderTool created")
 
    return acc


################################################################### 
# Configuration for hadronic interactions in ID material studies
#------------------------------------
def MaterialSVFinderToolCfg(flags, name="MaterialSVFinderTool", **myargs):

    mlog.info("entering MaterialSVFinderTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["CutPt"]        = 500.
    myargs["CutBLayHits"]  = 0
    myargs["CutPixelHits"] = 1
    myargs["CutSiHits"]    = 8,
    myargs["CutTRTHits"]   = 10
    myargs["useVertexCleaning"]  = False
    myargs["MultiWithOneTrkVrt"] = False
    myargs["removeTrkMatSignif"] = -1.    # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 5.0
    myargs["TrkSigCut"]      = 5.0
    myargs["SelVrtSigCut"]   = 10.0
    myargs["v2tIniBDTCut"]   =-1.01       #Remove BDT selection
    myargs["v2tFinBDTCut"]   =-1.01       #Remove BDT selection
    myargs["cosSVPVCut"]     = 0.
    myargs["FastZSVCut"]     = 10.
    myargs["VrtMassLimit"]   = 8000.
    myargs["Vrt2TrMassLimit"]= 8000.

    MaterialSVFinder = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(MaterialSVFinder)
    mlog.info("MaterialSVFinderTool created")
 
    return acc

#######################################################################
# Configuration for LLP search using LRT 
#------------------------------------
def DVFinderToolCfg(flags, name="DVFinderTool", **myargs):

    mlog.info("entering DVFinderTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["CutPt"]        = 1000.
    myargs["CutBLayHits"]  = 0
    myargs["CutPixelHits"] = 0
    myargs["CutSiHits"]    = 7,
    myargs["CutTRTHits"]   = 15
    myargs["useVertexCleaning"]  = False
    myargs["MultiWithOneTrkVrt"] = False
    myargs["removeTrkMatSignif"] = -1.    # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 6.0
    myargs["TrkSigCut"]      = 10.0
    myargs["SelVrtSigCut"]   = 8.0
    myargs["v2tIniBDTCut"]   =-1.01       # BDT selection is disabled
    myargs["v2tFinBDTCut"]   =-1.01       # BDT selection is disabled
    myargs["cosSVPVCut"]     = 0.
    myargs["FastZSVCut"]     = 30.
    myargs["VrtMassLimit"]   = 1000000.
    myargs["Vrt2TrMassLimit"]= 1000000.
    myargs["VertexMergeCut"] = 10.
    myargs["MaxSVRadiusCut"] = 350.
    myargs["CutD0Max"]       = 1000.   # Maximal track impact parameter
    myargs["CutD0Min"]       = 0.      # Minimal track impact parameter
    myargs["CutZVrt"]        = 100.

    DVFinder = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(DVFinder)
    mlog.info("DVFinderTool created")

    return acc

##########################################################################################################
# Configuration for creation of calibration ntuples for 2-track vertex classification BDT
# Version for B-hadrons
#----------------------------
def V2TCalibrationToolCfg(flags, name="V2TCalibrationTool", **myargs):

    mlog.info("entering V2TCalibrationTool configuration")
    acc = ComponentAccumulator()
    acc.merge(BeamSpotCondAlgCfg(flags))

    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["ExtrapolatorName"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    myargs["FillHist"]     = True,
    myargs["CutPt"]        = 400.
    myargs["CutBLayHits"]  = 0
    myargs["CutPixelHits"] = 1
    myargs["CutSiHits"]    = 8,
    myargs["CutTRTHits"]   = 10
    myargs["useVertexCleaning"]  = False
    myargs["MultiWithOneTrkVrt"] = False
    myargs["removeTrkMatSignif"] = -1.    # No additional material rejection
    myargs["AntiPileupSigRCut"]  = 2.0
    myargs["TrkSigCut"]      = 2.0
    myargs["SelVrtSigCut"]   = 2.0
    myargs["v2tIniBDTCut"]   =-1.01       #Remove BDT selection
    myargs["v2tFinBDTCut"]   =-1.01       #Remove BDT selection
    myargs["cosSVPVCut"]     = 0.
    myargs["FastZSVCut"]     = 15.
    myargs["VrtMassLimit"]   = 5500.
    myargs["Vrt2TrMassLimit"]= 4000.
    myargs["MaxSVRadiusCut"] = 140.
    myargs["CutD0Max"]       = 100.   # Maximal track impact parameter
    myargs["CutD0Min"]       = 0.     # Minimal track impact parameter
    myargs["CutZVrt"]        = 100.


    V2TCalibration = CompFactory.Rec.NewVrtSecInclusiveTool(name,**myargs)
    acc.setPrivateTools(V2TCalibration)
    mlog.info("V2TCalibrationTool created")

    return acc


