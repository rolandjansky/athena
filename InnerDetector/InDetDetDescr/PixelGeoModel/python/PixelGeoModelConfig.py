# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def PixelGeoModelCfg(flags):
    from AtlasGeoModel.GeometryDBConfig import InDetGeometryDBSvcCfg
    db = InDetGeometryDBSvcCfg(flags)

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    pixelDetectorTool = CompFactory.PixelDetectorTool("PixelDetectorTool")
    pixelDetectorTool.GeometryDBSvc = db.getPrimary()
    pixelDetectorTool.BCM_Tool = CompFactory.InDetDD.BCM_Builder()
    pixelDetectorTool.BLM_Tool = CompFactory.InDetDD.BLM_Builder()
    pixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ pixelDetectorTool ]
    acc.merge(db)
    return acc


def PixelAlignmentCfg(flags):
    if flags.GeoModel.Align.LegacyConditionsAccess:  # revert to old style CondHandle in case of simulation
        from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
        return addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align")
    else:
        from PixelConditionsAlgorithms.PixelConditionsConfig import PixelAlignCondAlgCfg
        return PixelAlignCondAlgCfg(flags)


def PixelSimulationGeometryCfg(flags):
    # main GeoModel config
    acc = PixelGeoModelCfg(flags)
    acc.merge(PixelAlignmentCfg(flags))
    return acc


def PixelReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = PixelGeoModelCfg(flags)
    acc.merge(PixelAlignmentCfg(flags))
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
    acc.merge(PixelDetectorElementCondAlgCfg(flags))
    return acc
