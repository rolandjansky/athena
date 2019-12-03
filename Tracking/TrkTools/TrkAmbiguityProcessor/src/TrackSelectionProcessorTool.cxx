/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackSelectionProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/ConstDataVector.h"
#include "GaudiKernel/MsgStream.h"
#include <map>
#include <ext/functional>
#include <iterator>

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

Trk::TrackSelectionProcessorTool::~TrackSelectionProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::TrackSelectionProcessorTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "AlgTool::initialise failed" << endmsg;
      return StatusCode::FAILURE;
    }

  ATH_CHECK( m_assoMapName.initialize(!m_assoMapName.key().empty()));
  ATH_CHECK( m_assoTool.retrieve() );
  
  sc = m_scoringTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_scoringTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_scoringTool << endmsg;
  
  sc = m_selectionTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_selectionTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_selectionTool << endmsg;


  if(m_disableSorting) 
    msg(MSG::INFO) << "Internal sorting disabled, using external ordering!" << endmsg;    
      
  return sc;
}
//==================================================================================================

StatusCode Trk::TrackSelectionProcessorTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); return sc;
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  Trk::TrackSelectionProcessorTool::process(const TrackCollection* tracksCol,
                                                            Trk::PRDtoTrackMap *prd_to_track_map) const
{
  
  //TODO: make sure the ownership; delete origin tracks from map?
  std::vector<const Track*> tracks;
  tracks.reserve(tracksCol->size());
  for(auto e: *tracksCol){
    tracks.push_back(e);
  }

  using namespace std;

  ATH_MSG_DEBUG ("Processing "<<tracks.size()<<" tracks");

  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map_cleanup;
  if (!prd_to_track_map) {
     prd_to_track_map_cleanup = std::move( m_assoTool->createPRDtoTrackMap() );
     if (!m_assoMapName.key().empty()) {
        SG::ReadHandle<Trk::PRDtoTrackMap> input_prd_map(m_assoMapName);
        if (!input_prd_map.isValid()) {
           ATH_MSG_ERROR("Failed to retrieve prd to track map " << m_assoMapName.key() );
        }
        else {
           *prd_to_track_map_cleanup = *input_prd_map;
        }
     }
     prd_to_track_map = prd_to_track_map_cleanup.get();
  }

  TrackScoreMap trackScoreTrackMap;
  //put tracks into maps etc
  addNewTracks(trackScoreTrackMap,*prd_to_track_map, tracks);
 
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 

  std::unique_ptr<TrackCollection> final_tracks(std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS)); //TODO, old or new
  solveTracks(trackScoreTrackMap, *prd_to_track_map, *final_tracks);
  
  if (msgLvl(MSG::DEBUG)) dumpTracks(*final_tracks);

  return final_tracks.release();
}


//==================================================================================================
void Trk::TrackSelectionProcessorTool::addNewTracks(TrackScoreMap &trackScoreTrackMap,
                                                    Trk::PRDtoTrackMap &prd_to_track_map,
                                                    const std::vector<const Track*> &tracks) const
{
  using namespace std;
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());
  PrdSignatureSet prdSigSet;

  TrackScore itrack=0;
  for (const Track*a_track : tracks )   {

    if(m_disableSorting) {
      // add track to map using ordering provided by the collection
      trackScoreTrackMap.insert( make_pair(itrack, TrackPtr(a_track)) );
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
        std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(prd_to_track_map, *a_track);
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
      trackScoreTrackMap.insert( make_pair(-score, TrackPtr(a_track) ) );

    }
  }
  
  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap.size());

  return;
}

void Trk::TrackSelectionProcessorTool::solveTracks(TrackScoreMap &trackScoreTrackMap,
                                                   Trk::PRDtoTrackMap &prd_to_track_map,
                                                   TrackCollection &final_tracks) const
{
  using namespace std;

  ATH_MSG_VERBOSE ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !trackScoreTrackMap.empty() ) {

    TrackScoreMap::iterator itnext = trackScoreTrackMap.begin();
    TrackPtr atrack( std::move(itnext->second) );
    TrackScore ascore( itnext->first);
    trackScoreTrackMap.erase(itnext);

    ATH_MSG_VERBOSE ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
    std::unique_ptr<Trk::Track> cleanedTrack;
    auto [cleanedTrack_tmp, keep_orig]  = m_selectionTool->getCleanedOutTrack( atrack.track() , -(ascore), prd_to_track_map);
    cleanedTrack.reset(cleanedTrack_tmp);

    if (keep_orig ){
      // track can be kept as identical to the input track
      ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-(ascore));
      // add track to PRD_AssociationTool
      StatusCode sc = m_assoTool->addPRDs(prd_to_track_map,*atrack);
      if (sc.isFailure()) msg(MSG::ERROR) << "addPRDs() failed" << endmsg;
      // add to output list
      final_tracks.push_back( const_cast<Track*>(atrack.track()) );

    }else if ( !cleanedTrack.get() ) {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " doesn't meet the cuts of the AmbiTrack Selection tool");

    }
    else  {

      // delete cleaned track
      cleanedTrack.reset();

      // stripped down version cannot be handled discarding
      ATH_MSG_DEBUG("Selection tool returned a new track, cannot handle memory management of new track, deleting it. Check you configuration ");
    }
    // don't forget to drop track from map
  }

  ATH_MSG_DEBUG ("Finished, number of track on output: "<<final_tracks.size());

  return;
}

//==================================================================================================

void Trk::TrackSelectionProcessorTool::dumpTracks( const TrackCollection& tracks ) const
{

  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it)
    {
      // score track:
      const TrackScore score = m_scoringTool->score( **it, true );
      ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
      totalScore+=score;
    }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
  return;
}
