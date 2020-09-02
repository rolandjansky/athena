# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# read alignment constants from DB to update MuonGeoModel
#
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonCondAlgConf import MuonAlignmentErrorDbAlg

from MuonRecExample.MuonRecUtils import logMuon
from IOVDbSvc.CondDB import conddb

from .MuonAlignFlags import muonAlignFlags

# defaults have to be re-set maybe 
muonAlignFlags.setDefaults()


###############################################################
# There are 3 types of problems in some old global tags (already locked)
# Pb 1: no TGC alignment folders exist
# Pb 2: Information of tags is wrong causing crashes (example endcap alignment corrections on COMCOND-ES1C-000-00)
# Pb 3: No association of tags to the global tag is done for the alignment folder
# 
# To avoid crashes, a check on old, locked global tags is done and action is taken not to read some alignment folders
###############################################################


import re
logMuon.info("Reading alignment constants from DB")
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
condSequence = AthSequencer("AthCondSeq")

from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
MuonAlignAlg = MuonAlignmentCondAlg()
from TriggerJobOpts.TriggerFlags import TriggerFlags
if TriggerFlags.MuonSlice.doTrigMuonConfig: MuonAlignAlg.DoRecRoiSvcUpdate = True # this should be removed as soon as RPC/TGCRecRoiSvc are migrated to use the MuonDetectorCondAlg
MuonAlignAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                               "/MUONALIGN/MDT/ENDCAP/SIDEA",
                               "/MUONALIGN/MDT/ENDCAP/SIDEC",
                               "/MUONALIGN/TGC/SIDEA",
                               "/MUONALIGN/TGC/SIDEC"]
condSequence+=MuonAlignAlg

# Disable caching. This will have some memory impact (TBC) but is necessary for the moment to make this thread safe.
MuonDetectorTool.FillCacheInitTime = 1

# Condition DB is needed only if A-lines or B-lines are requested
if not (muonAlignFlags.UseAlines=='none' and muonAlignFlags.UseBlines=='none'):
    MuonDetectorTool.UseConditionDb = 1
    # here define to what extent B-lines are enabled
    if muonAlignFlags.UseBlines=='none':
        MuonDetectorTool.EnableMdtDeformations = 0
    elif muonAlignFlags.UseBlines=='all':
        MuonDetectorTool.EnableMdtDeformations = 1
    elif muonAlignFlags.UseBlines=='barrel':
        MuonDetectorTool.EnableMdtDeformations = 2
    elif muonAlignFlags.UseBlines=='endcaps':
        MuonDetectorTool.EnableMdtDeformations = 3

# here define if I-lines (CSC internal alignment) are enabled
if muonAlignFlags.UseIlines: 
    if 'HLT' in globalflags.ConditionsTag() :
        logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
        MuonDetectorTool.UseIlinesFromGM = True
        MuonDetectorTool.EnableCscInternalAlignment = False
    else :
        logMuon.info("Reading CSC I-Lines from conditions database.")
        conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/CSC/ILINES','/MUONALIGN/CSC/ILINES',className='CondAttrListCollection')
        MuonDetectorTool.UseIlinesFromGM = False
        MuonDetectorTool.EnableCscInternalAlignment = True
        MuonAlignAlg.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
        MuonAlignAlg.ILinesFromCondDB = True

# here define if As-Built (MDT chamber alignment) are enabled
if muonAlignFlags.UseAsBuilt:
    if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or \
       'HLT' in globalflags.ConditionsTag() or conddb.isOnline :
        logMuon.info("No MDT As-Built parameters applied.")
        MuonDetectorTool.EnableMdtAsBuiltParameters = 0
    else :
        logMuon.info("Reading As-Built parameters from conditions database")
        MuonDetectorTool.EnableMdtAsBuiltParameters = 1
        conddb.addFolder('MUONALIGN_OFL','/MUONALIGN/MDT/ASBUILTPARAMS',className='CondAttrListCollection')
        MuonAlignAlg.ParlineFolders += ["/MUONALIGN/MDT/ASBUILTPARAMS"]

# nuisance parameter used during track fit to account for alignment uncertainty
if conddb.dbdata != 'COMP200' and conddb.dbmc != 'COMP200' and \
   'HLT' not in globalflags.ConditionsTag() and not conddb.isOnline :

    conddb.addFolder("MUONALIGN_OFL","/MUONALIGN/ERRS",className='CondAttrListCollection')
    condSequence += MuonAlignmentErrorDbAlg("MuonAlignmentErrorDbAlg")

from MuonGeoModel.MuonGeoModelConf import MuonDetectorCondAlg
MuonDetectorManagerCond = MuonDetectorCondAlg()
MuonDetectorManagerCond.MuonDetectorTool = MuonDetectorTool
condSequence+=MuonDetectorManagerCond
