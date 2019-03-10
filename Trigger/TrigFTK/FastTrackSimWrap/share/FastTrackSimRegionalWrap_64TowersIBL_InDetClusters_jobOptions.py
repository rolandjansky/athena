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

# SiLorentzAngleTool for SCT
from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()

from TrigFTKTrackConverter.TrigFTKTrackConverterConf import  TrigFTKClusterConverterTool
ClusterConvTool = TrigFTKClusterConverterTool(UsePixelCalibSvc = False,
                                              SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool)

ToolSvc += ClusterConvTool

print "Output file", OutputNTUP_FTKIPFile
wrapper = FTKRegionalWrapper(OutputLevel = DEBUG, 
                             PMapPath = pmap_path,
                             RMapPath = rmap_path,
                             OutFileName = OutputNTUP_FTKIPFile,
                             WriteClustersToESD = True,
                             SaveHits = True,
                             Clustering = True,
                             TrigFTKClusterConverterTool  = ClusterConvTool)
wrapper.IBLMode = 1
wrapper.HitInputTool = FTKSGInput
wrapper.Clustering = True
theJob += wrapper
 
from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamESD("InDet::SCT_ClusterContainer","FTK_SCT_Cluster")
objKeyStore.addStreamESD("InDet::PixelClusterContainer","FTK_Pixel_Clusters")
objKeyStore.addStreamESD("Trk::PRD_MultiTruthCollection","PRD_MultiTruthSCT_FTK")
objKeyStore.addStreamESD("PRD_MultiTruthCollection","PRD_MultiTruthSCT_FTK")
objKeyStore.addStreamESD("PRD_MultiTruthCollection","PRD_MultiTruthPixel_FTK")


print theJob
###############################################################
