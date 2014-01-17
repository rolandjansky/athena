/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////

#include "TRT_DetectorPerformance/TRT_DetectorPlots.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkEventPrimitives/FitQuality.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"

#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "CommissionEvent/ComTime.h"

#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "TRT_TrackSegmentsFinder/TRT_TrackSegmentsFinder.h"
//#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_BarrelCosmics.h"
//#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_ATLxk.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "TTree.h"


#include "TRT_DetectorPerformance/TRT_StrawMap.h"





//#include "TrkEventPrimitives/GlobalPosition.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"


#include "TrigSteeringEvent/Lvl1Result.h"

//#include "TrigSteering/Chain.h"
#include "TrigSteering/StreamTag.h"
#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/Algo.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

//#include "TrkParameters/MeasuredAtaStraightLine.h"

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetRawData/SCT3_RawData.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h" 
#include "TrigInDetEvent/TrigSiSpacePoint.h"

/////////////////////////////////////////////////////////////////////////////

InDet::TRT_DetectorPlots::TRT_DetectorPlots( const std::string& name
                         , ISvcLocator* pSvcLocator
                         ) : AthAlgorithm(name, pSvcLocator)
                           , m_doMC(0)
                           , m_doRDO(0)
                           , m_numNoise(0)
                           , m_doSegments(0)
                           , m_barrelSegmentsName("TRT_SegmentsBC")
                           , m_endcapSegmentsName("TRT_Segments_EC")
                           , m_tracksName("CombinedInDetTracks")
                           //, SCT_Cosmic_Tracks
                           //, Tracks
                           //, TRT_Cosmic_Tracks
                           //, InDetCosmic_Tracks
                           , m_fileName("TRT_DetectorPlots.L2eff.txt")
                           , m_histSvc("THistSvc", name)
                           , m_trtDetMgr(0)
                           , m_trtHelper(0)
                           , m_trtCondDbTool("TRT_CalDbSvc", name)
                           , m_trtStrawNeighbourTool("TRT_StrawNeighbourSvc", name)
                           , m_trtStrawMap("InDet::TRT_StrawMap")
                           , m_rdoContainer(0)
                           , m_trtDriftCircleContainer(0)
                           , m_segmentCollectionBarrel(0)
                           , m_segmentCollectionEc(0)
                           , m_trkCollection(0)
                           , m_truthTrt(0)
                           , m_runNumber(0)  
{ // Properties

  declareProperty("histogramService", m_histSvc);

  declareProperty("doMC", m_doMC);
  declareProperty("doRDO", m_doRDO);
  declareProperty("numNoise", m_numNoise);

  declareProperty("doSegments", m_doSegments);
  declareProperty("barrelSegmentsCollection", m_barrelSegmentsName);
  declareProperty("endcapSegmentsCollection", m_endcapSegmentsName);

  declareProperty("tracksCollection", m_tracksName);
  declareProperty("fileName", m_fileName);

  return;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetectorPlots::initialize()
{
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "InDet::TRT_DetectorPlots::initialize(), doSegments: " << m_doSegments 
                                        << " do Noise: " << m_numNoise 
                                        << endreq; 
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "InDet::TRT_DetectorPlots::initialize(), tracks collection: " << m_tracksName 
                                        << " segment collections: " << m_barrelSegmentsName << ", " << m_endcapSegmentsName
                                        << endreq;

  /////////////////////////////////////////////////
  // Create TTrees and register them to THistSvc
  /////////////////////////////////////////////////
  if ( m_histSvc.retrieve().isFailure() )
    {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "histogram service not found" << endreq;
      return StatusCode::FAILURE;
    }
  else 
    {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "retrieved histogramService" << endreq;
    }
  m_treeEvent   = new TTree("event"  , "event"  );
  m_treeTrack   = new TTree("track"  , "track"  );
  m_treeSegment = new TTree("segment", "segment");
  m_treeHits    = new TTree("hits"   , "hits"   );
  m_treePixel   = new TTree("pixel"  , "pixel"  );

  std::string fullTreeNameEvent =  "/AANT/event" ;
  if((m_histSvc->regTree(fullTreeNameEvent, m_treeEvent)).isFailure()) 
    {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeNameEvent << endreq;
      return StatusCode::FAILURE;
    }

  std::string fullTreeNameTrack =  "/AANT/track" ;
  if((m_histSvc->regTree(fullTreeNameTrack, m_treeTrack)).isFailure()) 
    {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeNameTrack << endreq;
      return StatusCode::FAILURE;
    }

  std::string fullTreeNameSegment =  "/AANT/segment" ;
  if((m_histSvc->regTree(fullTreeNameSegment, m_treeSegment)).isFailure()) 
    {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeNameSegment << endreq;
      return StatusCode::FAILURE;
    }

  std::string fullTreeNameHits =  "/AANT/hits" ;
  if((m_histSvc->regTree(fullTreeNameHits, m_treeHits)).isFailure()) 
    {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeNameHits << endreq;
      return StatusCode::FAILURE;
    }

  std::string fullTreeNamePixel =  "/AANT/pixel" ;
  if((m_histSvc->regTree(fullTreeNamePixel, m_treePixel)).isFailure()) 
    {
      if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeNamePixel << endreq;
      return StatusCode::FAILURE;
    }

  ///////////////////////////////////////////////
  /// Declare vectors that are used in filling 
  /// the output ntuple
  ///////////////////////////////////////////////
  m_trackSide            = new std::vector<int>;
  m_trackLayerOrWheel    = new std::vector<int>;
  m_trackPhi             = new std::vector<int>;
  m_trackStrawLayer      = new std::vector<int>;
  m_trackStraw           = new std::vector<int>;
  m_trackStrawNum        = new std::vector<int>;
  m_trackChip            = new std::vector<int>;
  m_trackBoard           = new std::vector<int>;
  m_trackDriftTimeBin    = new std::vector<int>;
  m_trackTrailingEdge    = new std::vector<int>;
  m_trackHighThreshold   = new std::vector<int>;
  m_trackDriftCircleWord = new std::vector<int>;
  m_trackX               = new std::vector<double>;
  m_trackY               = new std::vector<double>;
  m_trackZ               = new std::vector<double>;
  m_trackT0              = new std::vector<double>;

  m_trackTrackR = new std::vector<double>;
  m_trackLocR   = new std::vector<double>;
  m_trackErrR   = new std::vector<double>;

  m_eventPixelX          = new std::vector<double>;
  m_eventPixelY          = new std::vector<double>;
  m_eventPixelZ          = new std::vector<double>;
  m_eventPixelHitTimeBin = new std::vector<int>;
  m_eventSCTX            = new std::vector<double>;
  m_eventSCTY            = new std::vector<double>;
  m_eventSCTZ            = new std::vector<double>;
  m_eventSCTHitTimeBin   = new std::vector<int>;  
  m_eventTRTX            = new std::vector<double>;
  m_eventTRTY            = new std::vector<double>;
  m_eventTRTZ            = new std::vector<double>;

  m_segmentSide            = new std::vector<int>;
  m_segmentLayerOrWheel    = new std::vector<int>;
  m_segmentPhi             = new std::vector<int>;
  m_segmentStrawLayer      = new std::vector<int>;
  m_segmentStraw           = new std::vector<int>;
  m_segmentStrawNum        = new std::vector<int>;
  m_segmentChip            = new std::vector<int>;
  m_segmentBoard           = new std::vector<int>;
  m_segmentDriftTimeBin    = new std::vector<int>;
  m_segmentTrailingEdge    = new std::vector<int>;
  m_segmentDriftCircleWord = new std::vector<int>;
  m_segmentHitOnTrack      = new std::vector<int>;
  m_segmentX                 = new std::vector<double>;
  m_segmentY                 = new std::vector<double>;
  m_segmentZ                 = new std::vector<double>;
  m_segmentT0              = new std::vector<double>;

  m_onTrackIdentifiers = new std::vector<Identifier>;

  m_pixelL1TimeBin = new std::vector<int>;

  //////////////////////////////////////////////////////
  /// Book variables to be added to the output ntuple
  //////////////////////////////////////////////////////

  m_treeEvent->Branch("run_number"  , &m_trackRunNumber     , "run_number/I"   );
  m_treeEvent->Branch("event_number", &m_trackEventNumber   , "event_number/I" );
  m_treeEvent->Branch("n_tracks"    , &m_eventNumberOfTracks, "n_tracks/I"     ); 

  m_treeEvent->Branch("n_TRT_hits"              , m_eventNumberOfHitsOnTracks  , "n_TRT_hits/I"               );
  m_treeEvent->Branch("n_TRT_barrel_hits"       , m_eventNumberOfHitsOnTracks+1, "n_TRT_barrel_hits/I"        );
  m_treeEvent->Branch("n_TRT_maxonlybarrel_hits", m_eventNumberOfHitsOnTracks+2, "n_TRT_maxonlybarrel_hits/I" );
  m_treeEvent->Branch("n_SCT_hits"              , m_eventNumberOfHitsOnTracks+3, "n_SCT_hits/I"               );
  m_treeEvent->Branch("n_SCT_barrel_hits"       , m_eventNumberOfHitsOnTracks+4, "n_SCT_barrel_hits/I"        );
  m_treeEvent->Branch("n_pixel_hits"            , m_eventNumberOfHitsOnTracks+5, "n_pixel_hits/I"             );
  m_treeEvent->Branch("n_pixel_barrel_hits"     , m_eventNumberOfHitsOnTracks+6, "n_pixel_barrel_hits/I"      );

  m_treeEvent->Branch("n_sp_upper"  , m_siSP        , "n_sp_upper/I"   );
  m_treeEvent->Branch("n_sp_lower"  , m_siSP+1      , "n_sp_lower/I"   );
  m_treeEvent->Branch("SiTrack"     , m_eventL2Alg  , "SiTrack/I"      );
  m_treeEvent->Branch("IDScan"      , m_eventL2Alg+1, "IDScan/I"       );
  m_treeEvent->Branch("TRTSegFinder", m_eventL2Alg+2, "TRTSegFinder/I" );

  m_treeEvent->Branch("trigger_L1Type"  , &m_eventL1Type             , "trigger_L1Type/I"   ); 
  m_treeEvent->Branch("trigger_TGC"     , &m_trackTrig1MU1           , "trigger_TGC/I"      );
  m_treeEvent->Branch("trigger_RPC"     , &m_trackTrig1MU2           , "trigger_RPC/I"      );
  m_treeEvent->Branch("trigger_L1Calo"  , &m_trackTrigL1Calo           , "trigger_L1Calo/I"   );
  m_treeEvent->Branch("trigger_IDCosmic", &m_trackTrigAllTeIDSelected, "trigger_IDCosmic/I" );

  m_treeEvent->Branch("phi0"  , m_eventTrackInfo  , "phi0/D"   );
  m_treeEvent->Branch("d0"    , m_eventTrackInfo+1, "d0/D"     );
  m_treeEvent->Branch("z0"    , m_eventTrackInfo+2, "z0/D"     );
  m_treeEvent->Branch("theta" , m_eventTrackInfo+3, "theta/D"  );
  m_treeEvent->Branch("qOverP", m_eventTrackInfo+4, "qOverP/D" );

  m_treeEvent->Branch("comTime"     , &m_pixelComTime     , "comTime/D"      ); 
  m_treeEvent->Branch("pixelTimeBin", &m_eventPixelTimeBin, "pixelTimeBin/D" ); 

  m_treeEvent->Branch("pixelX"         , &m_eventPixelX          );
  m_treeEvent->Branch("pixelY"         , &m_eventPixelY          );
  m_treeEvent->Branch("pixelZ"         , &m_eventPixelZ          );
  m_treeEvent->Branch("pixelHitTimeBin", &m_eventPixelHitTimeBin );
  m_treeEvent->Branch("sctX"           , &m_eventSCTX            );
  m_treeEvent->Branch("sctY"           , &m_eventSCTY            );
  m_treeEvent->Branch("sctZ"           , &m_eventSCTZ            );
  m_treeEvent->Branch("sctHitTimeBin"  , &m_eventSCTHitTimeBin   );  
  m_treeEvent->Branch("trtX"           , &m_eventTRTX            );
  m_treeEvent->Branch("trtY"           , &m_eventTRTY            );
  m_treeEvent->Branch("trtZ"           , &m_eventTRTZ            );

  m_treeTrack->Branch("run_number"             , &m_trackRunNumber          , "run_number/I"              );
  m_treeTrack->Branch("event_number"           , &m_trackEventNumber        , "event_number/I"            );
  m_treeTrack->Branch("trigger_L1Type"         , &m_trackTrigL1Type         , "trigger_L1Type/I"          );
  m_treeTrack->Branch("trigger_stream_size"    , &m_trackTrigStreamSize     , "trigger_stream_size/I"     );
  m_treeTrack->Branch("trigger_1MU1"           , &m_trackTrig1MU1           , "trigger_1MU1/I"            );
  m_treeTrack->Branch("trigger_1MU2"           , &m_trackTrig1MU2           , "trigger_1MU2/I"            );
  m_treeTrack->Branch("trigger_1MU3"           , &m_trackTrig1MU3           , "trigger_1MU3/I"            );
  m_treeTrack->Branch("trigger_L1Calo"         , &m_trackTrigL1Calo         , "trigger_L1Calo/I"          );
  m_treeTrack->Branch("trigger_NIM0"           , &m_trackTrigNIM0           , "trigger_NIM0/I"            );
  m_treeTrack->Branch("trigger_NIM4"           , &m_trackTrigNIM4           , "trigger_NIM4/I"            );
  m_treeTrack->Branch("trigger_MBTS"           , &m_trackTrigMBTS           , "trigger_MBTS/I"            );
  m_treeTrack->Branch("trigger_EMCalo"         , &m_trackTrigEML1Calo       , "trigger_EMCalo/I"          );
  m_treeTrack->Branch("trigger_HLTerror"       , &m_trackTrigHLTerror       , "trigger_HLTerror/I"        );
  m_treeTrack->Branch("trigger_AllTeIDSelected", &m_trackTrigAllTeIDSelected, "trigger_AllTeIDSelected/I" );
  m_treeTrack->Branch("trigger_AllTePixel"     , &m_trackTrigAllTePixel     , "trigger_AllTePixel/I"      );
  m_treeTrack->Branch("n_TRT_tracks"           , &m_trackNumTrtTracks       , "n_TRT_tracks/I"            );
  m_treeTrack->Branch("n_tracks"               , &m_trackNumTracks          , "n_tracks/I"                );
  m_treeTrack->Branch("n_SCT_hits"             , &m_trackNumSctHits         , "n_SCT_hits/I"              );
  m_treeTrack->Branch("n_TRT_hits"             , &m_trackNumTrtHits         , "n_TRT_hits/I"              );
  m_treeTrack->Branch("n_TRT_hits_barrelA"     , &m_trackNumTrtHitsBarrelA  , "n_TRT_hits_barrelA/I"      );
  m_treeTrack->Branch("n_TRT_hits_barrelC"     , &m_trackNumTrtHitsBarrelC  , "n_TRT_hits_barrelC/I"      );
  m_treeTrack->Branch("n_TRT_hits_ecA"         , &m_trackNumTrtHitsEcA      , "n_TRT_hits_ecA/I"          );
  m_treeTrack->Branch("n_TRT_hits_ecC"         , &m_trackNumTrtHitsEcC      , "n_TRT_hits_ecC/I"          );
  m_treeTrack->Branch("ndf"                    , &m_trackNdf                , "ndf/I"                     );
  m_treeTrack->Branch("chi2"                   , &m_trackChi2               , "chi2/D"                    );
  m_treeTrack->Branch("theta"                  , &m_trackTheta              , "theta/D"                   );
  m_treeTrack->Branch("d0"                     , &m_trackD0                 , "d0/D"                      );
  m_treeTrack->Branch("z0"                     , &m_trackZ0                 , "z0/D"                      );
  m_treeTrack->Branch("phi0"                   , &m_trackPhi0               , "phi0/D"                    );
  m_treeTrack->Branch("EP"                     , &m_trackEventPhase         , "EP/D"                      );
  m_treeTrack->Branch("comTime"                , &m_pixelComTime            , "comTime/D"                 ); 

  m_treeTrack->Branch("side"          , &m_trackSide            );
  m_treeTrack->Branch("layer_or_wheel", &m_trackLayerOrWheel    );
  m_treeTrack->Branch("phi"           , &m_trackPhi             );
  m_treeTrack->Branch("straw_layer"   , &m_trackStrawLayer      );
  m_treeTrack->Branch("straw"         , &m_trackStraw           );
  m_treeTrack->Branch("straw_number"  , &m_trackStrawNum        );
  m_treeTrack->Branch("chip"          , &m_trackChip            );
  m_treeTrack->Branch("board"         , &m_trackBoard           );
  m_treeTrack->Branch("LE"            , &m_trackDriftTimeBin    );
  m_treeTrack->Branch("TE"            , &m_trackTrailingEdge    );
  m_treeTrack->Branch("HT"            , &m_trackHighThreshold   );
  m_treeTrack->Branch("word"          , &m_trackDriftCircleWord );
  m_treeTrack->Branch("x"             , &m_trackX               );
  m_treeTrack->Branch("y"             , &m_trackY               );
  m_treeTrack->Branch("z"             , &m_trackZ               );
  m_treeTrack->Branch("t0"            , &m_trackT0              );

  m_treeTrack->Branch("trackR", &m_trackTrackR );
  m_treeTrack->Branch("locR"  , &m_trackLocR   );
  m_treeTrack->Branch("errR"  , &m_trackErrR   );

  m_treeSegment->Branch("side"          , &m_segmentSide            );
  m_treeSegment->Branch("layer_or_wheel", &m_segmentLayerOrWheel    );
  m_treeSegment->Branch("phi"           , &m_segmentPhi             );
  m_treeSegment->Branch("straw_layer"   , &m_segmentStrawLayer      );
  m_treeSegment->Branch("straw"         , &m_segmentStraw           );
  m_treeSegment->Branch("straw_number"  , &m_segmentStrawNum        );
  m_treeSegment->Branch("chip"          , &m_segmentChip            );
  m_treeSegment->Branch("board"         , &m_segmentBoard           );
  m_treeSegment->Branch("LE"            , &m_segmentDriftTimeBin    );
  m_treeSegment->Branch("TE"            , &m_segmentTrailingEdge    );
  m_treeSegment->Branch("word"          , &m_segmentDriftCircleWord );
  m_treeSegment->Branch("hit_on_track"  , &m_segmentHitOnTrack      );
  m_treeSegment->Branch("x"             , &m_segmentX               );
  m_treeSegment->Branch("y"             , &m_segmentY               );
  m_treeSegment->Branch("z"             , &m_segmentZ               );
  m_treeSegment->Branch("t0"            , &m_segmentT0              );


  m_treeHits->Branch("side"          , &m_hitsSide           , "side/I"           );
  m_treeHits->Branch("layer_or_wheel", &m_hitsLayerOrWheel   , "layer_or_wheel/I" );
  m_treeHits->Branch("phi"           , &m_hitsPhi            , "phi/I"            );
  m_treeHits->Branch("straw_layer"   , &m_hitsStrawLayer     , "straw_layer/I"    );
  m_treeHits->Branch("straw"         , &m_hitsStraw          , "straw/I"          );
  m_treeHits->Branch("straw_number"  , &m_hitsStrawNum       , "straw_number/I"   );
  m_treeHits->Branch("chip"          , &m_hitsChip           , "chip/I"           );
  m_treeHits->Branch("board"         , &m_hitsBoard          , "board/I"          );
  m_treeHits->Branch("LE"            , &m_hitsDriftTimeBin   , "LE/I"             );
  m_treeHits->Branch("TE"            , &m_hitsTrailingEdge   , "TE/I"             );
  m_treeHits->Branch("word"          , &m_hitsDriftCircleWord, "word/I"           );
  m_treeHits->Branch("hit_on_track"  , &m_hitsHitOnTrack     , "hit_on_track/I"   );
  m_treeHits->Branch("x"             , &m_hitsX              , "x/D"              );
  m_treeHits->Branch("y"             , &m_hitsY              , "y/D"              );
  m_treeHits->Branch("z"             , &m_hitsZ              , "z/D"              );
  m_treeHits->Branch("t0"            , &m_hitsT0             , "t0/D"             );

  m_treePixel->Branch("run_number"        , &m_trackRunNumber                      , "run_number/I");
  m_treePixel->Branch("n_pixel_hits"      , &m_pixelNumberOfPixelHits              , "n_pixel_hits/I");
  m_treePixel->Branch("n_TRT_hits"        , &m_pixelMaxNumberOfTRTBarrelHitsOnTrack, "n_TRT_hits/I");
  m_treePixel->Branch("n_TRT_hits_barrelA", &m_trackNumTrtHitsBarrelA              , "n_TRT_hits_barrelA/I");
  m_treePixel->Branch("n_TRT_hits_barrelC", &m_trackNumTrtHitsBarrelC              , "n_TRT_hits_barrelC/I");
  m_treePixel->Branch("n_TRT_hits_ecA"    , &m_trackNumTrtHitsEcA                  , "n_TRT_hits_ecA/I");
  m_treePixel->Branch("n_TRT_hits_ecC"    , &m_trackNumTrtHitsEcC                  , "n_TRT_hits_ecC/I");

  m_treePixel->Branch("d0"                 , &m_pixelTRTtrackD0    , "d0/D"                  );
  m_treePixel->Branch("phi0"               , &m_pixelTRTtrackPhi0  , "phi0/D"                );
  m_treePixel->Branch("EP"                 , &m_pixelTRTtrackEP    , "EP/D"                  );
  m_treePixel->Branch("comTime"            , &m_pixelComTime       , "comTime/D"             );
  m_treePixel->Branch("trigger_L1Type"     , &m_trackTrigL1Type    , "trigger_L1Type/I"      );
  m_treePixel->Branch("trigger_stream_size", &m_trackTrigStreamSize, "trigger_stream_size/I" );
  m_treePixel->Branch("trigger_1MU1"       , &m_trackTrig1MU1      , "trigger_1MU1/I"        );
  m_treePixel->Branch("trigger_1MU2"       , &m_trackTrig1MU2      , "trigger_1MU2/I"        );
  m_treePixel->Branch("trigger_1MU3"       , &m_trackTrig1MU3      , "trigger_1MU3/I"        );
  m_treePixel->Branch("trigger_L1Calo"     , &m_trackTrigL1Calo    , "trigger_L1Calo/I"      );
  m_treePixel->Branch("trigger_NIM0"       , &m_trackTrigNIM0      , "trigger_NIM0/I"        );
  m_treePixel->Branch("trigger_NIM4"       , &m_trackTrigNIM4      , "trigger_NIM4/I"        );
  m_treePixel->Branch("trigger_MBTS"       , &m_trackTrigMBTS      , "trigger_MBTS/I"        );
  m_treePixel->Branch("trigger_EMCalo"     , &m_trackTrigEML1Calo  , "trigger_EMCalo/I"      );

  m_treePixel->Branch("time_bin", &m_pixelL1TimeBin);

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetectorPlots::execute() 
{
  static int eventCount(-1); eventCount++;
  // FILE *myFile = fopen("myTRT_DetectorPlots.txt", eventCount?"a":"w");

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::execute() event " << eventCount << endreq;

  if (!eventCount) // compare straw mapping
    {
      m_trtStrawMap->compareWithTRTStrawNeighbour();
      FILE *f = fopen("strawToChipMap.txt", "w");
      for (int i=0; i<m_trtStrawMap->nAllStraws(); i++) 
    {
      fprintf(f, "0 %4d %3d %2d\n", i, m_trtStrawMap->strawToChip(i, 1, 10), m_trtStrawMap->strawToBoard(i));
      if (i>=m_trtStrawMap->nBarrelStraws()) 
        fprintf(f, "1 %4d %3d %2d\n", i, m_trtStrawMap->strawToChip(i, 1, 0), m_trtStrawMap->strawToBoard(i)); // reversed endcap
    }
      fclose(f);
    }

  StatusCode sc = load();

  if (sc.isFailure()) 
    {
      msg(MSG::ERROR) << "InDet::TRT_DetectorPlots::load() failed, exiting" << endreq;
      return sc;
    }   

  if (m_trkCollection->size() > 100) 
    {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "N tracks greater than 100: " << m_trkCollection->size() << " for event " << eventCount << ", exiting" << endreq; 
      return sc;
    }

  const EventInfo *eventInfo = 0;
  sc = evtStore()->retrieve(eventInfo);
  if ( sc.isFailure() )
    {
      msg(MSG::ERROR) << "Unable to retrieve Event Info " << endreq;
      return sc;
    } 

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::execute() retrieved eventInfo " << eventCount << endreq;

  if (m_doRDO) RDOhits();
  if (m_doMC) MC();

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::execute() retrieved eventInfo " << eventCount << endreq;
  
  fillEventAndTrackTrees(eventInfo);


  if (1) { // special print
    //if (false) { // special print

    char fileName[300]; 
    snprintf(fileName, 300, "%s",  m_fileName.c_str());
    FILE *f = fopen(fileName, eventCount?"a":"w");

    fprintf(f, "%d %d %d %d ", m_eventNumberOfTracks, m_eventNumberOfHitsOnTracks[1], m_eventNumberOfHitsOnTracks[3], m_eventNumberOfHitsOnTracks[5]); // N tracks, TRT barrel hits, SCt hits, pixel hits
    fprintf(f, "%d %d ", m_siSP[0], m_siSP[1]); // n_sp_upper, lower
    if (!m_trackTrigAllTeIDSelected) for (int i=0; i<3; i++) m_eventL2Alg[i] *= -1;
    fprintf(f, "%d %d %d %d ", m_eventL1Type, m_eventL2Alg[0], m_eventL2Alg[1], m_eventL2Alg[2]);
    for (int i=0; i<5; i++) fprintf(f, "%f ", m_eventTrackInfo[i] );
    fprintf(f, "\n");

    fclose(f);

  }

  pixelRDOhits(m_runNumber);


  if (m_doSegments) 
    { 
        fillSegmentTree();
    }
  
  if(m_numNoise)
  {
      fillHitsTree();
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetectorPlots::finalize() {

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "InDet::TRT_DetectorPlots::finalize()" << endreq;

  if(m_trackSide)            delete m_trackSide;
  if(m_trackLayerOrWheel)    delete m_trackLayerOrWheel;
  if(m_trackPhi)             delete m_trackPhi;
  if(m_trackStrawLayer)      delete m_trackStrawLayer;
  if(m_trackStraw)           delete m_trackStraw;
  if(m_trackStrawNum)        delete m_trackStrawNum;
  if(m_trackChip)            delete m_trackChip;
  if(m_trackBoard)           delete m_trackBoard;
  if(m_trackDriftTimeBin)    delete m_trackDriftTimeBin;
  if(m_trackTrailingEdge)    delete m_trackTrailingEdge ;
  if(m_trackDriftCircleWord) delete m_trackDriftCircleWord;
  if(m_trackX)               delete m_trackX;
  if(m_trackY)               delete m_trackY;
  if(m_trackZ)               delete m_trackZ;
  if(m_trackT0)              delete m_trackT0;

  if(m_segmentSide)            delete m_segmentSide;
  if(m_segmentPhi)             delete m_segmentPhi;
  if(m_segmentStrawLayer)      delete m_segmentStrawLayer;
  if(m_segmentStraw)           delete m_segmentStraw;
  if(m_segmentStrawNum)        delete m_segmentStrawNum;
  if(m_segmentChip)            delete m_segmentChip;
  if(m_segmentBoard)           delete m_segmentBoard;
  if(m_segmentDriftTimeBin)    delete m_segmentDriftTimeBin;
  if(m_segmentTrailingEdge)    delete m_segmentTrailingEdge;
  if(m_segmentDriftCircleWord) delete m_segmentDriftCircleWord;
  if(m_segmentX)               delete m_segmentX;
  if(m_segmentY)               delete m_segmentY;
  if(m_segmentZ)               delete m_segmentZ;
  if(m_segmentT0)              delete m_segmentT0;


  if(m_onTrackIdentifiers) delete m_onTrackIdentifiers;

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetectorPlots::load() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::load()" << endreq;

  StatusCode sc = detStore()->retrieve(m_trtDetMgr, "TRT");
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve TRT Detector Manager " << endreq;
    return sc;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieved TRT Detector Manager " << endreq;
  }

  // TRT helper
  if (detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_SCTDetMgr, "SCT");
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve SCT Detector Manager " << endreq;
    return sc;
  }

  // SCT
  if (detStore()->retrieve(m_SCTHelper, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_PixelDetMgr, "Pixel");
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve Pixel Detector Manager " << endreq;
    return sc;
  }

  // Pixel
  if (detStore()->retrieve(m_PixelHelper, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_trtCondDbTool.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find TRTCalDbTool " << endreq;
    return sc;
  }

  sc = evtStore()->retrieve(m_rdoContainer, "TRT_RDOs");
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "no TRT_RDO container available " << endreq;
    return sc;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieved TRT RDOs" << endreq;
  }

  sc = evtStore()->retrieve( m_trtDriftCircleContainer, "TRT_DriftCircles" );
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find TRT_DriftCircles collection " << endreq;
    return sc;
  }

  sc = m_trtStrawNeighbourTool.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find TRTStrawNeighbourTool " << endreq;
    return sc;
  }

  if (m_doSegments) {

    sc = evtStore()->retrieve( m_segmentCollectionBarrel, m_barrelSegmentsName);// TRT_SegmentsBC "InDetCosmic_Segments" ); //"TRT_Segments" );
    if ( sc.isFailure() ) {    
      msg(MSG::ERROR) << "Could not find TRT Segments Collection" << endreq;
      return sc;
    } else {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "TRT_Segments collection found, size = " << m_segmentCollectionBarrel->size() << endreq;
    }

    sc = evtStore()->retrieve( m_segmentCollectionEc, m_barrelSegmentsName );// "TRT_Segments_EC" );
    if ( sc.isFailure() ) {    
      msg(MSG::ERROR) << "Could not find TRT EC Segments Collection" << endreq;
      return sc;
    } else {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "TRT_Segments_ECC collection found, size = " << m_segmentCollectionEc->size() << endreq;
    }
  } // end if (m_doSegments)

  sc = evtStore()->retrieve( m_trkCollection, m_tracksName );
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not find Tracks Collectioni " << m_tracksName << endreq;
    return sc;  
  }

  if (m_doMC) {
    sc = evtStore()->retrieve(m_truthTrt,"PRD_MultiTruthTRT");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not retrieve TruthTRT info PRD_MultiTruthTRT" << endreq;
      return sc;
    }   
  } 

  // get the TRT_Phase -- also called the Event Phase

  m_pixelComTime = 999.;
  ComTime* theComTime;
  sc = evtStore()->retrieve(theComTime, "TRT_Phase");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "ComTime object not found with name TRT_Phase !!!" << endreq;
    sc = StatusCode::SUCCESS;
  } else {
    m_pixelComTime = theComTime->getTime();
    if ( fabs(m_pixelComTime) > 100. ) m_pixelComTime = 999.;
  }

  return sc;
}

