# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    if InDetGeometryFlags.isSLHC():
        kwargs.setdefault("ServiceBuilderTool",    "InDetServMatBuilderToolSLHC");
    else:
        kwargs.setdefault("ServiceBuilderTool",    "");

    return CfgMgr.PixelDetectorTool(name, **kwargs)


############## ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def PixelGeometryCfg( flags ):
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  acc,geoModelSvc = GeoModelCfg( flags )
  from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
  acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
  from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
  pixelDetectorTool = PixelDetectorTool("PixelDetectorTool")
  from BCM_GeoModel.BCM_GeoModelConf import InDetDD__BCM_Builder
  bcmTool = InDetDD__BCM_Builder()
  acc.addPublicTool( bcmTool )
  pixelDetectorTool.BCM_Tool = bcmTool
  from BLM_GeoModel.BLM_GeoModelConf import InDetDD__BLM_Builder
  blmTool = InDetDD__BLM_Builder()
  acc.addPublicTool( blmTool )
  pixelDetectorTool.BLM_Tool = blmTool
  if flags.GeoModel.Run=="RUN4":
      from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatBuilderToolSLHC
      InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
      acc.addPublicTool( InDetServMatBuilderToolSLHC )
      pixelDetectorTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
  pixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
  geoModelSvc.DetectorTools += [ pixelDetectorTool ]
  acc.addService(geoModelSvc)
  # Pixel module distortions
  acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/PixelDist","/Indet/PixelDist")) # Not used ???
  # IBL stave distortions
  acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))
  if flags.GeoModel.Align.Dynamic:
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
  else:
    if (not flags.Detector.SimulatePixel) or flags.Detector.OverlayPixel:
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    else:
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
  return acc
