# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ITkStripGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("ITkGeometryDBSvc"))
    StripDetectorTool=CompFactory.StripDetectorTool
    ITkStripDetectorTool = StripDetectorTool()
    #ITkStripDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic #Will we need to do dynamic alignment for ITk?
    ITkStripDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkStripDetectorTool.DetectorName = "ITkStrip"
    if flags.ITk.useLocalGeometry:
      #Setting this filename triggers reading from local file rather than DB  
      ITkStripDetectorTool.GmxFilename = flags.ITk.stripGeometryFilename 
    geoModelSvc.DetectorTools += [ ITkStripDetectorTool ]
    
    #Alignment corrections and DetElements to conditions
    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer")) #Just load a single static folder for the moment - to be revisted with ITk-specific folders
            SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
            ITkStripAlignCondAlg = SCT_AlignCondAlg(name = "ITkStripAlignCondAlg",
                                                    UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic,
                                                    DetManagerName = "ITkStrip",
                                                    WriteKey = "ITkStripAlignmentStore")
            acc.addCondAlgo(ITkStripAlignCondAlg)
            SCT_DetectorElementCondAlg=CompFactory.SCT_DetectorElementCondAlg
            ITkStripDetectorElementCondAlg = SCT_DetectorElementCondAlg(name = "ITkStripDetectorElementCondAlg",
                                                                        ReadKey = "ITkStripAlignmentStore",
                                                                        WriteKey = "ITkStripDetectorElementCollection",
                                                                        DetManagerName = "ITkStrip")
            acc.addCondAlgo(ITkStripDetectorElementCondAlg)


    """
    # All this stuff probably needs revisting - comment for now...
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))

    """
    return acc
