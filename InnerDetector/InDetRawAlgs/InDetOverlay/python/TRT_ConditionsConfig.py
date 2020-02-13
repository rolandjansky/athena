"""Define methods to construct configured TRT conditions tools and
algorithms needed by TRT Overlay - FIXME should move somewhere else

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TRT_CalDbToolCfg(flags, name = "TRT_CalDbTool"):
    """Return a ComponentAccumulator for TRT_CalDbTool"""
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    acc = addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className='TRTCond::StrawT0MultChanContainer')
    TRT_CalDbTool=CompFactory.TRT_CalDbTool
    acc.setPrivateTools(TRT_CalDbTool(name = "TRT_CalDbTool"))
    return acc


def TRT_StrawStatusSummaryToolCfg(flags, name = "TRT_StrawStatusSummaryTool"):
    """Return a ComponentAccumulator for TRT_StrawStatusSummaryTool"""
    acc = ComponentAccumulator()
    TRT_StrawStatusSummaryTool=CompFactory.TRT_StrawStatusSummaryTool
    acc.setPrivateTools(TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                   isGEANT4 = flags.Detector.Simulate))
    return acc


def TRT_LocalOccupancyCfg(flags, name = "TRT_LocalOccupancy"):
    """Return a ComponentAccumulator for TRT_LocalOccupancy Tool"""
    acc = ComponentAccumulator()
    trtCalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    trtStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))

    InDet__TRT_LocalOccupancy=CompFactory.InDet__TRT_LocalOccupancy
    acc.setPrivateTools(InDet__TRT_LocalOccupancy(name = "TRT_LocalOccupancy",
                                                  isTrigger = False,
                                                  TRTCalDbTool = trtCalDbTool,
                                                  TRTStrawStatusSummaryTool =  trtStrawStatusSummaryTool))
    return acc


def TRTStrawCondAlgCfg(flags, name = "TRTStrawCondAlg"):
    """Return a ComponentAccumulator for TRTStrawCondAlg algorithm"""
    acc = ComponentAccumulator()
    trtStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    # Alive straws algorithm
    TRTStrawCondAlg=CompFactory.TRTStrawCondAlg
    acc.addCondAlgo(TRTStrawCondAlg(name = "TRTStrawCondAlg",
                                    TRTStrawStatusSummaryTool = trtStrawStatusSummaryTool,
                                    isGEANT4 =flags.Detector.Simulate))
    return acc
