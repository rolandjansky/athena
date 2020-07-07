# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def SCT_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    if flags.GeoModel.Run=="RUN4":
        if "GMX" == flags.GeoModel.StripGeoType():
            SCT_GMX_DetectorTool=CompFactory.SCT_GMX_DetectorTool
            sctDetectorTool = SCT_GMX_DetectorTool()
        else:
            SCT_SLHC_DetectorTool=CompFactory.SCT_SLHC_DetectorTool
            sctDetectorTool = SCT_SLHC_DetectorTool()
            InDetServMatBuilderToolSLHC=CompFactory.InDetServMatBuilderToolSLHC
            InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
            acc.addPublicTool( InDetServMatBuilderToolSLHC )
            sctDetectorTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    else:
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
        if (not flags.Detector.SimulateSCT) or flags.Detector.OverlaySCT:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if (not flags.Detector.SimulateSCT) or flags.Detector.OverlaySCT:
            SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
            sctAlignCondAlg = SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                               UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(sctAlignCondAlg)
            SCT_DetectorElementCondAlg=CompFactory.SCT_DetectorElementCondAlg
            sctDetectorElementCondAlg = SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")
            acc.addCondAlgo(sctDetectorElementCondAlg)
    return acc
