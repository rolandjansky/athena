###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl32TmodB_3D_t13.tmap')
print "Using RMAP:", rmap_path


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.GlobalFlags import globalflags
print globalflags


from FastTrackSimWrap.FastTrackSimWrapConf import FTKRegionalWrapper
if hasattr(runArgs,"outputNTUP_FTKIPFile") :
    OutputNTUP_FTKIPFile = runArgs.outputNTUP_FTKIPFile
else :
    OutputNTUP_FTKIPFile = "ftksim_32Towers_wrap.root"

from AthenaCommon.AppMgr import ToolSvc

from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
FTKSGInput.OutputLevel = DEBUG
FTKSGInput.ReadTruthTracks = False

ToolSvc += FTKSGInput

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = DEBUG, 
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = OutputNTUP_FTKIPFile)
wrapper.IBLMode = 2
wrapper.HitInputTool = FTKSGInput
wrapper.Clustering = True
wrapper.PixelClusteringMode=101
wrapper.Ibl3DRealistic=False
wrapper.SavePerPlane=True
wrapper.EmulateDF = True
wrapper.SaveRawHits = True
wrapper.SaveHits = True
wrapper.DuplicateGanged = False
wrapper.SctClustering=True

print "FTKRegionalWrapper setting ROBS"
wrapper.pixRodIds = ["0x112414", "0x140170", "0x111816", "0x112411", "0x112416", "0x140140", "0x140180", "0x130011"]
wrapper.sctRodIds = ["0x210000", "0x210109", "0x210108", "0x230100", "0x220000", "0x220109", "0x22010a", "0x21010a"]
theJob += wrapper

print theJob
###############################################################
