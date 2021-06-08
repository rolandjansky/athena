# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ITkPixelGeometryCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    GeometryDBSvc = CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("ITkGeometryDBSvc"))

    PixelDetectorTool = CompFactory.ITkPixelDetectorTool
    ITkPixelDetectorTool = PixelDetectorTool()
    # ITkPixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    ITkPixelDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkPixelDetectorTool.DetectorName = "ITkPixel"
    if flags.GeoModel.useLocalGeometry:
        # Setting this filename triggers reading from local file rather than DB
        ITkPixelDetectorTool.GmxFilename = flags.ITk.pixelGeometryFilename
    geoModelSvc.DetectorTools += [ ITkPixelDetectorTool ]

    # Alignment corrections and DetElements to conditions
    if flags.GeoModel.Align.Dynamic:
        raise RuntimeError("ITk dynamic alignment not supported yet!")
    else:
        if flags.Common.Project != "AthSimulation" and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
            acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align"))
    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            PixelAlignCondAlg = CompFactory.PixelAlignCondAlg
            pixelAlignCondAlg = PixelAlignCondAlg(name = "ITkPixelAlignCondAlg",
                                                  DetManagerName = "ITkPixel",
                                                  ReadKeyIBLDist = "",
                                                  WriteKey = "ITkPixelAlignmentStore",
                                                  UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(pixelAlignCondAlg)
            PixelDetectorElementCondAlg = CompFactory.PixelDetectorElementCondAlg
            pixelDetectorElementCondAlg = PixelDetectorElementCondAlg(name = "ITkPixelDetectorElementCondAlg",
                                                                      DetManagerName = "ITkPixel",
                                                                      ReadKey = "ITkPixelAlignmentStore",
                                                                      WriteKey = "ITkPixelDetectorElementCollection")
            acc.addCondAlgo(pixelDetectorElementCondAlg)

    return acc