/////////////////////////////////////////////////////////////////////////////

void InDet::TRT_DetectorPlots::pixelRDOhits(int runNumber) 
{
  if (runNumber != m_runNumber) {  // first call or new run
    ;
  }

  m_pixelL1TimeBin->clear();    
  const DataHandle<PixelRDO_Container> m_rdocontainer; //container for pixel RDO

  StatusCode sc = evtStore()->retrieve(m_rdocontainer, "PixelRDOs");
  if (sc.isFailure() || !m_rdocontainer) {
    msg(MSG::ERROR) << "Could not find the data object PixelRDOs !" << endreq;
    return;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Data object PixelRDOs found" << endreq;
  }

  //  char line[300];
  //  static int event(-1); event++;
  //  int count(0);

  m_pixelNumberOfPixelHits = 0;

  for (PixelRDO_Container::const_iterator it = m_rdocontainer->begin(); it != m_rdocontainer->end(); it++) {

    const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*it);
    if (!PixelCollection) continue;

    for(DataVector<PixelRDORawData>::const_iterator p_rdo = PixelCollection->begin(); p_rdo != PixelCollection->end(); p_rdo++) {

      //      sprintf(line, "pixelRDOhits: event %3d, hit %2d, getLVL1ID: %2d TOT: %3d, BCID: %3d", event, count++, (*p_rdo)->getLVL1A(),
      //                    (*p_rdo)->getToT(), (*p_rdo)->getBCID());
      //      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << line << endreq;
      m_pixelL1TimeBin->push_back( (*p_rdo)->getLVL1A() );
      m_pixelNumberOfPixelHits++;
    }
  }

  m_treePixel->Fill();
  return;
}

