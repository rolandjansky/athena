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
FTKDet.FTK_BadModuleMapPath = "badModulemap_12LIbl_FTK.bmap"
FTKDet.ATLAS_BadModuleMapPath = "badModulemap_12LIbl_ATLAS.bmap"
FTKDet.pmap_path = "../Trigger/TrigFTK/TrigFTKSim/config/map_file/raw_12LiblHW3D.pmap"
FTKDet.rmap_path = "../Trigger/TrigFTK/TrigFTKSim/config/map_file/raw_12Libl32TmodB_3D_t13.tmap"
ToolSvc += FTKDet

FTKDumpCond = FTKDumpCondAlgo( "FTKDumpCondAlgo" , OutputLevel=INFO)
FTKDumpCond.IBLMode = 2
####FTKDumpCond.DumpBadModules = True
FTKDumpCond.DumpGlobalToLocalMap = True
#FTKDumpCond.DumpIDMap = True
theJob += FTKDumpCond

print theJob




