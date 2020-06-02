#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def AddJPsiVertexingAlgs(prefix='',addAlgo=True):

  from AthenaCommon.Include import include
  from AthenaCommon.AlgSequence import AlgSequence
  job = AlgSequence()
  from AthenaCommon.AppMgr import ToolSvc
  include( "JpsiUpsilonTools/configureServices.py" )



  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
  VertexUpdater = Trk__KalmanVertexUpdator(name             = "MuonTP_KalmanVertexUpdator")
  ToolSvc += VertexUpdater
  print (     VertexUpdater)



  MuContainer = "Muons"
  if hasattr(job, "CalibratedMuonsProvider"):
    MuContainer = job.CalibratedMuonsProvider.Output


  from AthenaCommon.Constants import WARNING
  from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
  ExampleJpsiFinder = Analysis__JpsiFinder(name                        = prefix+"JpsiFinderName",
                                           muAndMu                     = False,
                                           muAndTrack                  = False,
                                           TrackAndTrack               = True,
                                           assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
                                           invMassUpper                = 3500.0,
                                           invMassLower                = 2700.0,
                                           Chi2Cut                     = 2000.,
                                           allChargeCombinations       = True,
                                           oppChargesOnly              = False,
                                           sameChargesOnly             = False,
                                           trackThresholdPt            = 2500,
                                           muonThresholdPt             = 4000,
                                           atLeastOneComb              = False,
                                           useCombinedMeasurement      = False, # Only takes effect if combOnly=True  
                                           muonCollectionKey           = MuContainer,
                                           # muonCollectionKey           = MuContainer,
                                           TrackParticleCollection     = "InDetTrackParticles",
                                           V0VertexFitterTool          = ToolSvc.TrkV0FitterName,             # V0 vertex fitter
                                           useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                           TrkVertexFitterTool         = ToolSvc.VKalVrtFitterName,        # VKalVrt vertex fitter
                                           TrackSelectorTool           = ToolSvc.InDetDetailedTrackSelectorTool,
                                           ConversionFinderHelperTool  = ToolSvc.InDetConversionFinderUtils,
                                           VertexPointEstimator        = ToolSvc.VtxPointEstimator,
                                           useMCPCuts                  = True, 
                                           OutputLevel                 = WARNING)
  ToolSvc += ExampleJpsiFinder


  # Here, we are stealing a ton of code from the BPHY1 derivation... Sorry! 
  # The reason that we don't simply import the BPHY code here is that we don't run the derivation kernel
  # So we need to plug our tools into a custom alg. 


  from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
  MuonTP_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = prefix+"MuonTP_Reco_mumu",
  JpsiFinder             = ExampleJpsiFinder,
  OutputVtxContainerName = prefix+"JpsiCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = prefix+"RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7,
  OutputLevel           = WARNING )

  ToolSvc += MuonTP_Reco_mumu
  print (MuonTP_Reco_mumu)


  from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

  ## a/ augment and select Jpsi->mumu candidates
  MuonTP_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = prefix+"MuonTP_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = prefix+"JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2700.0,
  MassMax               = 3500.0,
  Chi2Max               = 200,
  DoVertexType          = 7,
  OutputLevel           = WARNING)

  ToolSvc += MuonTP_Select_Jpsi2mumu
  print (MuonTP_Select_Jpsi2mumu)

  if addAlgo:
      from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import MuonTPJpsiVertexFittingAlg
      job += MuonTPJpsiVertexFittingAlg ( name = "MuonTPJpsiVertexFittingAlg",OutputLevel=WARNING)
      job.MuonTPJpsiVertexFittingAlg.AugmentationTools = [MuonTP_Reco_mumu, MuonTP_Select_Jpsi2mumu]
  else:
      return [MuonTP_Reco_mumu, MuonTP_Select_Jpsi2mumu]


  # from JpsiUpsilonTools.JpsiUpsilonToolsConf import JpsiAlg

  # job += JpsiAlg(JpsiCandidatesOutputName = "JpsiCandidates", OutputLevel=WARNING,
  #                        JpsiFinderName           = ToolSvc.JpsiFinderName)