/////////////////////////////////////////////////////////////////////////////

void InDet::TRT_DetectorPlots::RDOhits() 
{
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "InDet::TRT_DetectorPlots::RDOhits()" << endreq;

  static int eventCount(-1); eventCount++;

  if (!eventCount) {


    FILE *f = fopen("TRT_DetectorPlots.TRT_StrawMap.txt", "w");

    for (std::vector<Identifier>::const_iterator it = m_trtHelper->straw_layer_begin(); it != m_trtHelper->straw_layer_end(); it++  ) {

      int nStrawsInLayer = m_trtHelper->straw_max(*it);

      for (int i=0; i<=nStrawsInLayer; i++) {

        Identifier id = m_trtHelper->straw_id(*it, i);
        int index[5];
        m_trtStrawMap->setNewIdentifier(id, index);

        //const HepGeom::Point3D<double> &centerRef = m_trtDetMgr->getElement( id )->center( id );
	Amg::Vector3D centerRef = m_trtDetMgr->getElement( id )->center( id );

        for (int i=0; i<5; i++) fprintf(f, "%2d ", index[i]);
        fprintf(f, "%4d %3d %2d ", m_trtStrawMap->straw(), m_trtStrawMap->chip(), m_trtStrawMap->board() );
        fprintf(f, "%f %f %f\n", centerRef.x(), centerRef.y(), centerRef.z());
      }
    }

    fclose(f);



  }



  char fileName[300]; snprintf(fileName, 300, "/tmp/sfratina/TRT_DetectorPlots.RDOhits.%07d.txt", m_trackRunNumber);
  FILE *f = fopen(fileName, eventCount?"a":"w");

  for (TRT_RDO_Container::const_iterator rdoIt = m_rdoContainer->begin(); rdoIt != m_rdoContainer->end(); rdoIt++) {

    const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
    if (TRTCollection==0) continue;

    for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {

      // possible functions to call: 
      // unsigned int getWord(), int driftTimeBin() [bins], double timeOverThreshold() [ns], bool highLevel(), Identifier identify().

      //if ((*trtIt)->timeOverThreshold() < 10.) continue;

      Identifier id = (*trtIt)->identify();
      int index[6];
      m_trtStrawMap->setNewIdentifier(id, index);
      index[5] = m_trtStrawMap->straw();

      //const HepGeom::Point3D<double> &centerRef = mTRTDetMgr->getElement( id )->center( id );

      fprintf(f, "%3d %2d %2d %4d %2d %d %2d %2d %9d\n", eventCount, index[0], index[1], index[5], m_trtStrawMap->board(), (*trtIt)->highLevel(), (*trtIt)->driftTimeBin(), (int) ( (*trtIt)->driftTimeBin() + (*trtIt)->timeOverThreshold()/3.125 ), (*trtIt)->getWord() );      
    }
  } 


  fclose(f);
  return;
}

