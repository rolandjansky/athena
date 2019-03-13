
#use muon alignments
print "Haas: Reading muon alignment constants from DB for reco"

from IOVDbSvc.CondDB import conddb

MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
#MGM_AlignmentDbTool.OutputLevel=DEBUG
#print MGM_AlignmentDbTool
from AtlasGeoModel.MuonGM import GeoModelSvc
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
#MuonDetectorTool.OutputLevel=DEBUG

MuonDetectorTool.EnableFineClashFixing = 0 #this should be on for g4?
#print MuonDetectorTool
#print GeoModelSvc
#print ToolSvc

#Now use MC T0 for TRT during overlay digi, but MC or data T0/Rt during reco, depending on where digit came from... so this stuff is just for reco
conddb.blockFolder("/TRT/Calib/MC/RT")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/MC/RT","TrtCalibRt-R2-MC-run2-scenario1_04-03",force=True,forceData=True)
conddb.addMarkup("/TRT/Calib/MC/RT","<forceRunNumber>313000</forceRunNumber>")
conddb.blockFolder("/TRT/Calib/MC/T0")
conddb.addFolderWithTag("TRT_OFL","/TRT/Calib/MC/T0","TrtCalibT0-R2-MC-run2-scenario1_04-03",force=True,forceData=True)
conddb.addMarkup("/TRT/Calib/MC/T0","<forceRunNumber>313000</forceRunNumber>")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
TRTMCCalibDBSvc=TRT_CalDbSvc(name="TRTMCCalibDBSvc",RtFolderName="/TRT/Calib/MC/RT",T0FolderName="/TRT/Calib/MC/T0",ErrorSlopeFolderName="/TRT/Calib/slopes",ErrorFolderName ="/TRT/Calib/errors2d")
ServiceMgr += TRTMCCalibDBSvc

#This would add another trtdriftfuntiontool, using the new trtcaldb
ToolSvc.InDetTRT_DriftFunctionTool.IsOverlay=True
ToolSvc.InDetTRT_DriftFunctionTool.TRTCalDbTool2=TRTMCCalibDBSvc

if "EOJT_noLorentz" in globals():
    print "EOJT_noLorentz found in globals(), so not doing Lorentz corrections"
    svcMgr.PixelSiliconConditionsSvc.ForceUseGeoModel=True
    svcMgr.PixelSiliconConditionsSvc.CheckGeoModel=True
    svcMgr.PixelSiliconConditionsSvc.UseDBForHV=False
    svcMgr.PixelLorentzAngleSvc.usePixelDefaults=True
else:
    print "EOJT_noLorentz not found in globals(), so doing standard Lorentz corrections"

