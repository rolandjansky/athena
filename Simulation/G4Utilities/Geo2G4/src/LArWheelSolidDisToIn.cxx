/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DistanceToIn stuff for LArWheelSolid
#include <cassert>

#include "GaudiKernel/MsgStream.h"

#include "G4Polycone.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "Geo2G4/LArWheelSolid.h"

G4double LArWheelSolid::DistanceToIn(const G4ThreeVector &inputP) const
{
	static G4ThreeVector p;
	if(BoundingPolycone->Inside(inputP) == kOutside){
// here is an approximation - for the point being outside BoundingPolycone
// the solid looks like solid BoundingPolycone
		return BoundingPolycone->DistanceToIn(inputP);
	}
	p = inputP;
	G4double d = fabs(Calculator->DistanceToTheNearestFan(p)) - FanHalfThickness;
	if(d <= Tolerance) return 0.;
	return d;
}

// inputV should be unit
G4double LArWheelSolid::DistanceToIn(const G4ThreeVector &inputP,
                                     const G4ThreeVector &inputV) const
{
	static G4ThreeVector p, v;
	G4double distance = 0.;
	if(BoundingPolycone->Inside(inputP) == kOutside){
		distance = BoundingPolycone->DistanceToIn(inputP, inputV);
		if(distance == kInfinity) return kInfinity;
		p = inputP + inputV * distance;
		assert(BoundingPolycone->Inside(p) != kOutside);
	} else p = inputP;
	G4double phi0 = p.phi();
	G4double d = Calculator->DistanceToTheNearestFan(p);
	if(FanHalfThickness - fabs(d) > Tolerance) return distance;
	v = inputV;
	v.rotateZ(p.phi() - phi0);
#ifndef DEBUG_LARWHEELSOLID
	return distance + distance_to_in(p, v);
#else
	G4ThreeVector p1(p), v1(v), p0(p), v0(v);
	G4double dd1 = distance_to_in_ref(p1, v1);
	G4double dd = distance_to_in(p, v);
	if(fabs(dd - dd1) > IterationPrecision){
		static int cnt = 0;
		std::cout << "DTI " << p0 << " " << v0 << " "
		          << LArWheelSolidTypeString(Type) << std::endl
		          << "DTI_ref " << dd1 << " DTI " << dd << std::endl;
		std::cout << "DTI MISMATCH " << (dd - dd1) << std::endl;
		Verbose = true;
		p1 = p0; p = p0; v1 = v0; v = v0;
		G4double dd2 = distance_to_in_ref(p1, v1);
		G4double dd3 = distance_to_in(p, v);
		std::cout << " === " << dd2 << " === " << dd3 << " === " << std::endl;
		cnt ++;
//		if(cnt > 10) exit(0);
		Verbose = false;
		if(cnt < 10){
			FILE *F = fopen("test_dti", "a");
			fwrite(&Type, sizeof(Type), 1, F);
			fwrite(&p0, sizeof(p0), 1, F);
			fwrite(&v0, sizeof(v0), 1, F);
			fclose(F);
		}
	}
	bool inf = false;
	if(dd > 10.*m){
		dd = BoundingPolycone->DistanceToOut(p0, v);
		inf = true;
	}
	size_t i = 1;
	for(G4double t = 0.; t < dd; t += IterationPrecision * 10.){
		G4ThreeVector b = p0 + v * t;
		if(FanHalfThickness - fabs(Calculator->DistanceToTheNeutralFibre(b)) > Tolerance){
			if(fabs(t - dd) <= IterationPrecision) break;
			std::cout << "@# " << Type << " " << p << " " << v << " ";
			std::cout << i << " " << (inf? kInfinity: dd) << " " << t << std::endl;
			break;
		}
		i ++;
	}
	if(inf) return kInfinity;
	return(distance + dd);
#endif
}