/////////////////////////////////////////////////////////////////////////////

void InDet::TRT_DetectorPlots::MC() 
{
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "InDet::TRT_DetectorPlots::MC()" << endreq;
  
  std::vector<const HepMC::GenParticle*> genParticleList;
  
  for (PRD_MultiTruthCollection::const_iterator mc=m_truthTrt->begin(); mc != m_truthTrt->end(); mc++){

    Identifier id = (*mc).first;
    if ( abs(m_trtHelper->barrel_ec(id)) != 1) continue; // barrel hits only

    const HepMC::GenParticle* pa = (*mc).second.cptr();
    if (!pa) { if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "PRD_MultiTruthCollection loop, particle link missing" << endreq; continue; }

    int isListed(-1);
    for (unsigned int i=0; i<genParticleList.size(); i++) if (pa==genParticleList[i]) {isListed = i; break;}
    if (isListed==-1)
      { 
    isListed = (int) genParticleList.size(); 
    genParticleList.push_back(pa); 
      }
  }
  return; 
}


void InDet::TRT_DetectorPlots::fillEventAndTrackTrees(const EventInfo *eventInfo)
{
    m_trackTrigL1Type          = 0;  
    m_trackTrig1MU1            = 0;    
    m_trackTrig1MU2            = 0;
    m_trackTrig1MU3            = 0;
    m_trackTrigL1Calo          = 0;
    m_trackTrigNIM0            = 0;
    m_trackTrigNIM4            = 0;  
    m_trackTrigMBTS            = 0;  
    m_trackTrigEML1Calo        = 0;
    m_trackTrigHLTerror        = 0;
    m_trackTrigAllTeIDSelected = 0;
    m_trackTrigAllTePixel      = 0;  


    TriggerInfo *triggerInfo = eventInfo->trigger_info();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieved TriggerInfo " << endreq;

    if (triggerInfo==0) 
      { 
        msg(MSG::ERROR) << "no Trigger Info available" << endreq;
      } 
    else 
      {
        m_trackTrigL1Type = triggerInfo->level1TriggerType();  

        const std::vector<TriggerInfo::StreamTag> &getStreamTag = triggerInfo->streamTags();
        m_trackTrigStreamSize = getStreamTag.size();
        for (int i=0; i < m_trackTrigStreamSize; ++i) 
      {
        if( getStreamTag[i].name() == "IDCosmic" ) // L2 - can be in combination with any of the others
          {
            m_trackTrigAllTeIDSelected  = 1; 
            continue; 
          }
        if( getStreamTag[i].name() == "HLT_Cosmic_AllTePixel" ) // L2 - can be in combination with any of the others
          {
            m_trackTrigAllTePixel = 1; 
            continue;
          }
        if( getStreamTag[i].name() == "TGCwBeam" )
          {
            m_trackTrig1MU1 = 1;
            continue; 
          }
        if( getStreamTag[i].name() == "RPCwBeam" )
          {
            m_trackTrig1MU2 = 1;
            continue;
          }
        if( getStreamTag[i].name() == "CosmicDownwardMuons" )
          {
            m_trackTrig1MU2 = 2;
            continue;
          }
        if( getStreamTag[i].name() == "RNDM" )
          {
            m_trackTrig1MU3 = 1;
            continue;
          }
        if( getStreamTag[i].name() == "L1Calo" )
          {
            m_trackTrigL1Calo = 1;
            continue;
          }
        if( getStreamTag[i].name() == "IDTracks" )
          {
            m_trackTrigNIM0 = 1;
            continue;
          }
        if( getStreamTag[i].name() == "NIM4" )
          {
            m_trackTrigNIM4 = 1;
            continue;
          }
        if( getStreamTag[i].name() == "MBTS_BCM_LUCID" )
          {
            m_trackTrigMBTS = 1;
            continue;
          }
        if( getStreamTag[i].name() == "CosmicMuons" )
          {
            m_trackTrigEML1Calo = 1;
            continue;
          }
        if( getStreamTag[i].name() == "hlterror" )
          {
            m_trackTrigHLTerror = 1;
            continue;
          }
        else
          {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "new unknown trigger name: " << getStreamTag[i].name() << endreq;
          }
      }

        if( !(  m_trackTrig1MU1 
            || m_trackTrig1MU2 
            || m_trackTrig1MU3 
            || m_trackTrigL1Calo 
            || m_trackTrigNIM0 
            || m_trackTrigNIM4 
            || m_trackTrigMBTS 
            || m_trackTrigEML1Calo 
            || m_trackTrigHLTerror 
            )
        )
      {
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "no L1 trigger information available, tag names below, level1TriggerType: " << m_trackTrigL1Type << endreq;

        for (int i=0; i<m_trackTrigStreamSize; i++) 
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "L1 type, stream: " << getStreamTag[i].type() << " " << getStreamTag[i].name() << endreq;

        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "L1 error, run and event number: " << m_trackRunNumber << " " << eventInfo->event_ID()->event_number() << " counting only input events, event N: " << m_trackEventNumber << endreq;
      } // end debug missing L1 trigger

      } // end if (triggerInfo==0) {  } else {  }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "end TriggerInfo " << endreq;

    for (int i=0; i<3; i++) 
      m_eventL2Alg[i] = 0;

    for (int i=0; i<2; i++) 
      m_siSP[i] = 0;

    // L2 trigger info

    const DataHandle<TrigInDetTrackCollection> trackCollection;
    const DataHandle<TrigInDetTrackCollection> lastTrackCollection;
    StatusCode sc = evtStore()->retrieve(trackCollection, lastTrackCollection);

    if (sc.isFailure()) 
      {
        if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << " didn't find any TrigInDetTrackCollection objects" << endreq;
        //   return sc;
        sc = StatusCode::SUCCESS;
      } 
    else 
      {
        for (int iTrackColl=0; trackCollection != lastTrackCollection; ++trackCollection, ++iTrackColl)
          {
     //       int iTrack = 0;
            for (TrigInDetTrackCollection::const_iterator track = trackCollection->begin(); track != trackCollection->end(); track++) 
              {
                int algID = static_cast<int> ((*track)->algorithmId());

                if (algID < 1 || algID > 3) 
                  { 
                    msg(MSG::ERROR) << "TrigInDetTrack algID error: " << algID << endreq;
                    continue; 
                  }
                ++m_eventL2Alg[algID-1];

/*
                continue;

                int nSiSpacePoints(0);
                if ((*track)->siSpacePoints()) nSiSpacePoints = (int) (*track)->siSpacePoints()->size();
                const TrigInDetTrackFitPar *fitPar = (*track)->param();
                int surfaceType = (int) fitPar->surfaceType();//  dynamic_cast<int>(fitPar->surfaceType());

                std::vector <const TrigSiSpacePoint*>* siHits = (*track)->siSpacePoints();

                if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "track "            << iTrack 
                                                      << " ID "              << algID 
                                                      << " par a0 "          << fitPar->a0() 
                                                      << " z0 "              << fitPar->z0() 
                                                      << " phi "             << fitPar->phi0() 
                                                      << " eta "             << fitPar->eta() 
                                                      << " pT "              << fitPar->pT() 
                                                      << " N TRT hits "      << (*track)->NStrawHits()
                                                      << " N passed straws " << (*track)->NStraw() 
                                                      << " N straw time "    << (*track)->NStrawTime()
                                                      << " nSiSpacePoints "  << nSiSpacePoints 
                                                      <<  " si hits "        << ( (siHits) ? siHits->size() : 9999 ) 
                                                      << " chi2 "            << (*track)->chi2() 
                                                      << " surface "         << surfaceType 
                                                      << " coordinate "      << fitPar->surfaceCoordinate() 
                                                      <<  endreq;

*/
              }
          }
      } // if failed TrigInDetTrackCollection else


    /////////////////////////////////
    /// Loop over track collection 
    /////////////////////////////////
    m_eventNumberOfTracks = 0;
    int m_eventNumberOfTRT_HitsOnTracks = 0;
    int m_eventNumberOfSCT_HitsOnTracks = 0;
    m_eventL1Type = m_trackTrigL1Type;
    m_eventL1TagName = 0;
    m_eventL1TagName += 2 * m_trackTrig1MU1;
    m_eventL1TagName += 4 * m_trackTrig1MU2;
    m_eventL1TagName += 8 * m_trackTrig1MU3;
    m_eventL1TagName += 16 * m_trackTrigL1Calo;
    m_eventL1TagName += 32 * m_trackTrigEML1Calo;
    m_eventL1TagName += 64 * m_trackTrigNIM0;
    m_eventL1TagName += 128 * m_trackTrigNIM4;
    m_eventL1TagName += 256 * m_trackTrigMBTS;

    m_eventPixelX->clear();
    m_eventPixelY->clear();
    m_eventPixelZ->clear();
    m_eventPixelHitTimeBin->clear();
    m_eventSCTHitTimeBin->clear();
    m_eventSCTX->clear();
    m_eventSCTY->clear();
    m_eventSCTZ->clear();
    m_eventTRTX->clear();
    m_eventTRTY->clear();
    m_eventTRTZ->clear();

    for (int i=0; i<7; i++) 
      m_eventNumberOfHitsOnTracks[i] = 0; 
    for (int i=0; i<5; i++) 
      m_eventTrackInfo[i] = 0.;

    m_eventPixelTimeBin = 0.;

    DataVector<Trk::Track>::const_iterator bestTrackIt = m_trkCollection->end();
    for ( DataVector<Trk::Track>::const_iterator trackIt = m_trkCollection->begin(); trackIt != m_trkCollection->end(); trackIt++ )
      {
        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();
        if ( trackStates == 0 ) 
      continue; 

        int count_eventNumberOfHitsOnTracks[7];
        for (int i=0; i<7; i++) 
      count_eventNumberOfHitsOnTracks[i] = 0; 
        int eventPixelTimeBin = 0;

        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) 
      {
        if ( *trackStatesIt == 0 ) 
          { 
            msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; 
            continue; 
          }
        if ( !( (*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement) ) ) 
          continue;

        if ( dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() ) ) // TRT hit
          {
            ++count_eventNumberOfHitsOnTracks[0];
            const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );
            Identifier id = (driftCircleOnTrack->prepRawData())->identify(); 
            if ( abs(m_trtHelper->barrel_ec( id )) == 1 ) // barrel TRT hit
          {
            count_eventNumberOfHitsOnTracks[1]++; 
            count_eventNumberOfHitsOnTracks[2]++; 
          }

            //const CLHEP::Hep3Vector &p3d = (driftCircleOnTrack->prepRawData())->detectorElement()->surface( id ).center();
            Amg::Vector3D p3d = (driftCircleOnTrack->prepRawData())->detectorElement()->surface( id ).center();
            m_eventTRTX->push_back(p3d.x());
            m_eventTRTY->push_back(p3d.y());
            m_eventTRTZ->push_back(p3d.z());
          }
        else if ( dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) ) // SCT hit 
          { 
            count_eventNumberOfHitsOnTracks[3]++;
            Identifier id = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->prepRawData()->identify();
            if ( m_SCTHelper->barrel_ec( id )==0 ) // barrel hits
          count_eventNumberOfHitsOnTracks[4]++;

            //Trk::GlobalPosition p3d = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->globalPosition();
             Amg::Vector3D p3d = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->globalPosition();
            m_eventSCTX->push_back(p3d.x());
            m_eventSCTY->push_back(p3d.y());
            m_eventSCTZ->push_back(p3d.z()); 

            const InDet::SCT_Cluster *p_rdo = dynamic_cast<const InDet::SCT_Cluster *> ( (dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()))->prepRawData() );
            const SCT3_RawData* rdo3 = dynamic_cast<const SCT3_RawData*>(p_rdo);
            m_eventSCTHitTimeBin->push_back( rdo3 ? (rdo3)->getTimeBin() : 999 );

          } 
        else if ( dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) ) // pixel hit 
          {
            const InDet::PixelClusterOnTrack *pixelCluster = dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack());

            count_eventNumberOfHitsOnTracks[5]++;
            Identifier id = pixelCluster->prepRawData()->identify();
            if ( m_PixelHelper->barrel_ec( id ) == 0 ) // barrel hits
          {
            count_eventNumberOfHitsOnTracks[6]++;
            eventPixelTimeBin += pixelCluster->prepRawData()->LVL1A();
          } 

            m_eventPixelHitTimeBin->push_back( pixelCluster->prepRawData()->LVL1A() );

            //sprintf(line, "pixelRDOhits: event %3d, hit %2d, getLVL1ID: %2d TOT: %3d, BCID: %3d", event, count++, (*p_rdo)->getLVL1A(),
            //              (*p_rdo)->getToT(), (*p_rdo)->getBCID());
            //if (msgLvl(MSG::INFO)) msg(MSG::INFO) << line << endreq;
            //  m_pixelL1TimeBin->push_back( (*p_rdo)->getLVL1A() );
            //  InDetDD::SiDetectorElement* si_hit = m_PixelDetMgr->getDetectorElement( id )->surface( id );
            //  HepGeom::Point3D<double> p3d = m_PixelDetMgr->getDetectorElement( id )->surface( id ).center();
            //Trk::GlobalPosition p3d = pixelCluster->globalPosition();
            Amg::Vector3D p3d = pixelCluster->globalPosition();
            m_eventPixelX->push_back(p3d.x());
            m_eventPixelY->push_back(p3d.y());
            m_eventPixelZ->push_back(p3d.z()); 

          } else 
            {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "which measurement error?" << endreq;
            }
      } // end measurements loop

        for (int i=0; i<7; i+=2) 
      {
        if (count_eventNumberOfHitsOnTracks[i]>m_eventNumberOfHitsOnTracks[i]) 
          {
            for (int j=0; j<2; j++) 
          m_eventNumberOfHitsOnTracks[i+j] = count_eventNumberOfHitsOnTracks[i+j];
            if ( i==5 && m_eventNumberOfHitsOnTracks[6] ) // pixel hits
          m_eventPixelTimeBin = (1.*eventPixelTimeBin) / (1.*m_eventNumberOfHitsOnTracks[6]); 
            if (i==0 && !m_eventNumberOfHitsOnTracks[5] && !m_eventNumberOfHitsOnTracks[3]) // based on TRT if no pixel and SCT hits
          bestTrackIt = trackIt; 
            if (i==3 && !m_eventNumberOfHitsOnTracks[5]) // based on SCT if no pixel hits
          bestTrackIt = trackIt;  
            if (i==5) 
          bestTrackIt = trackIt;
          }   
        if (!i) // special for TRT - three values 
          ++i;
      }
        if (count_eventNumberOfHitsOnTracks[2]>m_eventNumberOfHitsOnTracks[2]) 
      m_eventNumberOfHitsOnTracks[2] = count_eventNumberOfHitsOnTracks[2];

      } // end TrkCollection loop

    if ( bestTrackIt != m_trkCollection->end() ) // set track parameters
      {
        //CLHEP::HepVector perigeeParameters = (**bestTrackIt).perigeeParameters()->parameters();
	const Trk::Perigee* per = (**bestTrackIt).perigeeParameters();
	AmgVector(5)     perigeeParameters = per->parameters();
        //m_eventTrackInfo[0] = perigeeParameters[Trk::phi0];  if (m_eventTrackInfo[0]>0.) m_eventTrackInfo[0] -= M_PI;
        m_eventTrackInfo[0] = (perigeeParameters[Trk::phi0] <= 0) ? perigeeParameters[Trk::phi0]       : perigeeParameters[Trk::phi0] - M_PI;
        m_eventTrackInfo[1] = perigeeParameters[Trk::d0];
        m_eventTrackInfo[2] = perigeeParameters[Trk::z0];
        m_eventTrackInfo[3] = perigeeParameters[Trk::theta];
        m_eventTrackInfo[4] = perigeeParameters[Trk::qOverP];
        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**bestTrackIt).trackStateOnSurfaces();
        if ( trackStates == 0 ) 
      {
        msg(MSG::ERROR) << "bestTrackIt states 0 ptr" << endreq;
      } 
        else 
      {
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) 
          {
            if ( *trackStatesIt == 0 ) 
          { 
            msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; 
            continue; 
          }
            if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) 
          continue;

            if ( dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) ) // SCT hit 
          {
            Identifier id = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->prepRawData()->identify();
            if ( m_SCTHelper->barrel_ec( id )==0 ) 
              {
                //Trk::GlobalPosition p3d = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->globalPosition();
                 Amg::Vector3D p3d = dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())->globalPosition();
                ++m_siSP[(p3d.y()>0.)?0:1];
              }

          } 
            else if ( dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) ) // pixel hit
          { 
            const InDet::PixelClusterOnTrack *pixelCluster = dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack());
            Identifier id = pixelCluster->prepRawData()->identify();
            if ( m_PixelHelper->barrel_ec( id ) == 0 ) // barrel hits       
              { 
                //Trk::GlobalPosition p3d = pixelCluster->globalPosition();
                 Amg::Vector3D p3d = pixelCluster->globalPosition();
                m_siSP[(p3d.y()>0.)?0:1]++;
              } 
          } // end pixel hit  
          } 
      }   
      }

    if ( !m_eventL2Alg[2] && bestTrackIt != m_trkCollection->end() ) // !m_trackTrigAllTeIDSelected 
      {
        for (TRT_RDO_Container::const_iterator rdoIt = m_rdoContainer->begin(); rdoIt != m_rdoContainer->end(); rdoIt++) 
      {
        const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
        if (TRTCollection==0)
          continue;

        for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) 
          {
            if ((*trtIt)->timeOverThreshold()<10.) 
          continue; 

            Identifier id = (*trtIt)->identify();
            //const HepGeom::Point3D<double> &p3d = m_trtDetMgr->getElement( id )->center( id );
	    Amg::Vector3D p3d	= m_trtDetMgr->getElement( id )->center( id );

            m_eventTRTX->push_back(p3d.x());
            m_eventTRTY->push_back(p3d.y());
            m_eventTRTZ->push_back(p3d.z()+10000.);
          }
      }
      } // end if () do RDO   




    m_pixelMaxNumberOfTRTBarrelHitsOnTrack = 0;
    m_pixelTRTtrackD0 = 0.; 
    m_pixelTRTtrackPhi0 = 0.;
    m_pixelTRTtrackEP  = 0.;


    int maxTRThits(0), countTRThits(0), countTRTbarrelHits(0), maxSCThits(0), countSCThits(0);
