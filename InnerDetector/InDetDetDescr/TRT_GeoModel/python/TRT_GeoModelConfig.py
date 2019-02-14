#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def TRT_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc,geoModelSvc = GeoModelCfg( flags )
    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    trtDetectorTool.DoXenonArgonMixture = flags.Detector.SimulateTRT
    trtDetectorTool.DoKryptonMixture = flags.Detector.SimulateTRT
    trtDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ trtDetectorTool ]
    acc.addService(geoModelSvc)
    # Inner Detector alignment
    acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX"))
    if flags.Detector.SimulateTRT: # revert to old style CondHandle in case of simulation
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
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignCondAlg
    TRTAlignCondAlg = TRTAlignCondAlg(name = "TRTAlignCondAlg",
                                      UseDynamicFolders = flags.GeoModel.Align.Dynamic)
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/AlignL1/TRT","/TRT/AlignL1/TRT",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/AlignL2","/TRT/AlignL2",className="AlignableTransformContainer"))
        TRTAlignCondAlg.ReadKeyDynamicGlobal = "/TRT/AlignL1/TRT"
        TRTAlignCondAlg.ReadKeyDynamicRegular = "/TRT/AlignL2"
    else:
        if (not flags.Detector.SimulateTRT) or flags.Detector.OverlayTRT:
            acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Align","/TRT/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"TRT","/TRT/Onl/Align","/TRT/Align"))
    if flags.Common.Project is not "AthSimulation": # Protection for AthSimulation builds
        if (not flags.Detector.SimulateTRT) or flags.Detector.OverlayTRT:
            acc.addCondAlgo(TRTAlignCondAlg)

    return acc
