# FTK algorithm inclusions
#from TrigFTKSim.TrigFTKSimConf import FTKRoadFinderAlgo, FTKTrackFitterAlgo, FTK_SGHitInput, FTKDetectorTool, FTKMergerAlgo

#FTKMerger = FTKMergerAlgo( "FTKMergerAlgo" , OutputLevel=INFO)
FTKMergerAlgo = Algorithm( "FTKMergerAlgo" )
#FTKMerger.force_merge     = True # true only for tests or well known reason
#FTKMergerAlgo.useStandalone   = True
#FTKMerger.SingleProces    = True
#FTKMergerAlgo.doMerging       = True

# Map path
from PyJobTransforms.trfUtils import findFile
import os.path
print runArgs
if hasattr(runArgs, 'pmap_path'):
    if os.path.exists(runArgs.pmap_path) :
        FTKMergerAlgo.pmap_path = runArgs.pmap_path
    else :
        stdpath = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/' + runArgs.pmap_path)
        if not os.path.exists(stdpath) :
            raise RuntimeError, 'PMAP %s file not find' % runArgs.pmap_path
        FTKMergerAlgo.pmap_path = stdpath
else:
    raise RuntimeError, "PMAP wasn't not set. Abort"

FTKMergerAlgo.NBanks        = 1
FTKMergerAlgo.NSubRegions     = 1
###Set Hit Warrior parameter
FTKMergerAlgo.HitWarrior      = 2  # Default 2
FTKMergerAlgo.KeepRejected    = 0  # >0 enable HW debug feature
if hasattr(runArgs,"FTKOldFormat") :
   FTKMergerAlgo.mergeTrackBName = "FTKBankMerged"
else :
   FTKMergerAlgo.mergeTrackBName = "FTKMergedTracksStream"

# Set the input FTK tracks file here
if hasattr(runArgs, 'inputNTUP_FTKFile'):
    FTKMergerAlgo.mergedTrackPaths = runArgs.inputNTUP_FTKFile
else:
    raise RuntimeError, 'Failed to find mandatory inputNTUP_FTKFile runtime argument for transform '


from RecExConfig.RecFlags import rec
rec.doAOD=False
rec.doWriteAOD=False
rec.doWriteTAG=False
rec.doTruth=False     # needs to be true if creating Fake RoI

#  ESD filename - switch here between r2e and e2d
if hasattr(runArgs, 'outputDESD_FTKFile'):
    rec.doESD.set_Value_and_Lock( True )
    rec.doWriteESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDOutput.set_Value_and_Lock( runArgs.outputDESD_FTKFile )
if hasattr(runArgs, 'inputDESD_FTKFile'):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readESD.set_Value_and_Lock( True )
    rec.doESD.set_Value_and_Lock( False )
    rec.doWriteESD.set_Value_and_Lock( False )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputDESD_FTKFile )


include("RecExCond/RecExCommon_flags.py")
from RecExConfig.ObjKeyStore import objKeyStore
# FTK Add track containers
objKeyStore.addStreamESD("Rec::TrackParticleContainer", "FTK_TrackParticles")
objKeyStore.addStreamESD("TrackCollection", "FTK_Trk_Tracks")
objKeyStore.addStreamESD("TrigInDetTrackCollection", "FTK_LVL2_Tracks")
objKeyStore.addStreamESD("TrackCollection", "FTK_Trk_Tracks_Refit")
objKeyStore.addStreamESD("TrigInDetTrackCollection", "FTK_LVL2_Tracks_Refit")
# Truth related collections
objKeyStore.addStreamESD("PRD_MultiTruthCollection","PRD_MultiTruthPixel_FTK")
objKeyStore.addStreamESD("PRD_MultiTruthCollection","PRD_MultiTruthSCT_FTK")
objKeyStore.addStreamESD("DetailedTrackTruthCollection","FTK_Trk_Tracks_RefitDetailedTruthCollection")
objKeyStore.addStreamESD("TrackTruthCollection","FTK_Trk_Tracks_RefitTruthCollection")
objKeyStore.addStreamESD("InDet::PixelClusterContainer","FTK_Pixel_Clusters")
objKeyStore.addStreamESD("InDet::SCT_ClusterContainer","FTK_SCT_Cluster")


from TriggerJobOpts.TriggerFlags import TriggerFlags


TriggerFlags.enableMonitoring = [ 'Validation','Time' ]

TriggerFlags.triggerMenuSetup = 'MC_pp_v4'
#TriggerFlags.triggerMenuSetup = 'Physics_pp_v2_no_prescale'
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.doMuon=True
TriggerFlags.doHypo=False
TriggerFlags.doTruth=True
TriggerFlags.doFTK=True

def resetSigs():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.TauSlice.setAll();
#    TriggerFlags.TauSlice.signatures = ['tauNoCut_FTK', 'tau20_medium_FTK','tauNoCut','tau20_medium']
    TriggerFlags.TauSlice.signatures = ['tauNoCut_FTK', 'tau20_medium_FTK']
    TriggerFlags.MuonSlice.setAll();
    TriggerFlags.MuonSlice.signatures = ['mu6_FTK','mu4_NoCut','mu24_tight_FTK','mu4_L2Star_NoCut']
#    TriggerFlags.EgammaSlice.setAll();
#    TriggerFlags.EgammaSlice.signatures = ['e24vh_medium1_FTK','2e12Tvh_loose1_FTK','e24vh_medium1','2e12Tvh_loose1']
    TriggerFlags.BjetSlice.setAll();
    TriggerFlags.BjetSlice.signatures = ['b55_NoCut_j55_a4tchad_FTK','b55_NoCut_j55_a4tchad']
#    
from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(resetSigs)
    

# TrigFTKTool configuration for conversion and refitting
from AthenaCommon.AppMgr import ToolSvc 

from TrigFTKTrackConverter.TrigFTKTrackConverter_Config import ConfiguredTrigFTKTrackConverter 
myConverter = ConfiguredTrigFTKTrackConverter(DoTrackFit=True, DoTruth=True)
ToolSvc += myConverter
FTKMergerAlgo.TrackConverter  = myConverter

from TrigFTKTrackConverter.TrigFTKTrackConverter_Config import ConfiguredTrigFTKUncertiantyTool
myUncertiantyTool = ConfiguredTrigFTKUncertiantyTool()

ToolSvc += myUncertiantyTool
FTKMergerAlgo.UncertiantyTool = myUncertiantyTool

theApp.Dlls += [ "TrigFTKSim" ]
