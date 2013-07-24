# jobOption fragment for TBCondRunParTool
# COOL version - assumes IOVDbSvc/IOVRecExTB.py has already been loaded

# set folders and partition location
IOVDbSvc=Service("IOVDbSvc")
ToolSvc=Service("ToolSvc")

# set partition for combined running
ToolSvc.TBCondRunParTool.RunParamFolder="/TDAQ/part_Combined"
IOVDbSvc.Folders+=[CondDBCool.TDAQ+"/TDAQ/part_Combined/RunParams.RunParams"]
