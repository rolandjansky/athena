# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags          import DetFlags
# Inner Detector alignment
#conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align")
#conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX")

# Dead/Noisy Straw Lists
if DetFlags.simulate.any_on() or athenaCommonFlags.EvtMax==1: # revert to old style CondHandle in case of simulation or streaming to sqlite
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')

if DetFlags.simulate.any_on() or athenaCommonFlags.EvtMax==1:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')

# Argon straw list
if DetFlags.simulate.any_on() or athenaCommonFlags.EvtMax==1:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT")
else:
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')


# Pixel module distortions
conddb.addFolderSplitOnline("INDET","/Indet/Onl/PixelDist","/Indet/PixelDist")
# IBL stave distortions 
conddb.addFolderSplitOnline("INDET","/Indet/Onl/IBLDist","/Indet/IBLDist")

# Control loading of the dynamic folder scheme;
# In future we might want to add also to MC DB
# related to JIRA ATLASSIM-2746

from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
if InDetGeometryFlags.useDynamicAlignFolders():
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL1/TRT","/TRT/AlignL1/TRT")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL2","/TRT/AlignL2")
else:
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")

# Condition algorithms for ID alignment
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "SCT_AlignCondAlg"):
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
    condSeq += SCT_AlignCondAlg(name = "SCT_AlignCondAlg",
                                UseDynamicAlignFolders =  InDetGeometryFlags.useDynamicAlignFolders())
if not hasattr(condSeq, "SCT_DetectorElementCondAlg"):
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
    condSeq += SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg")

