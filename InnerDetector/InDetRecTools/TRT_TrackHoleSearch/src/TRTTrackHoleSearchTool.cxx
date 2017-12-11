/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTTrackHoleSearchTool.cxx
// author: Ryan D. Reece <ryan.reece@cern.ch>
// created: Nov 2009

#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"

// STD
#include <bitset>
#include <cstdlib> // abs
#include <cmath> // fabs
#include <fstream>

// athena
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/Surface.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


//____________________________________________________________________________
TRTTrackHoleSearchTool::TRTTrackHoleSearchTool(const std::string& type, const std::string& name, const IInterface* parent)
	:   AthAlgTool(type, name, parent),
	    m_extrapolator("Trk::Extrapolator"),
	    m_conditions_svc("TRT_ConditionsSummarySvc", name),
	    m_has_been_called(false),
	    m_TRT_ID(0),
	    m_trt_outer_surf(0)
{
	declareInterface<ITrackHoleSearchTool>(this);

	declareProperty("extrapolator",             m_extrapolator );
	declareProperty("conditions_svc",           m_conditions_svc);
	declareProperty("use_conditions_svc",       m_use_conditions_svc =  true);
	declareProperty("outer_radius",             m_outer_radius =        1075.0*CLHEP::mm );  // barrel 1075.0, EC 1010.0
	declareProperty("max_z",                    m_max_z =               2715.0*CLHEP::mm );  // barrel 715.0, EC 2715.0
	declareProperty("max_trailing_holes",       m_max_trailing_holes =  1 );   // only used if not end_at_last_trt_hit
	declareProperty("begin_at_first_trt_hit",   m_begin_at_first_trt_hit = false ); // if not, extrapolate from last Si hit
	declareProperty("end_at_last_trt_hit",      m_end_at_last_trt_hit = false ); // if not, continue hole search to the edge of the TRT
	declareProperty("bcheck",                   m_bcheck =              false );
	declareProperty("do_dump_bad_straw_log",    m_do_dump_bad_straw_log = false );
	declareProperty("locR_cut",                 m_locR_cut =            -1.0 );   // 1.4*CLHEP::mm // negative means no cut
	declareProperty("locR_sigma_cut",           m_locR_sigma_cut =      -1.0 );
	declareProperty("locZ_cut",                 m_locZ_cut =            5.0*CLHEP::mm ); // 5.0*CLHEP::mm );
}


//____________________________________________________________________________
StatusCode TRTTrackHoleSearchTool::initialize() {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::initialize()" );

	// retrieve extrapolator
	if ( m_extrapolator.retrieve().isFailure() ) {
		ATH_MSG_FATAL( "Failed to retrieve the extrapolator." );
		return StatusCode::FAILURE;
	}

	// retrieve TRT_ID
	if( detStore()->retrieve(m_TRT_ID, "TRT_ID").isFailure() ) {
		ATH_MSG_FATAL( "Failed to retrieve TRT_ID." );
		return StatusCode::FAILURE;
	}

	// retrieve ConditionsSummarySvc
	if( m_conditions_svc.retrieve().isFailure() ) {
		ATH_MSG_FATAL( "Failed to retrieve ConditionsSummarySvc." );
		return StatusCode::FAILURE;
	}

	m_trt_outer_surf = new Trk::CylinderSurface(new Amg::Transform3D, m_outer_radius, m_max_z);
	// note: HepGeom::Translate3D is deleted by Trk::Surface destructor

	return StatusCode::SUCCESS;
}


//____________________________________________________________________________
StatusCode TRTTrackHoleSearchTool::finalize() {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::finalize()" );
	delete m_trt_outer_surf;
	return StatusCode::SUCCESS;
}


//____________________________________________________________________________
void TRTTrackHoleSearchTool::countHoles(const Trk::Track& track,
                                        std::vector<int>& information ,
                                        const Trk::ParticleHypothesis partHyp) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::countHoles" );
	const DataVector<const Trk::TrackStateOnSurface>* holes = getHolesOnTrack(track, partHyp);
	if (holes) {
		information[Trk::numberOfTRTHoles] = holes->size();
		delete holes;
	}	else {
		information[Trk::numberOfTRTHoles] = -1;
	}
}


