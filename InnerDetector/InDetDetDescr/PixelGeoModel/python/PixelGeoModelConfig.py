# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def PixelGeometryCfg(flags):
  from AtlasGeoModel.GeometryDBConfig import InDetGeometryDBSvcCfg
  db = InDetGeometryDBSvcCfg(flags)

  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  acc = GeoModelCfg(flags)
  geoModelSvc = acc.getPrimary()

  pixelDetectorTool = CompFactory.PixelDetectorTool("PixelDetectorTool")
  pixelDetectorTool.GeometryDBSvc = db.getPrimary()
  pixelDetectorTool.BCM_Tool = CompFactory.InDetDD.BCM_Builder()
  pixelDetectorTool.BLM_Tool = CompFactory.InDetDD.BLM_Builder()
  pixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
  geoModelSvc.DetectorTools += [ pixelDetectorTool ]
  acc.merge(db)

  # IBL module distortions
  acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

  if flags.GeoModel.Align.Dynamic:
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
  else:
    if flags.Common.Project != "AthSimulation" and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    else:
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
  if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
      if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
          from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
          acc.merge(PixelDetectorElementCondAlgCfg(flags))
  return acc
