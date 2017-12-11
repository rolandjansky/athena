/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTStrawEfficiency.cxx
// author: Ryan D. Reece <ryan.reece@cern.ch>
// created: Nov 2009

#ifdef CPU_PROFILER
#include <google/profiler.h>
#endif

#include "TRT_TrackHoleSearch/TRTStrawEfficiency.h"
#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"

//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "StoreGate/ReadHandle.h"

#include <cmath> // for fabs
#include <vector>
#include <string>

#include "TTree.h"

TRTStrawEfficiency::TRTStrawEfficiency(const std::string& name, ISvcLocator* pSvcLocator)
	: AthAlgorithm(name, pSvcLocator),
	  m_trt_hole_finder("TRTTrackHoleSearchTool"),
	  m_hist_svc("THistSvc", name),
	  m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", name),
	  m_tree(0),
	  m_TRT_ID(0),
	  m_updator("Trk::KalmanUpdator/TrkKalmanUpdator"),
	  m_trigDec( "Trig::ITrigDecisionTool/TrigDecisionTool"),
	  m_num_events(0),
	  m_num_tracks(0),
	  m_num_preselected_tracks(0),
	  m_event_number(0),
	  m_run_number(0),
	  m_lumi_block(0),
	  m_track_pt(0.),
	  m_track_eta(0.),
	  m_track_phi(0.),
	  m_track_d0(0.),
	  m_track_z0(0.),
	  m_n_pixel_hits(0),
	  m_n_sct_hits(0),
	  m_n_trt_hits(0),
	  m_n_pixel_holes(0),
	  m_n_sct_holes(0),
	  m_n_trt_holes(0),
	  m_n_primary_vertex(0),
	  m_n_tube_hits(0)
{
	declareProperty("trt_hole_finder",          m_trt_hole_finder);
	declareProperty("max_abs_d0",               m_max_abs_d0 =          600*CLHEP::mm );
	declareProperty("max_abs_z0",               m_max_abs_z0 =          600*CLHEP::mm );
	declareProperty("min_pT",                   m_min_pT =              1.0*CLHEP::GeV );
	declareProperty("min_p",                    m_min_p =               2.0*CLHEP::GeV ); // added by dan
	declareProperty("max_abs_eta",              m_max_abs_eta =         2.5 );
	declareProperty("min_pixel_hits",           m_min_pixel_hits =      0 );
	declareProperty("min_sct_hits",             m_min_sct_hits =        2 );
	declareProperty("min_trt_hits",             m_min_trt_hits =        15 );
	declareProperty("hist_svc",                 m_hist_svc );
	declareProperty("straw_neighbour_svc",      m_TRTStrawNeighbourSvc );
	declareProperty("tree_name",                m_tree_name =           "trt_eff" );
	declareProperty("stream_name",              m_stream_name =         "TRTEffStream" );
	declareProperty("required_trigger",         m_required_trigger =    "" );
}


//____________________________________________________________________________
StatusCode TRTStrawEfficiency::initialize() {
	ATH_MSG_DEBUG( "TRTStrawEfficiency::initialize()" );

	// retrieve TRTTrackHoleSearchTool
	ATH_CHECK(m_trt_hole_finder.retrieve());

	// retrieve TRT_ID
	ATH_CHECK(detStore()->retrieve(m_TRT_ID, "TRT_ID"));

	ATH_CHECK(m_TRTStrawNeighbourSvc.retrieve());

	// retrieve the Trk::KalmanUpdator to calculate unbiased track states
	ATH_CHECK(m_updator.retrieve());

	// retrieve THistSvc
	ATH_CHECK(m_hist_svc.retrieve());

	// retrieve TrigDecisionTool
	if (!m_required_trigger.empty()) {
		ATH_CHECK(m_trigDec.retrieve());
	}

	// ntuple
	m_tree = new TTree(m_tree_name.c_str(), m_tree_name.c_str());
	ATH_CHECK(m_hist_svc->regTree(std::string("/") + m_stream_name + std::string("/") + m_tree_name, m_tree));

	// Read handles
	ATH_CHECK(m_tracksKey.initialize());
	ATH_CHECK(m_eventInfoKey.initialize());
	ATH_CHECK(m_vertexContainerKey.initialize());

	make_branches();

#ifdef CPU_PROFILER
	std::string profile_file = "cpu.prof";
	ATH_MSG_ALWAYS( "ProfilerStart(" << profile_file << ")" );
	ProfilerStart(profile_file.c_str());
#endif

	return StatusCode::SUCCESS;
}