//____________________________________________________________________________
const DataVector<const Trk::TrackStateOnSurface>* TRTTrackHoleSearchTool::getHolesOnTrack(
	                    const Trk::Track& track,
	                    const Trk::ParticleHypothesis partHyp) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::getHolesOnTrack" );

	// write out list of bad straws for debugging purposes.
	// only dump log on first call of this function.
	if(m_do_dump_bad_straw_log && !m_has_been_called && m_use_conditions_svc) {
		dump_bad_straw_log();
	}
	m_has_been_called = true;

	if( track.perigeeParameters() ) {
		ATH_MSG_VERBOSE( "  This track has perigee: \n"
		                 << "    pT =  " << track.perigeeParameters()->pT()/CLHEP::GeV << " CLHEP::GeV\n"
		                 << "    eta = " << track.perigeeParameters()->eta() << "\n"
		                 << "    phi = " << track.perigeeParameters()->parameters()[Trk::phi0] << "\n"
		                 << "    d0 =  " << track.perigeeParameters()->parameters()[Trk::d0] << "\n"
		                 << "    z0 =  " << track.perigeeParameters()->parameters()[Trk::z0] );
	}

	// get TrackStateOnSurfaces
	const DataVector<const Trk::TrackStateOnSurface>* track_states = track.trackStateOnSurfaces();
	if(track_states) {
		ATH_MSG_DEBUG( "  This track has " << track_states->size() << " track states on surface." );
	} else {
		ATH_MSG_ERROR( "  This track has null track states on surface. Returning 0." );
		return 0;
	}

	if(track_states->size() < 2) {
		ATH_MSG_WARNING( "  Fewer than 2 TrackStatesOnSurface. Returning 0." );
		return 0;
	}

	// set beginning point of extrapolation
	DataVector<const Trk::TrackStateOnSurface>::const_iterator beginning_track_state;
	if(m_begin_at_first_trt_hit) {
		// begin at first TRT hit
		beginning_track_state = find_first_trt_hit(*track_states);
	} else {
		// begin at last Si hit
		beginning_track_state = find_last_hit_before_trt(*track_states);
		if(beginning_track_state == track_states->end()) {
			ATH_MSG_WARNING( "  beginning_track_state == track_states->end().  There must be no Si hits.\n"
			                 << "  Will try to begin at the first TRT hit." );
			beginning_track_state = find_first_trt_hit(*track_states);
		}
	}

	if(beginning_track_state == track_states->end()) {
		ATH_MSG_WARNING( "  beginning_track_state == track_states->end(). No where to extrapolate to. Returning 0." );
		return 0;
	}

	// to be returned:
	DataVector<const Trk::TrackStateOnSurface>* holes = new DataVector<const Trk::TrackStateOnSurface>;

	DataVector<const Trk::TrackStateOnSurface>::const_iterator track_state = beginning_track_state;
	const Trk::TrackParameters* start_parameters = (*track_state)->trackParameters();
	++track_state;
	if (!start_parameters) {
		ATH_MSG_VERBOSE("No start parameters");
	}
	// loop over TrackStateOnSurfaces (destination surfaces for extrapolation)
	for(; track_state != track_states->end(); ++track_state) {
		// skip track states that are not measurements
		if(!(*track_state)->type(Trk::TrackStateOnSurface::Measurement)) {
			ATH_MSG_VERBOSE( "    TrackStateOnSurface is not of type Trk::TrackStateOnSurface::Measurement." );
			continue;
		}

		const Trk::TrackParameters* end_parameters = (*track_state)->trackParameters();
		if(end_parameters) {
			ATH_MSG_VERBOSE( "    TrackStateOnSurface: (x, y, z) = (" 
			                 << end_parameters->position().x() << ", "
			                 << end_parameters->position().y() << ", "
			                 << end_parameters->position().z() << "); (rho, eta, phi) = (" 
			                 << end_parameters->position().perp() << ", "
			                 << end_parameters->position().eta() << ", "
			                 << end_parameters->position().phi() << ")");
		} else {
			ATH_MSG_ERROR( "    TrackStateOnSurface has no TrackParameters." );
			continue;
		}

		const Trk::Surface& end_surf = end_parameters->associatedSurface();

		extrapolateBetweenHits(start_parameters, end_surf, holes, partHyp);
		start_parameters = end_parameters;
	} // end loop over TrackStateOnSurfaces

	if( !m_end_at_last_trt_hit ) {
		// final extrapolation to the edge of TRT to check for holes in outermost straws
		int trailing_hole_count = extrapolateBetweenHits(start_parameters, *m_trt_outer_surf, holes, partHyp);

		// remove trailing holes
		if(trailing_hole_count > m_max_trailing_holes) {
			ATH_MSG_INFO("There are " << trailing_hole_count << " trailing holes removed.");
			for(int i=0; i < trailing_hole_count; i++) {
				holes->pop_back();
				/*
				  Note: DataVector::pop_back() deletes pointers if it owns them.
				  http://alxr.usatlas.bnl.gov/lxr-stb4/source/atlas/Control/DataModel/DataModel/DataVector.h#978
				*/
			}
		}
	}

	return holes;
}


