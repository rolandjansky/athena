
# most of them should be initialized already in RecExCommission
ToolSvc = Service( "ToolSvc" )

if not 'CheckEveryNoEvents' in dir():
    CheckEveryNoEvents=100

#----------------------------------------------------------
theApp.Dlls += [ "AthenaMonitoring"]
theApp.Dlls += [ "LArMonTools"]
theApp.TopAlg += ["AthenaMon/LArMon1"]
LArMon1 = Algorithm( "LArMon1" )
LArMon1.CheckEveryNoEvents=CheckEveryNoEvents

# include all monitoring tools
include ("LArMonTools/LAr2DNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArDigitNoiseMonTool_jobOptions.py" )
# include ("LArMonTools/LArDigitSimpleMonTool_jobOptions.py" )
#include ("LArMonTools/LArDigMonTool_jobOptions.py" )
include ("LArMonTools/LArFEBMon_jobOptions.py" )
#include ("LArMonTools/LArRawChannelMonTool_jobOptions.py" )
#include ("LArMonTools/LArRawChannelNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArScaNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArEventInfoMonTool_jobOptions.py" )
#include ("LArMonTools/LArAccumDigMonTool_jobOptions.py")
#include ("LArMonTools/LArFebNoiseMonTool_jobOptions.py")
