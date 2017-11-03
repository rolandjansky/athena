/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.03.2006, AUTHOR: OLIVER KORTNER
// Modified: 15.07.2006 by O. Kortner, error calculation corrected,
//                                     chi^2 refit functionality added.
//           13.01.2007 by O. Kortner, bug fix in candidate treatment, some
//                                     candidates were considered to be
//                                     identical although different;
//                                     modifications to improve the
//                                     reconstruction efficiency at very high
//                                     background count rates.
//           27.03.2007 by O. Kortner, distances with signs filled into
//                                     MuonCalibSegment.
//           23.03.2007 by O. Kortner, isnan check for chi^2.
//           08.06.2007 by O. Kortner, final track segment has rphi position and
//                                     direction of the initial segment.
//           23.06.2006 by O. Kortner, add convention for rphi track if the
//                                     pattern recognition has failed to
//                                     provide it.
//           26.11.2007 by O. Kortner, fix for segment refinement.
//           13.12.2007 by O. Kortner, time-out added.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include <iostream>
#include <fstream>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "time.h"
#include <cmath>

//:::::::::::::::::::::::
//:: NAMESPACE SETTING ::
//:::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS ::
//::         QuasianalyticLineReconstruction        ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void QuasianalyticLineReconstruction::init(void) {

	init(0.5*CLHEP::mm); // default road width = 0.5 CLHEP::mm
	return;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::::::::::::
//:: METHOD init(const double & r_road_width) ::
//::::::::::::::::::::::::::::::::::::::::::::::

