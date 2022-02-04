# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TRT_LocalOccupancyCfg(flags, name="TRT_LocalOccupancy", **kwargs):
    acc = ComponentAccumulator()
    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_CalDbToolCfg
    CalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    acc.addPublicTool(CalDbTool)  # public as it is has many clients to save some memory
    kwargs.setdefault("TRTCalDbTool", CalDbTool)

    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
    StrawStatusTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(StrawStatusTool)  # public as it is has many clients to save some memory
    kwargs.setdefault("TRTStrawStatusSummaryTool", StrawStatusTool )

    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTStrawCondAlgCfg
    acc.merge(TRTStrawCondAlgCfg(flags))

    kwargs.setdefault("isTrigger", False)

    if "TRT_DriftCircleCollection" not in kwargs:
        # TODO: Drift circles
        pass

    acc.setPrivateTools(CompFactory.InDet.TRT_LocalOccupancy(name, **kwargs))
    return acc


def TRT_OverlayLocalOccupancyCfg(flags, name="TRT_OverlayLocalOccupancy", **kwargs):
    """Return a ComponentAccumulator for overlay TRT_LocalOccupancy Tool"""
    kwargs.setdefault("TRT_DriftCircleCollection", "")
    return TRT_LocalOccupancyCfg(flags, name, **kwargs)


def TRTOccupancyIncludeCfg(flags, name="TRTOccupancyInclude", **kwargs):
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(TRT_LocalOccupancyCfg(flags))
    acc.addPublicTool(tool)
    kwargs.setdefault("TRT_LocalOccupancyTool", tool)
    acc.addEventAlgo(CompFactory.TRTOccupancyInclude(name, **kwargs))
    return acc


def TRT_dEdxToolCfg(flags, name="TRT_dEdxTool", **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTToTCondAlgCfg
    acc = TRTToTCondAlgCfg(flags)

    if not flags.Input.isMC:
        from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
        acc.merge(LumiBlockMuWriterCfg(flags))

    kwargs.setdefault("TRT_dEdx_isData", not flags.Input.isMC)
    kwargs.setdefault("TRT_LocalOccupancyTool", acc.popToolsAndMerge(TRT_LocalOccupancyCfg(flags)))
    
    acc.setPrivateTools(CompFactory.TRT_ToT_dEdx(name, **kwargs))
    return acc


def TRT_ElectronPidToolCfg(flags, name="TRT_ElectronPidTool", **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTHTCondAlgCfg, TRTPIDNNCondAlgCfg
    acc = TRTHTCondAlgCfg(flags)
    acc.merge(TRTPIDNNCondAlgCfg(flags))

    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
    StrawStatusTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(StrawStatusTool)  # public as it is has many clients to save some memory
    kwargs.setdefault("TRTStrawSummaryTool", StrawStatusTool)

    kwargs.setdefault("TRT_LocalOccupancyTool", acc.popToolsAndMerge(TRT_LocalOccupancyCfg(flags)))
    kwargs.setdefault("TRT_ToT_dEdx_Tool", acc.popToolsAndMerge(TRT_dEdxToolCfg(flags)))

    kwargs.setdefault("CalculateNNPid", True)

    acc.setPrivateTools(CompFactory.InDet.TRT_ElectronPidToolRun2(name, **kwargs))
    return acc