//    double SCTphi(0.), SCTd0(0.), SCTtheta(0.), TRTphi(0.), TRTd0(0.);


    m_trackNumTrtTracks = 0; 
    m_trackNumTracks = 0;

    for ( DataVector<Trk::Track>::const_iterator trackIt = m_trkCollection->begin(); trackIt != m_trkCollection->end(); trackIt++ ) 
      {
        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();
        if ( trackStates == 0 ) 
      { 
        if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Trk::TrackStateOnSurface empty" << endreq; 
        continue; 
      }

        m_trackNumTracks++;
        int countNumTrtHitsOnTrack(0);

        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ )
      {
        if ( *trackStatesIt == 0 ) 
          { 
            if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; 
            continue; 
          }
        if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) 
          continue;
        if ( dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() ) ) 
          ++countNumTrtHitsOnTrack; 
      } 
        if (countNumTrtHitsOnTrack>=10) 
      ++m_trackNumTrtTracks;
      }

    for ( DataVector<Trk::Track>::const_iterator trackIt = m_trkCollection->begin()
        ; trackIt != m_trkCollection->end()
        ; trackIt++ 
      ) 
      {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " track loop start" << endreq;
        ++m_eventNumberOfTracks;

        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();
        if ( trackStates == 0 ) 
      { 
        if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Trk::TrackStateOnSurface empty" << endreq; 
        continue; 
      }

        double theta = (**trackIt).perigeeParameters()->parameters()[Trk::theta];
        double d0    = (**trackIt).perigeeParameters()->parameters()[Trk::d0];
        double z0    = (**trackIt).perigeeParameters()->parameters()[Trk::z0];
        double phi0  = (**trackIt).perigeeParameters()->parameters()[Trk::phi0];

        int nSctHits        = 0;
        int nTrtHits        = 0;
        int nTrtHitsBarrelA = 0;
        int nTrtHitsBarrelC = 0;
        int nTrtHitsEcA     = 0;
        int nTrtHitsEcC     = 0;

        int ndf     = 0; 
        double chi2 = 0;
        const Trk::FitQuality *fitQuality = (**trackIt).fitQuality();
        if (fitQuality) 
      { 
        ndf = fitQuality->numberDoF(); 
        chi2 = fitQuality->chiSquared(); 
      }

        ///////////////////////////////////////////////
        /// Loop over track states within this track 
        /// essentially hits on track
        ///////////////////////////////////////////////
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin()
            ; trackStatesIt != trackStates->end()
            ; trackStatesIt++ 
          ) 
      {
        if ( *trackStatesIt == 0 ) 
          { 
            if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; 
            continue; 
          }

        if ( !( (*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement) ) ) 
          continue;

        if (dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) != 0) 
          { 
            ++nSctHits; 
            ++m_eventNumberOfSCT_HitsOnTracks; 
            ++countSCThits; 
            continue; 
          }

        const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );

        if(  ((*trackStatesIt)->type(Trk::TrackStateOnSurface::Outlier)) 
             && msgLvl(MSG::INFO)
             )
          msg(MSG::INFO) << "Outlier on track " << endreq;

        if( driftCircleOnTrack == 0 ) 
          { 
            if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "driftCircleOnTrack == 0 " << ( ((*trackStatesIt)->type(Trk::TrackStateOnSurface::Outlier)) ? "Outlier" : "." )  <<  endreq;
            continue; 
          }

        double locR(999.);
        //const Trk::MeasuredAtaStraightLine *trackMeasuredAtLine = dynamic_cast<const Trk::MeasuredAtaStraightLine*>( (**trackIt).perigeeParameters() );
