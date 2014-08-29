theApp.Dlls += ["MuonCondSvc"]

#

theApp.CreateSvc += [ "MuonCalib::MuonAlignmentDbSvc" ]

#theApp.ExtSvc += [ "MuonAlignmentDbSvc" ]
MuonAlignmentDbSvc = Service( "MuonCalib::MuonAlignmentDbSvc" )
MuonAlignmentDbSvc.OutputLevel=VERBOSE



MuonAlignmentDbSvc.DBToolType="MuonCalib::MuonAlignmentDbTool"
MuonAlignmentDbSvc.DBToolName="MuonCalib_MuonAlignmentDbTool"

include("MuonCondTool/MuonAlignmentDbTool.py")

MuonCalib_MuonAlignmentDbTool = Service( "ToolSvc.MuonCalib_MuonAlignmentDbTool" )
MuonCalib_MuonAlignmentDbTool.OutputLevel=VERBOSE
MuonCalib_MuonAlignmentDbTool.ParlineFolder="/MDT/COOL2.0/BARREL/ALIGN"