//____________________________________________________________________________
StatusCode TRTStrawEfficiency::execute() {
	ATH_MSG_DEBUG( "TRTStrawEfficiency::execute()  event: " << m_num_events );

	//    ATH_MSG_DEUBG( "getListOfTriggers: " << m_trigDec->getListOfTriggers() );

	// require trigger
	if ( !m_required_trigger.empty() ) {
		if ( !m_trigDec->isPassed(m_required_trigger) ) {
			// skip event
			ATH_MSG_DEBUG( "  trigger: " << m_required_trigger << " failed.  Skipping event.");
			return StatusCode::SUCCESS;
		} else {
			ATH_MSG_DEBUG( "  trigger: " << m_required_trigger << " passed.");
		}
	}

	// retrive tracks
	SG::ReadHandle<TrackCollection> tracks(m_tracksKey);
	if (!tracks.isValid()) {
		ATH_MSG_FATAL( "Failed to retrieve " << m_tracksKey.key() );
		return StatusCode::FAILURE;
	}

	// retrieve event info
	SG::ReadHandle<xAOD::EventInfo> event_info(m_eventInfoKey);
	if (!event_info.isValid()) {
		ATH_MSG_FATAL( "Failed to retrieve " << m_eventInfoKey.key() );
		return StatusCode::FAILURE;
	}

	m_event_number = event_info->eventNumber();
	m_run_number = event_info->runNumber();
	m_lumi_block = event_info->lumiBlock();

	ATH_MSG_DEBUG( "run_number = " << m_run_number << ", lumi_block = " << m_lumi_block << ", event_number = " << m_event_number );

	// loop over tracks
	ATH_MSG_DEBUG( "This event has " << tracks->size() << " tracks." );
	for (auto track : *tracks) {
		m_num_tracks++;

		// clear branches
		clear_branches();

		// get perigee
		const Trk::Perigee* perigee = track->perigeeParameters();
		if (perigee) {
			m_track_pt = perigee->pT();
			m_track_eta = perigee->eta();
			m_track_phi = perigee->parameters()[Trk::phi0];
			m_track_d0 = perigee->parameters()[Trk::d0];
			m_track_z0 = perigee->parameters()[Trk::z0];
			ATH_MSG_DEBUG( "  This track has perigee parameters:\n"
			               << "                 pT     = " << m_track_pt/CLHEP::GeV << " CLHEP::GeV" << "\n"
			               << "                 eta =    " << m_track_eta << "\n"
			               << "                 phi0 =   " << m_track_phi << "\n"
			               << "                 d0 =     " << m_track_d0 << "\n"
			               << "                 z0 =     " << m_track_z0 << "\n"
			               << "                 theta =  " << perigee->parameters()[Trk::theta] << "\n"
			               << "                 qOverP = " << perigee->parameters()[Trk::qOverP] );
		} else {
			ATH_MSG_ERROR( "  This track has null perigeeParameters." );
			continue;
		}

		// get TrackStateOnSurfaces
		const DataVector<const Trk::TrackStateOnSurface>* track_states = track->trackStateOnSurfaces();
		if (track_states) {
			ATH_MSG_DEBUG( "  This track has " << track_states->size() << " track states on surface." );
		} else {
			ATH_MSG_ERROR( "  This track has null track states on surface." );
			continue;
		}

		// count hits
		for (auto trackState : *track_states) {
			if (trackState->type(Trk::TrackStateOnSurface::Measurement)) {
				if (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> (trackState->measurementOnTrack())) m_n_trt_hits++;
				else if (dynamic_cast<const InDet::SCT_ClusterOnTrack*> (trackState->measurementOnTrack())) m_n_sct_hits++;
				else if (dynamic_cast<const InDet::PixelClusterOnTrack*> (trackState->measurementOnTrack())) m_n_pixel_hits++;
			}
		}

		ATH_MSG_DEBUG( "  This track has\n"
		               << "        # Pixel hits = " << m_n_pixel_hits << "\n"
		               << "        # SCT hits =   " << m_n_sct_hits << "\n"
		               << "        # TRT hits =   " << m_n_trt_hits );

		// preselect tracks
		bool passed_track_preselection =
			( fabs(perigee->parameters()[Trk::d0]) < m_max_abs_d0 )&&
			( fabs(perigee->parameters()[Trk::z0]) < m_max_abs_z0 )&&
			( 1/fabs(perigee->parameters()[Trk::qOverP]) > m_min_p )&& // added by dan
			( perigee->pT() > m_min_pT )&&
			( fabs(perigee->eta()) < m_max_abs_eta )&&
			( m_n_pixel_hits >= m_min_pixel_hits)&&
			( m_n_sct_hits >= m_min_sct_hits)&&
			( m_n_trt_hits >= m_min_trt_hits);

		if (!passed_track_preselection) {
			ATH_MSG_DEBUG( "  This track failed preselection." );
			continue;
		}

		ATH_MSG_DEBUG( "  This track passed preselection." );
		m_num_preselected_tracks++;

		for (auto trackState : *track_states ) {
			if (trackState->type(Trk::TrackStateOnSurface::Measurement)) {
				fill_hit_data(*trackState);
			}
		}

		const DataVector<const Trk::TrackStateOnSurface>* holes = m_trt_hole_finder->getHolesOnTrack(*track);
		if (!holes) {
			ATH_MSG_WARNING( "  TRTTrackHoleSearchTool returned null results." );
			continue;
		} else {
			m_n_pixel_holes = 0;
			m_n_sct_holes = 0;
			m_n_trt_holes = 0;
			for (auto hole : *holes) {
				if (hole->type(Trk::TrackStateOnSurface::Hole)) {
					int hole_det = fill_hole_data(*hole);
					switch (hole_det) {
					case 1:
						m_n_pixel_holes++;
						break;
					case 2:
						m_n_sct_holes++;
						break;
					case 3:
						m_n_trt_holes++;
						break;
					default:
						ATH_MSG_ERROR("fill_hole_data returned an invalid det type: " << hole_det);
					}
				} else {
					ATH_MSG_WARNING("  hole finder returned a TrackStateOnSurface not of type Hole.");
				}
			}
			delete holes;
		}

		//------- added by dan -------

		SG::ReadHandle<xAOD::VertexContainer> vxContainer(m_vertexContainerKey);

		if (!vxContainer.isValid()) {
			ATH_MSG_ERROR("  Failed to retrieve VxContainer: " << m_vertexContainerKey.key());
			return StatusCode::FAILURE;
		}
		m_n_primary_vertex = vxContainer->size() - 1;
		//-----------------------------


		m_tree->Fill();

		ATH_MSG_DEBUG( "  This track has\n"
		               << "        # Pixel holes = " << m_n_pixel_holes << "\n"
		               << "        # SCT holes =   " << m_n_sct_holes << "\n"
		               << "        # TRT holes =   " << m_n_trt_holes );

	} // end loop over tracks

	m_num_events++;
	return StatusCode::SUCCESS;
}


