/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackSelectionProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/MsgStream.h"
#include <map>

//==================================================================================================
Trk::TrackSelectionProcessorTool::TrackSelectionProcessorTool(const std::string& t, 
                const std::string& n,
                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool")
{
  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("DropDouble"           , m_dropDouble         = true);
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("DisableSorting"       , m_disableSorting     = false);
}
//==================================================================================================

Trk::TrackSelectionProcessorTool::~TrackSelectionProcessorTool(){
}
//==================================================================================================

StatusCode 
Trk::TrackSelectionProcessorTool::initialize(){
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "AlgTool::initialise failed" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_assoMapName.initialize(!m_assoMapName.key().empty()));
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_scoringTool.retrieve());
  ATH_CHECK(m_selectionTool.retrieve());
  if (m_disableSorting) ATH_MSG_INFO( "Internal sorting disabled, using external ordering!" );    
  return sc;
}
//==================================================================================================

StatusCode 
Trk::TrackSelectionProcessorTool::finalize(){
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  
Trk::TrackSelectionProcessorTool::process(const TrackCollection* tracksCol,
                                          Trk::PRDtoTrackMap *pPrdToTrackMap) const{
  //TODO: make sure the ownership; delete origin tracks from map?
  std::vector<const Track*> tracks;
  tracks.reserve(tracksCol->size());
  for(auto e: *tracksCol){
    tracks.push_back(e);
  }
  ATH_MSG_DEBUG ("Processing "<<tracks.size()<<" tracks");
  std::unique_ptr<Trk::PRDtoTrackMap> tmpPrdToTrackMap;
  if (!pPrdToTrackMap) {
     tmpPrdToTrackMap = m_assoTool->createPRDtoTrackMap();
     if (!m_assoMapName.key().empty()) {
        SG::ReadHandle<Trk::PRDtoTrackMap> inputPrdMap(m_assoMapName);
        if (!inputPrdMap.isValid()) {
           ATH_MSG_ERROR("Failed to retrieve prd to track map " << m_assoMapName.key() );
        } else {
           *tmpPrdToTrackMap = *inputPrdMap;
        }
     }
     pPrdToTrackMap = tmpPrdToTrackMap.get();
  }
  TrackScoreMap trackScoreTrackMap;
  //put tracks into maps etc
  addNewTracks(trackScoreTrackMap,*pPrdToTrackMap, tracks);
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 
  std::unique_ptr<TrackCollection> result(std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS)); //TODO, old or new
  solveTracks(trackScoreTrackMap, *pPrdToTrackMap, *result);
  if (msgLvl(MSG::DEBUG)) dumpTracks(*result);
  return result.release();
}


//==================================================================================================
void 
Trk::TrackSelectionProcessorTool::addNewTracks(TrackScoreMap &trackScoreTrackMap,
                                                    Trk::PRDtoTrackMap &prdToTrackMap,
                                                    const std::vector<const Track*> &tracks) const{
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());
  PrdSignatureSet prdSigSet;
  TrackScore itrack=0;
  for (const Track*a_track : tracks )   {
    if(m_disableSorting) {
      // add track to map using ordering provided by the collection
      trackScoreTrackMap.insert( std::make_pair(itrack, TrackPtr(a_track)) );
      itrack++;
      continue;
    }
    bool reject = false;
    // only fitted tracks get hole search, input is not fitted
    TrackScore score = m_scoringTool->score( *a_track, true );
    // veto tracks with score 0
    if (score==0) { 
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      reject = true;
    } else {
      if (m_dropDouble) {
        const std::vector<const Trk::PrepRawData*> & prds = m_assoTool->getPrdsOnTrack(prdToTrackMap, *a_track);
        // unfortunately PrepRawDataSet is not a set !
        PrdSignature prdSig;
        prdSig.insert( prds.begin(),prds.end() );
        // we try to insert it into the set, if we fail (pair.second), it then exits already
        if ( !(prdSigSet.insert(prdSig)).second ) {
          ATH_MSG_DEBUG ("Double track, reject it !");
          reject = true;
        } else {
          ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
        }
      }
    }
    if (!reject) {
      // add track to map, map is sorted small to big ! set if fitted
      ATH_MSG_VERBOSE ("Track  ("<< a_track <<") has score "<<score);
      trackScoreTrackMap.insert( std::make_pair(-score, TrackPtr(a_track) ) );
    }
  }
  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap.size());
}

void 
Trk::TrackSelectionProcessorTool::solveTracks(TrackScoreMap &trackScoreTrackMap,
                                                   Trk::PRDtoTrackMap &prdToTrackMap,
                                                   TrackCollection &result) const{
  ATH_MSG_VERBOSE ("Starting to solve tracks");
  // now loop as long as map is not empty
  while ( !trackScoreTrackMap.empty() ) {
    TrackScoreMap::iterator itnext = trackScoreTrackMap.begin();
    TrackPtr atrack( std::move(itnext->second) );
    TrackScore ascore( itnext->first);
    trackScoreTrackMap.erase(itnext);
    ATH_MSG_VERBOSE ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
    std::unique_ptr<Trk::Track> cleanedTrack;
    const auto &[cleanedTrack_tmp, keepOriginal]  = m_selectionTool->getCleanedOutTrack( atrack.track() , -(ascore), prdToTrackMap);
    cleanedTrack.reset(cleanedTrack_tmp);
    if (keepOriginal ){
      // track can be kept as identical to the input track
      ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-(ascore));
      // add track to PRD_AssociationTool
      StatusCode sc = m_assoTool->addPRDs(prdToTrackMap,*atrack);
      if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
      // add to output list
      Track *track_view ATLAS_THREAD_SAFE = const_cast<Track*>(atrack.track()); // ok, becasue the destination container is just a view on elements stored in  the input track collection.
      result.push_back( track_view );

    } else if ( !cleanedTrack ) {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " doesn't meet the cuts of the AmbiTrack Selection tool");
    } else  {
      // delete cleaned track
      cleanedTrack.reset();
      // stripped down version cannot be handled discarding
      ATH_MSG_DEBUG("Selection tool returned a new track, cannot handle memory management of new track, deleting it. Check you configuration ");
    }
    // don't forget to drop track from map
  }
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<result.size());
}

//==================================================================================================

void 
Trk::TrackSelectionProcessorTool::dumpTracks( const TrackCollection& tracks ) const{
  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it){
    // score track:
    const TrackScore score = m_scoringTool->score( **it, true );
    ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
    totalScore+=score;
  }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
}