//____________________________________________________________________________
const Trk::Track* TRTTrackHoleSearchTool::getTrackWithHoles(const Trk::Track& track,
                                                            const Trk::ParticleHypothesis partHyp) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::getTrackWithHoles" );
	const DataVector<const Trk::TrackStateOnSurface>* holes = getHolesOnTrack(track, partHyp);
	const Trk::Track* new_track = addHolesToTrack(track, holes);
	delete holes;
	return new_track;
}


//____________________________________________________________________________
const Trk::Track* TRTTrackHoleSearchTool::getTrackWithHolesAndOutliers(const Trk::Track& track, 
                                                                       const Trk::ParticleHypothesis partHyp) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::getTrackWithHolesAndOutliers" );
	return getTrackWithHoles(track, partHyp);
}

//----------------------------------------------------------------------------
// Private Methods
//----------------------------------------------------------------------------

//____________________________________________________________________________
int TRTTrackHoleSearchTool::extrapolateBetweenHits(const Trk::TrackParameters* start_parameters,
                                                   const Trk::Surface& end_surf,
                                                   DataVector<const Trk::TrackStateOnSurface>* holes,
                                                   const Trk::ParticleHypothesis partHyp/*=Trk::pion*/) const {
	int hole_count = 0;
	// initialize previous id
	const Trk::Surface& start_surf = start_parameters->associatedSurface();

	Identifier previous_id = start_surf.associatedDetectorElementIdentifier();

	// get end id
	const Identifier end_id = end_surf.associatedDetectorElementIdentifier();

	// look for holes
	const std::vector<const Trk::TrackParameters*>* steps = m_extrapolator->extrapolateStepwise(*start_parameters, end_surf, Trk::alongMomentum, m_bcheck, partHyp);

	if(!steps) {
		ATH_MSG_DEBUG("extrapolateBetweenHits: extrapolateStepwise returned null");
	} else {
		// loop over parameters from extrapolation
		// note: the last element in the vector is always the track parameters at the destination surface
		for(std::vector<const Trk::TrackParameters*>::const_iterator step = steps->begin(); step != steps->end()-1; ++step) {
			// check for surface
			const Trk::Surface& surf = (*step)->associatedSurface();
			
			// get id
			Identifier id = surf.associatedDetectorElementIdentifier();
			// check that the surface is a TRT straw
			if(!m_TRT_ID->is_trt(id)) {
				ATH_MSG_DEBUG("extrapolateBetweenHits: surf is not a TRT straw.  Skipping.");
				continue;
			}
			// check that we are not still on the previous id
			if(id == previous_id) {
				ATH_MSG_DEBUG("extrapolateBetweenHits: id == previous_id");
				continue;
			}
			// check that we haven't hit the destination surface
			if(id == end_id) {
				ATH_MSG_DEBUG("extrapolateBetweenHits: id == end_id");
				continue;
			}
			// ignore id 0xffffffff from m_trt_outer_surf
			if(id == 0xffffffff) {
				ATH_MSG_DEBUG("extrapolateBetweenHits: id == 0xffffffff. Skipping.");
				continue;
			}
			// don't count bad straws as holes
			if( (m_use_conditions_svc)&&(!m_conditions_svc->isGood(id)) ) {
				ATH_MSG_DEBUG("extrapolateBetweenHits: ConditionsSvc says this straw is bad. Skipping.");
				continue;
			}
			// check fiducial |locR| by sigma
			const float locR = (*step)->parameters()[Trk::locR];
			if(m_locR_sigma_cut > 0.0) {
				//const Trk::MeasuredTrackParameters* meas = dynamic_cast< const Trk::MeasuredTrackParameters* >(*step);
				//if(meas)
				//{

				const AmgSymMatrix(5)* merr = (*step)->covariance();

				if(merr){
					//const float locR_error = (*merr)(Trk::locR,Trk::locR);
					
					const float locR_error = Amg::error(*merr,Trk::locR);

					// hole must be within the straw by some tolerance
					if( (2.0*CLHEP::mm - fabs(locR))/locR_error < m_locR_sigma_cut ) {
						continue;
					}
				} else {
					ATH_MSG_WARNING("extrapolateBetweenHits: Track parameters failed to dynamic_cast< const Trk::MeasuredTrackParameters* >.");
					continue;
				}
			}
			// check fiducial |locR| by value
			if(m_locR_cut > 0.0) {
				if( fabs(locR) > m_locR_cut )
					continue;
			}
			// check fiducial |locZ| by value
			if(m_locZ_cut > 0.0) {
				const float locZ = (*step)->parameters()[Trk::locZ];
				const Trk::CylinderBounds* cylb = dynamic_cast<const Trk::CylinderBounds *>(&surf.bounds());
				if(cylb) {
					const float halfz=cylb->halflengthZ();
					if( fabs(locZ) > halfz - m_locZ_cut ) continue;
				} else {
					ATH_MSG_WARNING("extrapolateBetweenHits: Surface failed to dynamic_cast to Trk::CylinderBounds. Skipping.");
					continue;
				}
			}

			// if we've gotten here, it is a legitimate hole
			ATH_MSG_DEBUG("      HOLE Found! Identifier = " << id.getString() << " indicates:\n"
			              << "            is_pixel =       " << m_TRT_ID->is_pixel(id) << "\n"
			              << "            is_sct =         " << m_TRT_ID->is_sct(id) << "\n"
			              << "            is_trt =         " << m_TRT_ID->is_trt(id) << "\n"
			              << "            barrel_ec =      " << m_TRT_ID->barrel_ec(id) << "\n"
			              << "            phi_module =     " << m_TRT_ID->phi_module(id) << "\n"
			              << "            layer_or_wheel = " << m_TRT_ID->layer_or_wheel(id) << "\n"
			              << "            straw_layer =    " << m_TRT_ID->straw_layer(id) << "\n"
			              << "            straw =          " << m_TRT_ID->straw(id) << "\n"
			              << "            rho = " << (*step)->position().perp() );
			
			std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
			typePattern.set(Trk::TrackStateOnSurface::Hole);
			holes->push_back( new Trk::TrackStateOnSurface(0, (*step)->clone(), 0, 0, typePattern) );
			hole_count++;
			previous_id = id;
		} // end loop over parameters from extrapolation
		// clean up memory
		for(std::vector<const Trk::TrackParameters*>::const_iterator it = steps->begin(); it != steps->end(); ++it)
			delete (*it);
		delete steps;
		steps = 0;
	}
	return hole_count;
}


