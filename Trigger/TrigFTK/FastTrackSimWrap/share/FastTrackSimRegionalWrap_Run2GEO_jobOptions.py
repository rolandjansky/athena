###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile
#pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW_Run2GEO.pmap')#HW
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')#offline
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB_3D_t13.tmap')#new!
#rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl3DTempv0.tmap')#old
print "Using RMAP:", rmap_path


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


from FastTrackSimWrap.FastTrackSimWrapConf import FTKRegionalWrapper
if hasattr(runArgs,"outputNTUP_FTKIPFile") :
    OutputNTUP_FTKIPFile = runArgs.outputNTUP_FTKIPFile
else :
    OutputNTUP_FTKIPFile = "ftksim_64Towers_wrap.root"

from AthenaCommon.AppMgr import ToolSvc

from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
FTKSGInput.OutputLevel = DEBUG
FTKSGInput.ReadTruthTracks = True

ToolSvc += FTKSGInput

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = DEBUG, 
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = OutputNTUP_FTKIPFile)
wrapper.IBLMode = 2
wrapper.HitInputTool = FTKSGInput
wrapper.FixEndcapL0 = True
#wrapper.Clustering = True
theJob += wrapper

print theJob
###############################################################
