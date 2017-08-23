###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl.pmap')
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl.tmap')
print "Using RMAP:", rmap_path


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.GlobalFlags import globalflags
print globalflags




from FastTrackSimWrap.FastTrackSimWrapConf import FTKRegionalWrapper
if hasattr(runArgs,"outputNTUP_FTKIPFile") :
    OutputNTUP_FTKIPFile = runArgs.outputNTUP_FTKIPFile
else :
    OutputNTUP_FTKIPFile = "ftksim_64Towers_wrap.root"

from AthenaCommon.AppMgr import ToolSvc

from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
FTKSGInput.OutputLevel = DEBUG
FTKSGInput.ReadTruthTracks = False

ToolSvc += FTKSGInput

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = INFO, 
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = OutputNTUP_FTKIPFile)

wrapper.IBLMode = 1
wrapper.HitInputTool = FTKSGInput
wrapper.Clustering = True
wrapper.PixelClusteringMode=100
wrapper.Ibl3DRealistic=False
wrapper.SavePerPlane=True
wrapper.EmulateDF = True
wrapper.SaveRawHits = True
wrapper.SaveHits = True


#wrapper.pixRodIds = {0x130011, 0x111510, 0x111508, 0x112414, 0x130015, 0x111716, 0x112416}
#wrapper.sctRodIds = {0x220005, 0x210005, 0x220007}
#wrapper.pixRodIds = {130011, 111510, 111508, 112414, 130015, 111716, 112416}
#wrapper.sctRodIds = {220005, 210005, 220007}
theJob += wrapper

print theJob
###############################################################
