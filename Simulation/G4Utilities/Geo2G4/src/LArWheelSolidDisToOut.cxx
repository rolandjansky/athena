/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSolid.h"
#include "LArFanSection.h"

G4double LArWheelSolid::DistanceToOut(const G4ThreeVector &inputP) const
{
	LWSDBG(1, std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP) << std::endl);
	if(BoundingShape->Inside(inputP) != kInside){
		LWSDBG(2, std::cout << "DistanceToOut(p):"
                            << " point " << MSG_VECTOR(inputP)
                            << " is not inside of the BoundingShape."
                            << std::endl);
		return 0.;
	}
	G4ThreeVector p( inputP );
	int p_fan = 0;
	const G4double d = FanHalfThickness - fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
	if(d < Tolerance){
		LWSDBG(2, std::cout << "already not inside " << MSG_VECTOR(p) << std::endl);
		return 0.;
	}
	const G4double d0 = BoundingShape->DistanceToOut(inputP);
	LWSDBG(2, std::cout << "dto " << d << " " << d0 << std::endl);
	if(d > d0) return d0;
	else return d;
}

G4double LArWheelSolid::DistanceToOut(const G4ThreeVector &inputP,
                                      const G4ThreeVector &inputV,
                                      const G4bool calcNorm,
                                      G4bool* validNorm,
                                      G4ThreeVector* sn) const
{
	LWSDBG(1, std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP)
	                    << MSG_VECTOR(inputV) << std::endl);

	const EInside inside_BS = BoundingShape->Inside(inputP);
	if(inside_BS == kOutside){
		LWSDBG(2, std::cout << "DistanceToOut(p):"
                            << " point " << MSG_VECTOR(inputP)
                            << " is outside of BoundingShape." << std::endl);
		if(calcNorm) *validNorm = false;
		return 0.;
	}

	// If here inside or on surface of BS
	G4ThreeVector p(inputP);
	int p_fan = 0;
	const G4double adtnf_p = fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
	if(adtnf_p >= FHTplusT) {
		LWSDBG(2, std::cout << "DistanceToOut(p, v): point "
	                        << MSG_VECTOR(inputP)
	                        << " is outside of solid." << std::endl);
		if(calcNorm) *validNorm = false;
		return 0.;
	}

	G4ThreeVector v(inputV);
	const G4double phi0 = p.phi() - inputP.phi();
	v.rotateZ(phi0);

#ifdef CHECK_DIRTONORM_ANGLE_ON_SURFACE
	if(adtnf_p < FHTminusT) {
		LWSDBG(5, std::cout << "inside fan point " << MSG_VECTOR(inputP) << ", FHTminusT=" << FHTminusT << std::endl);
	} else {
		LWSDBG(5, std::cout << "on fan surface adtnf_p=" << adtnf_p << ", FHTplusT=" << FHTplusT << ", FHTminusT=" << FHTminusT << std::endl);

		const G4ThreeVector d = GetCalculator()->NearestPointOnNeutralFibre(p, p_fan);
		// check dot product between normal and v
		if ( (p-d).cosTheta(v) > AngularTolerance ) {
			// direction "v" definitely pointing outside
			// return 0.0
			return 0.;
		}
	}
#endif

// former distance_to_out starts here
	LWSDBG(4, std::cout << "dto: " << MSG_VECTOR(p) << " "
	                    << MSG_VECTOR(v) << std::endl);

	G4ThreeVector q(p);
#ifdef LARWHEELSOLID_USE_BS_DTO
	const G4double dto_bs = BoundingShape->DistanceToOut(
		inputP, inputV, calcNorm, validNorm, sn
	);
	q = p + v * dto_bs;
	if(q.y() < Ymin){
		LWSDBG(5, std::cout << "dto exit point too low " << MSG_VECTOR(q) << std::endl);
		const G4double dy = (Ymin - p.y()) / v.y();
		q.setX(p.x() + v.x() * dy);
		q.setY(Ymin);
		q.setZ(p.z() + v.z() * dy);
	}
