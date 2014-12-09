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
FTKDet.FTK_BadModuleMapPath = "badModulemap_8Lc_FTK.bmap"
FTKDet.ATLAS_BadModuleMapPath = "badModulemap_8Lc_ATLAS.bmap"
FTKDet.pmap_path = "../Trigger/TrigFTK/TrigFTKSim/config/map_file/raw_8LcIbl123.pmap"
FTKDet.rmap_path = "../Trigger/TrigFTK/TrigFTKSim/config/map_file/raw_12Libl.tmap"
ToolSvc += FTKDet

FTKDumpCond = FTKDumpCondAlgo( "FTKDumpCondAlgo" , OutputLevel=INFO)
FTKDumpCond.IBLMode = 1
FTKDumpCond.DumpBadModules = True
FTKDumpCond.DumpGlobalToLocalMap = True
theJob += FTKDumpCond  

print theJob




