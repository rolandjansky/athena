#
# This is for reading TRT conditions for CTB
# include it in the InDetTestBeamRecExample 

#load relevant libraries
if not "TRT_ConditionsAlgs" in theApp.Dlls:
    theApp.Dlls += [ "TRT_ConditionsAlgs" ]

#add the calibration folders
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/Defaults<tag>TrtDe_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B0<tag>TrtT0B0_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B1<tag>TrtT0B1_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/T0_B2<tag>TrtT0B2_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B0<tag>TrtRtB0_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B1<tag>TrtRtB1_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/RT_B2<tag>TrtRtB2_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B0<tag>TrtDfB0_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B1<tag>TrtDfB1_02</tag>"]
IOVDbSvc.Folders+=[CondDBCool.TRT+"/TRT/Calib/DF_B2<tag>TrtDfB2_02</tag>"]

ToolSvc = Service("ToolSvc")

ToolSvc.TRT_CondDbTool.useCOOL = True
#ToolSvc.TRT_CondDbTool.OutputLevel = 2


