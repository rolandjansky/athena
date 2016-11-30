###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

# Locate the tmap and pmap files
from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_ITkExample.pmap')
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_ITkExample.tmap')
print "Using PMAP:", pmap_path
print "Using RMAP:", rmap_path

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from FastTrackSimWrap.FastTrackSimWrapConf import FTKRegionalWrapper
if hasattr(runArgs,"outputNTUP_FTKIPFile") :
    OutputNTUP_FTKIPFile = runArgs.outputNTUP_FTKIPFile
else :
    OutputNTUP_FTKIPFile = "ftksim_ITk_wrap.root"

from AthenaCommon.AppMgr import ToolSvc

from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
FTKSGInput = FTK_SGHitInput(maxEta=3.0, minPt=4*GeV)
FTKSGInput.OutputLevel = DEBUG
FTKSGInput.ReadTruthTracks = True
FTKSGInput.DoOutFileRawHits = True

ToolSvc += FTKSGInput

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = DEBUG,
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = OutputNTUP_FTKIPFile)
wrapper.IBLMode = 0
wrapper.ITkMode = True
wrapper.HitInputTool = FTKSGInput
theJob += wrapper

print theJob
###############################################################
