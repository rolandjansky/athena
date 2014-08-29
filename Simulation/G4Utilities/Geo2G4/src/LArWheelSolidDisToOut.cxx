/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// November 2001, A. Soukharev
// LArWheelSolidDisToOut.cc (see also LArWheelSolid*.cc)
// DistanceToOut functions and stuff for them

#include <cassert>

#include "GaudiKernel/MsgStream.h"

#include "G4Polycone.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "Geo2G4/LArWheelSolid.h"

G4double LArWheelSolid::DistanceToOut(const G4ThreeVector &inputP) const
{
	static G4ThreeVector p;
	if(BoundingPolycone->Inside(inputP) == kOutside){
#ifdef DEBUG_LARWHEELSOLID
		(*msg) << MSG::VERBOSE << "DistanceToOut(p):"
		       << " point " << MSG_VECTOR(inputP)
		       << " is outside of BoundingPolycone."
		       << endreq;
#endif // DEBUG_LARWHEELSOLID
		return 0.;
	}
	G4double d0 = BoundingPolycone->DistanceToOut(inputP);
	p = inputP;
	G4double d = FanHalfThickness - fabs(Calculator->DistanceToTheNearestFan(p));
	if(d < Tolerance) return 0.;
	if(d > d0) return d0;
	else return d;
}

G4double LArWheelSolid::DistanceToOut(const G4ThreeVector &inputP,
                                      const G4ThreeVector &inputV,
                                      const G4bool calcNorm,
                                      G4bool* validNorm,
                                      G4ThreeVector* ) const
{
	if(calcNorm && validNorm != 0) *validNorm = false;
	if(BoundingPolycone->Inside(inputP) == kOutside){
#ifdef DEBUG_LARWHEELSOLID
	    (*msg) << MSG::VERBOSE << "DistanceToOut(p, v):"
		       << " point " << MSG_VECTOR(inputP)
		       << " is outside of BoundingPolycone."
		       << endreq;
#endif // DEBUG_LARWHEELSOLID
		return 0.;
	}
	static G4ThreeVector p, v;
	p = inputP;
	if(fabs(Calculator->DistanceToTheNearestFan(p)) - FanHalfThickness < Tolerance){
		v = inputV;
		v.rotateZ(p.phi() - inputP.phi());
		return distance_to_out(p, v);
	}
	if(msg->level() == MSG::VERBOSE)
	  (*msg) << MSG::VERBOSE << "DistanceToOut(p, v):"
		 << " point " << MSG_VECTOR(inputP)
		 << " is outside of solid."
		 << endreq;
	return 0.;
}

// calculates the distance from point p along vector v to the nearest
// surface of vertical fan
G4double LArWheelSolid::distance_to_out(const G4ThreeVector &p,
					                              const G4ThreeVector &v) const
{
	static G4ThreeVector out_section, out, diff, C;

	G4int fan_section = select_fan_section(p.z());
	assert(FanSection[fan_section]->Inside(p) != kOutside);
	G4double dist_from_p_to_out_section =
                             FanSection[fan_section]->DistanceToOut(p, v);
/* workaround for bug in G4Polycone  */
	if(dist_from_p_to_out_section > 10 * CLHEP::m){
		(*msg) << MSG::WARNING
		       << "distance_to_out: workaround for a 'bug' in G4Polycone "
		       << " at " << MSG_VECTOR(p) << ": "
			   << dist_from_p_to_out_section << endreq;
		return 0;
	}

	out_section = p + v * dist_from_p_to_out_section;
	G4bool outside = (fabs(Calculator->DistanceToTheNeutralFibre(out_section))
                     - FanHalfThickness > Tolerance);
	G4double dist_from_p_to_out;
	if(outside){
	// we are in the conditions for out_iteration_process,
		dist_from_p_to_out = out_iteration_process(p, out_section);
		out = p + v * dist_from_p_to_out;
	// but we are not sure the answer being right (see description)
	// so make fruther calculations
	} else {
		out = out_section;
		dist_from_p_to_out = dist_from_p_to_out_section;
	}

	if(search_for_most_remoted_point(p, out, C)){
		return out_iteration_process(p, C);
	}
// if initial exiting (from FanSection) point was outside fan,
// out_iteration_process had given correct result in the very beginning of the
// procedure. Let's remember and return it.
	G4double distance = dist_from_p_to_out;
	if(outside)	return distance;
// otherwise move search into neighbours
	G4double z_min = FanSectionLimits[fan_section] + Tolerance;
	G4double z_max = FanSectionLimits[fan_section + 1] - Tolerance;
	if((out.z() >= z_max && fan_section < MaxFanSection)
	|| (out.z() <= z_min && fan_section > 0)){ // another section
	  // select direction
		G4int next_fan_section = (v.z() < 0.)? fan_section - 1: fan_section + 1;
		G4double d = FanSection[next_fan_section]->DistanceToOut(out, v);
		static G4ThreeVector out_next_fs;
		out_next_fs = out + v * d;
		if(fabs(Calculator->DistanceToTheNeutralFibre(out_next_fs)) - FanHalfThickness
            < Tolerance)
		{
			if(search_for_most_remoted_point(out, out_next_fs, C)){
				distance += out_iteration_process(out, C);
			} else distance += d;
		} else {
			G4double d1 = out_iteration_process(out, out_next_fs);
			static G4ThreeVector out1;
			out1 = out + v * d1;
			if(search_for_most_remoted_point(out, out1, C)){
				distance += out_iteration_process(out, C);
			} else distance += d1;
		}
	}
	return distance;
}

