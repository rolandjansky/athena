# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSCT_DetectorTool(name="SCT_DetectorTool", **kwargs):
    kwargs.setdefault("DetectorName",     "SCT");
    kwargs.setdefault("Alignable",        True);
    kwargs.setdefault("RDBAccessSvc",     "RDBAccessSvc");
    kwargs.setdefault("GeometryDBSvc",    "InDetGeometryDBSvc");
    kwargs.setdefault("GeoDbTagSvc",      "GeoDbTagSvc");
    from AthenaCommon.DetFlags      import DetFlags
    return CfgMgr.SCT_DetectorTool(name, **kwargs)


###### ComponentAccumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def SCT_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc,geoModelSvc = GeoModelCfg( flags )
    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    if flags.GeoModel.Run=="RUN4":
        if "GMX" == flags.GeoModel.StripGeoType():
            from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
            sctDetectorTool = SCT_GMX_DetectorTool()
        else:
            from SCT_SLHC_GeoModel.SCT_SLHC_GeoModelConf import SCT_SLHC_DetectorTool
            sctDetectorTool = SCT_SLHC_DetectorTool()
            from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatBuilderToolSLHC
            InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
            acc.addPublicTool( InDetServMatBuilderToolSLHC )
            sctDetectorTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    else:
        from SCT_GeoModel.SCT_GeoModelConf import SCT_DetectorTool
        sctDetectorTool = SCT_DetectorTool()
    sctDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ sctDetectorTool ]
    acc.addService(geoModelSvc)
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        if (not flags.Detector.SimulateSCT) or flags.Detector.OverlaySCT:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
    import os
    if "AthSimulation_DIR" not in os.environ: # Protection for AthSimulation builds
        if (not flags.Detector.SimulateSCT) or flags.Detector.OverlaySCT:
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
            sctAlignCondAlg = SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                               UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(sctAlignCondAlg)
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
            sctDetectorElementCondAlg = SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")
            acc.addCondAlgo(sctDetectorElementCondAlg)
    return acc
