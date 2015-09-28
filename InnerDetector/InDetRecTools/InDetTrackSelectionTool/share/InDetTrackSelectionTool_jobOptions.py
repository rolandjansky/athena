#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#from RecExConfig.RecFlags import rec
#from AthenaCommon.DetFlags import DetFlags 
#DetFlags.ID_setOn()
#DetFlags.Muon_setOff()
#DetFlags.Calo_setOff()

# Set up the file reading:
#the rel20 no pileup
#FNAME = "valid1:valid1.147407.PowhegPythia8_AZNLO_Zmumu.recon.AOD.e3099_s1982_s1964_r6008/"

#FNAME = "root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking//DxAOD/data14_cos/data14_cos.00239908.physics_IDCosmic.recon.DAOD_Pix_Sct.x269_p002/InDetDxAOD_1.pool.root"
FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r6630/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s2576_s2132_r6630_tid05348608_00/AOD.05348608._000313.pool.root.1"

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
#alg.TrackSelectionTool.maxNSiHoles = -1
#alg.TrackSelectionTool.maxNPixelHoles = 0
#alg.TrackSelectionTool.minPt = 100. # default is 400. this will not be overwritten.
#alg.TrackSelectionTool.minNBothInnermostLayersHits = 0
#alg.TrackSelectionTool.minNUsedHitsdEdx = 1
#alg.TrackSelectionTool.maxNInnermostLayerSharedHits = 1
#alg.TrackSelectionTool.minPtForProbCut = 1e4 # 10 GeV
#alg.TrackSelectionTool.minProbAbovePtCutoff = .01
#alg.TrackSelectionTool.minEProbabilityHT = .01
#alg.TrackSelectionTool.eProbHTonlyForXe = True
#alg.TrackSelectionTool.minNSiHitsIfSiSharedHits = 10

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
