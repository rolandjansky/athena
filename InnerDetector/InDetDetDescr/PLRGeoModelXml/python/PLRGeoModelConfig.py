# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

# needed for alignment (copied from ITkPixel):
#from AthenaConfiguration.Enums import ProductionStep
#from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def PLRGeometryCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    PLRDetectorTool = CompFactory.PLRDetectorTool
    plrDetectorTool = PLRDetectorTool()
    # ITkPixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    plrDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    plrDetectorTool.DetectorName = "PLR"
    if flags.GeoModel.useLocalGeometry:
        # Setting this filename triggers reading from local file rather than DB
        plrDetectorTool.GmxFilename = flags.ITk.plrGeometryFilename
    geoModelSvc.DetectorTools += [ plrDetectorTool ]

    # Alignment corrections and DetElements to conditions (copied from ITkPixel):
    #if flags.Common.Project != "AthSimulation" and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
    #    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelDetectorElementCondAlgCfg
    #    acc.merge(ITkPixelDetectorElementCondAlgCfg(flags))
    #else:
    #    acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align"))

    return acc
