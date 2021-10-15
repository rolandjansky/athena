# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline


def TRT_StrawStatusSummaryToolCfg(flags, name="TRT_StrawStatusSummaryTool", forceLegacyAccess=False, **kwargs):
    """Return a ComponentAccumulator for TRT_StrawStatusSummaryTool"""
    acc = ComponentAccumulator()

    if flags.GeoModel.Align.LegacyConditionsAccess: # revert to old style CondHandle in case of simulation
        # Dead/Noisy Straw Lists not used!
        # Argon straw list
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Cond/StatusHT", "/TRT/Cond/StatusHT"))
    else:
        # Dead/Noisy Straw Lists
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Cond/Status", "/TRT/Cond/Status", className="TRTCond::StrawStatusMultChanContainer"))
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Cond/StatusPermanent", "/TRT/Cond/StatusPermanent", className="TRTCond::StrawStatusMultChanContainer"))
        # Argon straw list
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Cond/StatusHT", "/TRT/Cond/StatusHT", className="TRTCond::StrawStatusMultChanContainer"))

    kwargs.setdefault("isGEANT4", flags.GeoModel.Align.LegacyConditionsAccess or forceLegacyAccess)

    acc.setPrivateTools(CompFactory.TRT_StrawStatusSummaryTool(name, **kwargs))
    return acc


def TRT_CalDbToolCfg(flags, name="TRT_CalDbTool", **kwargs):
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

    acc.setPrivateTools(CompFactory.TRT_CalDbTool(name="TRT_CalDbTool", **kwargs))
    return acc


def TRT_MCCalDbToolCfg(flags, name="TRT_CalDbTool2", **kwargs):
    """Return a ComponentAccumulator for TRT_CalDbTool configured for MC in case of data overlay."""
    # TODO: conditions (DataOverlay)
    kwargs.setdefault("RtFolderName", "/TRT/Calib/MC/RT")
    kwargs.setdefault("T0FolderName", "/TRT/Calib/MC/T0")
    return TRT_CalDbToolCfg(flags, name, **kwargs)
