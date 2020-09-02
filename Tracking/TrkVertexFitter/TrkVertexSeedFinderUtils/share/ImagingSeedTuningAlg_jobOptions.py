
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput = [ 'AOD.pool.root' ]

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

theApp.EvtMax = -1

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = [ "file1 DATAFILE='tune_32_2048.root' OPT='RECREATE'" ]

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

from AthenaCommon.GlobalFlags import globalflags

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
InDetFlags.preProcessing      = False
InDetFlags.doPRDFormation        = False                   
InDetFlags.doSpacePointFormation = False                                             
InDetFlags.doNewTracking      = False
InDetFlags.doLowPt            = False
InDetFlags.doBackTracking     = False
InDetFlags.doTRTStandalone    = False
InDetFlags.doTrtSegments      = False
InDetFlags.postProcessing     = False
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT = False
InDetFlags.doTrackSegmentsTRT = False
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = False
InDetFlags.doParticleCreation = False
InDetFlags.doConversions      = False
InDetFlags.doSecVertexFinder  = False
InDetFlags.doV0Finder         = False
InDetFlags.doSimpleV0Finder   = False
InDetFlags.doTrkNtuple        = False
InDetFlags.doPixelTrkNtuple   = False
InDetFlags.doSctTrkNtuple     = False
InDetFlags.doTrtTrkNtuple     = False
InDetFlags.doSctClusterNtuple   = False
InDetFlags.doVtxNtuple        = False
InDetFlags.doConvVtxNtuple    = False
InDetFlags.doV0VtxNtuple      = False
InDetFlags.doRefit            = False
InDetFlags.doLowBetaFinder    = False
InDetFlags.doPrintConfigurables = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.lockAllExceptAlias()
InDetKeys.print_JobProperties()

OutputLevel = INFO
doEdmMonitor = False
doJiveXML = False
doVP1 = False
doAuditors = False
doWriteESD = False
doWriteAOD = False

include("InDetRecExample/InDetRec_all.py")

from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__LocalMax1DClusterFinder, Trk__VertexImageMaker

InDetMedImgClusterFinder = Trk__LocalMax1DClusterFinder( name            = "InDetMedImgClusterFinder",
                                                         weightThreshold = 0.02,
                                                         mergeParameter  = 0.5,
                                                         clusterWindowXY = 3.0 )
ToolSvc += InDetMedImgClusterFinder
InDetMedImgMaker = Trk__VertexImageMaker( name                           = "InDetMedImgMaker",
                                          xbins                          = 32,
                                          ybins                          = 32,
                                          zbins                          = 2048,
                                          xrange                         = 2.0,
                                          yrange                         = 2.0,
                                          zrange                         = 200.0,
                                          cutoffFreqDenominator_xy       = 2,
                                          cutoffFreqDenominator_z        = 1 )
ToolSvc += InDetMedImgMaker
from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ImagingSeedFinder
InDetVtxSeedFinder = Trk__ImagingSeedFinder( name                     = "InDetMedImgSeedFinder",
                                             VertexCluster            = InDetMedImgClusterFinder,
                                             VertexImageMaker         = InDetMedImgMaker )
ToolSvc += InDetVtxSeedFinder
  
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name              = "InDetImpactPoint3dEstimator",
                                                          Extrapolator      = InDetExtrapolator)
ToolSvc += InDetImpactPoint3dEstimator


from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool(name = "InDetDetailedTrackSelectionTool",
                                                        CutLevel = InDetPrimaryVertexingCuts.TrackCutLevel(),
                                                        minPt = InDetPrimaryVertexingCuts.minPT(),
                                                        maxD0 = InDetPrimaryVertexingCuts.IPd0Max(),
                                                        maxZ0 = InDetPrimaryVertexingCuts.z0Max(),
                                                        maxZ0SinTheta = InDetPrimaryVertexingCuts.IPz0Max(),
                                                        maxSigmaD0 = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                        maxSigmaZ0SinTheta = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                        maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
                                                        minNInnermostLayerHits = InDetPrimaryVertexingCuts.nHitInnermostLayer(),
                                                        minNPixelHits = InDetPrimaryVertexingCuts.nHitPix(),
                                                        maxNPixelHoles = InDetPrimaryVertexingCuts.nHolesPix(),
                                                        minNSctHits = InDetPrimaryVertexingCuts.nHitSct(),
                                                        minNTrtHits = InDetPrimaryVertexingCuts.nHitTrt(),
                                                        minNSiHits = InDetPrimaryVertexingCuts.nHitSi(),
                                                        TrackSummaryTool = InDetTrackSummaryTool,
                                                        Extrapolator = InDetExtrapolator)
ToolSvc += InDetTrackSelectorTool


from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__ImagingSeedTuningAlg
algSeq += Trk__ImagingSeedTuningAlg( name                   = "SeedTuningAlg",
                                     useBeamConstraint      = True,
                                     truthWindow            = 0.5,
                                     trackFilter            = InDetTrackSelectorTool,
                                     impactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                     seedFinder             = InDetVtxSeedFinder,
                                     angularCutoffParameterValues =  [ 0.43, 0.5, 0.625, 0.75, 0.82 ] ,
                                     clusterWindowXYValues = [ 0.2, 0.34, 0.48, 0.62, 0.76 ],
                                     mergeParameterValues = [ 0.65, 0.7, 0.8, 0.9, 0.95 ],
                                     weightThresholdValues = [ 750.0, 1500.0, 3000.0, 4500.0, 6000.0 ],
                                     refineZValues = [ True, False ],
                                     gaussianWindowValues = [ True, False ])


