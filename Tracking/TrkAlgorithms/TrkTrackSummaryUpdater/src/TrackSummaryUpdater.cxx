/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkTrackSummaryUpdater/TrackSummaryUpdater.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

Trk::TrackSummaryUpdater::TrackSummaryUpdater
(const std::string& name, ISvcLocator* pSvcLocator  ) :
  AthAlgorithm(name, pSvcLocator ),
  m_updateSharedHits(true),
  m_updateAdditionalInfo(false){
  m_inputTracksStoregateKey = "CombinedInDetTracks";
  declareProperty("InputTracksKey",            m_inputTracksStoregateKey);
  declareProperty("OutputTracksKey",           m_updatedTracksStoregateKey); 
  declareProperty("SummaryTool" ,              m_trkSummaryTool);
  declareProperty("UpdateSharedHits" ,         m_updateSharedHits);
  declareProperty("UpdateAdditionalInfo" ,     m_updateAdditionalInfo);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode 
Trk::TrackSummaryUpdater::initialize(){
  ATH_MSG_DEBUG("Initializing TrackSummaryUpdater");
  ATH_CHECK( m_inputTracksStoregateKey.initialize() );
  ATH_CHECK( m_updatedTracksStoregateKey.initialize() );
  ATH_CHECK( m_trkSummaryTool.retrieve() ); //..so needs at least a TrackSummaryTool to be configured
  if (not m_assoTool.empty())  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_assoMapName.initialize( !m_assoMapName.key().empty() ));
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode 
Trk::TrackSummaryUpdater::execute(){
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap (m_assoTool ? m_assoTool->createPRDtoTrackMap(): std::make_unique<Trk::PRDtoTrackMap>());
  std::unique_ptr<TrackCollection> updatedCollection = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
  SG::ReadHandle<TrackCollection> inputTrackCollection(m_inputTracksStoregateKey);
  updatedCollection->reserve(inputTrackCollection->size());
  ATH_MSG_DEBUG("Update summaries");  
  const bool createNewSummary = (not m_updateAdditionalInfo) and (not m_updateSharedHits);
  for (const auto *const pThisTrack : *inputTrackCollection) {
    auto *nonConstTrackPtr = const_cast<Trk::Track *>(pThisTrack);
    if (m_updateAdditionalInfo)  m_trkSummaryTool->updateAdditionalInfo(*nonConstTrackPtr);
    if (m_updateSharedHits) m_trkSummaryTool->updateSharedHitCount(*nonConstTrackPtr, prdToTrackMap.get());
    if (createNewSummary) m_trkSummaryTool->computeAndReplaceTrackSummary(*nonConstTrackPtr, prdToTrackMap.get(), false /* DO NOT suppress hole search*/);
    updatedCollection->push_back(nonConstTrackPtr);
  }
  SG::WriteHandle<TrackCollection> sgTargetWriteHandle(m_updatedTracksStoregateKey);
  ATH_CHECK(sgTargetWriteHandle.record(std::move(updatedCollection)));	     
  if (m_assoTool and (not m_assoMapName.key().empty())) {
     SG::WriteHandle<Trk::PRDtoTrackMap> storegateWriter(m_assoMapName);
     if (storegateWriter.record( m_assoTool->reduceToStorableMap(std::move(prdToTrackMap))).isFailure()) {
        ATH_MSG_FATAL("Failed to add PRD to track association map.");
     }
  }
  
  ATH_MSG_DEBUG("Done !");  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode 
Trk::TrackSummaryUpdater::finalize(){
   return StatusCode::SUCCESS;
}

