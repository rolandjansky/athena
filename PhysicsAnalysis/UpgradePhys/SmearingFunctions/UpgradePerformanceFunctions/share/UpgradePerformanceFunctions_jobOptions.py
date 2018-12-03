# Skeleton joboption for a simple analysis job

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [os.environ['ASG_TEST_FILE_MC']]  # insert your list of input files here

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Configure the UPF instance:
ToolSvc += CfgMgr.Upgrade__UpgradePerformanceFunctions("UpgradePerformanceFunctions")
ToolSvc.UpgradePerformanceFunctions.UseTrackConfirmation = True
ToolSvc.UpgradePerformanceFunctions.AvgMu = 200
ToolSvc.UpgradePerformanceFunctions.JetRandomSeed = 1111

theJob += CfgMgr.UpgradePerformanceFunctionsAlg("myTestAlg")
theJob.myTestAlg.UpgradePerformanceFunctions = ToolSvc.UpgradePerformanceFunctions

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 100000
