"""Define methods to construct configured TRT conditions tools and
algorithms needed by TRT Overlay - FIXME should move somewhere else

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRT_CalDbToolCfg(flags, name = "TRT_CalDbTool"):
    """Return a ComponentAccumulator for TRT_CalDbTool"""
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    acc = addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className='TRTCond::StrawT0MultChanContainer')
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
    acc.setPrivateTools(TRT_CalDbTool(name = "TRT_CalDbTool"))
    return acc


def TRT_StrawStatusSummaryToolCfg(flags, name = "TRT_StrawStatusSummaryTool"):
    """Return a ComponentAccumulator for TRT_StrawStatusSummaryTool"""
    acc = ComponentAccumulator()
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
    acc.setPrivateTools(TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                   isGEANT4 = flags.Detector.Simulate))
    return acc


def TRT_LocalOccupancyCfg(flags, name = "TRT_LocalOccupancy"):
    """Return a ComponentAccumulator for TRT_LocalOccupancy Tool"""
    acc = ComponentAccumulator()
    trtCalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    trtStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
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
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawCondAlg
    acc.addCondAlgo(TRTStrawCondAlg(name = "TRTStrawCondAlg",
                                    TRTStrawStatusSummaryTool = trtStrawStatusSummaryTool,
                                    isGEANT4 =flags.Detector.Simulate))
    return acc
