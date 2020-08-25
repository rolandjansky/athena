# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
#Comment to avoid python compilation warnings until block below is re-added
#from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ITkStripGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("ITkGeometryDBSvc"))
    StripDetectorTool=CompFactory.StripDetectorTool
    ITkStripDetectorTool = StripDetectorTool()
    #ITkStripDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    ITkStripDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkStripDetectorTool.DetectorName = "ITkStrip"
    geoModelSvc.DetectorTools += [ ITkStripDetectorTool ]
    
    """
    # All this stuff probably needs revisting - comment for now...
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        if (not flags.Detector.SimulateITkStrip) or flags.Detector.OverlayITkStrip:
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
    """
    return acc
