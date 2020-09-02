
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput = [ 'AOD.pool.root' ]

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

theApp.EvtMax = -1
#athenaCommonFlags.SkipEvents = 1

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = [ "file1 DATAFILE='gaussianDensity.root' OPT='RECREATE'" ]

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

from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianTrackDensity
GaussianDensity = Trk__GaussianTrackDensity( name = "GaussianDensity" )

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

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
IPTool = Trk__TrackToVertexIPEstimator( name =          "IPEstimator",
                                        Extrapolator =  InDetExtrapolator )


from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianDensityTestAlg
myAlg =   Trk__GaussianDensityTestAlg( name                   = "TestAlg",
                                       Estimator              = GaussianDensity,
                                       TrackSelector          = InDetTrackSelectorTool,
                                       IPEstimator            = IPTool)
myAlg.OutputLevel = DEBUG
algSeq += myAlg