#else
	FanBoundExit_t exit = find_exit_point(p, v, q);
	LWSDBG(5, std::cout << "dto exit " << exit << std::endl);
#endif
	LWSDBG(5, std::cout << "dto exit point " << MSG_VECTOR(q) << std::endl);

	G4double distance = 0.;
	G4int start = select_section(p.z());
	G4int stop = select_section(q.z());
	G4int step = -1;
	if(stop > start){ step = 1;	start ++; stop ++; }
	LWSDBG(5, std::cout << "dto sections " << start << " " << stop << " " << step << std::endl);

	G4double tmp;
	G4ThreeVector p1, C;

	for(G4int i = start; i != stop; i += step){
		const G4double d1 = (Zsect[i] - p.z()) / v.z();
// v.z() can't be 0, otherwise start == stop, so the exit point could be only
// at z border of the fan section
		LWSDBG(5, std::cout << "at " << i << " dist to zsec = " << d1 << std::endl);
		const G4double x1 = p.x() + v.x() * d1, y1 = p.y() + v.y() * d1;
		p1.set(x1, y1, Zsect[i]);
		const G4double dd = fabs(GetCalculator()->DistanceToTheNeutralFibre(p1, p_fan));
		if(dd > FHTplusT){
			tmp = out_iteration_process(p, p1, p_fan);
			//while(search_for_most_remoted_point(p, out_section, C)){
			if(search_for_most_remoted_point(p, p1, C, p_fan)){
				tmp = out_iteration_process(p, C, p_fan);
			}
			distance += tmp;
#ifndef LARWHEELSOLID_USE_BS_DTO
			exit = NoCross;
#endif
			goto end_dto;
		}
		if(search_for_most_remoted_point(p, p1, C, p_fan)){
			distance += out_iteration_process(p, C, p_fan);
#ifndef LARWHEELSOLID_USE_BS_DTO
			exit = NoCross;
#endif
			goto end_dto;
		}
		distance += d1;
		p.set(x1, y1, Zsect[i]);
	}

	if(fabs(GetCalculator()->DistanceToTheNeutralFibre(q, p_fan)) > FHTplusT){
		LWSDBG(5, std::cout << "q=" << MSG_VECTOR(q) << " outside fan cur distance=" << distance << ", FHTplusT=" << FHTplusT << std::endl);
		tmp = out_iteration_process(p, q, p_fan);
#ifndef LARWHEELSOLID_USE_BS_DTO
		exit = NoCross;
#endif
    } else {
		tmp = (q - p).mag();
	}
	//while(search_for_most_remoted_point(out, out1, C, p_fan)){
	if(search_for_most_remoted_point(p, q, C, p_fan)){
		tmp = out_iteration_process(p, C, p_fan);
#ifndef LARWHEELSOLID_USE_BS_DTO
		exit = NoCross;
#endif
	}
	distance += tmp;
// former distance_to_out ends here
  end_dto:
	LWSDBG(5, std::cout << "At end_dto distance=" << distance  << std::endl);
#ifdef LARWHEELSOLID_USE_BS_DTO
	if(calcNorm && distance < dto_bs) *validNorm = false;
#else
	if(calcNorm){
		LWSDBG(5, std::cout << "dto exit1 " << exit << std::endl);
		switch(exit){
			case ExitAtBack:
				sn->set(0., 0., 1.);
				*validNorm = true;
				break;
			case ExitAtFront:
				sn->set(0., 0., -1.);
				*validNorm = true;
				break;
			case ExitAtOuter:
				q.rotateZ(-phi0);
				sn->set(q.x(), q.y(), 0.);
				if(q.z() <= Zmid) sn->setZ(- q.perp() * m_fs->Amax);
				sn->setMag(1.0);
				*validNorm = true;
				break;
			default:
				*validNorm = false;
				break;
		}
	}
#endif