//        if ( trackMeasuredAtLine ) // point of track's closest approach, I believe
	   locR 	= ((**trackIt).perigeeParameters())->parameters()[Trk::locR]; 
          //locR = trackMeasuredAtLine->parameters()[Trk::locR];





        const Trk::MeasurementBase* mesb = (*trackStatesIt)->measurementOnTrack();
        if(mesb == 0) 
          {
            msg(MSG::ERROR) << "measurementBase == 0, and we should have had it!!!" << endreq;
            continue;
          }
        float locRPat = mesb->localParameters()[Trk::locR];
        //float errLocR = mesb->localErrorMatrix().covValue(Trk::locR);
        float errLocR = mesb->localCovariance()(Trk::locX,Trk::locX) ;



        m_eventNumberOfTRT_HitsOnTracks++;
        countTRThits++;

        if(driftCircleOnTrack->prepRawData()==0)  {
          //const Trk::GlobalPosition& gloPos = driftCircleOnTrack->globalPosition();
           Amg::Vector3D gloPos = driftCircleOnTrack->globalPosition();
          msg(MSG::ERROR) << "driftCircleOnTrack->prepRawData()==0, global pos: " << gloPos.x() << " " << gloPos.y() << " " << gloPos.z() << endreq;
          continue;
        }


        if( abs(m_trtHelper->barrel_ec((driftCircleOnTrack->prepRawData())->identify())) == 1 ) 
          countTRTbarrelHits++;

        nTrtHits++;
        switch( m_trtHelper->barrel_ec((driftCircleOnTrack->prepRawData())->identify()) )
          {
            // if barrel hit, incriment barrel hit count
          case 1:
            nTrtHitsBarrelA++;
            break;
          case 2:
            nTrtHitsEcA++;
            break;
          case -1:
            nTrtHitsBarrelC++;
            break;
          case -2:
            nTrtHitsEcC++;
            break;
          default:
            msg(MSG::ERROR) << "invalid value for side: " 
                    << m_trtHelper->barrel_ec((driftCircleOnTrack->prepRawData())->identify()) 
                    << endreq;
            break;
          }


        const InDet::TRT_DriftCircle *driftCircle = driftCircleOnTrack->prepRawData();
        if ( driftCircle == 0 ) 
          { 
            msg(MSG::ERROR) << "driftCircle == 0" << endreq; 
            continue; 
          }

        /////////////////////////////////////////////////////////////////////////
        /// Find drift circle parameters to be filled into the "on track" tree
        /////////////////////////////////////////////////////////////////////////
        Identifier id = driftCircle->identify();
        m_trtStrawMap->setNewIdentifier(id);

        int side         = m_trtHelper->barrel_ec(id);
        int layerOrWheel = m_trtHelper->layer_or_wheel(id);
        int phi          = m_trtHelper->phi_module(id);
        int strawLayer   = m_trtHelper->straw_layer(id);
        int straw        = m_trtHelper->straw(id);
        int strawNumber  = m_trtStrawMap->straw();
        int board        = m_trtStrawMap->board();
        int chip; 
        m_trtStrawNeighbourTool->getChip(id, chip);

        if(abs(side) == 1) // barrel
          {
            // set chip number to correct scale 
            chip-=1; 
            if(layerOrWheel == 1)      
          chip+=21;
            else if(layerOrWheel == 2) 
          chip+=54;
          } 
        else // endcap
          {
            if(chip > 11) 
          chip-=20;
            chip+=(104+12*board);
          }


        //const CLHEP::Hep3Vector &center = driftCircle->detectorElement()->surface( id ).center();
        Amg::Vector3D  center = driftCircle->detectorElement()->surface( id ).center();
        double dbT0 = m_trtCondDbTool->getT0(id, TRTCond::ExpandedIdentifier::STRAW);

        m_trackSide->push_back(side);
        m_trackLayerOrWheel->push_back(layerOrWheel);
        m_trackPhi->push_back(phi);
        m_trackStrawLayer->push_back(strawLayer);
        m_trackStraw->push_back(straw);
        m_trackStrawNum->push_back(strawNumber);
        m_trackChip->push_back(chip);
        m_trackBoard->push_back(board);
        m_trackDriftTimeBin->push_back(driftCircle->driftTimeBin());
        m_trackTrailingEdge->push_back(driftCircle->trailingEdge()); 
        m_trackHighThreshold->push_back(driftCircle->highLevel());
        m_trackDriftCircleWord->push_back(driftCircle->getWord());
        m_trackX->push_back(center.x());
        m_trackY->push_back(center.y());
        m_trackZ->push_back(center.z());
        m_trackT0->push_back(dbT0);

        m_trackTrackR->push_back(locR);
        m_trackLocR->push_back(locRPat);
        m_trackErrR->push_back(errLocR);

        m_onTrackIdentifiers->push_back(driftCircle->identify());
      }

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "track loop start filling tree" << endreq;

        if (nSctHits>maxSCThits) 
      { 
        //SCTphi = phi0; 
        //SCTd0 = d0; 
        //SCTtheta = theta; 
        maxSCThits = nSctHits; 
      }
        if (nTrtHits>maxTRThits) 
      { 
        //TRTphi = phi0; 
        //TRTd0 = d0;
        maxTRThits = nTrtHits; 
      }

        m_trackNumSctHits        = nSctHits;
        m_trackNumTrtHits        = nTrtHits;
        m_trackNumTrtHitsBarrelA = nTrtHitsBarrelA;
        m_trackNumTrtHitsBarrelC = nTrtHitsBarrelC;
        m_trackNumTrtHitsEcA     = nTrtHitsEcA;
        m_trackNumTrtHitsEcC     = nTrtHitsEcC;
        m_trackNdf                 = ndf;
        m_trackChi2                = chi2;
        m_trackTheta               = theta;
        m_trackD0                  = d0;
        m_trackZ0                  = z0;
        m_trackPhi0              = phi0;

        ////////////////////////////////////////////////////////
        /// Find event phase for this track and use it to set 
        /// event phase for all hits in this track
        ////////////////////////////////////////////////////////
        m_trackEventPhase = 0.;
        for(unsigned int hitItr = 0; hitItr < m_trackTrailingEdge->size(); hitItr++) 
      {
        m_trackEventPhase += (double) m_trackTrailingEdge->at(hitItr);
      }
        m_trackEventPhase /= (double)  m_trackTrailingEdge->size();

        if (countTRTbarrelHits>m_pixelMaxNumberOfTRTBarrelHitsOnTrack) 
      { 
        m_pixelTRTtrackD0 = d0; 
        m_pixelTRTtrackPhi0 = phi0; 
        m_pixelMaxNumberOfTRTBarrelHitsOnTrack = countTRTbarrelHits;
        m_pixelTRTtrackEP = m_trackEventPhase;
      }

        /////////////////////////////////////////////////////////////////////
        /// Fill "on track" tree and clear all vectors linked to this tree
        /////////////////////////////////////////////////////////////////////
        m_treeTrack->Fill();

        m_trackSide->clear();
        m_trackLayerOrWheel->clear();
        m_trackPhi->clear();
        m_trackStrawLayer->clear();
        m_trackStraw->clear();
        m_trackStrawNum->clear();
        m_trackChip->clear();
        m_trackBoard->clear();
        m_trackDriftTimeBin->clear();
        m_trackTrailingEdge->clear();
        m_trackHighThreshold->clear();
        m_trackDriftCircleWord->clear();
        m_trackX->clear();
        m_trackY->clear();
        m_trackZ->clear();
        m_trackT0->clear();

        m_trackTrackR->clear();
        m_trackLocR->clear(); 
        m_trackErrR->clear();

      }




    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "end track info ttree" << endreq;

    m_eventNumberOfTRT_HitsOnTracks = maxTRThits;
    m_eventNumberOfSCT_HitsOnTracks = maxSCThits;

    m_treeEvent->Fill();

}


