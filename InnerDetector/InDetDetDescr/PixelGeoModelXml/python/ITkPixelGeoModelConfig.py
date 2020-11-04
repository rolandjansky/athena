# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
#Comment to avoid python compilation warnings until block below is re-added
#from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ITkPixelGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("ITkGeometryDBSvc"))
    PixelDetectorTool=CompFactory.ITkPixelDetectorTool
    ITkPixelDetectorTool = PixelDetectorTool()
    #ITkPixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    ITkPixelDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkPixelDetectorTool.DetectorName = "ITkPixel"
    geoModelSvc.DetectorTools += [ ITkPixelDetectorTool ]