//____________________________________________________________________________
void TRTTrackHoleSearchTool::dump_bad_straw_log() const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::dump_bad_straw_log" );
	std::ofstream out("TRT_ConditionsSummarySvc_bad_straws.log");
	out << "# id  barrel_ec  phi_module  layer_or_wheel  straw_layer  straw" << std::endl;
	for(std::vector<Identifier>::const_iterator it = m_TRT_ID->straw_layer_begin(); it != m_TRT_ID->straw_layer_end(); it++) {
		for(int i=0; i<= m_TRT_ID->straw_max(*it); i++) {
			Identifier id = m_TRT_ID->straw_id(*it, i);
			if(!m_conditions_svc->isGood(id)) {
				out << id.getString()
				    << std::setw(3) << m_TRT_ID->barrel_ec(id)
				    << std::setw(4) << m_TRT_ID->phi_module(id)
				    << std::setw(4) << m_TRT_ID->layer_or_wheel(id)
				    << std::setw(4) << m_TRT_ID->straw_layer(id)
				    << std::setw(4) << m_TRT_ID->straw(id)
				    << std::endl;
			}
		}
	}
	out.close();
}


//____________________________________________________________________________
DataVector<const Trk::TrackStateOnSurface>::const_iterator
TRTTrackHoleSearchTool::find_first_trt_hit(const DataVector<const Trk::TrackStateOnSurface>& track_states) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::find_first_trt_hit" );
	// finds the first TRT hit or returns end
	DataVector<const Trk::TrackStateOnSurface>::const_iterator track_state = track_states.begin();
	for(; track_state != track_states.end(); ++track_state) {
		// skip track states that are not measurements
		if(!(*track_state)->type(Trk::TrackStateOnSurface::Measurement)) {
			ATH_MSG_VERBOSE( "    TrackStateOnSurface is not of type Trk::TrackStateOnSurface::Measurement." );
			continue;
		}
		if( dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>( (*track_state)->measurementOnTrack() )  ) {
			ATH_MSG_DEBUG("    Found first TRT hit.");
			break;
		}
	}
	return track_state;
}


