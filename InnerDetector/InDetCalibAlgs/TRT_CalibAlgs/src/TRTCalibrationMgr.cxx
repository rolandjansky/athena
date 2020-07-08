/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************

   TRTCalibrationMgr.cxx : Manager for TRT calibration

   * ***************************************************************** */

#include "TRT_CalibAlgs/TRTCalibrationMgr.h"

#include "TrkTrack/Track.h"

#include "TRT_CalibData/TrackInfo.h"
#include "TRT_ConditionsData/FloatArrayStore.h"
#include "TRT_CalibTools/IFillAlignTrkInfo.h"
#include "TRT_CalibTools/IAccumulator.h"
#include "TRT_CalibTools/IFitTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TROOT.h"
#include "StoreGate/ReadHandle.h"

TRTCalibrationMgr::TRTCalibrationMgr(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm   (name, pSvcLocator),
	m_TrackInfoTools(),
	m_AccumulatorTools(),
	m_FitTools(),
	m_trackFitter("Trk::KalmanFitter/TrkKalmanFitter"),
        m_streamer("AthenaOutputStreamTool/CondStream1"),
	m_dorefit(true),
	m_docalibrate(false),
	m_writeConstants(false),
	m_ntrk(0),
	m_trackSelector("InDet::InDetTrackSelectorTool/InDetTrackSelectorTool"),
	m_max_ntrk(100000),
        m_par_rtcontainerkey("/TRT/Calib/RT"),
        m_par_t0containerkey("/TRT/Calib/T0")
{
	m_TrackInfoTools.push_back("FillAlignTrkInfo");
	m_AccumulatorTools.push_back("TRTCalAccumulator");
	m_TRTCalibTools.push_back("TRTCalibrator");
	m_FitTools.push_back("FitTool");
	// declare algorithm parameters
	declareProperty("StreamTool", m_streamer);
	declareProperty("AlignTrkTools",m_TrackInfoTools);
	declareProperty("AccumulatorTools",m_AccumulatorTools);
	declareProperty("FitTools",m_FitTools);
	declareProperty("Max_ntrk",m_max_ntrk);
	declareProperty("WriteConstants",m_writeConstants);
	declareProperty("TrackFitter", m_trackFitter);
	declareProperty("DoRefit",m_dorefit);
	declareProperty("TrackSelectorTool", m_trackSelector, "Tool for the selection of tracks");
	declareProperty("DoCalibrate",m_docalibrate);

}

//---------------------------------------------------------------------

TRTCalibrationMgr::~TRTCalibrationMgr(void)
{}

//--------------------------------------------------------------------------

StatusCode TRTCalibrationMgr::initialize()
{
  ATH_MSG_DEBUG( "initialize()" );

	if (m_docalibrate) {
		if( !m_TRTCalibTools.size() || m_TRTCalibTools.retrieve().isFailure()){
		  ATH_MSG_FATAL( "Cannot get Calibration tool " << m_TRTCalibTools );
			return StatusCode::FAILURE;
		}
	} else {
		if( !m_TrackInfoTools.size() || m_TrackInfoTools.retrieve().isFailure()){
		  ATH_MSG_FATAL( "Cannot get TrackInfo filler tool " << m_TrackInfoTools );
			return StatusCode::FAILURE;
		}
	}

	if( !m_FitTools.size() || m_FitTools.retrieve().isFailure()){
	  ATH_MSG_FATAL( "Cannot get Fit tools " << m_FitTools );
		return StatusCode::FAILURE;
	}

	if(m_trackFitter.retrieve().isFailure()){
	  ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFitter );
		return StatusCode::FAILURE;
	}

        if(m_writeConstants) ATH_CHECK(m_streamer.retrieve());

	//Initialize ReadHandles and ReadHandleKeys
	ATH_CHECK(m_verticesKey.initialize());
	ATH_CHECK(m_EventInfoKey.initialize());
	ATH_CHECK(m_TrkCollections.initialize());
	ATH_CHECK(m_comTimeKey.initialize());
	// Each ROI/road may create its own collection....
	ATH_MSG_INFO( "Tracks from Trk::Track collection(s):");
	for (unsigned int i=0;i<m_TrkCollections.size();i++)
	  ATH_MSG_INFO( "\n\t" << m_TrkCollections[i]);

	// Get the Track Selector Tool
	if ( !m_trackSelector.empty() ) {
		StatusCode sc = m_trackSelector.retrieve();
		if (sc.isFailure()) {
		  ATH_MSG_FATAL( "Could not retrieve "<< m_trackSelector <<" (to select the tracks which are written to the ntuple) ");
		  ATH_MSG_INFO( "Set the ToolHandle to None if track selection is supposed to be disabled" );
			return sc;
		}
	}

	ATH_MSG_INFO("Track Selector retrieved" );


	m_ntrk=0;

	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode TRTCalibrationMgr::execute() {

	if (m_docalibrate){
	  ATH_MSG_INFO( "skipping execute() calibrating instead" );
		m_TRTCalibTools[0]->calibrate();
		return StatusCode::SUCCESS;
	}

	if(m_writeConstants){
		StatusCode sc=streamOutCalibObjects() ;
		return sc;
	}


	// Get Primary vertex
	SG::ReadHandle<xAOD::VertexContainer> vertices(m_verticesKey);
	if (not vertices.isValid()) {
		ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key: PrimaryVertices");
		return StatusCode::FAILURE;
	}

	int countVertices(0);
	for (const xAOD::Vertex* vx : *(vertices.cptr()) ) {
		if (vx->vertexType() == xAOD::VxType::PriVtx) {
			if ( vx-> nTrackParticles() >= 3) countVertices++;
		}
	}
	if (countVertices < 1) {
	  ATH_MSG_INFO( "no vertices found" );
		return StatusCode::SUCCESS;
	}

	// get event info pointer
	SG::ReadHandle<xAOD::EventInfo> EventInfo(m_EventInfoKey);
	if (not EventInfo.isValid()) {
	  ATH_MSG_FATAL( "skipping event, could not get EventInfo" );
		return StatusCode::FAILURE;
	}

	// ComTime can be missing from files not returning failure here
	SG::ReadHandle<ComTime> comTime(m_comTimeKey);
	if (not comTime.isValid()) {
		ATH_MSG_INFO("Could not find ComTime object named " << m_comTimeKey.key());
	}

	// Loop over tracks; get track info and accumulate it
	const Trk::Track* aTrack;

	TrackCollection::const_iterator t;

	for (SG::ReadHandle<TrackCollection>& trks : m_TrkCollections.makeHandles()) {
		// retrieve all tracks from TDS

		if (trks.isValid()){
			//      if (trks->size()>100){

			if(trks->size()<3) {
			  ATH_MSG_INFO( "skipping event, it contains only " << trks->size() << " tracks (less than 3)");
				return StatusCode::SUCCESS;
			}


			if(trks->size()>m_max_ntrk) {
			  ATH_MSG_INFO( "skipping event, it contains " << trks->size() << " tracks, more than max: " << m_max_ntrk);
				return StatusCode::SUCCESS;
			}

			for (t=trks->begin();t!=trks->end();++t) {

				if ( m_trackSelector->decision(*(*t), 0)) {

					m_ntrk++;
					aTrack=*t;

					if(m_dorefit){
						//Refit Track with new ROT creator
						Trk::RunOutlierRemoval runOutlier=true;
						aTrack= m_trackFitter->fit(*aTrack,runOutlier,aTrack->info().particleHypothesis());
					}
					// Check selection if requested
					if (aTrack ){
						// fill track info
						TRT::TrackInfo at;
						// Run, event, track id
						at[TRT::Track::run]=EventInfo->runNumber();
						at[TRT::Track::event]=EventInfo->eventNumber();
						at[TRT::Track::trackNumber]=m_ntrk;
						ATH_MSG_DEBUG( "  Track " << m_ntrk << " accepted Info: run="
						               << at[TRT::Track::run] << "   event=" << at[TRT::Track::event]);
						for (unsigned int j=0;j<m_TrackInfoTools.size();j++)
							if (!m_TrackInfoTools[j]->fill(aTrack, &at, comTime.ptr(), *EventInfo, *vertices)) break;
						if(m_dorefit)
							delete aTrack;
					}
				}
			}
		}
	}
	//}

	//}
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode TRTCalibrationMgr::finalize()
{
  ATH_MSG_INFO( "CALIBSTAT CM_TRKS: " << m_ntrk);

	gROOT->SetMustClean(false);

	// Accumulators to finalize
	std::vector<IdentifierProfileHistogram*> histograms;
	for (unsigned int j=0;j<m_TrackInfoTools.size();j++)
		if ((m_TrackInfoTools[j]->finalize()).isFailure()){
		  ATH_MSG_FATAL( "Error calling TrackInfo tool finalize ");
			return StatusCode::FAILURE;
		}

	return StatusCode::SUCCESS;
}

StatusCode TRTCalibrationMgr::streamOutCalibObjects()
{
  ATH_MSG_INFO( "entering streamOutCalibObjects " );
  StatusCode sc = m_streamer->connectOutput();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not connect stream to output");
    return( StatusCode::FAILURE);
  }
  
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawT0Container::classname(),m_par_t0containerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),m_par_rtcontainerkey)) ;
  
  sc = m_streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not stream out Containers ");
    return( StatusCode::FAILURE);
  }
  
  sc = m_streamer->commitOutput();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not commit output stream");
    return( StatusCode::FAILURE);
  }
  
  ATH_MSG_INFO( "   Streamed out and committed "  << typeKeys.size() << " objects " );
  return StatusCode::SUCCESS;
}

