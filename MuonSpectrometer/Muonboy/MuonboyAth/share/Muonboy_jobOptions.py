include.block ( "MuonboyAth/Muonboy_jobOptions.py" )
#--------------------------------------------------------------
# MuonboyAth
#-----------------------------------------------------------------
#Control Flags
if not 'doMuonboyBarrelOnly' in dir():  
     doMuonboyBarrelOnly=False

if not 'doMuonboyAllChecks' in dir():  
     doMuonboyAllChecks=False

if not 'doMuonboyDumpOnly' in dir():  
     doMuonboyDumpOnly=False

if doMuonboyDumpOnly:
   doMuonboyAllChecks=False


from MboyEDMTool.MboyEDMToolConf import MboyGetPrepDataFromEventTool
MboyDigiEmptyLoopMboyGetPrepDataFromEventTool = MboyGetPrepDataFromEventTool(
    name = "MboyDigiEmptyLoopMboyGetPrepDataFromEventTool",
    CollectMDT                       = 1                  ,
    CollectRPC                       = 1                  ,
    CollectTGC                       = 1                  ,
    CollectCSC                       = 0                  ,
    CollectCscCluster                = 1                  ,
    MdtPrepDataContainerName         = "MDT_DriftCircles" ,
    RpcPrepDataContainerName         = "RPC_Measurements" ,
    TgcPrepDataContainerName         = "TGC_Measurements" ,
    CscPrepDataContainerName         = "CSC_Measurements" ,
    CscClustersPrepDataContainerName = "CSC_Clusters"     ,
    ContainerAccessMethodMDT         = 0                  ,
    ContainerAccessMethodRPC         = 0                  ,
    ContainerAccessMethodTGC         = 0                  ,
    ContainerAccessMethodCSC         = 0                  )
ToolSvc += MboyDigiEmptyLoopMboyGetPrepDataFromEventTool
print MboyDigiEmptyLoopMboyGetPrepDataFromEventTool


from MboyEDMTool.MboyEDMToolConf import MboyGetPrepDataFromEventTool
MboyRecMboyGetPrepDataFromEventTool = MboyGetPrepDataFromEventTool(
    name = "MboyRecMboyGetPrepDataFromEventTool",
    CollectMDT                       = 1                  ,
    CollectRPC                       = 1                  ,
    CollectTGC                       = 1                  ,
    CollectCSC                       = 0                  ,
    CollectCscCluster                = 1                  ,
    MdtPrepDataContainerName         = "MDT_DriftCircles" ,
    RpcPrepDataContainerName         = "RPC_Measurements" ,
    TgcPrepDataContainerName         = "TGC_Measurements" ,
    CscPrepDataContainerName         = "CSC_Measurements" ,
    CscClustersPrepDataContainerName = "CSC_Clusters"     ,
    ContainerAccessMethodMDT         = 0                  ,
    ContainerAccessMethodRPC         = 0                  ,
    ContainerAccessMethodTGC         = 0                  ,
    ContainerAccessMethodCSC         = 0                  )
ToolSvc += MboyRecMboyGetPrepDataFromEventTool
print MboyRecMboyGetPrepDataFromEventTool


#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "MuonboyAth"]

#-----------------------------------------------------------------
#MboyDigiEmptyLoop: empty loop on digits (Muonboy independent)
theApp.TopAlg += [ "MboyDigiEmptyLoop" ]

#-----------------------------------------------------------------
#MboyRec: reconstruction
theApp.TopAlg += [ "MboyRec" ]

#-----------------------------------------------------------------
if doMuonboyBarrelOnly:
   ToolSvc.MboyRecMboyGetPrepDataFromEventTool.CollectMDT        = 1 ;
   ToolSvc.MboyRecMboyGetPrepDataFromEventTool.CollectRPC        = 1 ;
   ToolSvc.MboyRecMboyGetPrepDataFromEventTool.CollectTGC        = 0 ;
   ToolSvc.MboyRecMboyGetPrepDataFromEventTool.CollectCSC        = 0 ;
   ToolSvc.MboyRecMboyGetPrepDataFromEventTool.CollectCscCluster = 0 ;

#--------------------------------------------------------------
if doMuonboyDumpOnly:
   MboyDigiEmptyLoop = Algorithm( "MboyDigiEmptyLoop" )
   MboyDigiEmptyLoop.SwitchOff = 1
   MboyRec = Algorithm( "MboyRec" )
   MboyRec.SwitchOff = 1

#-----------------------------------------------------------------
if doMuonboyAllChecks:
   theApp.TopAlg += [ "MboyChecks" ]
   theApp.TopAlg += [ "MboySegmentCheck" ]
   MboyRec = Algorithm( "MboyRec" )
   MboyRec.DoDump = 1 
   MboyRec.ShortSummaryTrack = 1    
   MboyRec.ShortSummarySegment = 1    
   MboyRec.PrintDigitsSummary = 1 
