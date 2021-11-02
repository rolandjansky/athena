#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def TRT_GeoModelCfg(flags):
    from AtlasGeoModel.GeometryDBConfig import InDetGeometryDBSvcCfg
    db = InDetGeometryDBSvcCfg(flags)

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    trtDetectorTool = CompFactory.TRT_DetectorTool()
    trtDetectorTool.GeometryDBSvc = db.getPrimary()
    trtDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
    acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags, forceLegacyAccess=True))  # FIXME: if we set the tool, things break for unknown reasons
    geoModelSvc.DetectorTools += [ trtDetectorTool ]
    acc.merge(db)
    return acc


def TRT_AlignmentCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    if flags.GeoModel.Align.LegacyConditionsAccess:  # revert to old style CondHandle in case of simulation
        from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Calib/DX", "/TRT/Calib/DX"))
        acc.merge(addFoldersSplitOnline(flags, "TRT", "/TRT/Onl/Align", "/TRT/Align"))
    else:
        from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTAlignCondAlgCfg
        acc.merge(TRTAlignCondAlgCfg(flags))
    return acc


def TRT_SimulationGeometryCfg(flags):
    # main GeoModel config
    acc = TRT_GeoModelCfg(flags)
    acc.merge(TRT_AlignmentCfg(flags))
    return acc


def TRT_ReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = TRT_GeoModelCfg(flags)
    acc.merge(TRT_AlignmentCfg(flags))
    # Note: this has almost the same content but different name on purpose if
    # we ever split readout geometry in a separate conditions algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTAlignCondAlgCfg
    acc.merge(TRTAlignCondAlgCfg(flags))
    return acc