//____________________________________________________________________________
DataVector<const Trk::TrackStateOnSurface>::const_iterator
TRTTrackHoleSearchTool::find_last_hit_before_trt(const DataVector<const Trk::TrackStateOnSurface>& track_states) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::find_last_hit_before_trt" );
	// start at first TRT hit
	DataVector<const Trk::TrackStateOnSurface>::const_iterator track_state = find_first_trt_hit(track_states);
	// if there is no room to step backwards in the hits, return end
	if(track_states.size() < 2 || track_state == track_states.begin()) {
		return track_states.end();
	}
	track_state--; // step back and look for last measurement before the TRT hit
	for(; track_state != track_states.begin(); track_state--) {
		if((*track_state)->type(Trk::TrackStateOnSurface::Measurement)) {
			break;
		}
	}
	// if the loop ended without finding a measurement, return end
	if(!(*track_state)->type(Trk::TrackStateOnSurface::Measurement)) {
		return track_states.end();
	}
	return track_state;
}


//____________________________________________________________________________
const Trk::Track* TRTTrackHoleSearchTool::addHolesToTrack(const Trk::Track& track,
                                                          const DataVector<const Trk::TrackStateOnSurface>* holes) const {
	ATH_MSG_DEBUG( "TRTTrackHoleSearchTool::addHolesToTrack" );
	/*
	  This method was basically coppied from here:
	  http://alxr.usatlas.bnl.gov/lxr-stb4/source/atlas/InnerDetector/InDetRecTools/InDetTrackHoleSearch/src/InDetTrackHoleSearchTool.cxx#931
	*/

	// get states from track
	DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>;
	for(DataVector<const Trk::TrackStateOnSurface>::const_iterator it = track.trackStateOnSurfaces()->begin();
	    it != track.trackStateOnSurfaces()->end(); ++it) {
		// veto old holes
		if ( !(*it)->type(Trk::TrackStateOnSurface::Hole) ) {
			tsos->push_back(new Trk::TrackStateOnSurface(**it));
		}
	}

	// if we have no holes on the old track and no holes found by search, then we just copy the track
	if(track.trackStateOnSurfaces()->size() == tsos->size() && holes->size() == 0) {
		// create copy of track
		const Trk::Track* new_track = new Trk::Track(track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone() : 0);
		return new_track;
	}

	// add new holes
	for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = holes->begin();
	     it != holes->end(); ++it) {
		tsos->push_back(*it);
	}

	// sort
	const Trk::TrackParameters* perigee = track.perigeeParameters();
	if (!perigee) perigee = (*(track.trackStateOnSurfaces()->begin()))->trackParameters();

	if (perigee) {
		Trk::TrackStateOnSurfaceComparisonFunction* CompFunc =
			new Trk::TrackStateOnSurfaceComparisonFunction( perigee->momentum() );

		if (fabs(perigee->parameters()[Trk::qOverP]) > 0.002) {
			/* invest n*(logN)**2 sorting time for lowPt, coping with a possibly
			   not 100% transitive comparison functor.
			   DataVector doesn't have stable sort, so we need to tamper with
			   its vector content in order to avoid sort to get caught in DV full
			   object ownership */
			if (msgLvl(MSG::DEBUG)) {
				msg() << "sorting vector with stable_sort" << endmsg;
			}
			std::vector<const Trk::TrackStateOnSurface*>* PtrVector
				= const_cast<std::vector<const Trk::TrackStateOnSurface*>* > (&tsos->stdcont());
			stable_sort( PtrVector->begin(), PtrVector->end(), *CompFunc );
		} else {
			tsos->sort( *CompFunc ); // respects DV object ownership
		}

		delete CompFunc;
	}

	// create copy of track
	const Trk::Track* new_track = new Trk::Track( track.info(),tsos,track.fitQuality() ? track.fitQuality()->clone() : 0);

	return new_track;
}

// EOF
