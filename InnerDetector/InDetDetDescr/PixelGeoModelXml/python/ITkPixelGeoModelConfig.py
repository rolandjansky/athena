# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory


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
    if flags.ITk.useLocalGeometry:
      # Setting this filename triggers reading from local file rather than DB
      ITkPixelDetectorTool.GmxFilename = flags.ITk.pixelGeometryFilename
    geoModelSvc.DetectorTools += [ ITkPixelDetectorTool ]

    return acc
