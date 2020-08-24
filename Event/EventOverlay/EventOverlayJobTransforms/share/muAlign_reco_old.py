
#use muon alignments
printfunc ("Haas: Reading muon alignment constants from DB for reco")

#from IOVDbSvc.CondDB import conddb
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
#conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')

#from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
#MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
#MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
#                                      "/MUONALIGN/MDT/ENDCAP/SIDEA",
#                                      "/MUONALIGN/MDT/ENDCAP/SIDEC",
#                                      "/MUONALIGN/TGC/SIDEA",
#                                      "/MUONALIGN/TGC/SIDEC"]

#ToolSvc += MuonAlignmentDbTool
#MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
#MGM_AlignmentDbTool.OutputLevel=DEBUG
#printfunc (MGM_AlignmentDbTool)
from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
MuonDetectorTool.OutputLevel=DEBUG

MuonDetectorTool.EnableFineClashFixing = 0 #this should be on for g4?
printfunc (MuonDetectorTool)
printfunc (GeoModelSvc)
printfunc (ToolSvc)


