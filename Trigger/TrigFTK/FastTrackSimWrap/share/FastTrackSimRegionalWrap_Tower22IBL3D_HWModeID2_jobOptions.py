###############################################################
#
# FTKRegionalWrapper job options file
# copy of FastTrackSimRegionalWrap_64TowersIBL3D_HWModeID2_jobOptions.py
# with Tower22 pixRodIds and sctRodIds specified
#==============================================================

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB_3D_t13.tmap')
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
wrapper.FixEndcapL0 = True
wrapper.HitInputTool = FTKSGInput
wrapper.PixelClusteringMode = 101
wrapper.Ibl3DRealistic = False
wrapper.SctClustering = True
wrapper.Clustering = True
wrapper.EmulateDF = True

####wrapper.GetOffline = True
wrapper.GetOffline = False

### EnhancedSlice22 ROD ID list, see:
### https://its.cern.ch/jira/browse/FTKSIM-107?focusedCommentId=2350985&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-2350985
wrapper.pixRodIds = ['0x140103','0x111750','0x112521','0x112559','0x140093','0x130149','0x111710','0x130146'] 
wrapper.sctRodIds = ['0x220100','0x220103','0x220101','0x220102']

theJob += wrapper

print theJob
###############################################################
