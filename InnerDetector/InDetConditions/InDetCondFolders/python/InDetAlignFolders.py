# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb

# Inner Detector alignment
#conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align")
#conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX")
#conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT")
#conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status")
#conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent")
# Pixel module distortions
conddb.addFolderSplitOnline("INDET","/Indet/Onl/PixelDist","/Indet/PixelDist")
# IBL stave distortions 
conddb.addFolderSplitOnline("INDET","/Indet/Onl/IBLDist","/Indet/IBLDist")

# Adding protection against new dynamic folder scheme;
# In future we might want to add also to MC DB
# Solution below is not pretty but in response to JIRA ATLASSIM-2746
useDynamicAlignFolders = False
try:
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.useDynamicAlignFolders and conddb.dbdata == "CONDBR2":
        useDynamicAlignFolders = True
except ImportError:
    pass
if useDynamicAlignFolders:
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT")
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/AlignL3","/Indet/AlignL3")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL1/TRT","/TRT/AlignL1/TRT")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/AlignL2","/TRT/AlignL2")
else:
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align")
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")
del useDynamicAlignFolders #tidy up
