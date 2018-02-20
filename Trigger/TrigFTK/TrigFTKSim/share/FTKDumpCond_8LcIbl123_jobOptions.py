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
FTKDet.pmap_path = "x86_64-slc6-gcc62-opt/share/ftk_configuration/map_files/raw_8LcIbl123.pmap"
FTKDet.rmap_path = "x86_64-slc6-gcc62-opt/share/ftk_configuration/map_files/raw_12Libl.tmap"
ToolSvc += FTKDet

FTKDumpCond = FTKDumpCondAlgo( "FTKDumpCondAlgo" , OutputLevel=INFO)
FTKDumpCond.IBLMode = 1
FTKDumpCond.DumpBadModules = True
FTKDumpCond.DumpGlobalToLocalMap = True
FTKDumpCond.DumpModulePositions = True
theJob += FTKDumpCond  

print theJob




