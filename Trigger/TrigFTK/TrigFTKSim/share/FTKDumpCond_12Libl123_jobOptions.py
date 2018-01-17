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
FTKDet.FTK_BadModuleMapPath = "badModulemap_12LiblHW3D_FTK.bmap"
FTKDet.ATLAS_BadModuleMapPath = "badModulemap_12LinlHW3D_ATLAS.bmap"
FTKDet.dumpAllModules = True

from PyJobTransforms.trfUtils import findFile

pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW.pmap')
FTKDet.pmap_path = pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB.tmap')
FTKDet.rmap_path = rmap_path
ToolSvc += FTKDet

FTKDumpCond = FTKDumpCondAlgo( "FTKDumpCondAlgo" , OutputLevel=INFO)
FTKDumpCond.IBLMode = 1
FTKDumpCond.DumpBadModules = True
FTKDumpCond.DumpGlobalToLocalMap = True
FTKDumpCond.DumpModulePositions = True
theJob += FTKDumpCond  

print theJob