void InDet::TRT_DetectorPlots::fillSegmentTree()
{
  ///////////////////////////////////////////////
  /// Loop over barrel track segments in event
  ///////////////////////////////////////////////
    for ( DataVector<Trk::Segment>::const_iterator segIt = m_segmentCollectionBarrel->begin(); segIt != m_segmentCollectionBarrel->end(); segIt++ ) 
    {
        const Trk::TrackSegment *segment = dynamic_cast<const Trk::TrackSegment *>(*segIt);
        if (segment==0) 
        { 
            if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "no segment from segIt???" << endreq; 
            continue; 
        }

        unsigned int numberOfHits = segment->numberOfMeasurementBases(); // number of TRT hits in this segment

        for (unsigned int i=0; i<numberOfHits; i++) // access the hits
        { 
            const InDet::TRT_DriftCircleOnTrack *circle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>(segment->measurement(i));
            if (circle==0) 
                continue;
            const InDet::TRT_DriftCircle *driftCircle = circle->prepRawData();
            if ( driftCircle == 0 ) 
            { 
                if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "driftCircleOnTrack == 0" << endreq; 
                continue; 
            }

          /////////////////////////////////////////////////////////////////////////
          /// Find drift circle parameters to be filled into the "segment" tree
          /////////////////////////////////////////////////////////////////////////
            Identifier id = driftCircle->identify();
            m_trtStrawMap->setNewIdentifier(id);

            int side         = m_trtHelper->barrel_ec(id);
            int layerOrWheel = m_trtHelper->layer_or_wheel(id);
            int phi          = m_trtHelper->phi_module(id);
            int strawLayer   = m_trtHelper->straw_layer(id);
            int straw        = m_trtHelper->straw(id);
            int strawNumber  = m_trtStrawMap->straw();
            int board        = m_trtStrawMap->board();
            int chip; 
            m_trtStrawNeighbourTool->getChip(id, chip);

            if(abs(side) == 1)    // barrel
            {
          // set chip number to correct scale 
                chip-=1; 
                if(layerOrWheel == 1)      chip+=21;
                else if(layerOrWheel == 2) chip+=54;
            } 
            else // endcap
            {
                if(chip > 11) chip-=20;
                chip+=(104+12*board);
            }

            //const CLHEP::Hep3Vector &center = driftCircle->detectorElement()->surface( id ).center();
            Amg::Vector3D  center = driftCircle->detectorElement()->surface( id ).center();
            unsigned int word = driftCircle->getWord();
            double dbT0 = m_trtCondDbTool->getT0(id, TRTCond::ExpandedIdentifier::STRAW);

            m_segmentSide->push_back(side);
            m_segmentLayerOrWheel->push_back(layerOrWheel);
            m_segmentPhi->push_back(phi);
            m_segmentStrawLayer->push_back(strawLayer);
            m_segmentStraw->push_back(straw);
            m_segmentStrawNum->push_back(strawNumber);
            m_segmentChip->push_back(chip);
            m_segmentBoard->push_back(board);
            m_segmentDriftTimeBin->push_back(driftCircle->driftTimeBin());
            m_segmentTrailingEdge->push_back(driftCircle->trailingEdge()); 
            m_segmentDriftCircleWord->push_back(word);
          //m_segmentHitOnTrack->push_back(_onTrack);
            m_segmentX->push_back(center.x());
            m_segmentY->push_back(center.y());
            m_segmentZ->push_back(center.z());
            m_segmentT0->push_back(dbT0);
        } // end hits loop
    } 


      ///////////////////////////////////////////////
      /// Loop over endcap track segments in event
      ///////////////////////////////////////////////
    for ( DataVector<Trk::Segment>::const_iterator segIt = m_segmentCollectionEc->begin(); segIt != m_segmentCollectionEc->end(); segIt++ ) {

        const Trk::TrackSegment *segment = dynamic_cast<const Trk::TrackSegment *>(*segIt);
        if (segment==0) { msg(MSG::ERROR) << "no segment from segIt???" << endreq; continue; }

        unsigned int numberOfHits = segment->numberOfMeasurementBases(); // number of TRT hits in this segment

        for (unsigned int i=0; i<numberOfHits; i++) { // access the hits

            const InDet::TRT_DriftCircleOnTrack *circle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>(segment->measurement(i));
            if (circle==0) continue;
            const InDet::TRT_DriftCircle *driftCircle = circle->prepRawData();
            if ( driftCircle == 0 ) { if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "driftCircleOnTrack == 0" << endreq; continue; }

      /////////////////////////////////////////////////////////////////////////
      /// Find drift circle parameters to be filled into the "segment" tree
      /////////////////////////////////////////////////////////////////////////
            Identifier id = driftCircle->identify();
            m_trtStrawMap->setNewIdentifier(id);

            int side         = m_trtHelper->barrel_ec(id);
            int layerOrWheel = m_trtHelper->layer_or_wheel(id);
            int phi          = m_trtHelper->phi_module(id);
            int strawLayer   = m_trtHelper->straw_layer(id);
            int straw        = m_trtHelper->straw(id);
            int strawNumber  = m_trtStrawMap->straw();
            int board        = m_trtStrawMap->board();
            int chip; 
            m_trtStrawNeighbourTool->getChip(id, chip);

            if(abs(side) == 1)    // barrel
            {
          // set chip number to correct scale 
                chip-=1; 
                if(layerOrWheel == 1)      chip+=21;
                else if(layerOrWheel == 2) chip+=54;
            } 
            else // endcap
            {
          // set all chips on a scale from 0-11
                if(chip > 11) chip-=20;
                chip+=(104+12*board);
            }


            //const CLHEP::Hep3Vector &center = driftCircle->detectorElement()->surface( id ).center();
            Amg::Vector3D center = driftCircle->detectorElement()->surface( id ).center();
            unsigned int word = driftCircle->getWord();
            double dbT0 = m_trtCondDbTool->getT0(id, TRTCond::ExpandedIdentifier::STRAW);

            m_segmentSide->push_back(side);
            m_segmentLayerOrWheel->push_back(layerOrWheel);
            m_segmentPhi->push_back(phi);
            m_segmentStrawLayer->push_back(strawLayer);
            m_segmentStraw->push_back(straw);
            m_segmentStrawNum->push_back(strawNumber);
            m_segmentChip->push_back(chip);
            m_segmentBoard->push_back(board);
            m_segmentDriftTimeBin->push_back(driftCircle->driftTimeBin());
            m_segmentTrailingEdge->push_back(driftCircle->trailingEdge()); 
            m_segmentDriftCircleWord->push_back(word);
            m_segmentX->push_back(center.x());
            m_segmentY->push_back(center.y());
            m_segmentZ->push_back(center.z());
            m_segmentT0->push_back(dbT0);

        } // end hits loop
    } 


  /////////////////////////////////////////////////
  /// Fill tree and clear vectors for next event
  /////////////////////////////////////////////////
    m_treeSegment->Fill();

    m_segmentSide->clear();
    m_segmentLayerOrWheel->clear();
    m_segmentPhi->clear();
    m_segmentStrawLayer->clear();
    m_segmentStraw->clear();
    m_segmentStrawNum->clear();
    m_segmentChip->clear();
    m_segmentBoard->clear();
    m_segmentDriftTimeBin->clear();
    m_segmentTrailingEdge->clear();
    m_segmentDriftCircleWord->clear();
    m_segmentHitOnTrack->clear();
    m_segmentX->clear();
    m_segmentY->clear();
    m_segmentZ->clear();
    m_segmentT0->clear();
}


