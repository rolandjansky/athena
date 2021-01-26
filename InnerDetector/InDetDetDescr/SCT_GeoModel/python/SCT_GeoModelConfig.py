# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def SCT_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    SCT_DetectorTool=CompFactory.SCT_DetectorTool
    sctDetectorTool = SCT_DetectorTool()
    sctDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    sctDetectorTool.Alignable = True # make this a flag?
    sctDetectorTool.DetectorName = "SCT"
    geoModelSvc.DetectorTools += [ sctDetectorTool ]
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
            sctAlignCondAlg = SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                               UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(sctAlignCondAlg)
            SCT_DetectorElementCondAlg=CompFactory.SCT_DetectorElementCondAlg
            sctDetectorElementCondAlg = SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")
            acc.addCondAlgo(sctDetectorElementCondAlg)
    return acc
