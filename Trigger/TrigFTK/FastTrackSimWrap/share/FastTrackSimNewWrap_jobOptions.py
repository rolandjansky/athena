###############################################################
#
# FTKRegionalWrapper job options file
#
#==============================================================

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_11L.pmap')
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_11L.tmap')
print "Using RMAP:", rmap_path

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


from FastTrackSimWrap.FastTrackSimWrapConf import FTKRegionalWrapper
if hasattr(runArgs,"outputNTUP_FTKIPFile") :
    OutputNTUP_FTKIPFile = runArgs.outputNTUP_FTKIPFile
else :
    OutputNTUP_FTKIPFile = 'ftksim_newwrap_raw.root'

print "Output file", OutputNTUP_FTKIPFile

theJob += FTKRegionalWrapper(OutputLevel = DEBUG, 
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = "ftksim_regionalwrap.root")

print theJob
###############################################################
