/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Trk::SimpleTrackCollectionMerger
///////////////////////////////////////////////////////////////////


#include "GaudiKernel/MsgStream.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkTrackCollectionMerger/SimpleTrackCollectionMerger.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////
Trk::SimpleTrackCollectionMerger::SimpleTrackCollectionMerger
(const std::string& name, ISvcLocator* pSvcLocator  ) :
  AthAlgorithm(name, pSvcLocator ){
  m_combinedTracksStoregateKey         = "CombinedInDetTracks"    ;
  declareProperty("TracksLocation",                 m_inputTracksStoregateKeys         );
  declareProperty("OutputTracksLocation",           m_combinedTracksStoregateKey       ); 
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////
StatusCode 
Trk::SimpleTrackCollectionMerger::initialize(){
  ATH_MSG_DEBUG("Initializing SimpleTrackCollectionMerger");
  ATH_CHECK( m_inputTracksStoregateKeys.initialize() );
  ATH_CHECK( m_combinedTracksStoregateKey.initialize() );
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_associationMapKey.initialize( !m_associationMapKey.key().empty() ));
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode 
Trk::SimpleTrackCollectionMerger::execute(){
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap(m_assoTool->createPRDtoTrackMap());
  std::unique_ptr<TrackCollection> outputCollection = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
  ATH_MSG_DEBUG("Number of Track collections " << m_inputTracksStoregateKeys.size());

  // pre-loop to reserve enough space in the output collection
  std::vector<const TrackCollection*> trackCollections;
  trackCollections.reserve(m_inputTracksStoregateKeys.size());
  size_t totalNumberOfTracks = 0;
  for (auto& sgKey : m_inputTracksStoregateKeys){
    ///Retrieve track collections from StoreGate
    SG::ReadHandle<TrackCollection> thisTrackCollection (sgKey);
    trackCollections.push_back(thisTrackCollection.cptr());
    totalNumberOfTracks += thisTrackCollection->size();
  }
  // reserve the right number of entries for the output collection
  outputCollection->reserve(totalNumberOfTracks);
  // merging loop
  for(auto& pThisTrackCollection : trackCollections){
    if(mergeTrack(pThisTrackCollection, *prdToTrackMap, outputCollection.get()).isFailure()){
	     ATH_MSG_ERROR( "Failed to merge tracks! ");
      }
  }
  ATH_MSG_DEBUG("Size of combined tracks " << totalNumberOfTracks);
  SG::WriteHandle<TrackCollection> sgTrackCollectionWriter(m_combinedTracksStoregateKey);
  ATH_CHECK(sgTrackCollectionWriter.record(std::move(outputCollection)));	    
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
StatusCode 
Trk::SimpleTrackCollectionMerger::finalize(){
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Merge track collections and remove duplicates
///////////////////////////////////////////////////////////////////
StatusCode 
Trk::SimpleTrackCollectionMerger::mergeTrack(const TrackCollection* pTrackCollection, Trk::PRDtoTrackMap &prdToTrackMap,TrackCollection* outputCollection){
  // loop over tracks, accept them and add them into association tool
  if(pTrackCollection and not pTrackCollection->empty()) {
    ATH_MSG_DEBUG("Size of track collection " << pTrackCollection->size());
    // loop over tracks
    for(const auto& pThisTrack: *pTrackCollection){
      // add track into output
      Trk::Track* newTrack = const_cast<Trk::Track*>(pThisTrack);
      outputCollection->push_back(newTrack);
      // add tracks into PRD tool
      if (m_assoTool->addPRDs(prdToTrackMap, *newTrack).isFailure())
	      msg(MSG::WARNING) << "Failed to add PRDs to map" << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