void InDet::TRT_DetectorPlots::fillHitsTree()
{
    ///////////////////////////////////////////
    /// Loop over all drift circles in event
    ///////////////////////////////////////////
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "start loop over all hits" << endreq;

    int count=0;
    for( InDet::TRT_DriftCircleContainer::const_iterator it=m_trtDriftCircleContainer->begin()
        ; it!=m_trtDriftCircleContainer->end() && count<m_numNoise
        ; ++it, ++count
        ) 
    {
        const InDet::TRT_DriftCircleCollection *colNext = &(**it);    
        if (!colNext) 
        { 
            if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "!colNext (mTRTDriftCircleContainer loop)" << endreq; 
            continue; 
        }    

        for( DataVector<InDet::TRT_DriftCircle>::const_iterator circleit = (*colNext).begin()
            ; circleit!=(*colNext).end()
            ; ++circleit
            ) 
        {
            const InDet::TRT_DriftCircle *driftCircle = *circleit;
            if ( driftCircle == 0 ) 
            { 
                if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "driftCircle == 0" << endreq; 
                continue; 
            }

            Identifier id = driftCircle->identify();
            m_trtStrawMap->setNewIdentifier(id);

            ////////////////////////////////
            /// Is drift circle on track?
            ////////////////////////////////
            bool onTrack = false;
            //int trackNumber = 0;
            for(unsigned int onTrackItr = 0; onTrackItr < m_onTrackIdentifiers->size(); onTrackItr++) 
            {
                if(id == m_onTrackIdentifiers->at(onTrackItr)) 
                {
                    onTrack = true;
                    m_onTrackIdentifiers->erase(m_onTrackIdentifiers->begin()+onTrackItr);
                    continue;
                }
            }

            /////////////////////////////////////////////////////////////////////////
            /// Find drift circle parameters to be filled into the "hits" tree
            /////////////////////////////////////////////////////////////////////////
            int side         = m_trtHelper->barrel_ec(id);
            int layerOrWheel = m_trtHelper->layer_or_wheel(id);
            int phi          = m_trtHelper->phi_module(id);
            int strawLayer   = m_trtHelper->straw_layer(id);
            int straw        = m_trtHelper->straw(id);
            int strawNumber  = m_trtStrawMap->straw();
            int board        = m_trtStrawMap->board();
            int chip; 
            m_trtStrawNeighbourTool->getChip(id, chip);


            if(abs(side) == 1)    // barrel
            {
                // set chip number to correct scale 
                chip-=1; 
                if(layerOrWheel == 1)      chip+=21;
                else if(layerOrWheel == 2) chip+=54;
            } 
            else // endcap
            {
                // set all chips on a scale from 0-11
                if(chip > 11) chip-=20;
                chip+=(104+12*board);
            }

            //const CLHEP::Hep3Vector &center = driftCircle->detectorElement()->surface( id ).center();
            Amg::Vector3D center = driftCircle->detectorElement()->surface( id ).center();

            unsigned int word = driftCircle->getWord();
            double dbT0 = m_trtCondDbTool->getT0(id, TRTCond::ExpandedIdentifier::STRAW);

            m_hitsSide            = (side);
            m_hitsLayerOrWheel    = (layerOrWheel);
            m_hitsPhi             = (phi);
            m_hitsStrawLayer      = (strawLayer);
            m_hitsStraw           = (straw);
            m_hitsStrawNum        = (strawNumber);
            m_hitsChip            = (chip);
            m_hitsBoard           = (board);
            m_hitsDriftTimeBin    = (driftCircle->driftTimeBin());
            m_hitsTrailingEdge    = (driftCircle->trailingEdge()); 
            m_hitsDriftCircleWord = (word);
            m_hitsHitOnTrack      = (int) (onTrack);
            m_hitsX               = (center.x());
            m_hitsY               = (center.y());
            m_hitsZ               = (center.z());
            m_hitsT0              = (dbT0);

            m_treeHits->Fill();
        }
    }
}