void QuasianalyticLineReconstruction::init(const double & r_road_width) {

//:::::::::::::::
//:: VARIABLES ::
//:::::::::::::::

	Amg::Vector3D null_vec(0.0, 0.0, 0.0); // auxiliary 0 vector

//::::::::::::::::::
//:: SET TIME-OUT ::
//::::::::::::::::::

	m_time_out = 10.0;

//::::::::::::::::::::::::::::
//:: SET THE MAXIMUM RADIUS ::
//::::::::::::::::::::::::::::

	m_r_max = 15.0;

//::::::::::::::::::::::::
//:: SET THE ROAD WIDTH ::
//::::::::::::::::::::::::

	m_road_width = r_road_width;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: INITIALIZE PRIVATE VARIABLES WHICH ARE ACCESSIBLE BY METHODS ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	m_nb_track_hits = 0;
	m_chi2 = 0.0;
	m_track = MTStraightLine(null_vec, null_vec, null_vec, null_vec);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: SET THE TRACK IN THE x1-x3 PLANE (LATER VERSION MAY ALLOW TO SET ::
//:: USER-DEFINED VALUES)                                             ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	m_a_x1 = 1.0; m_b_x1 = 0.0;

	return;

}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD tangent ::
//::::::::::::::::::::

MTStraightLine QuasianalyticLineReconstruction::tangent(
		const Amg::Vector3D & r_w1,
		const double & r_r1, const double & r_sigma12,
		const Amg::Vector3D & r_w2, const double & r_r2,
		const double & r_sigma22, const int & r_case) const {

//:::::::::::::::
//:: VARIABLES ::
//:::::::::::::::

	double sinalpha; // auxiliary sinus of an angle
	double cosalpha; // auxiliary sinus of an angle
	Amg::Vector3D e2prime(0.,0.,0.), e3prime(0.,0.,0.); // auxiliary direction vectors
	MTStraightLine tang; // tangent to drift circles of a hit pair
	Amg::Vector3D p1(0.,0.,0.), p2(0.,0.,0.); // hit points defining a tangent
	Amg::Vector3D null_vec(0.0, 0.0, 0.0); // auxiliary 0 vector
	double mx1, bx1, mx2, bx2; // auxiliary track parameters

//::::::::::::::::::::::::::::::::::::::::::::
//:: CHECK WHETHER THE SELECTED CASE EXISTS ::
//::::::::::::::::::::::::::::::::::::::::::::

	if (r_case<1 || r_case>4) {
		cerr << endl
			<< "Class QuasianalyticLineReconstruction, "
			<< "method tangent: ERROR!\n"
			<< "Illegal case " << r_case << ", must be 1,2,3, or 4."
			<< endl;
		exit(1);
	}

//:::::::::::::::::::::::::::::::::::::
//:: CALCULATE THE REQUESTED TANGENT ::
//:::::::::::::::::::::::::::::::::::::

// local coordinate axis vectors //
	e3prime = (r_w2-r_w1).unit();
	e2prime = Amg::Vector3D(0.0, e3prime.z(), -e3prime.y());

// case 1 and 2 //
	if (r_case==1 || r_case==2) {

		sinalpha = fabs(r_r2-r_r1)/(r_w2-r_w1).mag();
		cosalpha = sqrt(1.0-sinalpha*sinalpha);

   // case 1 //
		if (r_case==1) {
			p1 = r_w1+((1 && r_r2>=r_r1)-(1 && r_r2<r_r1))*r_r1*
					(cosalpha*e2prime-sinalpha*e3prime);
			p2 = r_w2+((1 && r_r2>=r_r1)-(1 && r_r2<r_r1))*r_r2*
					(cosalpha*e2prime-sinalpha*e3prime);
		}

   // case 2 //
		if (r_case==2) {
			p1 = r_w1-((1 && r_r2>=r_r1)-(1 && r_r2<r_r1))*r_r1*
					(cosalpha*e2prime+sinalpha*e3prime);
			p2 = r_w2-((1 && r_r2>=r_r1)-(1 && r_r2<r_r1))*r_r2*
					(cosalpha*e2prime+sinalpha*e3prime);
		}


	}

// case 3 and 4 //
	if (r_case==3  || r_case==4) {

		sinalpha = (r_r1+r_r2)/(r_w2-r_w1).mag();
		cosalpha = sqrt(1.0-sinalpha*sinalpha);

   // case 3 //
		if (r_case==3) {
// 			p1 = r_w1+r_r1*(cosalpha*e2prime-sinalpha*e3prime);
// 			p2 = r_w2-r_r2*(cosalpha*e2prime-sinalpha*e3prime);
			p1 = r_w1+r_r1*(cosalpha*e2prime+sinalpha*e3prime);
			p2 = r_w2-r_r2*(cosalpha*e2prime+sinalpha*e3prime);
		}

   // case 4 //
		if (r_case==4) {
			p1 = r_w1-r_r1*(cosalpha*e2prime-sinalpha*e3prime);
			p2 = r_w2+r_r2*(cosalpha*e2prime-sinalpha*e3prime);
		}

	}

// calculation of the tangent and estimation of its errors //
	if ((p2-p1).z()!=0.0) {
		tang = MTStraightLine(p1, p2-p1, null_vec, null_vec);
	} else {
		Amg::Vector3D direction(p2-p1);
		direction[2] = 1.0e-99;
		tang = MTStraightLine(p1, direction, null_vec, null_vec);	
	}
	mx1 = tang.a_x1(); bx2 = tang.b_x1();
	mx2 = tang.a_x2(); bx2 = tang.b_x2();
	tang = MTStraightLine(mx1, bx1, mx2, bx2,1.0, 1.0, 
			sqrt(r_sigma12+r_sigma22)/fabs(p2.z()-p1.z()),
			sqrt(r_sigma12+p1.z()*p1.z()*(r_sigma12+r_sigma22)/
						std::pow(p2.z()-p1.z(), 2)));
			// errors in mx1 and bx1 are arbitrary since they are
			// not used at a later stage.


	return tang;

}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD track_candidate ::
//::::::::::::::::::::::::::::

MTStraightLine QuasianalyticLineReconstruction::track_candidate(
				const IndexSet & r_index_set,
				const int & r_k_cand,
				const int & r_l_cand,
				const int & r_cand_case,
				vector<Amg::Vector3D> r_w,
				vector<double> r_r,
				vector<double> r_sigma2,
				double & r_chi2) const {

//:::::::::::::::
//:: VARIABLES ::
//:::::::::::::::

	int nb_hits(r_index_set.size()); // number of hits used in the track
					    // reconstruction
	int nb_tangents(0); // number of tangents used in the track
			       // reconstruction
	vector<double> mx1, bx1, mx2, bx2; // slopes and intercepts of the
					      // tangents building the track
	vector<double> mx1_err, bx1_err, mx2_err, bx2_err; // their errors
	MTStraightLine aux_track; // auxiliary straight line
	MTStraightLine tang; // tangent to drift circles of a hit pair
	Amg::Vector3D d1(0.0, 0.0, 0.0), d2(0.0, 0.0, 0.0); // auxiliary distance vectors
	Amg::Vector3D e2prime(0.0, 0.0, 0.0), e3prime(0.0, 0.0, 0.0); // auxiliary basis vectors
	Amg::Vector3D a(0.0, 0.0, 0.0), u(0.0, 0.0, 0.0); // position and direction vector of the candidate
			 // tangent
	double sinalpha; // auxiliary sinus of an angle
	double cosalpha; // auxiliary sinus of an angle
	Amg::Vector3D p1(0.0, 0.0, 0.0), p2(0.0, 0.0, 0.0); // hit points defining a tangent
	Amg::Vector3D null_vec(0.0, 0.0, 0.0); // auxiliary 0 vector
	double sum[4]; // auxiliary summation variables

//:::::::::::::::::::::
//:: GET THE TANGENT ::
//:::::::::::::::::::::

// calculate the tangent //
	aux_track = tangent(r_w[r_k_cand], r_r[r_k_cand], r_sigma2[r_k_cand],
			r_w[r_l_cand], r_r[r_l_cand], r_sigma2[r_l_cand],
			r_cand_case);
	a = aux_track.positionVector();
	u = (aux_track.directionVector()).unit();

// store the parameters of this tangent //
	mx1.push_back(aux_track.a_x1());
	mx1_err.push_back(aux_track.a_x1_error());
	bx1.push_back(aux_track.b_x1());
	bx1_err.push_back(aux_track.b_x1_error());
	mx2.push_back(aux_track.a_x2());
	mx2_err.push_back(aux_track.a_x2_error());
	bx2.push_back(aux_track.b_x2());
	bx2_err.push_back(aux_track.b_x2_error());

//:::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: LOOP OVER THE OTHER HITS AND CALCULATE TANGENTS ::                                                            //
//:::::::::::::::::::::::::::::::::::::::::::::::::::::

	for (int kk=0; kk<nb_hits-1; kk++) {
	for (int ll=kk+1; ll<nb_hits; ll++) {

		int k(r_index_set[kk]);
		int l(r_index_set[ll]);

		if (k==r_k_cand && l==r_l_cand) {
			continue;
		}

// local coordinate axis vectors //
		e3prime = (r_w[l]-r_w[k]).unit();
		e2prime = Amg::Vector3D(0.0, e3prime.z(), -e3prime.y());

// distance vectors //
		d1 = (r_w[k]-a).dot(u)*u-(r_w[k]-a); d1[0] = (0.0);
		d2 = (r_w[l]-a).dot(u)*u-(r_w[l]-a); d2[0] = (0.0);

// one must distinguish 2 cases //

   // case 1: candidate tangent passes both wires on the same side //
		if (d1.dot(d2)>=0) {

			sinalpha = fabs(r_r[l]-r_r[k])/(r_w[l]-r_w[k]).mag();
			cosalpha = sqrt(1.0-sinalpha*sinalpha);

			if (d1.dot(e2prime)>=0) {
				if (r_r[k]<=r_r[l]) {
					p1 = r_w[k]+r_r[k]*(cosalpha*e2prime-
						sinalpha*e3prime);
				} else {
					p1 = r_w[k]+r_r[k]*(cosalpha*e2prime+
						sinalpha*e3prime);
				}
			}
			else {
				if (r_r[k]>=r_r[l]) {
					p1 = r_w[k]-r_r[k]*(cosalpha*e2prime-
						sinalpha*e3prime);
				} else {
					p1 = r_w[k]-r_r[k]*(cosalpha*e2prime+
						sinalpha*e3prime);
				}
			}

			if (d2.dot(e2prime)>=0) {
				if (r_r[k]<=r_r[l]) {
					p2 = r_w[l]+r_r[l]*(cosalpha*e2prime-
						sinalpha*e3prime);
				} else {
					p2 = r_w[l]+r_r[l]*(cosalpha*e2prime+
						sinalpha*e3prime);
				}
			}
			else {
				if (r_r[k]>=r_r[l]) {
					p2 = r_w[l]-r_r[l]*(cosalpha*e2prime-
						sinalpha*e3prime);
				} else {
					p2 = r_w[l]-r_r[l]*(cosalpha*e2prime+
						sinalpha*e3prime);
				}
			}

		}

   // case 2: candidate tangent passes both wires on opposite sides //
		if (d1.dot(d2)<0) {

			sinalpha = (r_r[l]+r_r[k])/(r_w[l]-r_w[k]).mag();
			cosalpha = sqrt(1.0-sinalpha*sinalpha);

	// case 2(a) //
			if (d1.dot(e2prime)>=0 && d2.dot(e2prime)<=0) {
				p1 = r_w[k]+r_r[k]*(cosalpha*e2prime+
							sinalpha*e3prime);
				p2 = r_w[l]-r_r[l]*(cosalpha*e2prime+
							sinalpha*e3prime);
			}

	// case 2(b) //
			if (d1.dot(e2prime)<0 && d2.dot(e2prime)>=0) {
				p1 = r_w[k]-r_r[k]*(cosalpha*e2prime-
							sinalpha*e3prime);
				p2 = r_w[l]+r_r[l]*(cosalpha*e2prime-
							sinalpha*e3prime);
			}

		}

// get the slope and intercept of the new tangents and error estimates //
		if ((p2-p1).z()!=0.0) {
			tang = MTStraightLine(p1, p2-p1, null_vec, null_vec);
		} else {
			Amg::Vector3D direction(p2-p1);
			direction[2] = (1.0e-99);
			tang = MTStraightLine(p1, direction,
							null_vec, null_vec);
		}
		mx1.push_back(tang.a_x1());
		bx1.push_back(tang.b_x1());
		mx2.push_back(tang.a_x2());
		bx2.push_back(tang.b_x2());

		mx1_err.push_back(1.0);
		bx1_err.push_back(1.0);
		
		mx2_err.push_back((r_sigma2[k]+r_sigma2[l])/
							std::pow(p2.z()-p1.z(), 2));
		bx2_err.push_back(r_sigma2[k]+
				p1.z()*p1.z()*(r_sigma2[k]+r_sigma2[l])/
						std::pow(p2.z()-p1.z(),2));
			// errors in x1 and x2 are arbitrary since they are not
			// used at a later stage.

// increase the number of tangents //
		nb_tangents = nb_tangents+1;

	}
	}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: CALCULATE A WEIGHTED AVERAGE OVER THE TANGENTS AND THE chi^2 FOR ::
//:: THE RECONSTRUCTED TRAJECTORY                                     ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	sum[0] = 0.0; sum[1] = 0.0; sum[2] = 0.0; sum[3] = 0.0;
	for (int k=0; k<nb_tangents; k++) {

		if (mx2_err[k]>0) {
			sum[0] = sum[0] + mx2[k]/mx2_err[k];
			sum[1] = sum[1] + 1.0/mx2_err[k];
		} else {
			sum[0] = sum[0] + mx2[k];
			sum[1] = sum[1] + 1.0;
		}
		if (bx2_err[k]>0) {
			sum[2] = sum[2] + bx2[k]/bx2_err[k];
			sum[3] = sum[3] + 1.0/bx2_err[k];
		} else {
			sum[2] = sum[2] + bx2[k];
			sum[3] = sum[3] + 1.0;
		}

	}

	aux_track = MTStraightLine(0.0, 0.0, sum[0]/sum[1], sum[2]/sum[3],
				0.0, 1.0, sqrt(1.0/sum[1]), sqrt(1.0/sum[3]));

	r_chi2 = 0.0;
	for (int k=0; k<nb_hits; k++) {
		r_chi2 = r_chi2+std::pow(aux_track.distFromLine(
					r_w[r_index_set[k]])
					-r_r[r_index_set[k]], 2)/
								r_sigma2[k];
	}

	return aux_track;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD roadWidth ::
//::::::::::::::::::::::

double QuasianalyticLineReconstruction::roadWidth(void) const {

	return m_road_width;

}


//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD numberOfTrackHits ::
//::::::::::::::::::::::::::::::

unsigned int QuasianalyticLineReconstruction::numberOfTrackHits(void) const {

	return m_nb_track_hits;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD trackHits ::
//::::::::::::::::::::::

const vector<const MdtCalibHitBase*> & QuasianalyticLineReconstruction::trackHits(void) const {

	return m_track_hits;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD chi2 ::
//:::::::::::::::::

double QuasianalyticLineReconstruction::chi2(void) const {

	if (m_nb_track_hits == 0) {
		return -1;
	}
	return m_chi2;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::
//:: METHOD chi2PerDegreesOfFreedom ::
//::::::::::::::::::::::::::::::::::::

double QuasianalyticLineReconstruction::chi2PerDegreesOfFreedom(
								void) const{

	if (m_nb_track_hits<3) {
		return -1;
	}
	return chi2()/static_cast<double>(numberOfTrackHits()-2);


}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD track ::
//::::::::::::::::::

MTStraightLine QuasianalyticLineReconstruction::track(void) const {

	return m_track;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD setRoadWidth ::
//:::::::::::::::::::::::::

void QuasianalyticLineReconstruction::setRoadWidth(
					const double & r_road_width) {

	m_road_width = fabs(r_road_width);
	return;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD setTimeOut ::
//:::::::::::::::::::::::

void QuasianalyticLineReconstruction::setTimeOut(const double & time_out) {

	m_time_out = time_out;
	return;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD setTimeOut ::
//:::::::::::::::::::::::

void setTimeOut(const double & /*time_out*/) {
}

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD fit(.) ::
//:::::::::::::::::::

bool QuasianalyticLineReconstruction::fit(MuonCalibSegment & r_segment) const {

// select all hits //
	HitSelection selection(r_segment.mdtHitsOnTrack(), 0);

// call the other fit function //
	return fit(r_segment, selection);

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD fit(.,.) ::
//:::::::::::::::::::::

bool QuasianalyticLineReconstruction::fit(MuonCalibSegment & r_segment,
					HitSelection r_selection) const {

///////////////
// VARIABLES //
///////////////

	time_t start,end; // start and end time (needed for time-out)
	time(&start);
	double diff; // difference of start and end time (needed for time-out)
	unsigned int nb_selected_hits(0); // number of selected hits
	vector<const MdtCalibHitBase*> selected_hits; // vector of pointers to the
	                                        // selected hits
	vector<unsigned> selected_hits_index(r_selection.size());
	                                      // vector containing the indices
	                                      // of the selected hits (needed
	                                      // a requested refit)
	vector<Amg::Vector3D> w; // wire coordinates
	vector<double> r; // drift CLHEP::radii of the selected hits
	vector<double> sigma2; // sigma(r)^2 (spatial resolution in r)
	int counter1, counter2, counter3; // auxiliary counters
	int max_cand_hits; // the maximum number of hits on a track candidate
	                      // found so far
	int max_cand_HOTs; // the maximum number of hit tubes on a track
			      // candidate found so far
	int nb_candidates; // number of candidate tracks
	vector<int> k_cand, l_cand; // candidate track index
	vector<int> cand_case; // tangent case for the candidate
	vector<int> nb_HOTs; // number of hits on a candidate
	int candidate; // index of the candidate which becomes the track
	IndexSet aux_set; // auxiliary index set
	vector<IndexSet> index_set; // index set for the storage of the hits
	                            // forming a track
	vector<double> intercept; // intercepts of track candidates
	MTStraightLine tangents[4]; // the four tangents to the drift circles of a
	                            // hit pair
	MTStraightLine aux_track; // auxiliary track
	double aux_chi2; // auxiliary chi^2 variable
	Amg::Vector3D null(0.0, 0.0, 0.0);
	Amg::Vector3D xhat(1.0, 0.0, 0.0);
	MTStraightLine initial_track(r_segment.position(),
					r_segment.direction(), null, null);
					// initial track stored in the segment
	Amg::Vector3D aux_pos, aux_dir; // auxiliary position and direction vectors					

/////////////////////////////////////
// GET THE NUMBER OF SELECTED HITS //
/////////////////////////////////////
	if (r_segment.mdtHitsOnTrack() != r_selection.size()) {
		cerr << "\n"
			<< "Class QuasianalyticLineReconstruction, "
			<< "METHOD fit(., .): WARNING!\n"
			<< "Vector with selected hits unequal to the number "
			<< "of hits on the segment!\n"
			<< "The user selection will be ignored!\n";
		r_selection.clear();
		r_selection.assign(r_segment.hitsOnTrack(), 0);
	} else {
		for (unsigned int k=0; k<r_selection.size(); k++) {
			if (r_selection[k] == 0) {
				nb_selected_hits = nb_selected_hits+1;
			}
		}
	}

//////////////////////////////////////////////////////////////////////////
// FIX POTENTIAL SECOND-COORDINATE PROBLEM OF THE INITIAL TRACK SEGMENT //
//////////////////////////////////////////////////////////////////////////

	if (r_segment.direction().z()==0.0) {
		Amg::Vector3D dir(r_segment.direction().x(),
				r_segment.direction().y(),
				1.0);
		initial_track = MTStraightLine(r_segment.position(), dir,
								null, null);
	}

///////////////////////////////////////////////////////////////////////////
// RETURN, IF THERE ARE LESS THAN 3 HITS, I.E. THE TRACK IS NOT UNIQUELY //
// DEFINED BY THE HITS.                                                  //
///////////////////////////////////////////////////////////////////////////

	if (nb_selected_hits<3) {
		cerr << "\n"
			<< "Class QuasianalyticLineReconstruction, "
			<< "METHOD fit(., .): WARNING!\n"
			<< "Too few hits for the track reconstructions!\n";
		return false;
	}

/////////////////////////////////////////////////
// COPY THE WIRE COORDINATES TO A LOCAL VECTOR //
/////////////////////////////////////////////////

// vector assignments //
	selected_hits.clear();// = vector<const MdtCalibHitBase*>(nb_selected_hits);
	w.clear();// = vector<Amg::Vector3D>(nb_selected_hits);
	r.clear();// = vector<double>(nb_selected_hits);
	sigma2.clear();// = vector<double>(nb_selected_hits);
	selected_hits_index.clear();//
// vector filling //
	counter2 = 0;
	for (unsigned int k=0; k<r_segment.mdtHitsOnTrack(); k++) {

   // skip the hit, if it has not been selected //
		const MdtCalibHitBase *hit = (r_segment.mdtHOT())[k];
		if (r_selection[k]==1 || hit->sigmaDriftRadius()>100.0) {
			continue;
		}

   // copy the hit information into local vectors //
		selected_hits.push_back(hit);//[counter2] = hit;
		selected_hits_index.push_back(k);//[counter2] = k;
		w.push_back(Amg::Vector3D(0.0, (hit->localPosition()).y(), (hit->localPosition()).z()));//[counter2] = Amg::Vector3D(0.0, (hit->localPosition()).y(), (hit->localPosition()).z());
		r.push_back(fabs(hit->driftRadius()));//[counter2] = fabs(hit->driftRadius());
		sigma2.push_back(hit->sigma2DriftRadius());//[counter2] = hit->sigma2DriftRadius();
   // if the spatial resolution has not been set, set it to 0.1 CLHEP::mm //
		if (sigma2[counter2] == 0) {
			sigma2[counter2] = std::pow(0.1*CLHEP::mm, 2);
		}

		counter2 = counter2+1;

	}
	nb_selected_hits=selected_hits.size();
	if(nb_selected_hits<3)
		return false;
//////////////////////////
// TRACK RECONSTRUCTION //
//////////////////////////

// reset counters //
	max_cand_hits = 0;
	max_cand_HOTs = 0;
	nb_candidates = 0;
// 1st step: loop over all hit pairs and determine a track candidate //
	for (unsigned int k=0; k<nb_selected_hits; k++) {
	for (unsigned int l=k+1; l<nb_selected_hits; l++) {

// time-out //
		time (&end);
  		diff = difftime (end,start);
		if (diff>m_time_out) {
			cerr << endl
				<< "Class QuasianalyticLineReconstruction: "
				<< "time-out for track finding after "
				<< m_time_out
				<< " seconds!\n";
			return false;
		}

// 2nd step: determine all four tangents to the drift circles of the hit pair //
		for (unsigned int r_case=1; r_case<5; r_case++) {
			tangents[r_case-1] = tangent(w[l], r[l], sigma2[l],
						w[k], r[k], sigma2[k], r_case);
		}

// 3rd step: determine additional track points within the road width around //
//           the four tangents                                              //
		for (unsigned int r_case=1; r_case<5; r_case++) {
			bool same(false);
			counter1 = 0;
			counter3 = 0;
			vector<int> indices; // indices of the hits forming a
					     // track
			for (unsigned int n=0; n<nb_selected_hits; n++) {
				MTStraightLine aux_line(w[n], xhat, null, null);
				if (fabs(fabs(tangents[r_case-1].signDistFrom(
						aux_line))-r[n])<m_r_max) {
					counter3++;
				}
				if (fabs(fabs(tangents[r_case-1].signDistFrom(
						aux_line))-
					r[n])<=m_road_width) {
					counter1 = counter1+1;
					indices.push_back(n);
				}
			}
			if (counter1>2) {
				aux_set = IndexSet(counter1, indices);
				aux_set.sort();
				for (int ca=0; ca<nb_candidates; ca++) {
					if (aux_set==index_set[ca] &&
						fabs(intercept[ca]-
						tangents[r_case-1].b_x2())
						<m_road_width) {
						same = true;
						break;
					}
				}
				if (same) {
					continue;
				}
				nb_candidates = nb_candidates+1;
				k_cand.push_back(k);
				l_cand.push_back(l);
				cand_case.push_back(r_case);
				index_set.push_back(aux_set);
				intercept.push_back(tangents[r_case-1].b_x2());
				nb_HOTs.push_back(counter3);
				if (counter1>max_cand_hits) {
					max_cand_hits = counter1;
				}
			}
		}

	}
	}

// 4th step: reconstruct a straight-line trajectory for all candidates with   //
//           the maximum number of hits, keep the one with the smallest chi^2 //
	m_nb_track_hits = max_cand_hits;
	m_chi2 = -1.0;
	if (m_nb_track_hits<3) {
		return false;
	}
	
	candidate = 0;
	for (int ca=0; ca<nb_candidates; ca++) {
		if (index_set[ca].size()<static_cast<unsigned int>(
							m_nb_track_hits)) {
			continue;
		}

		aux_track = track_candidate(index_set[ca],
					k_cand[ca], l_cand[ca], 
					cand_case[ca], w, r, sigma2, aux_chi2);
		if (nb_HOTs[ca]>=max_cand_HOTs) {
			max_cand_HOTs = nb_HOTs[ca];
			if (m_chi2==-1.0 || m_chi2>aux_chi2) {
				m_chi2 = aux_chi2;
				m_track = aux_track;
				candidate = ca;
			}
		}
	}

	m_track_hits.resize(m_nb_track_hits);
	for (int k=0; k<m_nb_track_hits; k++) {
		m_track_hits[k] = selected_hits[index_set[candidate][k]];
	}

   // make the segment in rphi as the initial segment //
	aux_pos = Amg::Vector3D(initial_track.b_x1(), m_track.b_x2(), 0);
	aux_dir = Amg::Vector3D(initial_track.a_x1(), m_track.a_x2(), 1.0);
	m_track = MTStraightLine(aux_pos, aux_dir, null, null);

// 5th step: update the segment//
	if (std::isnan(m_chi2)) {
		m_chi2=1.0e6;
	}
	r_segment.set(m_chi2/static_cast<double>(m_nb_track_hits-2),
			m_track.positionVector(), m_track.directionVector());


	vector<unsigned int> refit_hit_selection(r_selection.size(), 1);
	for (int k=0; k<m_nb_track_hits; k++) {
		refit_hit_selection[selected_hits_index[
					index_set[candidate][k]]] = 0;
	}
	if(!m_refit && m_refine_segment) {
		r_segment.refineMdtSelection(refit_hit_selection);
	}
	
   // chi^2 refit, if requested //
	if (m_refit) {
		if (m_nfitter.fit(r_segment, refit_hit_selection)) {
// 			m_chi2 = r_segment.chi2()*(m_nb_track_hits-2);
			Amg::Vector3D dir(r_segment.direction());
			if (dir.z()==0.0) {
				dir[2] = (1.0e-99);
			}
			MTStraightLine aux_line(r_segment.position(),
					dir,
					Amg::Vector3D(0.0, 0.0, 0.0),
					Amg::Vector3D(0.0, 0.0, 0.0));
			double a_err(m_track.a_x2_error());
			double b_err(m_track.b_x2_error());
			m_track = MTStraightLine(0.0, 0.0,
						aux_line.a_x2(),
						aux_line.b_x2(),
						0.0, 0.0,
						a_err, b_err);
			aux_pos = Amg::Vector3D(initial_track.b_x1(),
							m_track.b_x2(), 0);
			aux_dir = Amg::Vector3D(initial_track.a_x1(),
							m_track.a_x2(), 1.0);
			m_track = MTStraightLine(aux_pos, aux_dir, null, null);
	// recompute chi^2 because of different convention in DCSLFitter //
			m_chi2 = 0.0;
			for (int k=0; k<m_nb_track_hits; k++) {
				MTStraightLine wire(Amg::Vector3D(0.0, 
					m_track_hits[k]->localPosition().y(),
					m_track_hits[k]->localPosition().z()),
					xhat, null, null);
				double d(fabs(m_track.signDistFrom(wire)));
				m_chi2 = m_chi2+
					std::pow(d-m_track_hits[k]->driftRadius(),
						2)/
					m_track_hits[k]->sigma2DriftRadius();
			}
			r_segment.set(
			m_chi2/static_cast<double>(m_nb_track_hits-2),
			m_track.positionVector(), m_track.directionVector());
			if (std::isnan(m_chi2)) {
				m_chi2=1.0e6;
			}
			if (m_refine_segment) {
				r_segment.refineMdtSelection(
							refit_hit_selection);
			}
			return true;
		}
		return false;
	}

   // finish the update (necessary, if no refit has been performed) //
	MuonCalibSegment::MdtHitIt it = r_segment.mdtHOTBegin();
	while(it!=r_segment.mdtHOTEnd()){
		MdtCalibHitBase& hit = const_cast< MdtCalibHitBase& >( **it );

		Amg::Vector3D pos(0.0, (hit.localPosition()).y(),
					(hit.localPosition()).z());
					// wire position
		MTStraightLine aux_line(pos, xhat, null, null);
// 		double dist(fabs(m_track.signDistFrom(aux_line))); // track distance
		double dist(m_track.signDistFrom(aux_line)); // track distance
		double dist_err(sqrt(std::pow(pos.z()*m_track.a_x2_error(), 2)+
				std::pow(m_track.b_x2_error(), 2)));
				// approximate error of the track distance
		hit.setDistanceToTrack(dist, dist_err);
    
		++it;
	}

	return true;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD printLevel ::
//:::::::::::::::::::::::

void QuasianalyticLineReconstruction::printLevel(int level) {

	cerr << "\n"
		<< "Class QuasianalyticLineReconstruction, "
		<< "method printLevel: WARNING!\n"
		<< "Print level " << level << " is ignored.\n";
	return;

}
