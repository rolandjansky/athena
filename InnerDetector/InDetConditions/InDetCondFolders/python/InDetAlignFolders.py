# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags          import DetFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags

# Top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Inner Detector alignment
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX")

# Dead/Noisy Straw Lists
if hasattr(topSequence,"OutputConditionsAlg"): # revert to old style CondHandle in case streaming to POOL
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')

if hasattr(topSequence,"OutputConditionsAlg"):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')

# Argon straw list
if DetFlags.simulate.any_on() or hasattr(topSequence,"OutputConditionsAlg"):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')


# Pixel module distortions
conddb.addFolderSplitOnline("INDET","/Indet/Onl/PixelDist","/Indet/PixelDist")
# IBL stave distortions 
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
    if ((not DetFlags.simulate.SCT_on()) or (not DetFlags.simulate.pixel_on())) or (DetFlags.overlay.SCT_on() or DetFlags.overlay.pixel_on()):
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer")

    else:
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align")

    if (not DetFlags.simulate.TRT_on()) or DetFlags.overlay.TRT_on():
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align",className="AlignableTransformContainer")
    else:
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

# Condition algorithms for ID alignment
if DetFlags.TRT_on() and ((not DetFlags.simulate.TRT_on()) or DetFlags.overlay.TRT_on()):
    if not hasattr(condSeq, "TRTAlignCondAlg"):
        import os
        if "AthSimulation_DIR" not in os.environ: # Protection for AthSimulation builds
            condSeq += TRTAlignCondAlg
if DetFlags.SCT_on() and ((not DetFlags.simulate.SCT_on()) or DetFlags.overlay.SCT_on()):

    if not hasattr(condSeq, "SCT_AlignCondAlg"):
        import os
        if "AthSimulation_DIR" not in os.environ: # Protection for AthSimulation builds
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
            condSeq += SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                        UseDynamicAlignFolders =  InDetGeometryFlags.useDynamicAlignFolders())
            if not hasattr(condSeq, "SCT_DetectorElementCondAlg"):
                from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
                condSeq += SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")

