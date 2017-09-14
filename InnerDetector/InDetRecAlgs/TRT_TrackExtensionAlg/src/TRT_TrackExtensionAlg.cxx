/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	AthAlgorithm(name, pSvcLocator),
	m_trtExtension("InDet::TRT_TrackExtensionTool_xk") {
	m_outputlevel = 0;
	m_nprint = 0;
	m_nTracks = 0;
	m_nTracksExtended = 0;
	m_nTracksTotal = 0;
	m_nTracksExtendedTotal = 0;

	// TRT_TrackExtensionAlg steering parameters
	//
	//	m_tracksLocation = "SiSPSeededTracks";
	//	m_extendedTracksLocation = "TRT_ExtendedTracks";

	//	declareProperty("InputTracksLocation", m_tracksLocation);
	//	declareProperty("ExtendedTracksLocation", m_extendedTracksLocation);
	declareProperty("TrackExtensionTool", m_trtExtension);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::initialize() {
	// Get tool for track ectension to TRT
	//
	if( m_trtExtension.retrieve().isFailure()) {

		msg(MSG::FATAL)<< "Failed to retrieve tool " << m_trtExtension << endmsg;
		return StatusCode::FAILURE;
	}
	else {
		msg(MSG::INFO) << "Retrieved tool " << m_trtExtension << endmsg;
	}

	// Get output print level
	//
	m_outputlevel = msg().level()-MSG::DEBUG;
	if(m_outputlevel<=0) {
		m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endmsg;
	}
	m_nTracksTotal         = 0;
	m_nTracksExtendedTotal = 0;
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::execute() {
	m_nTracks         = 0;
	m_nTracksExtended = 0;

	// Get input tracks collection
	//
	/*
	const TrackCollection*  inputTracks = 0;
	StatusCode sc	= evtStore()->retrieve(inputTracks, m_tracksLocation);
	if (sc.isFailure() || !inputTracks) {

		if(m_outputlevel<=0) {m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;}
		return StatusCode::SUCCESS;
	}	*/
	SG::ReadHandle<TrackCollection> inputTracks(m_inputTracksKey);
	if (not inputTracks.isValid()) {
		ATH_MSG_DEBUG("Could not find input track collection " << m_inputTracksKey);
		return StatusCode::SUCCESS;
	}

	m_trtExtension->newEvent();

	// Loop through all input track and output tracks collection production
	//
	SG::WriteHandle<TrackExtensionMap> outputTracks(m_outputTracksKey);
	ATH_CHECK( outputTracks.record(std::make_unique<TrackExtensionMap>()) );

	TrackCollection::const_iterator trk,trkEnd = inputTracks->end();
	for (trk = inputTracks->begin(); trk != trkEnd; ++trk) {
		if ( !(*trk) ) continue;
		++m_nTracks;

		std::vector<const Trk::MeasurementBase*>& trkExt = m_trtExtension->extendTrack(*(*trk));
		if( !trkExt.size() ) continue;
		
		outputTracks->insert( std::make_pair((*trk), trkExt) ); 
		++m_nTracksExtended;
	}

	m_nTracksTotal        += m_nTracks        ;
	m_nTracksExtendedTotal+= m_nTracksExtended;

	// Save extended to TRT RIOs
	//
	/*
	sc = evtStore()->record(extendedTracks,m_extendedTracksLocation,false);
	if (sc.isFailure() ) {
		msg(MSG::ERROR)<<"Could not save converted extended to TRT tracks"<<endmsg;
		return sc;
	}	*/

	// Print common event information
	//
	if(m_outputlevel<=0) {m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;}
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionAlg::finalize() {
	m_nprint=2; msg(MSG::INFO)<<(*this)<<endmsg;
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    <<
(MsgStream& sl,const InDet::TRT_TrackExtensionAlg& se) {
	return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator <<
(std::ostream& sl,const InDet::TRT_TrackExtensionAlg& se) {
	return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionAlg::dump( MsgStream& out ) const {
	out<<std::endl;
	if(m_nprint)  return dumpEvent(out);
	return dumpConditions(out);
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
	out<<"| Location of input tracks                        | "<<m_inputTracksKey
	   <<s2<<std::endl;
	out<<"| Extended tracks location                        | "<<m_outputTracksKey
	   <<s3<<std::endl;
	out<<"|----------------------------------------------------------------"
	   <<"----------------------------------------------------|"
	   <<std::endl;
 return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionAlg::dumpEvent( MsgStream& out ) const {

	int nt  = m_nTracks        ;
	int nte = m_nTracksExtended;
	if(m_nprint > 1) {nt = m_nTracksTotal; nte = m_nTracksExtendedTotal;}

	out<<"|-------------------------------------------------------------------";
	out<<"-----------------------------|"
	   <<std::endl;
	out<<"|  Investigated "
	   <<std::setw(7)<<nt <<" input tracks and extended    "
	   <<std::setw(7)<<nte<<" tracks                              |"
	   <<std::endl;
	out<<"|-------------------------------------------------------------------";
	out<<"-----------------------------|"
	   <<std::endl;
	return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackExtensionAlg::dump( std::ostream& out ) const {
	return out;
}