//____________________________________________________________________________
StatusCode TRTStrawEfficiency::finalize() {
	ATH_MSG_DEBUG( "TRTStrawEfficiency::finalize()" );
	ATH_MSG_INFO( "# tracks = " << m_num_tracks );
	ATH_MSG_INFO( "# preselected tracks = " << m_num_preselected_tracks );

#ifdef ANP_CPU_PROFILER
	ATH_MSG_ALWAYS("ProfilerStop.");
	ProfilerStop();
#endif

	return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------
// private methods
//----------------------------------------------------------------------------

//____________________________________________________________________________
void TRTStrawEfficiency::make_branches() {
	m_tree->Branch("event_number", &m_event_number, "event_number/i");
	m_tree->Branch("run_number", &m_run_number, "run_number/i");
	m_tree->Branch("lumi_block", &m_lumi_block, "lumi_block/i");
	m_tree->Branch("track_pt", &m_track_pt, "track_pt/F");
	m_tree->Branch("track_eta", &m_track_eta, "track_eta/F");
	m_tree->Branch("track_phi", &m_track_phi, "track_phi/F");
	m_tree->Branch("track_d0", &m_track_d0, "track_d0/F");
	m_tree->Branch("track_z0", &m_track_z0, "track_z0/F");
	m_tree->Branch("n_pixel_hits", &m_n_pixel_hits, "n_pixel_hits/I");
	m_tree->Branch("n_sct_hits", &m_n_sct_hits, "n_sct_hits/I");
	m_tree->Branch("n_trt_hits", &m_n_trt_hits, "n_trt_hits/I");
	m_tree->Branch("hit_bec", &m_hit_bec);
	m_tree->Branch("hit_phi", &m_hit_phi);
	m_tree->Branch("hit_layer", &m_hit_layer);
	m_tree->Branch("hit_strawlayer", &m_hit_strawlayer);
	m_tree->Branch("hit_straw", &m_hit_straw);
	m_tree->Branch("hit_chip", &m_hit_chip);
	m_tree->Branch("hit_pad", &m_hit_pad);
	m_tree->Branch("hit_x", &m_hit_x);
	m_tree->Branch("hit_y", &m_hit_y);
	m_tree->Branch("hit_z", &m_hit_z);
	m_tree->Branch("hit_center_x", &m_hit_center_x);
	m_tree->Branch("hit_center_y", &m_hit_center_y);
	m_tree->Branch("hit_center_z", &m_hit_center_z);
	m_tree->Branch("hit_R", &m_hit_R);
	m_tree->Branch("hit_locR", &m_hit_locR);
	m_tree->Branch("hit_HL", &m_hit_HL);
	m_tree->Branch("hit_det", &m_hit_det);
	m_tree->Branch("hit_ub_locR", &m_hit_ub_locR);
	m_tree->Branch("hit_ub_x", &m_hit_ub_x);
	m_tree->Branch("hit_ub_y", &m_hit_ub_y);
	m_tree->Branch("hit_ub_z", &m_hit_ub_z);
	m_tree->Branch("n_pixel_holes", &m_n_pixel_holes, "n_pixel_holes/I");
	m_tree->Branch("n_sct_holes", &m_n_sct_holes, "n_sct_holes/I");
	m_tree->Branch("n_trt_holes", &m_n_trt_holes, "n_trt_holes/I");
	m_tree->Branch("hole_bec", &m_hole_bec);
	m_tree->Branch("hole_phi", &m_hole_phi);
	m_tree->Branch("hole_layer", &m_hole_layer);
	m_tree->Branch("hole_strawlayer", &m_hole_strawlayer);
	m_tree->Branch("hole_straw", &m_hole_straw);
	m_tree->Branch("hole_chip", &m_hole_chip);
	m_tree->Branch("hole_pad", &m_hole_pad);
	m_tree->Branch("hole_x", &m_hole_x);
	m_tree->Branch("hole_y", &m_hole_y);
	m_tree->Branch("hole_z", &m_hole_z);
	m_tree->Branch("hole_center_x", &m_hole_center_x);
	m_tree->Branch("hole_center_y", &m_hole_center_y);
	m_tree->Branch("hole_center_z", &m_hole_center_z);
	m_tree->Branch("hole_locR", &m_hole_locR);
	m_tree->Branch("hole_locR_error", &m_hole_locR_error);
	m_tree->Branch("hole_det", &m_hole_det);

	// ---- branches added by dan ------
	m_tree->Branch("n_primary_vertex", &m_n_primary_vertex, "n_primary_vertex/I");
	m_tree->Branch("hit_tube_hit", &m_hit_tube_hit);
	m_tree->Branch("n_tube_hits", &m_n_tube_hits, "n_tube_hits/I");
	// -------------------------------
}


//____________________________________________________________________________
void TRTStrawEfficiency::clear_branches() {
	m_event_number = 0;
	m_run_number = 0;
	m_lumi_block = 0;
	m_track_pt = 0.0;
	m_track_eta = 0.0;
	m_track_phi = 0.0;
	m_track_d0 = 0.0;
	m_track_z0 = 0.0;
	m_n_pixel_hits = 0;
	m_n_sct_hits = 0;
	m_n_trt_hits = 0;
	m_hit_bec.clear();
	m_hit_phi.clear();
	m_hit_layer.clear();
	m_hit_strawlayer.clear();
	m_hit_straw.clear();
	m_hit_chip.clear();
	m_hit_pad.clear();
	m_hit_x.clear();
	m_hit_y.clear();
	m_hit_z.clear();
	m_hit_center_x.clear();
	m_hit_center_y.clear();
	m_hit_center_z.clear();
	m_hit_R.clear();
	m_hit_locR.clear();
	m_hit_HL.clear();
	m_hit_det.clear();
	m_hit_ub_locR.clear();
	m_hit_ub_x.clear();
	m_hit_ub_y.clear();
	m_hit_ub_z.clear();
	m_n_pixel_holes = 0;
	m_n_sct_holes = 0;
	m_n_trt_holes = 0;
	m_hole_bec.clear();
	m_hole_phi.clear();
	m_hole_layer.clear();
	m_hole_strawlayer.clear();
	m_hole_straw.clear();
	m_hole_chip.clear();
	m_hole_pad.clear();
	m_hole_x.clear();
	m_hole_y.clear();
	m_hole_z.clear();
	m_hole_center_x.clear();
	m_hole_center_y.clear();
	m_hole_center_z.clear();
	m_hole_locR.clear();
	m_hole_locR_error.clear();
	m_hole_det.clear();

	m_n_primary_vertex = 0;  // added by dan
	m_hit_tube_hit.clear();  // added by dan
	m_n_tube_hits = 0;       // added by dan
}


//____________________________________________________________________________
int TRTStrawEfficiency::fill_hit_data(const Trk::TrackStateOnSurface& hit) {
	const Trk::TrackParameters* track_parameters = hit.trackParameters();
	if (!track_parameters) {
		ATH_MSG_ERROR("fill_hit_data(hit): null track_parameters");
		return 0;
	}

	m_hit_x.push_back( track_parameters->position().x() );
	m_hit_y.push_back( track_parameters->position().y() );
	m_hit_z.push_back( track_parameters->position().z() );

	m_hit_center_x.push_back( track_parameters->associatedSurface().center().x() );
	m_hit_center_y.push_back( track_parameters->associatedSurface().center().y() );
	m_hit_center_z.push_back( track_parameters->associatedSurface().center().z() );

	Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();

	int det = 0;
	if(m_TRT_ID->is_pixel(id)) {
		det = 1;
	}	else if(m_TRT_ID->is_sct(id)) {
		det = 2;
	}	else if(m_TRT_ID->is_trt(id)) {
		det = 3;
	}
	m_hit_det.push_back( det );

	m_hit_bec.push_back( det == 3 ?         m_TRT_ID->barrel_ec(id) : 0 );
	m_hit_phi.push_back( det == 3 ?         m_TRT_ID->phi_module(id) : -1 );
	m_hit_layer.push_back( det == 3 ?       m_TRT_ID->layer_or_wheel(id) : -1 );
	m_hit_strawlayer.push_back( det == 3 ?  m_TRT_ID->straw_layer(id) : -1 );
	m_hit_straw.push_back( det == 3 ?       m_TRT_ID->straw(id) : -1 );

	int chip = -1;
	if(det == 3) {
		m_TRTStrawNeighbourSvc->getChip(id, chip);
	}
	m_hit_chip.push_back(chip);

	int pad = -1;
	if(det == 3) {
		m_TRTStrawNeighbourSvc->getPad(id, pad);
	}
	m_hit_pad.push_back(pad);

	m_hit_locR.push_back( det == 3 ? track_parameters->parameters()[Trk::locR] : -1 );

	const Trk::MeasurementBase* measurement = hit.measurementOnTrack();
	const InDet::TRT_DriftCircleOnTrack* trtcircle = 0;
	if(measurement) {
		trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> (measurement);
	}	else {
		ATH_MSG_ERROR("fill_hit_data(hit): null measurement");
	}

	if(!trtcircle) {
		ATH_MSG_DEBUG("fill_hit_data(hit): null trtcircle");
	}

	m_hit_R.push_back( (det == 3)&&(trtcircle != 0) ? trtcircle->localParameters()[Trk::driftRadius] : -1 );
	m_hit_HL.push_back( (det == 3)&&(trtcircle != 0) ? trtcircle->highLevel() : -1 );

	// unbiased trk parameters
	const Trk::TrackParameters* unbiased_track_parameters = nullptr;
	unbiased_track_parameters = m_updator->removeFromState(*(hit.trackParameters()),
	                                                       hit.measurementOnTrack()->localParameters(),
	                                                       hit.measurementOnTrack()->localCovariance());

	m_hit_ub_locR.push_back( det == 3 && unbiased_track_parameters ? unbiased_track_parameters->parameters()[Trk::locR] : -1 );
	m_hit_ub_x.push_back( unbiased_track_parameters ? unbiased_track_parameters->position().x() : -1 );
	m_hit_ub_y.push_back( unbiased_track_parameters ? unbiased_track_parameters->position().y() : -1 );
	m_hit_ub_z.push_back( unbiased_track_parameters ? unbiased_track_parameters->position().z() : -1 );
	delete unbiased_track_parameters;

	// ------- added by dan -------
	int is_tube_hit = -1;
	if (measurement && (det == 3) ) {
		is_tube_hit = ((measurement->localCovariance())(Trk::locX,Trk::locX) > 1.0)? 1 : 0;
		if (is_tube_hit) m_n_tube_hits++;
	}

	m_hit_tube_hit.push_back( (det == 3)&&(measurement != 0) ? is_tube_hit : -1);

	// ----------------------------


	return det;
}


//____________________________________________________________________________
int TRTStrawEfficiency::fill_hole_data(const Trk::TrackStateOnSurface& hole) {
	const Trk::TrackParameters* track_parameters = hole.trackParameters();
	if (!track_parameters) {
		ATH_MSG_ERROR("fill_hole_data(hole): null track_parameters");
		return 0;
	}

	m_hole_x.push_back( track_parameters->position().x() );
	m_hole_y.push_back( track_parameters->position().y() );
	m_hole_z.push_back( track_parameters->position().z() );

	m_hole_center_x.push_back( track_parameters->associatedSurface().center().x() );
	m_hole_center_y.push_back( track_parameters->associatedSurface().center().y() );
	m_hole_center_z.push_back( track_parameters->associatedSurface().center().z() );

	Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();

	int det = 0;
	if(m_TRT_ID->is_pixel(id)) {
		det = 1;
	}	else if(m_TRT_ID->is_sct(id)) {
		det = 2;
	}	else if(m_TRT_ID->is_trt(id)) {
		det = 3;
	}
	m_hole_det.push_back( det );

	m_hole_bec.push_back( det == 3 ? m_TRT_ID->barrel_ec(id) : -1 );
	m_hole_phi.push_back( det == 3 ? m_TRT_ID->phi_module(id) : -1 );
	m_hole_layer.push_back( det == 3 ? m_TRT_ID->layer_or_wheel(id) : -1 );
	m_hole_strawlayer.push_back( det == 3 ? m_TRT_ID->straw_layer(id) : -1 );
	m_hole_straw.push_back( det == 3 ? m_TRT_ID->straw(id) : -1 );

	int chip = -1;
	if (det == 3) {
		m_TRTStrawNeighbourSvc->getChip(id, chip);
	}
	m_hole_chip.push_back(chip);

	int pad = -1;
	if (det == 3) {
		m_TRTStrawNeighbourSvc->getPad(id, pad);
	}
	m_hole_pad.push_back(pad);

	float locR = track_parameters->parameters()[Trk::locR];
	m_hole_locR.push_back( det == 3 ? locR : -1 );

	float locR_error = 0.0;
	//const Trk::MeasuredTrackParameters* meas = dynamic_cast< const Trk::MeasuredTrackParameters* >(track_parameters);
	//if(meas)
	//{

	const AmgSymMatrix(5)* merr = track_parameters->covariance();
	if(merr){
		locR_error = Amg::error(*merr,Trk::locR);
	}	else {
		ATH_MSG_ERROR("Track parameters have no covariance attached.");
	}
	m_hole_locR_error.push_back( det == 3 ? locR_error : 0.0 );

	/*
	  ATH_MSG_DEBUG("hole trk x = " << track_parameters->position().x() <<
	  ", surf x = " << track_parameters->associatedSurface()->center().x());
	  ATH_MSG_DEBUG("hole trk y = " << track_parameters->position().y() <<
	  ", surf y = " << track_parameters->associatedSurface()->center().y());
	  ATH_MSG_DEBUG("hole trk z = " << track_parameters->position().z() <<
	  ", surf z = " << track_parameters->associatedSurface()->center().z());
	*/

	return det;
}