#ifdef DEBUG_LARWHEELSOLID
	if(Verbose > 2){
		if(Verbose > 3){
			std::cout << MSG_VECTOR(inputP)
			          << " " << MSG_VECTOR(inputV) << std::endl;
		}
		std::cout << "DTO: " << distance << " ";
		if (validNorm) {
		  std::cout << *validNorm << " " << MSG_VECTOR((*sn));
		} else {
		  std::cout << "Norm is not valid";
		}
		std::cout << std::endl;

		if(Verbose > 3){
			G4ThreeVector p2 = inputP + inputV * distance;
			EInside i = Inside(p2);
			std::cout << "DTO hit at " << MSG_VECTOR(p2) << ", "
			          << inside(i) << std::endl;
		}
	}
#ifdef LWS_HARD_TEST_DTO
	if(test_dto(inputP, inputV, distance)){
		if(Verbose == 1){
			std::cout << TypeStr() << " DisToOut" << MSG_VECTOR(inputP)
	                  << MSG_VECTOR(inputV) << std::endl;
		}
	}
#endif
#endif
	return distance;
}

// This functions should be called in the case when we are sure that
// point p is NOT OUTSIDE of vertical fan and point out is NOT INSIDE vertical fan
// returns distance from point p to fan surface, sets last
// parameter to the found point
// may give wrong answer - see description
G4double LArWheelSolid::out_iteration_process(const G4ThreeVector &p,
                                              G4ThreeVector &B, const int p_fan) const
{
	LWSDBG(6, std::cout << "oip: " << p << " " << B);
	assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(p, p_fan)) < FHTplusT);
	assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(B, p_fan)) > FHTminusT);
	G4ThreeVector A(p), C, diff;
	unsigned int niter = 0;
	do {
		C = A + B;
		C *= 0.5;
		if(fabs(GetCalculator()->DistanceToTheNeutralFibre(C, p_fan)) < FHTplusT){
			A = C;
		} else {
			B = C;
		}
		niter ++;
		diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
	assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(B, p_fan)) > FHTplusT);
	assert(niter < IterationsLimit);
	diff = p - B;
	LWSDBG(7, std::cout << " -> " << B << " " << diff.mag());
	LWSDBG(6, std::cout << std::endl);
	return diff.mag();
}

// returns true if the point being outside vert. fan is found, also set C to
// that point in this case
// returns false if the whole track looks like being inside vert. fan
G4bool LArWheelSolid::search_for_most_remoted_point(
	const G4ThreeVector &a, const G4ThreeVector &b, G4ThreeVector &C, const int p_fan
) const
{
	LWSDBG(6, std::cout << "sfmrp " << a << " " << b << std::endl);
	G4ThreeVector diff(b - a);

	if(diff.mag2() <= IterationPrecision2) return false;
	G4ThreeVector A(a), B(b), l(diff.unit() * IterationPrecision);
  // find the most remoted point on the line AB
  // and check if it is outside vertical fan
  // small vector along the segment AB
	G4double d1, d2;
	unsigned int niter = 0;
  // searching for maximum of (GetCalculator()->DistanceToTheNeutralFibre)^2 along AB
	do {
		C = A + B;
		C *= 0.5;
		d1 = GetCalculator()->DistanceToTheNeutralFibre(C, p_fan);
		if(fabs(d1) > FHTplusT){
      // here out_iteration_process gives the answer
			LWSDBG(7, std::cout << "sfmrp -> " << C << " " << fabs(d1)
                                << "  " << (C - a).unit() << " "
                                << (C - a).mag() << std::endl);
			return true;
		}
    // sign of derivative
    //d1 = GetCalculator()->DistanceToTheNeutralFibre(C + l);
		d2 = GetCalculator()->DistanceToTheNeutralFibre(C - l, p_fan);
		if(d1 * d1 - d2 * d2 > 0.) A = C;
		else B = C;
		niter ++;
		diff = A - B;
	} while(diff.mag2() > IterationPrecision2 && niter < IterationsLimit);
  // the line entirely lies inside fan
	assert(niter < IterationsLimit);
	return false;
}
