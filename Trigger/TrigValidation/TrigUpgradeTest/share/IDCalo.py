#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
viewSeq = AthSequencer("AthViewSeq", Sequential=True, ModeOR=False, StopOverride=False)
topSequence += viewSeq

roiCollectionName =  "EMRoIs"  

# View maker alg
from AthenaCommon import CfgMgr
viewNodeName = "allViewAlgorithms"
viewMaker = CfgMgr.AthViews__RoiCollectionToViews("viewMaker")
viewMaker.ViewBaseName = "testView"
viewMaker.InputRoICollection = roiCollectionName
viewMaker.ViewNodeName = viewNodeName
viewMaker.OutputRoICollection = "EMViewRoIs"
viewMaker.ViewFallThrough = True
viewSeq += viewMaker

# Set of view algs
allViewAlgorithms = AthSequencer(viewNodeName, Sequential=False, ModeOR=False, StopOverride=False)


if TriggerFlags.doID:

  from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
  
  (viewAlgs, eventAlgs) = makeInDetAlgs("FS")

  for eventAlg in eventAlgs:
    viewSeq += eventAlg

  for viewAlg in viewAlgs:
    allViewAlgorithms += viewAlg



  for viewAlg in viewAlgs:
        if "RoIs" in viewAlg.properties():
            viewAlg.RoIs = roiCollectionName
        if "roiCollectionName" in viewAlg.properties():
            viewAlg.roiCollectionName = roiCollectionName


   #Adding vertexing
  #from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices 
  from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
  #from TrigInDetConfig.TrigInDetPriVtxConfig
  #from TrigUpgradeTest.TrigInDetPriVtxConfig import makeVertices
#TrigInDetConfig/TrigInDetPriVtxConfig

  #TODO need to change the name of the output vertex collection to something recordable
  vtxAlgs = makeVertices( "egamma", "HLT_xAODTracks_FS", "HLT_xPrimVx"  )
  allViewAlgorithms += vtxAlgs


   #Adding precision tracking
  from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
  PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "egamma", inputFTFtracks="TrigFastTrackFinder_Tracks_FS" )

  allViewAlgorithms += PTAlgs

   #Testing BeamSpotAlg in Run3 configuration
  prefixName = "InDetTrigMT"
  from TrigVertexFitter.TrigVertexFitterConf import TrigPrimaryVertexFitter
  primaryVertexFitter = TrigPrimaryVertexFitter(  name = prefixName + "VertexFitter",
                                                   zVariance=3.0, 
                                                   CreateTrackLists=True )

  #Can it be added to the service when we need to make it private?
  ToolSvc += primaryVertexFitter

  from TrigT2BeamSpot.T2VertexBeamSpotMonitoring import  T2VertexBeamSpotAlgMonitoring, T2VertexBeamSpotToolMonitoring
  alg = T2VertexBeamSpotAlgMonitoring()
  toolMon = T2VertexBeamSpotToolMonitoring()

  from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpotTool
  InDetTrigMTBeamSpotTool = PESA__T2VertexBeamSpotTool( name = "TestBeamSpotTool",
                                                        OutputLevel = INFO,
                                                        MonTool = toolMon,
                                                        nSplitVertices      = 1,        # Turn on (>1) or off vertex splitting
                                                        ReclusterSplit      = False,    # Recluster split track collections before vertex fitting
                                                        WeightClusterZ      = True,     # Use the track Z0 weighted cluster Z position as seed            
                                                        
                                                        TotalNTrackMin      = 4,        # Minimum number of tracks required in an event
                                                        TrackMinPt          = 0.5,      # Minimum track pT to be considered for vertexing
                                                        TrackSeedPt         = 0.7,      # Minimum track pT to be considered for seeding a vertex fit
                                                        TrackClusterDZ      = 0.35,      # Maximum distance between tracks considered as a cluster
                                                        TrackMaxZ0          = 200.0,    # Maximum track Z0 to be considered for vertexing
                                                        TrackMaxD0          = 10.0,     # Maximum track d0 to be considered for vertexing
                                                        TrackMaxZ0err       = 5.0,      # Maximum track Z0 error to be considered for vertexing
                                                        TrackMaxD0err       = 5.0,      # Maximum track d0 error to be considered for vertexing
                                                        TrackMinNDF         = 2.0,      # Minimum track NDF to be considered for vertexing
                                                        TrackMinQual        = 0.0,      # Minimum track chi^2/NDF to be considered for vertexing
                                                        TrackMaxQual        = 10.0,     # Maximum track chi^2/NDF to be considered for vertexing
                                                        TrackMinChi2Prob    = -10.0,    # Minimum track cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh 
                                                        TrackMinSiHits      = 7,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                        TrackMinPIXHits     = 0,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                        TrackMinSCTHits     = 0,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                        TrackMinTRTHits     = -10,      # Minimum # track TRT hits to be considered for vertexing
                                                        
                                                        VertexMinNTrk       = 2,        # Minimum # tracks in a cluster to be considered for vertexing
                                                        VertexMaxNTrk       = 100,      # Maximum # tracks in a cluster to be considered for vertexing (saves on time!)
                                                        VertexMaxXerr       = 1.,       # Maximum resulting X error on vertex fit for "good" vertices
                                                        VertexMaxYerr       = 1.,       # Maximum resulting Y error on vertex fit for "good" vertices
                                                        VertexMaxZerr       = 10.,      # Maximum resulting Z error on vertex fit for "good" vertices
                                                        VertexMinQual       = 0.0,      # Minimum resulting chi^2/NDF on vertex fit for "good" vertices
                                                        VertexMaxQual       = 100.0,    # Maximum resulting chi^2/NDF on vertex fit for "good" vertices
                                                        VertexMinChi2Prob   = -10.0,    # Minimum cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh      
                                                        VertexBCIDMinNTrk   = 10,       # Minimum # tracks in a vertex to be used for per-BCID monitoring
                                                        PrimaryVertexFitter = primaryVertexFitter )
      
  ToolSvc += InDetTrigMTBeamSpotTool 



#Testing base default class
  from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpot
  InDetTrigMTBeamSpotAlg = PESA__T2VertexBeamSpot( name = "TestBeamSpotAlg",
                                                   OutputLevel =INFO,
                                                   MonTool = alg,
                                                   vertexCollName      = "TrigBeamSpotVertex", # Output vertex collection Name
                                                   TrackCollections  = [ PTTracks[-1] ],   #For now using PT tracks as a test but FTF should be enough
                                                   BeamSpotTool = InDetTrigMTBeamSpotTool )   



  allViewAlgorithms += InDetTrigMTBeamSpotAlg

if TriggerFlags.doCalo:
  from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
  algo=T2CaloEgamma_ReFastAlgo("testFastAlgo")

  algo.RoIs="EMViewRoIs"
  allViewAlgorithms += algo

viewSeq += allViewAlgorithms
