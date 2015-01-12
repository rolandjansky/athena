#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#from RecExConfig.RecFlags import rec
#from AthenaCommon.DetFlags import DetFlags 
#DetFlags.ID_setOn()
#DetFlags.Muon_setOff()
#DetFlags.Calo_setOff()

# Set up the file reading:
#FNAME = "root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking//DxAOD/data14_cos/data14_cos.00239908.physics_IDCosmic.recon.DAOD_Pix_Sct.x269_p002/InDetDxAOD_1.pool.root"
FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5597/data12_8TeV.00204158.physics_JetTauEtmiss.recon.AOD.r5597/AOD.01495682._003054.pool.root.1" 
#FNAME = "/afs/cern.ch/work/m/miclark/someData/mc14_13TeV.180593.Pythia8_AUMSTW2008LO_Wtaunu_3mu_noPhotos.recon.ESD.e3170_s1982_s2008_r5787_tid01578932_00/ESD.01578932._000007.pool.root.1"
import AthenaPoolCnvSvc.ReadAthenaPool

# this line is used w/out RecExCommon
ServiceMgr.EventSelector.InputCollections = [ FNAME ]
# this line is used with RecExCommon (instead of the above)
#athenaCommonFlags.FilesInput = [ FNAME ]

from AthenaCommon.AppMgr import ToolSvc

#import the magnetic field service
#import MagFieldServices.SetupField

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__ToolTester
alg = InDet__ToolTester()

#alg.TrackSelectionTool.UseTrkTrackTools = True # set this to True if working with Trk::Track
alg.TrackSelectionTool.CutLevel = "TightPrimary"
alg.TrackSelectionTool.minPt = 100.0 # default is 400. this will not be overwritten.
alg.TrackSelectionTool.minNBothInnermostLayersHits = 0
# shared BLayer hits does not appear to be patched
alg.TrackSelectionTool.maxNInnermostLayerSharedHits = 1

# control the loading of the TrackSummaryTool
#from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
#AtlasTrackSummaryTool = AtlasTrackSummaryTool()
#ToolSvc += AtlasTrackSummaryTool
#alg.TrackSelectionTool.TrackSummaryTool = AtlasTrackSummaryTool

alg.OutputLevel = DEBUG
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.ReflexPluginDebugLevel = 1
theApp.EvtMax = 20
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

#include("RecExCommon/RecExCommon_topOptions.py")
#include("ElectronPhotonTagTools/ElectronTagTool_jobOptions.py")
