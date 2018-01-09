/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Trk::TrackCollectionMerger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 11/26/2007 Thomas Koffas
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkTrackCollectionMerger/TrackCollectionMerger.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

Trk::TrackCollectionMerger::TrackCollectionMerger
(const std::string& name, ISvcLocator* pSvcLocator  ) :
  AthReentrantAlgorithm(name, pSvcLocator ),
  m_createViewCollection(true),
  m_updateSharedHitsOnly(true),
  m_updateAdditionalInfo(false)
{
  m_outtracklocation         = "CombinedInDetTracks"    ;

  declareProperty("TracksLocation",                 m_tracklocation          );
  declareProperty("OutputTracksLocation",           m_outtracklocation       ); 
  declareProperty("AssoTool",                       m_assoTool               );
  declareProperty("SummaryTool" ,                   m_trkSummaryTool         );
  declareProperty("CreateViewColllection" ,         m_createViewCollection   );
  declareProperty("UpdateSharedHitsOnly" ,          m_updateSharedHitsOnly);
  declareProperty("UpdateAdditionalInfo" ,          m_updateAdditionalInfo);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode Trk::TrackCollectionMerger::initialize()
{

  ATH_MSG_DEBUG("Initializing TrackCollectionMerger");

  if ( m_assoTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endmsg;
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved tool " << m_assoTool);

  if (m_trkSummaryTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSummaryTool << endmsg;
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("Retrieved tool " << m_trkSummaryTool);

  if( m_updateSharedHitsOnly &&  m_updateAdditionalInfo){
    msg(MSG::WARNING) << "Both UpdateAdditionalInfo and UpdateSharedHitsOnly set true - UpdateAdditionalInfo includes a shared hits update. " << endmsg;
    msg(MSG::WARNING) << " If you *only* want to update shared hits, set UpdateAdditionalInfo=False and UpdateSharedHitsOnly=True" << endmsg;
  }
 
  ATH_CHECK(  m_tracklocation.initialize() );
  ATH_CHECK( m_outtracklocation.initialize() );
 
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode Trk::TrackCollectionMerger::execute_r(const EventContext& ctx) const
{
  // clean up association tool
  m_assoTool->reset();
  
  std::unique_ptr<TrackCollection> outputCol = m_createViewCollection ? 
    std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS) : std::make_unique<TrackCollection>();
  ATH_MSG_DEBUG("Number of Track collections " << m_tracklocation.size());

  // pre-loop to reserve enough space in the output collection
  std::vector<const TrackCollection*> trackCollections;
  trackCollections.reserve(m_tracklocation.size());
  size_t ttNumber = 0;
  for (auto& tcname : m_tracklocation){
    ///Retrieve forward tracks from StoreGate
    SG::ReadHandle<TrackCollection> trackCol (tcname,ctx);
    trackCollections.push_back(trackCol.cptr());
    ttNumber += trackCol->size();
  }

  // reserve the right number of entries for the output collection
  outputCol->reserve(ttNumber);
  // merging loop
  for(auto& tciter : trackCollections){
      // merge them in
    if(mergeTrack(tciter,outputCol.get()).isFailure()){
	     ATH_MSG_ERROR( "Failed to merge tracks! ");
      }
  }

  ATH_MSG_DEBUG("Size of combined tracks " << outputCol->size());

  ATH_MSG_DEBUG("Update summaries");  
  // now loop over all tracks and update summaries with new shared hit counts
  for (Trk::Track* trk : *outputCol) {
    if (m_updateAdditionalInfo)  m_trkSummaryTool->updateAdditionalInfo(*trk);
    else if (m_updateSharedHitsOnly) m_trkSummaryTool->updateSharedHitCount(*trk);
    else  m_trkSummaryTool->updateTrack(*trk);
  }

  SG::WriteHandle<TrackCollection> h_write(m_outtracklocation,ctx);
  ATH_CHECK(h_write.record(std::move(outputCol)));	     


  //Print common event information
  if(msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG((*this));
  }


  ATH_MSG_DEBUG("Done !");  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode Trk::TrackCollectionMerger::finalize()
{
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  Trk::TrackCollectionMerger::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(msgLvl(MSG::DEBUG))  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& Trk::TrackCollectionMerger::dumptools( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& Trk::TrackCollectionMerger::dumpevent( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& Trk::TrackCollectionMerger::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& Trk::operator    << 
  (MsgStream& sl,const Trk::TrackCollectionMerger& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& Trk::operator << 
  (std::ostream& sl,const Trk::TrackCollectionMerger& se)
{
  return se.dump(sl); 
}   


///////////////////////////////////////////////////////////////////
// Merge track collections and remove duplicates
///////////////////////////////////////////////////////////////////

StatusCode Trk::TrackCollectionMerger::mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol) const
{
  // loop over forward track, accept them and add them imto association tool
  if(trackCol && trackCol->size()) {
    ATH_MSG_DEBUG("Size of track collection " << trackCol->size());

    // loop over tracks
    for(const auto& rf: *trackCol){
      // add track into output
      // FIXME: const_cast
      // These objects are modified in the `Update summaries' section
      Trk::Track* newTrack = m_createViewCollection ? const_cast<Trk::Track*>(rf) : new Trk::Track(*rf);
      outputCol->push_back(newTrack);
      // add tracks into PRD tool
      if (m_assoTool->addPRDs(*newTrack).isFailure())
	      msg(MSG::WARNING) << "Failed to add PRDs to map" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

