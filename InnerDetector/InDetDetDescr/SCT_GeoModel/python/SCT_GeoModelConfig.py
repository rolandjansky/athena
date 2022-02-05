# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def SCT_GeoModelCfg(flags):
    from AtlasGeoModel.GeometryDBConfig import InDetGeometryDBSvcCfg
    db = InDetGeometryDBSvcCfg(flags)

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    sctDetectorTool = CompFactory.SCT_DetectorTool()
    sctDetectorTool.GeometryDBSvc = db.getPrimary()
    sctDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    sctDetectorTool.Alignable = True # make this a flag?
    sctDetectorTool.DetectorName = "SCT"
    geoModelSvc.DetectorTools += [ sctDetectorTool ]
    acc.merge(db)
    return acc


def SCT_AlignmentCfg(flags):
    if flags.GeoModel.Align.LegacyConditionsAccess:  # revert to old style CondHandle in case of simulation
        from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
        return addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align")
    else:
        from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig import SCT_AlignCondAlgCfg
        return SCT_AlignCondAlgCfg(flags)


def SCT_SimulationGeometryCfg(flags):
    # main GeoModel config
    acc = SCT_GeoModelCfg(flags)
    acc.merge(SCT_AlignmentCfg(flags))
    return acc


def SCT_ReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = SCT_GeoModelCfg(flags)
    acc.merge(SCT_AlignmentCfg(flags))
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig import SCT_DetectorElementCondAlgCfg
    acc.merge(SCT_DetectorElementCondAlgCfg(flags))
    return acc
