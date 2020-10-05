/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_TrackExtensionAlg/TRT_TrackExtensionAlg.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkTrack/TrackCollection.h"

// Store gate handles
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionAlg::TRT_TrackExtensionAlg
(const std::string& name,ISvcLocator* pSvcLocator) :
	AthReentrantAlgorithm(name, pSvcLocator) {
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::initialize() {
	// Get tool for track ectension to TRT

	ATH_CHECK(m_trtExtension.retrieve());

	ATH_CHECK( m_inputTracksKey.initialize() );
	ATH_CHECK( m_outputTracksKey.initialize() );

	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::execute(const EventContext& ctx) const {
        Counter_t counter;

	// Get input tracks collection
	SG::ReadHandle<TrackCollection> inputTracks(m_inputTracksKey,ctx);
	if (not inputTracks.isValid()) {
		ATH_MSG_DEBUG("Could not find input track collection " << m_inputTracksKey);
		return StatusCode::SUCCESS;
	}

        std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>
           event_data_p( m_trtExtension->newEvent(ctx) );

	// Loop through all input track and output tracks collection production
	SG::WriteHandle<TrackExtensionMap> outputTracks(m_outputTracksKey,ctx);
	ATH_CHECK( outputTracks.record(std::make_unique<TrackExtensionMap>()) );

	TrackCollection::const_iterator trk,trkEnd = inputTracks->end();
	for (trk = inputTracks->begin(); trk != trkEnd; ++trk) {
		if ( !(*trk) ) continue;
		++counter.m_nTracks;

		std::vector<const Trk::MeasurementBase*>& trkExt = m_trtExtension->extendTrack(ctx, *(*trk), *event_data_p);
		if( !trkExt.size() ) continue;

		outputTracks->insert( std::make_pair((*trk), trkExt) ); 
		++counter.m_nTracksExtended;
	}

        {
           std::lock_guard<std::mutex> lock(m_counterMutex);
           m_totalCounts += counter;
        }

        if (msgLvl(MSG::DEBUG)) {
           dumpEvent(msg(MSG::DEBUG), counter);
           msg(MSG::DEBUG) << endmsg;
        }
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::finalize() {
        if (msgLvl(MSG::INFO)) {
           dumpEvent(msg(MSG::INFO), m_totalCounts);
           dumpConditions(msg(MSG::INFO));
        }
	return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionAlg::dumpConditions( MsgStream& out ) const {
	int n = 65-m_trtExtension.type().size();
	std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
	n     = 65-m_inputTracksKey.key().size();
	std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
	n     = 65-m_outputTracksKey.key().size();
	std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

	out<<"|----------------------------------------------------------------"
	   <<"----------------------------------------------------|"
	   <<std::endl;
	out<<"| Tool for tracks extension to TRT                | "<<m_trtExtension.type()   
	   <<s1<<std::endl;
	out<<"| Location of input tracks                        | "<<m_inputTracksKey.key()
	   <<s2<<std::endl;
	out<<"| Extended tracks location                        | "<<m_outputTracksKey.key()
	   <<s3<<std::endl;
	out<<"|----------------------------------------------------------------"
	   <<"----------------------------------------------------|"
	   <<std::endl;
 return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionAlg::dumpEvent( MsgStream& out, const InDet::TRT_TrackExtensionAlg::Counter_t &counter ) const {

	out<<"|-------------------------------------------------------------------";
	out<<"-----------------------------|"
	   <<std::endl;
	out<<"|  Investigated "
	   <<std::setw(7)<< counter.m_nTracks         <<" input tracks and extended    "
	   <<std::setw(7)<< counter.m_nTracksExtended <<" tracks                              |"
	   <<std::endl;
	out<<"|-------------------------------------------------------------------";
	out<<"-----------------------------|"
	   <<std::endl;
	return out;
}
