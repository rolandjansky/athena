#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import FTKDetectorTool, FTKDumpCondAlgo

print "Add FTKDetector tool"
FTKDet = FTKDetectorTool()
FTKDet.pmap_path = "../Trigger/TrigFTK/TrigFTKSim//config/map_file/raw_ITkExample.pmap"
FTKDet.rmap_path = "../Trigger/TrigFTK/TrigFTKSim//config/map_file/raw_ITkExample.tmap"
ToolSvc += FTKDet

FTKDumpCond = FTKDumpCondAlgo( "FTKDumpCondAlgo" , OutputLevel=INFO)
FTKDumpCond.IBLMode = 0
FTKDumpCond.ITkMode = True
FTKDumpCond.DumpIDMap = True
theJob += FTKDumpCond  

print theJob




