# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def PLRGeometryCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    plrDetectorTool = CompFactory.PLRDetectorTool()
    # ITkPixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    plrDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    plrDetectorTool.DetectorName = "PLR"
    if flags.ITk.Geometry.PLRLocal:
        # Setting this filename triggers reading from local file rather than DB
        plrDetectorTool.GmxFilename = flags.ITk.Geometry.PLRFilename
    if flags.ITk.Geometry.PLRClobOutputName:
        plrDetectorTool.ClobOutputName = flags.ITk.Geometry.PLRClobOutputName
    geoModelSvc.DetectorTools += [ plrDetectorTool ]

    return acc
