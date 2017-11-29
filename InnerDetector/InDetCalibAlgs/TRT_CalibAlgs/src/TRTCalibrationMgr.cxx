/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************

   TRTCalibrationMgr.cxx : Manager for TRT calibration

   * ***************************************************************** */

#include "TRT_CalibAlgs/TRTCalibrationMgr.h"

#include "TrkTrack/Track.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

#include "TRT_CalibData/TrackInfo.h"
#include "TRT_ConditionsData/FloatArrayStore.h"
#include "TRT_CalibTools/IFillAlignTrkInfo.h"
#include "TRT_CalibTools/ITRTCalibrator.h"
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
	m_trtcaldbSvc("TRT_CalDbSvc", name),
	m_dorefit(true),
	m_docalibrate(false),
	m_writeConstants(false),
	m_ntrk(0),
	m_trackSelector("InDet::InDetTrackSelectorTool/InDetTrackSelectorTool"),
	m_max_ntrk(100000)
{
	m_TrackInfoTools.push_back("FillAlignTrkInfo");
	m_AccumulatorTools.push_back("TRTCalAccumulator");
	m_TRTCalibTools.push_back("TRTCalibrator");
	m_FitTools.push_back("FitTool");
	// declare algorithm parameters
	declareProperty("TRTCalDBTool", m_trtcaldbSvc);
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
	msg(MSG::INFO) << "initialize()" << endmsg;

	if (m_docalibrate) {
		if( !m_TRTCalibTools.size() || m_TRTCalibTools.retrieve().isFailure()){
			msg(MSG::FATAL) << "Cannot get Calibration tool " << m_TRTCalibTools << endmsg;
			return StatusCode::FAILURE;
		}
	} else {
		if( !m_TrackInfoTools.size() || m_TrackInfoTools.retrieve().isFailure()){
			msg(MSG::FATAL) << "Cannot get TrackInfo filler tool " << m_TrackInfoTools << endmsg;
			return StatusCode::FAILURE;
		}
	}

	if( !m_FitTools.size() || m_FitTools.retrieve().isFailure()){
		msg(MSG::FATAL) << "Cannot get Fit tools " << m_FitTools << endmsg;
		return StatusCode::FAILURE;
	}

	if(m_trackFitter.retrieve().isFailure()){
		msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackFitter << endmsg;
		return StatusCode::FAILURE;
	}

	//Initialize ReadHandles and ReadHandleKeys
	ATH_CHECK(m_verticesKey.initialize());
	ATH_CHECK(m_EventInfoKey.initialize());
	ATH_CHECK(m_TrkCollections.initialize());
	ATH_CHECK(m_comTimeKey.initialize());
	// Each ROI/road may create its own collection....
	msg(MSG::INFO) << "Tracks from Trk::Track collection(s):";
	for (unsigned int i=0;i<m_TrkCollections.size();i++)
		msg(MSG::INFO) << "\n\t" << m_TrkCollections[i];
	msg(MSG::INFO) << endmsg;

	// Get the Track Selector Tool
	if ( !m_trackSelector.empty() ) {
		StatusCode sc = m_trackSelector.retrieve();
		if (sc.isFailure()) {
			msg(MSG::FATAL) << "Could not retrieve "<< m_trackSelector <<" (to select the tracks which are written to the ntuple) "<< endmsg;
			msg(MSG::INFO) << "Set the ToolHandle to None if track selection is supposed to be disabled" << endmsg;
			return sc;
		}
	}

	msg(MSG::INFO) <<"Track Selector retrieved" << endmsg;


	m_ntrk=0;

	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------

StatusCode TRTCalibrationMgr::execute() {

	if (m_docalibrate){
		msg(MSG::INFO) << "skipping execute() calibrating instead" << endmsg;
		m_TRTCalibTools[0]->calibrate();
		return StatusCode::SUCCESS;
	}

	if(m_writeConstants){
		StatusCode sc=m_trtcaldbSvc->streamOutCalibObjects() ;
		return sc;
	}

	// require at least one vertex per event
	/*
	  const VxContainer* vxContainer(0);
	  StatusCode sc = evtStore()->retrieve(vxContainer,"VxPrimaryCandidate");
	  if ( sc.isFailure() ) { msg(MSG::ERROR) << "vertex container missing!" << endmsg; }
	  else {
	  int countVertices(0);
	  for (VxContainer::const_iterator it = vxContainer->begin() ; it != vxContainer->end() ; ++it ) {
	  if ( (*it)->vxTrackAtVertex()->size() >= 3 ) countVertices++;
	  }
	  if (countVertices < 1) {msg(MSG::INFO) << "no vertices found" << endmsg;}// return sc;}
	  }
	*/

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
		msg(MSG::INFO) << "no vertices found" << endmsg;
		return StatusCode::SUCCESS;
	}

	// get event info pointer
	SG::ReadHandle<xAOD::EventInfo> m_EventInfo(m_EventInfoKey);
	if (not m_EventInfo.isValid()) {
		msg(MSG::FATAL) << "skipping event, could not get EventInfo" << endmsg;
		return StatusCode::FAILURE;
	}

	// ComTime can be missing from files not returning failure here
	SG::ReadHandle<ComTime> comTime(m_comTimeKey);
	if (not comTime.isValid()) {
		ATH_MSG_INFO("Could not find ComTime object named " << m_comTimeKey.key());
	}

	// Loop over tracks; get track info and accumulate it
	const Trk::Track* aTrack;
	//const DataVector<Trk::Track>* trks;

	TrackCollection::const_iterator t;

	for (SG::ReadHandle<TrackCollection>& trks : m_TrkCollections.makeHandles()) {
		// retrieve all tracks from TDS

		if (trks.isValid()){
			//      if (trks->size()>100){

			if(trks->size()<3) {
				msg(MSG::INFO) << "skipping event, it contains only " << trks->size() << " tracks (less than 3)" << endmsg;
				return StatusCode::SUCCESS;
			}


			if(trks->size()>m_max_ntrk) {
				msg(MSG::INFO) << "skipping event, it contains " << trks->size() << " tracks, more than max: " << m_max_ntrk  << endmsg;
				return StatusCode::SUCCESS;
			}

			for (t=trks->begin();t!=trks->end();++t) {
				//msg(MSG::INFO) << "Tracks seees "  << endmsg;
				if ( m_trackSelector->decision(*(*t), 0)) {
					//msg(MSG::INFO) << "Tracks ACCEPTED "  << endmsg;
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
						at[TRT::Track::run]=m_EventInfo->runNumber();
						at[TRT::Track::event]=m_EventInfo->eventNumber();
						at[TRT::Track::trackNumber]=m_ntrk;
						if (msgLvl(MSG::DEBUG)) msg() << "  Track " << m_ntrk << " accepted Info: run="
						                              << at[TRT::Track::run] << "   event=" << at[TRT::Track::event] << endmsg;
						for (unsigned int j=0;j<m_TrackInfoTools.size();j++)
							if (!m_TrackInfoTools[j]->fill(aTrack, &at, comTime.ptr(), *m_EventInfo, *vertices)) break;
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
	msg(MSG::INFO) << "finalise()" << endmsg;
	std::cout << "CALIBSTAT CM_TRKS: " << m_ntrk << std::endl;

	//argh this is insane ...
	gROOT->SetMustClean(false);

	// Accumulators to finalize
	std::vector<IdentifierProfileHistogram*> histograms;
	for (unsigned int j=0;j<m_TrackInfoTools.size();j++)
		if ((m_TrackInfoTools[j]->finalize()).isFailure()){
			msg(MSG::FATAL) << "Error calling TrackInfo tool finalize " << endmsg;
			return StatusCode::FAILURE;
		}

	return StatusCode::SUCCESS;
}
