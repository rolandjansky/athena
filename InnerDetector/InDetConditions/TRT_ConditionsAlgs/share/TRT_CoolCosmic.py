#
# This is for reading TRT constants for SR1 Cosmics
# use it in the InDetCosmicRecExample

#load relevant libraries
if not "TRT_ConditionsAlgs" in theApp.Dlls:
    theApp.Dlls += [ "TRT_ConditionsAlgs" ]

#add the calibration folders
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/Defaults<tag>TrtDe_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B0<tag>TrtT0B0_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B0<tag>TrtT0B1_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B0<tag>TrtT0B2_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B0<tag>TrtRTB0_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B0<tag>TrtRTB1_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B0<tag>TrtRTB2_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B0<tag>TrtDFB0_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B0<tag>TrtDFB1_01</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B0<tag>TrtDFB2_01</tag>"]

ToolSvc = Service("ToolSvc")

ToolSvc.TRT_CondDbTool.useCOOL = True
#ToolSvc.TRT_CondDbTool.OutputLevel = 2