// This functions should be called in the case when we are sure that
// points p (which sould be OUTSIDE of vertical fan) and out_point have
// the surface of the vertical fan between them.
// returns distance from point p to absorber surface
G4double LArWheelSolid::in_iteration_process(const G4ThreeVector &p,
                                             G4double dist_p,
                                             const G4ThreeVector &out_point)
                                             const
{
#ifdef DEBUG_LARWHEELSOLID
	if(Verbose){
		std::cout << "iip from " << p << " to " << out_point
		          << " dir " << (out_point - p).unit() << std::endl;
	}
#endif
	static G4ThreeVector A, B, C, diff;
	A = p;
	B = out_point;
	G4double dist_c;
	unsigned int niter = 0;
//  assert(fabs(Calculator->DistanceToTheNeutralFibre(A)) - FanHalfThickness > Tolerance);
//  assert(Calculator->DistanceToTheNeutralFibre(A) == dist_p);
	do {
		C = A + B;
		C *= 0.5;
		dist_c = Calculator->DistanceToTheNeutralFibre(C);
		if(dist_c * dist_p < 0
		|| (FanHalfThickness - fabs(dist_c) > Tolerance)){
			B = C;
		} else {
			A = C;
		}
		niter ++;
		diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
	assert(niter < IterationsLimit);
	assert(fabs(Calculator->DistanceToTheNeutralFibre(B)) - FanHalfThickness < Tolerance);
/*std::cout << "diff^2 " << diff.mag2() << " " << IterationPrecision2;
std::cout << " dist_c " << dist_c << " " << (FanHalfThickness - fabs(dist_c)) << " tol " << Tolerance
<< std::endl;*/
	diff = p - B;
#ifdef DEBUG_LARWHEELSOLID
	if(Verbose){
		std::cout << "iip result in " << niter << " = " << B
		          << " " << diff.mag() << std::endl;
	}
#endif
	return diff.mag();
}

// search for the nearest to the neutral fibre of the vertical fan point
// on the segment between p_in and p_out
G4double LArWheelSolid::search_for_nearest_point(const G4ThreeVector &p_in,
                                          const G4ThreeVector &p_out) const
{
	static G4ThreeVector A, B, C, l, diff;
	A = p_in;
	B = p_out;
	diff = B - A;
	l = diff.unit() * IterationPrecision;
  // this is to correctly take the sign of the distance into account
	G4double dist_p_in = Calculator->DistanceToTheNeutralFibre(p_in);
	G4double sign = dist_p_in < 0.? -1. : 1.;
	G4double d_prime;
	G4double dist_c;
	unsigned long niter = 0;
	do{
		C = A + B;
		C *= 0.5;
		dist_c = Calculator->DistanceToTheNeutralFibre(C);
		if(dist_c * sign <= 0.){ // we are in coditions for in_iteration_process
			return in_iteration_process(p_in, dist_p_in, C);
		}
	  // calculate sign of derivative of distance to the neutral fibre
//		d_prime = (Calculator->DistanceToTheNeutralFibre(C + l) -
//		           Calculator->DistanceToTheNeutralFibre(C - l)) * sign;
// hope this substitution is acceptable
		diff = C - l;
		d_prime = (dist_c - Calculator->DistanceToTheNeutralFibre(diff)) * sign;
		if(d_prime < 0.) A = C;
		else B = C;
		niter ++;
		diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
/*	if(niter >= IterationsLimit){
		G4cout << p_in << " " << p_out << G4endl;
		G4cout << A << " " << B << " " << diff.mag() << " "
		<< dist_p_in << " " << dist_c << " " << sign << " "
		<< G4endl;
	}
*/	assert(niter < IterationsLimit);
 	if(FanHalfThickness - fabs(dist_c) > Tolerance){
		return in_iteration_process(p_in, dist_p_in, C);
	}
//  let's check at p_in and p_out
	if(dist_p_in * sign < dist_c * sign){
		C = p_in;
		dist_c = dist_p_in;
	}
	G4double dist_p_out = Calculator->DistanceToTheNeutralFibre(p_out);
	if(dist_p_out *sign < dist_c * sign) C = p_out;
 	if(FanHalfThickness - fabs(dist_p_out) > Tolerance){
		return in_iteration_process(p_in, dist_p_in, C);
	}
	return kInfinity;
}

#ifdef LArWheelSolidDTI_NEW
G4double LArWheelSolid::distance_to_in_ref
#else
G4double LArWheelSolid::distance_to_in
#endif
(G4ThreeVector &p, const G4ThreeVector &v) const
{
	static G4ThreeVector out_point;
#ifdef DEBUG_LARWHEELSOLID
	G4ThreeVector p0 = p;
	if(Verbose) std::cout << "dti old" << std::endl;
#endif
	G4int sign, start, stop;
	if(v[2] < 0.){ sign = -1; start = MaxFanSection; stop = -1; }
	else { sign = 1; start = 0; stop = MaxFanSection + 1; }
	G4double distance = 0., result = kInfinity;
	G4double dist_p = 0.;
	for(G4int i = start; i != stop; i += sign){
#ifdef DEBUG_LARWHEELSOLID
		if(Verbose){
			std::cout << "dti old at " << i << " " << p << " d = "
			          << distance << std::endl;
		}
#endif
		if(FanSection[i]->Inside(p) == kOutside){
			G4double d1 = FanSection[i]->DistanceToIn(p, v);
//			if(d1 < kInfinity){ // crossed this FS
// I guess here is again bug in G4Polycone. Assertion below failed sometimes.
// Let's try to work it around.
			if(d1 < 10.*CLHEP::m){ // crossed this FS
				distance += d1;
				p += v * d1;
			} else {
#ifdef DEBUG_LARWHEELSOLID
				if(d1 < kInfinity){
					(*msg) << MSG::ERROR << "DistanceToIn(p, v):" << G4endl
					       << "FanSection[" << i << "]->DistanceToIn(" << MSG_VECTOR(p)
						   << ", " << MSG_VECTOR(v) << ") = " << d1 << endreq;
				}
#endif
				continue;
			}
		}
		assert(FanSection[i]->Inside(p) != kOutside);
		dist_p = Calculator->DistanceToTheNeutralFibre(p);
		if(FanHalfThickness - fabs(dist_p) > Tolerance){
			result = distance;
			break;
		}

		G4double dist_from_p_to_out = FanSection[i]->DistanceToOut(p, v);

/* workaround for bug in G4Polycone  */
		if(dist_from_p_to_out > 10 * CLHEP::m){
#ifdef DEBUG_LARWHEELSOLID
			(*msg) << MSG::WARNING
			       << "distance_to_in: workaround for a 'bug' in G4Polycone "
			       << " at " << MSG_VECTOR(p) << ": "
				   << dist_from_p_to_out << endreq;
#endif
			return kInfinity;
		}

		out_point = p + v * dist_from_p_to_out;

		G4double dist_out = Calculator->DistanceToTheNeutralFibre(out_point);
#ifdef DEBUG_LARWHEELSOLID
		if(Verbose){
			std::cout << ">" << p << " " << dist_p << " " << out_point
			          << " " << dist_out << std::endl
			          << "out_point r " << out_point.perp() << " phi "
			          << out_point.phi() << std::endl;
		}
#endif
		if(dist_p * dist_out < 0.){// it certanly cross current half-wave
			result = distance + in_iteration_process(p, dist_p, out_point);
			break;
		}
		G4double d2 = search_for_nearest_point(p, out_point);
		if(d2 < kInfinity){
			result = distance + d2; // this half-wave is intersected
			break;
		}
		p = out_point;
		distance += dist_from_p_to_out;
	}
#ifdef DEBUG_LARWHEELSOLID
	if(Verbose) std::cout << "dti old end " << result << std::endl;
#endif
	return result;
}


// this is obsolete version of distance_to_in
// it is kept for information
// calculates distance from point p along vector v to the nearest surface of
// the vertical fan
G4double LArWheelSolid::distance_to_in(const G4ThreeVector &p,
                                       const G4double dist_p,
                                       const G4ThreeVector &v) const
{
	static G4ThreeVector out_point, A, B;
	static G4int warning = 0;

	G4int fan_section = select_fan_section(p.z());
	assert(fan_section >= 0);
	assert(fan_section <= MaxFanSection);
	G4Polycone *current_section = FanSection[fan_section];
//  assert(current_section->Inside(p) != kOutside);

	if(current_section->Inside(p) == kOutside){
#ifdef DEBUG_LARWHEELSOLID
		(*msg) << MSG::WARNING
		       << "#" << (warning ++) << " from istance_to_in:" << G4endl
			   << "current_section->Inside(p) == kOutside!!!" << G4endl
			   << "  point: " << MSG_VECTOR(p) << ", phi = " << p.phi()
			   << ", r = " << p.r()
			   << "  DTI: " << current_section->DistanceToIn(p)
			   << ", DTNeaFan: " << dist_p
			   << "  fs parametes:" << G4endl
			   << "\tphi borders: " << current_section->GetStartPhi()
			   << ", " << current_section->GetEndPhi()
			   << endreq;
#endif
		if(warning >= 5){
			(*msg) << MSG::WARNING << "iterations are stopped" << endreq;
			return kInfinity;
		}
		G4double d = current_section->DistanceToIn(p, v);
		(*msg) << MSG::WARNING << "  DTI(p, v) = " << d << endreq;
		if(d < kInfinity){
			G4ThreeVector p1 = p + d * v;
			d += distance_to_in(p1, dist_p, v);
		}
		return d;
	}
	warning = 0;
/**/
// determine where vector v cross current polycone section
	G4double dist_from_p_to_out = current_section->DistanceToOut(p, v);
	out_point = p + v * dist_from_p_to_out;
	G4double dist_out = Calculator->DistanceToTheNeutralFibre(out_point);
	if(dist_p * dist_out < 0){// it certanly cross current half-wave
		return in_iteration_process(p, dist_p, out_point);
	}
	G4double distance = search_for_nearest_point(p, out_point);
	if(distance < kInfinity) return distance; // this half-wave is intersected
// another section or no intersection at all
	G4double z_min = FanSectionLimits[fan_section] + Tolerance;
	G4double z_max = FanSectionLimits[fan_section + 1] - Tolerance;
	if((out_point.z() >= z_max && fan_section < MaxFanSection)
	|| (out_point.z() <= z_min && fan_section != 0))
	{
	  // another section is crossed, no escape from bounding
		distance = dist_from_p_to_out;
	  // select direction
		if(v.z() < 0.) current_section = FanSection[fan_section - 1];
		else current_section = FanSection[fan_section + 1];
		A = out_point + v * current_section->DistanceToOut(out_point, v);
		G4double side_a = Calculator->DistanceToTheNeutralFibre(A);
		if(dist_p * side_a < 0.){
			distance += in_iteration_process(out_point, dist_out, A);
		} else {
			G4double dd = search_for_nearest_point(out_point, A);
			if((  (fan_section == 0 && A.z() >= FanSectionLimits[2])
			   || (fan_section == MaxFanSection && A.z() <=
			       FanSectionLimits[MaxFanSection - 1]))
			&& !(dd < kInfinity))
			{
				if(fan_section == 0) B = A + v * FanSection[2]->DistanceToOut(A, v);
				else B = A + v * FanSection[fan_section - 2]->DistanceToOut(A, v);
				dd += search_for_nearest_point(A, B);
			}
			distance += dd;
		}
	}
	return distance;
}

#ifdef DEBUG_LARWHEELSOLID

G4double LArWheelSolid::in_chord_method(
	const G4ThreeVector &p0, const G4ThreeVector &p1,
	const G4ThreeVector &v) const
{
#if 0
if(Verbose) std::cout << "icm: " << p0 << " " << v << std::endl;
	const G4double d0 = Calculator->DistanceToTheNeutralFibre(p0);
	const G4double ht = d0 < 0.? FanHalfThickness: (-FanHalfThickness);
	G4double f = d0 + ht;
if(Verbose) std::cout << "at p0: " << f << std::endl;

{
	FILE *F = fopen("test_icm", "r");
	if(F == 0){
		F = fopen("test_icm", "w");
		for(double x = 0.; x < (p1 - p0).mag(); x += IterationPrecision){
			G4ThreeVector pi = p0 + v * x;
			fprintf(F, "%e %e\n", x, Calcuator->DistanceToTheNeutralFibre(pi) + ht);
		}
		std::cout << "Vz = " << v.z() << std::endl;
		fclose(F);
	} else fclose(F);
}
	G4double F = Calculator->DistanceToTheNeutralFibre(p1) + ht;
	G4double T = (p1 - p0).mag();
if(Verbose) std::cout << "at p1: T " << T << " " << " F " << F << std::endl;
	G4double t = 0.;
	for(size_t i = 0; i < IterationsLimit; ++ i){
		const G4double dt = f * (t - T) / (f - F);
if(Verbose) std::cout << "at " << i << ": t " << t << " dt " << dt << " f " << f << std::endl;
		if(fabs(dt) < IterationPrecision){
			return t;
		}
		if(std::signbit(f) != std::signbit
		t -= dt;
		f = Calculator->DistanceToTheNeutralFibre(p0 + v * t) + ht;
	}
#endif
	return 0.;
}
#endif
