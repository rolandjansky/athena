# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb

from AthenaCommon.DetFlags          import DetFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags

if DetFlags.TRT_on():
    # Dead/Noisy Straw Lists
    if not conddb.folderRequested("/TRT/Cond/Status"):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')
    if not conddb.folderRequested("/TRT/Cond/StatusPermanent"):
        conddb.addFolderSplitOnline("TRT","/TRT/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')
    # Argon straw list
    if not conddb.folderRequested("/TRT/Cond/StatusHT"):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')
    # TRT Lvl 3 alignment
    if not conddb.folderRequested("/TRT/Calib/DX"):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX")

if DetFlags.pixel_on() :
    # Pixel/IBL module distortions
    if not conddb.folderRequested("/Indet/PixelDist"):
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/PixelDist","/Indet/PixelDist", className="DetCondCFloat")
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection")

# TRT Condition Algorithm
from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignCondAlg
TRTAlignCondAlg = TRTAlignCondAlg(name = "TRTAlignCondAlg",
                                  UseDynamicFolders = InDetGeometryFlags.useDynamicAlignFolders())

# Control loading of the dynamic folder scheme;
# In future we might want to add also to MC DB
# related to JIRA ATLASSIM-2746

if InDetGeometryFlags.useDynamicAlignFolders():
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection")



    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL1/TRT","/TRT/AlignL1/TRT",className="CondAttrListCollection")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL2","/TRT/AlignL2",className="AlignableTransformContainer")
    TRTAlignCondAlg.ReadKeyDynamicGlobal="/TRT/AlignL1/TRT"
    TRTAlignCondAlg.ReadKeyDynamicRegular="/TRT/AlignL2"

else:

    if DetFlags.SCT_on() or DetFlags.pixel_on():
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer")

    if DetFlags.TRT_on():
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align",className="AlignableTransformContainer")

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

# Condition algorithms for ID alignment
if DetFlags.TRT_on():
    if not hasattr(condSeq, "TRTAlignCondAlg"):
        condSeq += TRTAlignCondAlg

if DetFlags.SCT_on():
    if not hasattr(condSeq, "SCT_AlignCondAlg"):
        from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
        condSeq += SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                    UseDynamicAlignFolders =  InDetGeometryFlags.useDynamicAlignFolders())
        if not hasattr(condSeq, "SCT_DetectorElementCondAlg"):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
            condSeq += SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")

if DetFlags.pixel_on():
    if not hasattr(condSeq, "PixelAlignCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelAlignCondAlg
        condSeq += PixelAlignCondAlg(name = "PixelAlignCondAlg",
                                     UseDynamicAlignFolders =  InDetGeometryFlags.useDynamicAlignFolders())
        if not hasattr(condSeq, "PixelDetectorElementCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDetectorElementCondAlg
            condSeq += PixelDetectorElementCondAlg(name = "PixelDetectorElementCondAlg")

