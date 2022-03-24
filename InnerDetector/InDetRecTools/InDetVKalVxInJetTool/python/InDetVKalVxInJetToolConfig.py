# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkVKalVrtFitter.VKalVrtFitterConfig import TrkVKalVrtFitterCfg
from InDetVKalVxInJetTool.InDetTrkInJetTypeConfig import InDetTrkInJetTypeCfg

from AthenaCommon.Logging  import logging
mlog = logging.getLogger( 'InDetVKalVxInJetTool::__init__ ' )

#------------------------------------
def InDetVKalVxInJetFinderCfg(flags, name="InDetVKalVxInJetFinder", **myargs):
 
    mlog.info("entering InDetVKalVxInJetFinder configuration")
    acc = ComponentAccumulator()
    
    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["TrackClassTool"]   = acc.popToolsAndMerge(InDetTrkInJetTypeCfg(flags))

    VKalVxInJet = CompFactory.InDet.InDetVKalVxInJetTool(name,**myargs)
    acc.setPrivateTools(VKalVxInJet)
    mlog.info("InDetVKalVxInJetFinder created")

    return acc


#------------------------------------
def InDetVKalVxInHiPtJetFinderCfg(flags, name="InDetVKalVxInHiPtJetFinder", **myargs):
 
    mlog.info("entering InDetVKalVxInHiPtJetFinder configuration")
    acc = ComponentAccumulator()
    
    myargs["VertexFitterTool"] = acc.popToolsAndMerge(TrkVKalVrtFitterCfg(flags))
    myargs["TrackClassTool"]   = acc.popToolsAndMerge(InDetTrkInJetTypeCfg(flags))
    myargs["CutSharedHits"]    = 0
    myargs["Sel2VrtChi2Cut"]   = 4.
    myargs["CutBVrtScore"]     = 0.002

    VKalVxInJet = CompFactory.InDet.InDetVKalVxInJetTool(name,**myargs)
    acc.setPrivateTools(VKalVxInJet)
    mlog.info("InDetVKalVxInHiPtJetFinder created")

    return acc





