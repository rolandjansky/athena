# jobOption fragment for TBCondRunParTool
# assumes RecExTB_CondDB.py has already by loaded to setup condDB access
# set folders and partition location
IOVDbSvc=Service("IOVDbSvc")
ToolSvc=Service("ToolSvc")
# this for muon standalone (run 600940 etc)
#IOVDbSvc.Folders+= [ "/tdaq/part_MDT+RPC/RunParams.RunParams<offset>+1</offset>"]
#IOVDbSvc.Folders+= [ "/tdaq/part_MDT+RPC/RunParams.RunInfo<offset>+1</offset>"]
#ToolSvc.TBCondRunParTool.RunParamFolder="/tdaq/part_MDT+RPC"
# this for combined run (run 2000036 etc)
IOVDbSvc.Folders+= [ "<db>COOLONL_TDAQ/TBDP200</db>/TDAQ/part_Combined/RunParams.RunParams"]
#IOVDbSvc.Folders+= [ "<db>COOLONL_TDAQ/TBDP200</db>/TDAQ/part_Combined/RunParams.RunInfo<offset>+1</offset>"]
ToolSvc.TBCondRunParTool.RunParamFolder="/TDAQ/part_Combined"
