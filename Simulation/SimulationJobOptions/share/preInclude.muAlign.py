print ("Reading alignment constants from DB")
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                       "/MUONALIGN/TGC/SIDEA",
                                       "/MUONALIGN/TGC/SIDEC"]

from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
