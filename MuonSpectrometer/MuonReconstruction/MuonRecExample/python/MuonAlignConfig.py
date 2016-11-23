# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# read alignment constants from DB to update MuonGeoModel
#
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

from MuonRecExample.MuonRecUtils import logMuon
from IOVDbSvc.CondDB import conddb

from MuonAlignFlags import muonAlignFlags

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
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')
from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                      "/MUONALIGN/TGC/SIDEA",
                                      "/MUONALIGN/TGC/SIDEC"]
ToolSvc += MuonAlignmentDbTool
MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool

from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]

# Condition DB is needed only if A-lines or B-lines are requested
if not (muonAlignFlags.UseAlines=='none' and muonAlignFlags.UseBlines=='none'):
    MuonDetectorTool.UseConditionDb = 1
    MuonDetectorTool.TheMuonAlignmentTool = MGM_AlignmentDbTool
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
    MuonDetectorTool.EnableCscInternalAlignment = True
    if conddb.dbdata == 'COMP200' and 'HLT' in globalflags.ConditionsTag() :
        logMuon.info("Reading CSC I-Lines from layout - special configuration for COMP200 in HLT setup.")
        MuonDetectorTool.UseIlinesFromGM = True
        MuonAlignmentDbTool.ILinesFromCondDB = False
    else :
        logMuon.info("Reading CSC I-Lines from conditions database.")
        conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/CSC/ILINES','/MUONALIGN/CSC/ILINES')
        MuonAlignmentDbTool.ParlineFolders += ["/MUONALIGN/CSC/ILINES"]
        MuonDetectorTool.UseIlinesFromGM = False
        MuonAlignmentDbTool.ILinesFromCondDB = True

# here define if As-Built (MDT chamber alignment) are enabled
if muonAlignFlags.UseAsBuilt:
    if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or \
       'HLT' in globalflags.ConditionsTag() or conddb.isOnline :
        logMuon.info("No MDT As-Built parameters applied.")
        MuonDetectorTool.EnableMdtAsBuiltParameters = 0
    else :
        logMuon.info("Reading As-Built parameters from conditions database")
        MuonDetectorTool.EnableMdtAsBuiltParameters = 1
        #MuonAlignmentDbTool.AsBuiltFile = "clob_asbuilt_nominal.txt"
        conddb.addFolder('MUONALIGN_OFL','/MUONALIGN/MDT/ASBUILTPARAMS')
        MuonAlignmentDbTool.ParlineFolders += ["/MUONALIGN/MDT/ASBUILTPARAMS"]

# nuisance parameter used during track fit to account for alignment uncertainty
if conddb.dbdata != 'COMP200' and conddb.dbmc != 'COMP200' and \
   'HLT' not in globalflags.ConditionsTag() and not conddb.isOnline :
    conddb.addFolder("MUONALIGN_OFL","/MUONALIGN/ERRS")
