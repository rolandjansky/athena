# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
    # ITkStripDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic #Will we need to do dynamic alignment for ITk?
    ITkStripDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkStripDetectorTool.DetectorName = "ITkStrip"
    if flags.GeoModel.useLocalGeometry:
        # Setting this filename triggers reading from local file rather than DB  
        ITkStripDetectorTool.GmxFilename = flags.ITk.stripGeometryFilename 
    geoModelSvc.DetectorTools += [ ITkStripDetectorTool ]

    # Alignment corrections and DetElements to conditions
    if flags.GeoModel.Align.Dynamic:
        raise RuntimeError("ITk dynamic alignment not supported yet!")
    else:
        if flags.Common.Project != "AthSimulation" and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
            acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align"))

    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            SCT_AlignCondAlg = CompFactory.SCT_AlignCondAlg
            ITkStripAlignCondAlg = SCT_AlignCondAlg(name = "ITkStripAlignCondAlg",
                                                    DetManagerName = "ITkStrip",
                                                    WriteKey = "ITkStripAlignmentStore",
                                                    UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(ITkStripAlignCondAlg)
            SCT_DetectorElementCondAlg = CompFactory.SCT_DetectorElementCondAlg
            ITkStripDetectorElementCondAlg = SCT_DetectorElementCondAlg(name = "ITkStripDetectorElementCondAlg",
                                                                        DetManagerName = "ITkStrip",
                                                                        ReadKey = "ITkStripAlignmentStore",
                                                                        WriteKey = "ITkStripDetectorElementCollection")
            acc.addCondAlgo(ITkStripDetectorElementCondAlg)

    return acc
