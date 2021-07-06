"""Define methods to construct configured TRT conditions tools and
algorithms needed by TRT Overlay - FIXME should move somewhere else

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TRT_OnlineFoldersCfg(flags):
    """Setup online conditions folders for TRT data overlay"""
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/TRT/Onl/ROD/Compress",
                         "TRT_ONL", className='CondAttrListCollection'))
    return acc


def TRT_CalDbToolCfg(flags, name="TRT_CalDbTool"):
    """Return a ComponentAccumulator for TRT_CalDbTool"""
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Calib/RT", "/TRT/Calib/RT",
                                    className="TRTCond::RtRelationMultChanContainer"))
    acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Calib/T0", "/TRT/Calib/T0",
                                    className='TRTCond::StrawT0MultChanContainer'))
    acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Calib/errors2d", "/TRT/Calib/errors2d",
                                    className="TRTCond::RtRelationMultChanContainer"))
    acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Calib/slopes", "/TRT/Calib/slopes",
                                    className='TRTCond::RtRelationMultChanContainer'))

    TRT_CalDbTool = CompFactory.TRT_CalDbTool
    acc.setPrivateTools(TRT_CalDbTool(name="TRT_CalDbTool"))
    return acc


def TRT_StrawStatusSummaryToolCfg(flags, name="TRT_StrawStatusSummaryTool"):
    """Return a ComponentAccumulator for TRT_StrawStatusSummaryTool"""
    acc = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    isGeant4 = flags.Common.ProductionStep == ProductionStep.Simulation
    TRT_StrawStatusSummaryTool = CompFactory.TRT_StrawStatusSummaryTool
    acc.setPrivateTools(TRT_StrawStatusSummaryTool(name="TRT_StrawStatusSummaryTool",
                                                   isGEANT4=isGeant4))
    return acc


def TRT_LocalOccupancyCfg(flags, name="TRT_LocalOccupancy"):
    """Return a ComponentAccumulator for TRT_LocalOccupancy Tool"""
    acc = ComponentAccumulator()
    trtCalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    trtStrawStatusSummaryTool = acc.popToolsAndMerge(
        TRT_StrawStatusSummaryToolCfg(flags))

    InDet__TRT_LocalOccupancy = CompFactory.InDet.TRT_LocalOccupancy
    acc.setPrivateTools(InDet__TRT_LocalOccupancy(name="TRT_LocalOccupancy",
                                                  isTrigger=False,
                                                  TRTCalDbTool=trtCalDbTool,
                                                  TRTStrawStatusSummaryTool=trtStrawStatusSummaryTool,
                                                  TRT_RDOContainerName="",
                                                  TRT_DriftCircleCollection="",
                                                  ))
    return acc


def TRTStrawCondAlgCfg(flags, name="TRTStrawCondAlg"):
    """Return a ComponentAccumulator for TRTStrawCondAlg algorithm"""
    acc = ComponentAccumulator()
    from AthenaConfiguration.Enums import ProductionStep
    isGeant4 = flags.Common.ProductionStep == ProductionStep.Simulation
    trtStrawStatusSummaryTool = acc.popToolsAndMerge(
        TRT_StrawStatusSummaryToolCfg(flags))
    # Alive straws algorithm
    TRTStrawCondAlg = CompFactory.TRTStrawCondAlg
    acc.addCondAlgo(TRTStrawCondAlg(name="TRTStrawCondAlg",
                                    TRTStrawStatusSummaryTool=trtStrawStatusSummaryTool,
                                    isGEANT4=isGeant4))
    return acc


def TRT_CablingSvcCfg(flags):
    """Return a ComponentAccumulator for TRT_CablingSvc service"""
    acc = ComponentAccumulator()
    # Properly configure MC/data for TRT cabling
    TRT_FillCablingData_DC3 = CompFactory.TRT_FillCablingData_DC3
    tool = TRT_FillCablingData_DC3(RealData=not flags.Input.isMC)
    acc.addPublicTool(tool)
    # Setup TRT cabling service
    TRT_CablingSvc = CompFactory.TRT_CablingSvc
    acc.addService(TRT_CablingSvc())
    return acc
