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
  AthAlgorithm(name, pSvcLocator ),
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
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endreq;
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved tool " << m_assoTool);

  if (m_trkSummaryTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSummaryTool << endreq;
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("Retrieved tool " << m_trkSummaryTool);

  if( m_updateSharedHitsOnly &&  m_updateAdditionalInfo){
    msg(MSG::WARNING) << "Both UpdateAdditionalInfo and UpdateSharedHitsOnly set true - UpdateAdditionalInfo includes a shared hits update. " << endreq;
    msg(MSG::WARNING) << " If you *only* want to update shared hits, set UpdateAdditionalInfo=False and UpdateSharedHitsOnly=True" << endreq;
  }
  
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode Trk::TrackCollectionMerger::execute()
{
  // clean up association tool
  m_assoTool->reset();
  
  // out output track collection
  TrackCollection* outputCol = m_createViewCollection ? 
      new TrackCollection(SG::VIEW_ELEMENTS) : new TrackCollection;
  ATH_MSG_DEBUG("Number of Track collections " << m_tracklocation.size());

  // pre-loop to reserve enough space in the output collection
  std::vector<const TrackCollection*> trackCollections;
  trackCollections.reserve(m_tracklocation.size());
  size_t ttNumber = 0;
  for (auto& tcname : m_tracklocation){
      const TrackCollection* trackCol;
      ///Retrieve forward tracks from StoreGate
      if (evtStore()->retrieve(trackCol,tcname).isFailure()){
        ATH_MSG_DEBUG("No tracks with name " << tcname);
      } else {
        ATH_MSG_DEBUG("Found track collection " << tcname );
        trackCollections.push_back(trackCol);
        ttNumber += trackCol->size();
      }
  }
  // reserve the right number of entries for the output collection
  outputCol->reserve(ttNumber);
  // merging loop
  for(auto& tciter : trackCollections){
      // merge them in
      if(mergeTrack(tciter,outputCol).isFailure()){
	     ATH_MSG_ERROR( "Failed to merge tracks! ");
      }
  }

  ATH_MSG_DEBUG("Size of combined tracks " << outputCol->size());
  if( evtStore()->record(outputCol,m_outtracklocation,false).isFailure() ){
    ATH_MSG_ERROR("Could not save the reconstructed TRT seeded Si tracks!");
    return StatusCode::FAILURE ;
  }

  ATH_MSG_DEBUG("Update summaries");  
  // now loop over all tracks and update summaries with new shared hit counts
  TrackCollection::iterator rf  = outputCol->begin();
  TrackCollection::iterator rfE = outputCol->end();
  for(  ; rf != rfE; ++rf){ 
    if (m_updateAdditionalInfo)  m_trkSummaryTool->updateAdditionalInfo(**rf);
    else if (m_updateSharedHitsOnly) m_trkSummaryTool->updateSharedHitCount(**rf);
    else  m_trkSummaryTool->updateTrack(**rf);
  }

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

StatusCode Trk::TrackCollectionMerger::mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol)
{
  // loop over forward track, accept them and add them imto association tool
  if(trackCol && trackCol->size()) {
    ATH_MSG_DEBUG("Size of track collection " << trackCol->size());

    // loop over tracks
    for(auto& rf: *trackCol){
      // add track into output
      Trk::Track* newTrack = m_createViewCollection ? rf : new Trk::Track(*rf);
      outputCol->push_back(newTrack);
      // add tracks into PRD tool
      if (m_assoTool->addPRDs(*newTrack).isFailure())
	      msg(MSG::WARNING) << "Failed to add PRDs to map" << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

