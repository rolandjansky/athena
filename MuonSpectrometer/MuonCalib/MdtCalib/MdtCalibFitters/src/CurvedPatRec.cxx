/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 05.04.2008, AUTHOR: OLIVER KORTNER
// Modified: 25.07.2008 by O. Kortner, improved pattern recognition by using
//                                     the class "CurvedCandidateFinder"
//           04.08.2008 by O. Kortner, further improvements of the pattern
//                                     recognition for large incidence angles.
//           07.08.2008 by O. Kortner, bug fig in the pattern recognition.
//           18.08.2008 by O. Kortner, update of chi^2 and segment position
//                                     and direction added.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS CurvedPatRec ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MdtCalibFitters/CurvedPatRec.h"
#include "MdtCalibFitters/CurvedCandidateFinder.h"
#include "MuonCalibMath/Combination.h"
#include "time.h"
#include "cmath"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//*****************************************************************************

//:::::::::::::::::::::::::
//:: DEFAULT CONSTRUCTOR ::
//:::::::::::::::::::::::::

CurvedPatRec::CurvedPatRec(void) {

	m_chi2 = -1.0;
    m_road_width = 0.5;
    m_track_hits.clear();
    m_time_out = 10;

}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

CurvedPatRec::CurvedPatRec(const double & road_width) {

	m_chi2 = -1.0;
    m_road_width = road_width;
    m_track_hits.clear();
    m_time_out = 10;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD roadWidth ::
//::::::::::::::::::::::

double CurvedPatRec::roadWidth(void) const {

	return m_road_width;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD numberOfTrackHits ::
//::::::::::::::::::::::::::::::

unsigned int CurvedPatRec::numberOfTrackHits(void) const {

	return m_track_hits.size();

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD trackHits ::
//::::::::::::::::::::::

const std::vector<const MdtCalibHitBase*> & CurvedPatRec::trackHits(
																void) const {

	return m_track_hits;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD chi2 ::
//:::::::::::::::::

double CurvedPatRec::chi2(void) const {

	return m_chi2;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::
//:: METHOD chi2PerDegreesOfFreedom ::
//::::::::::::::::::::::::::::::::::::

double CurvedPatRec::chi2PerDegreesOfFreedom(void) const {

	return m_chi2/static_cast<double>(m_track_hits.size()-3);

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD curvedTrack ::
//::::::::::::::::::::::::

const CurvedLine & CurvedPatRec::curvedTrack(void) const {

	return m_curved_track;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD setRoadWidth ::
//:::::::::::::::::::::::::

void CurvedPatRec::setRoadWidth(const double & r_road_width) {

	m_road_width = r_road_width;
	return;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD setTimeOut ::
//:::::::::::::::::::::::

void CurvedPatRec::setTimeOut(const double & time_out) {

	m_time_out = time_out;
	return;

}

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD fit(.) ::
//:::::::::::::::::::

bool CurvedPatRec::fit(MuonCalibSegment & r_segment) const {

// select all hits //
	HitSelection selection(r_segment.mdtHitsOnTrack(), 0);
// call the other fit function //
	return fit(r_segment, selection);

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD fit(.,.) ::
//:::::::::::::::::::::

bool CurvedPatRec::fit(MuonCalibSegment & r_segment,
											HitSelection r_selection) const {
///////////////
// VARIABLES //
///////////////

    time_t start, end; // start and end times (needed for time-out)
	double diff; // difference of start and end time (needed for time-out)
    Combination combination;
    vector<unsigned int> hit_index; // hit indices for a given combination
    unsigned int try_nb_hits; // try to find a segment with try_nb_hits hits
    bool segment_found(false); // flag indicating the a segment has been found
    vector<const MdtCalibHitBase *> cand_track_hits; // vector of the track hits
                                                     // found so far
    CurvedLine aux_line; // memory for reconstructed curved lines
	Amg::Vector3D null(0.0, 0.0, 0.0);
	Amg::Vector3D xhat(1.0, 0.0, 0.0);
    vector<Amg::Vector3D> points; // hit points for the track fit
	vector<const MdtCalibHitBase*> loc_track_hits; // track hit store

////////////
// RESETS //
////////////

	m_chi2 = -1.0;
    m_track_hits.clear();
	time(&start);

////////////////////////////////////////
// CHECK SIZE OF THE SELECTION VECTOR //
////////////////////////////////////////

    if (r_selection.size()!=r_segment.mdtHitsOnTrack()) {
        cerr << endl
             << "Class CurvedPatRec, method fit: ERROR!\n"
             << "Size of selection vector does not match the number of hits on"
             << "track!\n";
        exit(1);
    }

//////////////////////
// PREPARATORY WORK //
//////////////////////

// perform a straight track fit to get an estimate of the incidence angle //
	Amg::Vector3D est_dir(0.0, 0.0, 1.0);
	m_sfitter.setRoadWidth(2.0*m_road_width);
	m_sfitter.setTimeOut(0.5*m_time_out);
	if (m_sfitter.fit(r_segment, r_selection)) {
		est_dir = m_sfitter.track().directionVector();
	}
	
// store track hits //
	for (unsigned int k=0; k<r_segment.mdtHitsOnTrack(); k++) {
        if (r_selection[k]==0 && r_segment.mdtHOT()[k]->sigmaDriftRadius()<100) {
            m_track_hits.push_back(r_segment.mdtHOT()[k]);
        	loc_track_hits.push_back(r_segment.mdtHOT()[k]);
		}
    }

// return, if there are too few hits //
	if (m_track_hits.size()<4) {
		return false;
	}
/////////////////////////
// PATTERN RECOGNITION //
/////////////////////////

// try to find a segment with as many hits on it as possible //
    try_nb_hits = loc_track_hits.size();
	while (!segment_found && try_nb_hits>3) {

   // reset //
        m_chi2 = -1.0;

   // loop over the combinations //
        combination.setNewParameters(loc_track_hits.size(), try_nb_hits);
        for (unsigned int cb=0; cb<combination.numberOfCombinations(); cb++) {

   // time-out //
            time (&end);
            diff = difftime (end,start);
            if (diff>m_time_out) {
                cerr << endl
	   		         << "Class CurvedPatRec: "
	   		         << "time-out for track finding after "
			         << m_time_out
			         << " seconds!\n";
                return false;
            }

    // analyse the hit combination //
            if (cb==0) {
                combination.currentCombination(hit_index);
            } else {
                combination.nextCombination(hit_index);
            }
            vector<const MdtCalibHitBase *> track_hits;
            for (unsigned int k=0; k<try_nb_hits; k++) {
                track_hits.push_back(loc_track_hits[hit_index[k]-1]);
            }

        // find candidates //
            CurvedCandidateFinder finder(track_hits);
            const vector<CurvedLine> &candidates(finder.getCandidates(
                                                       m_road_width, est_dir));
			if (candidates.size()==0) {
                continue;
            }

            segment_found = true; 

        // store the track hits //
 //           m_track_hits = track_hits;

            for (unsigned int cand=0; cand<candidates.size(); cand++) {
                vector<Amg::Vector3D> errors(track_hits.size());
                for (unsigned int k=0; k<errors.size(); k++) {
                    if (track_hits[k]->sigmaDriftRadius()>0.0) {
 			            errors[k] = Amg::Vector3D(1.0,
									track_hits[k]->sigmaDriftRadius(), 0.0);
		            } else {
			            errors[k] = Amg::Vector3D(1.0, 1.0, 0.0);
		            }
	            }

        // get hit points //
                points = getHitPoints(track_hits, candidates[cand]);

        // fit a curved line through the points //
                aux_line = CurvedLine(points, errors);

        // calculate chi^2 //
                double tmp_chi2(0.0);
	            for (unsigned int k=0; k<track_hits.size(); k++) {
		            MTStraightLine tang(m_curved_track.getTangent(
										(track_hits[k]->localPosition()).z()));
                    MTStraightLine wire(Amg::Vector3D(0.0, 
									track_hits[k]->localPosition().y(),
									track_hits[k]->localPosition().z()),
									xhat, null, null);
                    double d(fabs(tang.signDistFrom(wire)));
                    if (track_hits[k]->sigma2DriftRadius()!=0) {
                        tmp_chi2 = tmp_chi2+
                                    std::pow(d-track_hits[k]->driftRadius(), 2)/
									track_hits[k]->sigma2DriftRadius();
                    } else {
                        tmp_chi2 = tmp_chi2+
                                    std::pow(d-track_hits[k]->driftRadius(), 2)/0.01;
                    }
	            }

        // compare chi^2 with chi^2 values found so far //
                if (m_chi2<0) {
                    m_chi2 = tmp_chi2;
                    m_curved_track = aux_line;
        // store the track hits //
					m_track_hits = track_hits;
               } else {
 					if (tmp_chi2<m_chi2) {
                        m_chi2 = tmp_chi2;
                        m_curved_track = aux_line;
 						m_track_hits = track_hits;
					}
                }

            }

        }

        try_nb_hits = try_nb_hits-1;

    }

    if (segment_found==false) {
        return false;
    }

///////////////////////////////
// SECOND REFINED CURVED FIT //
///////////////////////////////

// get hit points //
    points = getHitPoints(m_track_hits, m_curved_track);
    vector<Amg::Vector3D> errors(m_track_hits.size());
    for (unsigned int k=0; k<errors.size(); k++) {
        if (m_track_hits[k]->sigmaDriftRadius()>0.0) {
 		    errors[k] = Amg::Vector3D(1.0,
									m_track_hits[k]->sigmaDriftRadius(), 0.0);
		} else {
		    errors[k] = Amg::Vector3D(1.0, 1.0, 0.0);
		}
	}

// fit a curved line through the points //
	m_curved_track = CurvedLine(points, errors);

/////////////////////
// CALCULATE CHI^2 //
/////////////////////

	m_chi2 = 0.0;
	for (unsigned int k=0; k<m_track_hits.size(); k++) {
		MTStraightLine tang(m_curved_track.getTangent(
									(m_track_hits[k]->localPosition()).z()));
		MTStraightLine wire(Amg::Vector3D(0.0, 
									m_track_hits[k]->localPosition().y(),
									m_track_hits[k]->localPosition().z()),
									xhat, null, null);
		double d(fabs(tang.signDistFrom(wire)));
		if (m_track_hits[k]->sigma2DriftRadius()!=0) {
            m_chi2 = m_chi2+ std::pow(d-m_track_hits[k]->driftRadius(), 2)/
									m_track_hits[k]->sigma2DriftRadius();
        } else {
            m_chi2 = m_chi2+ std::pow(d-m_track_hits[k]->driftRadius(), 2)/0.01;
        }
	}

//////////////////////////
// UPDATE HIT RESIDUALS //
//////////////////////////

	MuonCalibSegment::MdtHitIt it = r_segment.mdtHOTBegin();
	while(it!=r_segment.mdtHOTEnd()){

		MdtCalibHitBase& hit = const_cast< MdtCalibHitBase& >( **it );

		Amg::Vector3D pos(0.0, (hit.localPosition()).y(),
					(hit.localPosition()).z());
		MTStraightLine aux_line(pos, xhat, null, null);

		MTStraightLine tang(m_curved_track.getTangent(pos.z()));

		double dist(tang.signDistFrom(aux_line)); // track distance
		double dist_err(1.0); // unknown error of the track distance
		hit.setDistanceToTrack(dist, dist_err);
    
		++it;

	}

	if (std::isnan(m_chi2)) {
		m_chi2=1.0e6;
	}

///////////////////////////////////////////////
// UPDATE SEGMENT POSITION, DIRECTION, CHI^2 //
///////////////////////////////////////////////

	MTStraightLine tangent(m_curved_track.getTangent(
												(r_segment.position()).z()));
	r_segment.set(m_chi2/static_cast<double>(m_track_hits.size()-3),
												tangent.positionVector(),
												tangent.directionVector());

	return true;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD printLevel ::
//:::::::::::::::::::::::

void CurvedPatRec::printLevel(int level) {

	cerr << "Class CurvedPatRec: method printLevel: " << level
			<< " has no effect!\n";

	return;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD getHitPoint ::
//:::::::::::::::::::::::::

Amg::Vector3D CurvedPatRec::getHitPoint(const MdtCalibHitBase * hit,
                                const MTStraightLine & straight_track) const {

///////////////
// VARIABLES //
///////////////

// 	double dy, dz; // wire coordinates in the yz (precision) plane
// 	double my, by; // slope and intercept of the given line in the yz
				   // (precision) plane
// 	double d0; // distance of the track from the wire

////////////////////
// FILL VARIABLES //
////////////////////

// 	dy = (hit->localPosition()).y();
// 	dz = (hit->localPosition()).z();
// 	my = straight_track.m_x2();
// 	by = straight_track.b_x2();

///////////////////////////////////////////
// CALCULATE THE POINT OF CLOSE APPROACH //
///////////////////////////////////////////

// 	double z0((dz-(by-dy)*my)/(1+my*my));
// 	double y0(my*z0+by);

/////////////////////////
// CALCULATE HIT POINT //
/////////////////////////

// 	d0 = sqrt((y0-dy)*(y0-dy)+(z0-dz)*(z0-dz));



  Amg::Vector3D point = straight_track.positionVector() + (straight_track.directionVector().unit().dot(hit->localPosition() - straight_track.positionVector() ) ) * straight_track.directionVector().unit();
  Amg::Vector3D point_2 = hit->localPosition() + hit->driftRadius() * (point - hit->localPosition()).unit() ;

  return point_2;
	
}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD getHitPoints ::
//:::::::::::::::::::::::::

std::vector<Amg::Vector3D> CurvedPatRec::getHitPoints(
                                std::vector<const MdtCalibHitBase*> track_hits,
                                const MTStraightLine & straight_track) const {

///////////////
// VARIABLES //
///////////////

	vector<Amg::Vector3D> hit_vec;

/////////////////////
// FILL HIT VECTOR //
/////////////////////

	for (unsigned int k=0; k<track_hits.size(); k++) {
		hit_vec.push_back(getHitPoint(track_hits[k], straight_track));
	}

///////////////////////////
// RETURN THE HIT VECTOR //
///////////////////////////

	return hit_vec;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD getHitPoints ::
//:::::::::::::::::::::::::

std::vector<Amg::Vector3D> CurvedPatRec::getHitPoints(
                                std::vector<const MdtCalibHitBase*> track_hits,
                                const CurvedLine & curved_track) const {
///////////////
// VARIABLES //
///////////////

	vector<Amg::Vector3D> hit_vec;

/////////////////////
// FILL HIT VECTOR //
/////////////////////

	for (unsigned int k=0; k<track_hits.size(); k++) {
		hit_vec.push_back(getHitPoint(track_hits[k],
							curved_track.getTangent(
									(track_hits[k]->localPosition()).z())));
	}

///////////////////////////
// RETURN THE HIT VECTOR //
///////////////////////////

	return hit_vec;

}
