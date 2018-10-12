/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackSelectionProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
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
  m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_finalTracks(0)
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

TrackCollection*  Trk::TrackSelectionProcessorTool::process(const TrackCollection* tracks)
{
  using namespace std;

  ATH_MSG_DEBUG ("Processing "<<tracks->size()<<" tracks");
    
  // clear all caches etc.
  reset();
  
  //put tracks into maps etc
  addNewTracks(tracks);
 
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 

  solveTracks();
  
  if (msgLvl(MSG::DEBUG)) dumpTracks(*m_finalTracks);
  
// memory defragmantation fix. Cleaning before returning the result 
  m_prdSigSet.clear(); 
  m_trackScoreTrackMap.clear(); 
   
  return m_finalTracks;
}

void Trk::TrackSelectionProcessorTool::reset()
{
  //this is a map which contains pointers to copies of all the input tracks
  m_trackScoreTrackMap.clear();

  //Signature Set
  m_prdSigSet.clear();
  
  // clear prdAssociationTool via selection tool
  m_selectionTool->reset();

  //final copy - ownership is passed out of algorithm
  m_finalTracks = new TrackCollection(SG::VIEW_ELEMENTS);

  return;
}

//==================================================================================================
void Trk::TrackSelectionProcessorTool::addNewTracks(const TrackCollection* tracks)
{
  using namespace std;
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks->size());
 
  TrackCollection::const_iterator trackIt    = tracks->begin();
  TrackCollection::const_iterator trackItEnd = tracks->end();
 
  TrackScore itrack=0;
  for ( ; trackIt != trackItEnd ; ++trackIt)   {

    if(m_disableSorting) {
      // add track to map using ordering provided by the collection
      m_trackScoreTrackMap.insert( make_pair(itrack, make_pair(*trackIt, true)) );
      itrack++;
      continue;
    }
    
    bool reject = false;
    
    // only fitted tracks get hole search, input is not fitted
    TrackScore score = m_scoringTool->score( **trackIt, true );
    // veto tracks with score 0
    if (score==0) { 
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      reject = true;

    } else {
       
      if (m_dropDouble) {
	std::vector<const Trk::PrepRawData*> prds = m_selectionTool->getPrdsOnTrack(*trackIt);
	// unfortunately PrepRawDataSet is not a set !
	PrdSignature prdSig;
	prdSig.insert( prds.begin(),prds.end() );
	// we try to insert it into the set, if we fail (pair.second), it then exits already
	if ( !(m_prdSigSet.insert(prdSig)).second ) {
	  ATH_MSG_DEBUG ("Double track, reject it !");
	  reject = true;
	} else {
	  ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
	}
      }
    }
 
    if (!reject) {
      // add track to map, map is sorted small to big ! set if fitted
      ATH_MSG_VERBOSE ("Track  ("<< *trackIt <<") has score "<<score);
      m_trackScoreTrackMap.insert( make_pair(-score, make_pair(*trackIt, true) ) );

    }
  }
  
  ATH_MSG_DEBUG ("Number of tracks in map:"<<m_trackScoreTrackMap.size());

  return;
}

void Trk::TrackSelectionProcessorTool::solveTracks()
{
  using namespace std;

  ATH_MSG_VERBOSE ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !m_trackScoreTrackMap.empty() ) {

    TrackScoreMap::iterator itnext = m_trackScoreTrackMap.begin();
    ATH_MSG_VERBOSE ("--- Trying next track "<<itnext->second.first<<"\t with score "<<-itnext->first);
    const Trk::Track* cleanedTrack = m_selectionTool->getCleanedOutTrack( itnext->second.first , -(itnext->first));
    if (cleanedTrack == itnext->second.first ){
      // track can be kept as identical to the input track
      ATH_MSG_DEBUG ("Accepted track "<<itnext->second.first<<"\t has score "<<-(itnext->first));
      // add track to PRD_AssociationTool
      StatusCode sc = m_selectionTool->registerPRDs(itnext->second.first);
      if (sc.isFailure()) msg(MSG::ERROR) << "addPRDs() failed" << endmsg;
      // add to output list 
      m_finalTracks->push_back( const_cast<Track*>(itnext->second.first) );

    }else if ( !cleanedTrack ) {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< itnext->second.first << " doesn't meet the cuts of the AmbiTrack Selection tool");

    }else if ( cleanedTrack != itnext->second.first ) {
      
      // delete cleaned track
      delete cleanedTrack;
      
      // stripped down version cannot be handled discarding
      ATH_MSG_DEBUG("Selection tool returned a new track, cannot handle memory management of new track, deleting it. Check you configuration ");
    }else{
      // this should not happen
      ATH_MSG_WARNING ("Unexpect return from selection tool, discarding track");

      // clean up memory to avoid leak
      if( cleanedTrack && cleanedTrack != itnext->second.first ) delete cleanedTrack;
	
    }
    // don't forget to drop track from map
    m_trackScoreTrackMap.erase(itnext);
  }
  
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<m_finalTracks->size());
  
  return;
}

//==================================================================================================

void Trk::TrackSelectionProcessorTool::dumpTracks( const TrackCollection& tracks )
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