// This functions should be called in the case when we are sure that
// point p is NOT OUTSIDE of vertical fan and point out is NOT INSIDE vertical fan
// returns distance from point p to fan surface
// may give wrong answer - see description
G4double LArWheelSolid::out_iteration_process(const G4ThreeVector &p,
                                              const G4ThreeVector &out) const
{
	assert(fabs(Calculator->DistanceToTheNeutralFibre(p)) - FanHalfThickness < Tolerance);
	assert(fabs(Calculator->DistanceToTheNeutralFibre(out)) - FanHalfThickness > -Tolerance);
	static G4ThreeVector A, B, C, diff;
	A = p;
	B = out;
	unsigned int niter = 0;
	do{
		C = A + B;
		C *= 0.5;
		if(fabs(Calculator->DistanceToTheNeutralFibre(C)) - FanHalfThickness
		< Tolerance) A = C;
		else B = C;
		niter ++;
		diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
	assert(fabs(Calculator->DistanceToTheNeutralFibre(B)) - FanHalfThickness > Tolerance);
/*
if(niter >= IterationsLimit){
G4cout << p << " " << Calculator->DistanceToTheNeutralFibre(p) << " "
<< out << " " << Calculator->DistanceToTheNeutralFibre(out) << G4endl;
G4cout << (p - out).mag() << " ";
G4cout << diff.mag();
G4cout << G4endl;
}
*/
	assert(niter < IterationsLimit);
	diff = p - B;
	return diff.mag();
}

// returns true if the point being outside vert. fan is found, also set C to
// that point in this case
// returns false if the whole track looks like being inside vert. fan
G4bool LArWheelSolid::search_for_most_remoted_point(const G4ThreeVector &a,
                                                    const G4ThreeVector &b,
                                                    G4ThreeVector &C) const
{
	static G4ThreeVector A, B, diff, l;
	diff = b - a;
	if(diff.mag2() <= IterationPrecision2) return false;
	A = a;
	B = b;
	l = diff.unit() * IterationPrecision;
// find the most remoted point on the line AB
// and check if it is outside vertical fan
// small vector along the segment AB
	G4double d1, d2;
	unsigned int niter = 0;
// searching for maximum of (Calculator->DistanceToTheNeutralFibre)^2 along AB
	do{
		C = A + B;
		C *= 0.5;
		d1 = Calculator->DistanceToTheNeutralFibre(C);
		if(fabs(d1) - FanHalfThickness > Tolerance){
		 // here out_iteration_process gives the definitive answer
			return true;
		}
 	 // sign of derivative
//		d1 = Calculator->DistanceToTheNeutralFibre(C + l);
		d2 = Calculator->DistanceToTheNeutralFibre(C - l);
		if(d1 * d1 - d2 * d2 > 0.) A = C;
		else B = C;
		niter ++;
    diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
// the line entirely lies inside fan
	assert(niter < IterationsLimit);
	return false;
}
