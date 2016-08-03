###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

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
FTKSGInput.DoOutFileRawHits = False

ToolSvc += FTKSGInput

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = DEBUG, 
                             PMapPath = 'maps/raw_l1track.pmap',
                             RMapPath = 'maps/raw_l1track.tmap',
                             OutFileName = OutputNTUP_FTKIPFile)
wrapper.IBLMode = 0
wrapper.ITkMode = True
wrapper.HitInputTool = FTKSGInput
theJob += wrapper

print theJob
###############################################################
