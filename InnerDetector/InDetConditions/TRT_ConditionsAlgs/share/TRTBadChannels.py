#this is for reading TRTStrawStatusTool bad channels!
#include it from the alg where you need to read the info, eg clusterization
###############################################################
theApp.Dlls += [ "TRT_ConditionsAlgs" ]
ToolSvc = Service( "ToolSvc" )
ToolSvc.TRTStrawStatusTool.Readjob = True
ToolSvc.TRTStrawStatusTool.Writejob = False
#if ToolSvc.TRTStrawStatusTool.Writejob:
#	ToolSvc.TRTStrawStatusTool.InputTRTBadChanFile="TRT_BadChannels_CTB04.dat";
IOVDbSvc.Folders += [ CondDBCool.TRT+"/TRT/TRTStrawStatusData <tag>HEAD</tag>" ]
