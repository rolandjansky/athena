#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep

def TRT_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    TRT_DetectorTool=CompFactory.TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    trtDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ trtDetectorTool ]
    acc.addService(geoModelSvc)
    # Inner Detector alignment
    acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX"))
    if flags.Common.ProductionStep == ProductionStep.Simulation: # revert to old style CondHandle in case of simulation
        # Dead/Noisy Straw Lists
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status"))
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent"))
        # Argon straw list
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT"))
    else:
        # Dead/Noisy Straw Lists
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer'))
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer'))
        # Argon straw list
        acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer'))
    # TRT Condition Algorithm
    TRTAlignCondAlg=CompFactory.TRTAlignCondAlg
    TRTAlignCondAlg = TRTAlignCondAlg(name = "TRTAlignCondAlg",
                                      UseDynamicFolders = flags.GeoModel.Align.Dynamic)
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/AlignL1/TRT","/TRT/AlignL1/TRT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/AlignL2","/TRT/AlignL2",className="AlignableTransformContainer"))
        TRTAlignCondAlg.ReadKeyDynamicGlobal = "/TRT/AlignL1/TRT"
        TRTAlignCondAlg.ReadKeyDynamicRegular = "/TRT/AlignL2"
    else:
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Align","/TRT/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Align","/TRT/Align"))
    if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            acc.addCondAlgo(TRTAlignCondAlg)

    return acc
