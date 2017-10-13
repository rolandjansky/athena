/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 27.10.2007, AUTHOR: OLIVER KORTNER
// Modified: 26.11.2007 by O. Kortner, fix for segment refinement.
//           13.12.2007 by O. Kortner, time-out added.
//           07.08.2008 by O. Kortner, bug fix when hits are disabled.
//				 19.11.2008 by I. Potrap: 1) several bugs have been fixed;
//												  2) external fitter(DCLS) replaced by
//													  internal code of linear-least-chi2-fit
//				 01.12.2008 by I. Potrap, track() method and segment position are fixed
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS ::
//::                 StraightPatRec                 ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

#include "MdtCalibFitters/StraightPatRec.h"
#include <iostream>
#include <fstream>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "MuonCalibMath/Combination.h"
#include "time.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

//:::::::::::::::::::::::
//:: NAMESPACE SETTING ::
//:::::::::::::::::::::::

using namespace MuonCalib;
using namespace std;

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void StraightPatRec::init(void) {

	init(0.5*CLHEP::mm); // default road width = 0.5 CLHEP::mm
	return;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::::::::::::
//:: METHOD init(const double & r_road_width) ::
//::::::::::::::::::::::::::::::::::::::::::::::

void StraightPatRec::init(const double & r_road_width) {


//coverity
	m_b_x1_err=0.0;
	m_b_x2_err=0.0;
	m_b_x2=0.0;
	m_a_x1_err=0.0;
	m_a_x2_err=0.0;
	m_a_x2=0.0;


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

	m_fix_selection = false;

	return;

}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD tangent ::
//::::::::::::::::::::

MTStraightLine StraightPatRec::tangent(
		const Amg::Vector3D & r_w1,
		const double & r_r1, const double & r_sigma12,
		const Amg::Vector3D & r_w2, const double & r_r2,
		const double & r_sigma22, const int & r_case) const {

//:::::::::::::::
//:: VARIABLES ::
//:::::::::::::::

	double sinalpha; // auxiliary sinus of an angle
	double cosalpha; // auxiliary sinus of an angle
	Amg::Vector3D e2prime, e3prime; // auxiliary direction vectors
	MTStraightLine tang; // tangent to drift circles of a hit pair
	Amg::Vector3D p1(0.0, 0.0, 0.0), p2(0.0, 0.0, 0.0); // hit points defining a tangent
	Amg::Vector3D null_vec(0.0, 0.0, 0.0); // auxiliary 0 vector
	double mx1, bx1, mx2, bx2; // auxiliary track parameters

//::::::::::::::::::::::::::::::::::::::::::::
//:: CHECK WHETHER THE SELECTED CASE EXISTS ::
//::::::::::::::::::::::::::::::::::::::::::::

	if (r_case<1 || r_case>4) {
		cerr << endl
			<< "Class StraightPatRec, "
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
		direction[2] = (1.0e-99);
		tang = MTStraightLine(p1, direction, null_vec, null_vec);	
	}
	mx1 = tang.a_x1(); bx2 = tang.b_x1();
	mx2 = tang.a_x2(); bx2 = tang.b_x2();
	tang = MTStraightLine(mx1, bx1, mx2, bx2,1.0, 1.0, 
			sqrt(r_sigma12+r_sigma22)/fabs(p2.z()-p1.z()),
			sqrt(r_sigma12+p1.z()*p1.z()*(r_sigma12+r_sigma22)/
						pow(p2.z()-p1.z(), 2)));
			// errors in mx1 and bx1 are arbitrary since they are
			// not used at a later stage.


	return tang;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD fitCandidate ::
//:::::::::::::::::::::::::
//


MTStraightLine StraightPatRec::fitCandidate(MuonCalibSegment & r_segment,
				const std::vector<unsigned int> & r_selection,
				const MTStraightLine & cand_line) const {

///////////////
// VARIABLES //
///////////////
	Amg::Vector3D null(0.0, 0.0, 0.0);
	Amg::Vector3D xhat(1.0, 0.0, 0.0);

	int num_selected_hits(0);
	for (unsigned int k=0; k<r_selection.size(); k++) {
		if(!r_selection[k]) {
			num_selected_hits++;
		}
	}
	if(num_selected_hits<3) return cand_line;

	Amg::Vector3D init_dir(r_segment.direction());
	Amg::Vector3D init_pos(r_segment.position());
////////////////////////////////////////////
// SET THE CORRECT SIGNED TRACK DISTANCES //
////////////////////////////////////////////

	r_segment.set(r_segment.chi2(), cand_line.positionVector(), cand_line.directionVector());

	for (unsigned int k=0; k<r_segment.mdtHitsOnTrack(); k++) {
		MTStraightLine aux_line(r_segment.mdtHOT()[k]->localPosition(),
							xhat, null, null);
		r_segment.mdtHOT()[k]->setDistanceToTrack(
				cand_line.signDistFrom(aux_line),
				r_segment.mdtHOT()[k]->sigmaDriftRadius());
	}

//////////////////////////////////////////////////////////////////////////////////////////
//Analitical fit of linearized measurements (transformation to reference frame is used) //
//////////////////////////////////////////////////////////////////////////////////////////
	unsigned int NLC=2; //number of fit parameters

//Normalization on z-direction
	Amg::Vector3D dir_norm(r_segment.direction().x()/r_segment.direction().z(),
	r_segment.direction().y()/r_segment.direction().z(), 1.0);

	Amg::Vector3D refTransl(r_segment.position());
	refTransl[0] = 0;
	Amg::Vector3D refDir(dir_norm);
	refDir[0] = 0;
	refDir = refDir.unit();

// Rotation Matrix for Toewr_to_track transformation
	
	double rotAngle = Amg::angle(Amg::Vector3D::UnitZ(),refDir)*(refDir.y())/fabs(refDir.y());
	Amg::AngleAxis3D RotMatr(rotAngle, Amg::Vector3D::UnitX() ); // Matrix for Track reference transformation
	Amg::AngleAxis3D RotMatr_inv(-rotAngle, Amg::Vector3D::UnitX() ); // inverse

	Amg::VectorX alpha = Amg::VectorX(NLC);
	alpha.setZero();
	Amg::VectorX betha = Amg::VectorX(NLC);
	betha.setZero();
	Amg::MatrixX Gamma = Amg::MatrixX(NLC,NLC);
	Gamma.setZero();
// vector to store hit positions
	vector<Amg::Vector3D > hit_position_track;

	for(unsigned int l=0;l<r_segment.mdtHitsOnTrack();l++) {
// Transformation to track reference frame
		Amg::Vector3D WirPosLocal = r_segment.mdtHOT()[l]->localPosition();
		Amg::Vector3D WirPosTrack = WirPosLocal-refTransl;
		WirPosTrack = RotMatr*WirPosTrack;
		double signedDrifRadius = fabs(r_segment.mdtHOT()[l]->driftRadius())*
			(r_segment.mdtHOT()[l]->signedDistanceToTrack()/
			fabs(r_segment.mdtHOT()[l]->signedDistanceToTrack()));
		Amg::Vector3D HitPosTrack = WirPosTrack;
		HitPosTrack[0] = HitPosTrack.y() + signedDrifRadius;
		HitPosTrack[1] = r_segment.mdtHOT()[l]->sigma2DriftRadius(); //trick to store hit resolution

		hit_position_track.push_back(HitPosTrack);

		Amg::VectorX delta = Amg::VectorX(NLC);
		delta.setZero();
		delta[0] = 1.0;
		delta[1] = HitPosTrack.z();

		if(!r_selection[l]) {
			double weight=1.0/(r_segment.mdtHOT()[l]->sigma2DriftRadius());
			Gamma += weight*delta*delta.transpose();
			betha += weight*HitPosTrack.y()*delta;
		}
	}

// solution of linear system of equations
	Gamma = Gamma.inverse();
	alpha = Gamma*betha;

	double refit_chi2(0.0);
	for(unsigned int j=0;j<hit_position_track.size();j++) {
		double res = (alpha[0] + alpha[1]*hit_position_track[j].z()
			- hit_position_track[j].y());
		if(!r_selection[j]) refit_chi2 += res*res/hit_position_track[j].x();
	}

// Backward transformation
	Amg::Vector3D seg_dir(0.0,alpha[1],1.0);
	seg_dir = RotMatr_inv*seg_dir;
	seg_dir[1] = seg_dir.y()/seg_dir.z();
	seg_dir[0] = init_dir.x()/init_dir.z();
	seg_dir[2] = 1.0;

	Amg::Vector3D seg_pos(0.0,alpha[0],0.0);
	seg_pos = RotMatr_inv*seg_pos;
	seg_pos = seg_pos + refTransl;

	seg_pos[1] = seg_pos.y()+seg_dir.y()*(init_pos.z()-seg_pos.z());
	seg_pos[0] = init_pos.x();
	seg_pos[2] = init_pos.z();


// Rewriting segment
	r_segment.set(refit_chi2/static_cast<double>(num_selected_hits-2), seg_pos, seg_dir);

	MTStraightLine aux_line(seg_pos, seg_dir,
			Amg::Vector3D(0.0, 0.0, 0.0), Amg::Vector3D(0.0, 0.0, 0.0));
	
	for (unsigned int k=0; k<r_segment.mdtHitsOnTrack(); k++) {
		MTStraightLine aux_t(r_segment.mdtHOT()[k]->localPosition(), xhat, null, null);
		r_segment.mdtHOT()[k]->setDistanceToTrack(aux_line.signDistFrom(aux_t),
				r_segment.mdtHOT()[k]->sigmaDriftRadius());
	}

	return aux_line;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD roadWidth ::
//::::::::::::::::::::::

double StraightPatRec::roadWidth(void) const {

	return m_road_width;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD numberOfTrackHits ::
//::::::::::::::::::::::::::::::

unsigned int StraightPatRec::numberOfTrackHits(void) const {

	return m_nb_track_hits;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD trackHits ::
//::::::::::::::::::::::

const vector<const MdtCalibHitBase*> & StraightPatRec::trackHits(
								void) const {

	return m_track_hits;

}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD chi2 ::
//:::::::::::::::::

double StraightPatRec::chi2(void) const {

	if (m_nb_track_hits == 0) {
		return -1;
	}
	return m_chi2;

}

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::
//:: METHOD chi2PerDegreesOfFreedom ::
//::::::::::::::::::::::::::::::::::::

double StraightPatRec::chi2PerDegreesOfFreedom(void) const{

	if (m_nb_track_hits<3) {
		return -1;
	}
	return chi2()/static_cast<double>(numberOfTrackHits()-2);


}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD track ::
//::::::::::::::::::

MTStraightLine StraightPatRec::track(void) const {

	return m_track;

}

//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD setRoadWidth ::
//:::::::::::::::::::::::::

void StraightPatRec::setRoadWidth(
					const double & r_road_width) {

	m_road_width = fabs(r_road_width);
	return;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD setTimeOut ::
//:::::::::::::::::::::::

void StraightPatRec::setTimeOut(const double & time_out) {

	m_time_out = time_out;
	return;

}

void StraightPatRec::setFixSelection(bool fix_sel) {
	m_fix_selection = fix_sel;
}

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD fit(.) ::
//:::::::::::::::::::

bool StraightPatRec::fit(MuonCalibSegment & r_segment) const {

// select all hits //
	HitSelection selection(r_segment.mdtHitsOnTrack(), 0);

// call the other fit function //
	return fit(r_segment, selection);

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD fit(.,.) ::
//:::::::::::::::::::::
bool StraightPatRec::fit(MuonCalibSegment & r_segment,

					HitSelection r_selection) const {

	return fitCallByReference(r_segment, r_selection);

}

//*****************************************************************************

bool StraightPatRec::fitCallByReference(MuonCalibSegment & r_segment,

					HitSelection &r_selection) const {

///////////////
// VARIABLES //
///////////////

	time_t start,end; // start and end time (needed for time-out)
	time(&start);
	double diff; // difference of start and end time (needed for time-out)
	Combination combination;

	vector<unsigned int> hit_index; // hit indices for given selection
	unsigned int try_nb_hits; // try this given number of hits for the
				  // segment reconstruction

	vector<const MdtCalibHitBase*> selected_hits;
	vector<unsigned int> selected_hits_index;

	Amg::Vector3D w_min, w_max; // wire with the minimum local z coordinate,
	                         // wire with the maximum local z coordinate
	double r_min, r_max; // corresponding drift CLHEP::radii
	double sigma2_min, sigma2_max; // corresponding spatial resolution

	unsigned int counter1; // auxiliary counter

	unsigned int nb_candidates(0); // number of straight-track candidates

	MTStraightLine tangents[4]; // the four tangents to the drift circles of a
	                            // hit pair
	MTStraightLine aux_track; // auxiliary track
	Amg::Vector3D null(0.0, 0.0, 0.0);
	Amg::Vector3D xhat(1.0, 0.0, 0.0);

	MTStraightLine initial_track(r_segment.position(),
					r_segment.direction(), null, null);
					// initial track stored in the segment
	Amg::Vector3D aux_pos, aux_dir; // auxiliary position and direction vectors					

///////////
// RESET //
///////////

	m_chi2 = -1.0;

	if (r_segment.mdtHitsOnTrack() != r_selection.size()) {
		cerr << "\n"
			<< "Class StraightPatRec, "
			<< "METHOD fit(., .): WARNING!\n"
			<< "Vector with selected hits unequal to the number "
			<< "of hits on the segment!\n"
			<< "The user selection will be ignored!\n";
		r_selection.clear();
		r_selection.assign(r_segment.hitsOnTrack(), 0);
	}

// Filling selected_hits vector
	for (unsigned int k=0; k<r_segment.mdtHitsOnTrack(); k++) {
		if(!r_selection[k]) {
			selected_hits.push_back(r_segment.mdtHOT()[k]);
			selected_hits_index.push_back(k);
		}
	}

///////////////////////////////////////////////////////////////////////////
// RETURN, IF THERE ARE LESS THAN 3 HITS, I.E. THE TRACK IS NOT UNIQUELY //
// DEFINED BY THE HITS.                                                  //
///////////////////////////////////////////////////////////////////////////

	if (selected_hits.size()<3) {
//		cerr << "\n"
//			<< "Class StraightPatRec, "
//			<< "METHOD fit(., .): WARNING!\n"
//			<< "Too few hits for the track reconstructions!\n";
		return false;
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

//////////////////////////
// TRACK RECONSTRUCTION //
//////////////////////////
// try to find a segment with as many hits as selected initially //

	HitSelection final_selection(r_selection);

	try_nb_hits = selected_hits.size();
//=============================================================================
	while(nb_candidates==0) {
//-----------------------------------------------------------------------------
// Return in case of large amount of fake hits
if(try_nb_hits<5 && (selected_hits.size()-try_nb_hits)>2) return false;
// reset //
	double best_chi2ndf = -1.0;
	MTStraightLine	best_aux_line(initial_track);

// loop over all combinations //
	combination.setNewParameters(selected_hits.size(), try_nb_hits);

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	for (unsigned int cb=0; cb<combination.numberOfCombinations(); cb++) {
//.............................................................................

   // time-out //
	   time (&end);
	   diff = difftime (end,start);
	   if (diff>m_time_out) {
//	   	cerr << endl
//	   		<< "Class StraightPatRec: "
//	   		<< "time-out for track finding after "
//			<< m_time_out
//			<< " seconds!\n";
	   	return false;
	   }

   // get the present hit combination //
	   if (cb==0) {
		combination.currentCombination(hit_index);
	   } else {
		combination.nextCombination(hit_index);
	   }

// Current selection of hits
	HitSelection tmp_selection(r_selection);
	tmp_selection.assign(tmp_selection.size(), 1);
	for (unsigned int k=0; k<try_nb_hits; k++) {
		tmp_selection[selected_hits_index[hit_index[k]-1]]=0;
	}

   // investigate the current combination //
	// find the wires with minimum and maximum local z //
	   w_min = selected_hits[hit_index[0]-1]->localPosition();
	   w_max = selected_hits[hit_index[0]-1]->localPosition();
	   r_min = selected_hits[hit_index[0]-1]->driftRadius();
	   r_max = selected_hits[hit_index[0]-1]->driftRadius();
	   sigma2_min = selected_hits[hit_index[0]-1]->sigma2DriftRadius();
	   sigma2_max = selected_hits[hit_index[0]-1]->sigma2DriftRadius();
	   for (unsigned int k=1; k<try_nb_hits; k++) {
		if (selected_hits[hit_index[k]-1]->localPosition().z()<
								w_min.z()) {
			w_min = selected_hits[hit_index[k]-1]->localPosition();
			r_min = selected_hits[hit_index[k]-1]->driftRadius();
			sigma2_min = selected_hits[hit_index[k]-1
							]->sigma2DriftRadius();
		}
	   	if (selected_hits[hit_index[k]-1]->localPosition().z()>
								w_max.z()) {
			w_max = selected_hits[hit_index[k]-1]->localPosition();
			r_max = selected_hits[hit_index[k]-1]->driftRadius();
			sigma2_max = selected_hits[hit_index[k]-1
							]->sigma2DriftRadius();
		}
	   }

	// set the spatial resolution to 0.1 CLHEP::mm if it is 0 //
	   if (sigma2_min==0) {
	   	sigma2_min=0.1;
	   }
	   if (sigma2_max==0) {
	   	sigma2_max=0.1;
	   }

	// get the four segment candidates tangential to the outermost hits //
	   for (unsigned int r_case=1; r_case<5; r_case++) {
		tangents[r_case-1] = tangent(w_min, r_min, sigma2_min,
					     w_max, r_max, sigma2_max, r_case);
	   }

	// determine additional track points within the road width around //
	// the four tangents and determine a segment                      //
	   for (unsigned int r_case=1; r_case<5; r_case++) {
		counter1 = 0;
		for (unsigned int n=0; n<try_nb_hits; n++) {
			MTStraightLine aux_line(
 				selected_hits[hit_index[n]-1]->localPosition(),
				xhat, null, null);
			if (fabs(fabs(tangents[r_case-1].signDistFrom(
						aux_line))-
				selected_hits[hit_index[n]-1]->driftRadius())
								<=m_road_width) {
				counter1 = counter1+1;
			}
		}

// Used to fix initial configuration of hits
		if(m_fix_selection) counter1=try_nb_hits;
// perform a straight line fit for the candidate //
		if (counter1==try_nb_hits) {
			nb_candidates++;
			aux_track = fitCandidate(r_segment, tmp_selection,
						tangents[r_case-1]);

			if ((best_chi2ndf==-1) || (best_chi2ndf>r_segment.chi2())) {
				best_chi2ndf = r_segment.chi2();
				best_aux_line = aux_track;
				final_selection.assign(final_selection.size(),1);
				for (unsigned int j=0; j<try_nb_hits; j++) {
					final_selection[selected_hits_index[hit_index[j]-1]]=0;
				}

			}
		}
	   }
//.............................................................................
	}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//////////////////////////////////////////////////
// MAKE THE FINAL STRAIGHT SEGMENT, IF POSSIBLE //
//////////////////////////////////////////////////

	if (nb_candidates>0) {

   // store track hits, rewrite hit selection //
		m_track_hits.clear();
		for (unsigned int k=0; k<r_selection.size(); k++) {
			r_selection[k] = final_selection[k];
			if(!final_selection[k]) {
				m_track_hits.push_back(r_segment.mdtHOT()[k]);
			}
		}

// Final refit //
		m_track = fitCandidate(r_segment, final_selection,
						best_aux_line);

		m_nb_track_hits = m_track_hits.size();
		m_chi2 = r_segment.chi2()*(static_cast<double>(m_nb_track_hits-2));

		if(m_refine_segment) {
			r_segment.refineMdtSelection(r_selection);
		}

	} else {
		try_nb_hits = try_nb_hits-1;
		if (try_nb_hits<3) {
			return false;
		}
	}
	
//-----------------------------------------------------------------------------
	}
//=============================================================================

	if (std::isnan(m_chi2)) {
		m_chi2 = -1.0;
		return false;
	}

	return true;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD printLevel ::
//:::::::::::::::::::::::

void StraightPatRec::printLevel(int level) {

	cerr << "\n"
		<< "Class StraightPatRec, "
		<< "method printLevel: WARNING!\n"
		<< "Print level " << level << " is ignored.\n";
	return;

}
