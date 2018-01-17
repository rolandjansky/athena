/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArWheelSolid.h"
#include "LArFanSection.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include<iostream>


void LArFanSections::print(void) const
{
  std::cout << "LArFanSections at " << this << std::endl
            << "Amin = " << Amin << ", Amax = " << Amax
            << std::endl
            << "Bmin = " << Bmin << ", Bmax = " << Bmax << std::endl
            << "xmin = " << xmin << ", xmax = " << xmax
            << "Cflat2 = " << Cflat2 << std::endl;
}

LArFanSections::LArFanSections(
	G4double ri1, G4double ri2, G4double ro1, G4double ro2,
	G4double Xmax, G4double z1, G4double z2
)
{
	const G4double dz = z2 - z1;
	Amin = (ri2 - ri1) / dz;
	Amax = (ro2 - ro1) / dz;
	Bmin = ri1 - Amin * z1;
	Bmax = ro1 - Amax * z1;
	Cflat2 = ro2*ro2;

	Amin2 = Amin*Amin; Amax2 = Amax*Amax;
	Bmin2 = Bmin*Bmin; Bmax2 = Bmax*Bmax;
	ABmin = Amin*Bmin; ABmax = Amax*Bmax;

	xmax = Xmax;
	xmin = -xmax;
}

G4bool LArWheelSolid::check_D(
	G4double &t1, G4double A, G4double B, G4double C, G4bool out
) const
{
	// G4bool out is to be set true if the point is surface-outside
    // then have to discard first intersection

	const G4double D = B*B - A*C;
	LWSDBG(8, std::cout << "check D=" << D << " out=" << out << std::endl);
	if(D < 0.) return false;

	const G4double D1 = sqrt(D);
	t1 = (-B + D1) / A;
	const G4double t2 = (-B - D1) / A;
	LWSDBG(8, std::cout << "t1=" << t1 << " t2=" << t2 << std::endl);
	if(t1 > 0.){
		if(t2 > 0.){
			if(out){
				if(t2 > t1) t1 = t2;
			} else {
				if(t2 < t1) t1 = t2;
			}
		} else if(t2 < 0.){
			if(out) return false;
		} else { // answer is t1
		}
	} else if(t1 < 0.){
		if(t2 > 0.){
			if(out) return false;
			t1 = t2;
		} else if(t2 < 0.){
			return false;
		} else {
			return false;
		}
	} else {
		if(t2 > 0.){
			t1 = t2;
		} else if(t2 < 0.){
			return false;
		} else {
			return false;
		}
	}
	return true;
}

// p must be not outside of the "FanBound"
// if track crosses inner cone in valid (z, x) interval,
// returns true, sets q to the cross point
bool LArWheelSolid::fs_cross_lower(
	const G4ThreeVector &p, const G4ThreeVector &v,
	G4ThreeVector &q) const
{
	LWSDBG(7, std::cout << "fcl" << std::endl);
	const G4double A = v.perp2() - m_fs->Amin2*v.z()*v.z();
	const G4double B = p.x()*v.x() + p.y()*v.y()
                     - m_fs->Amin2*p.z()*v.z() - m_fs->ABmin*v.z();
	const G4double C = p.perp2() - m_fs->Amin2*p.z()*p.z()
                     - 2.*m_fs->ABmin*p.z() - m_fs->Bmin2;
	G4double t1(0.0);
	const G4double out_dist = m_fs->Amin*p.z() + m_fs->Bmin - p.perp();
	LWSDBG(8, std::cout << "fcl out_dist(p)=" << out_dist << " Tolerance=" << s_Tolerance << std::endl);
	const G4bool out = out_dist >= 0.0;
	if(check_D(t1, A, B, C, out)){
		const G4double zz1 = p.z() + v.z() * t1;
		if(zz1 < m_Zsect.front() || zz1 > m_Zsect.back()){
			LWSDBG(8, std::cout << "fcl out on Z " << zz1 << std::endl);
			return false;
		}
		const G4double xx1 = p.x() + v.x() * t1;
		if(xx1 < m_fs->xmin || xx1 > m_fs->xmax){
			LWSDBG(8, std::cout << "fcl out on X " << xx1 << std::endl);
			return false;
		}
		if(out_dist == 0.){ // entry point is exactly on the cone
		// here we got t1 > 0 from check_D, founded point seems to be in x and z ranges
		// if the track leaves the surface, then the entry is the intersection,
		// and the distance is 0
		// if the track is on the surface, then there is no lower cone intersection

		// estimate deviation of the track from the surface
		// (exact calculations are too complicated)
			const G4double xx2 = p.x() + v.x() * t1 * 0.5;
			const G4double yy2 = p.y() + v.y() * t1 * 0.5;
			const G4double dev = fabs(sqrt(xx2 *xx2 + yy2*yy2)
		                       - m_fs->Amin*(p.z() + zz1)*0.5
		                       - m_fs->Bmin);
			if(dev < s_Tolerance){
				LWSDBG(8, std::cout << "fcl on the surface" << std::endl);
				return false;
			} else {
				LWSDBG(8, std::cout << "fcl out = in" << std::endl);
				q = p;
				return true;
			}
		}
		q.setX(xx1);
		q.setY(p.y() + v.y() * t1);
		q.setZ(zz1);
		LWSDBG(8, std::cout << "fcl got " << t1 << std::endl);
		return true;
	}
	LWSDBG(8, std::cout << "fcl no intersection" << std::endl);
	return false;
}

// p must be not outside of the "FanBound"
// if track crosses outer cone in valid (z, x) interval,
// returns true, adds to b the distance to the cross point,
// sets q to the cross point
bool LArWheelSolid::fs_cross_upper(
	const G4ThreeVector &p, const G4ThreeVector &v,
	G4ThreeVector &q) const
{
	LWSDBG(7, std::cout << "fcu" << std::endl);
	G4double A = v.perp2();
	G4double B = p.x()*v.x() + p.y()*v.y();
	G4double C = p.perp2();

	if(m_IsOuter){
		const G4double &Af = A, &Bf = B;
		const G4double Cf = C - m_fs->Cflat2;
		G4double b1;
		if(check_D(b1, Af, Bf, Cf, Cf >= 0.)){
			const G4double zz1 = p.z() + v.z() * b1;
			if(zz1 >= m_Zmid && zz1 <= m_Zsect.back()){
				const G4double xx1 = p.x() + v.x() * b1;
				if(xx1 < m_fs->xmin || xx1 > m_fs->xmax) return false;
				q.setX(xx1);
				q.setY(p.y() + v.y() * b1);
				q.setZ(zz1);;
				return true;
			}
		}
		LWSDBG(8, std::cout << "fcu no cyl intersection" << std::endl);
	}

	A -= m_fs->Amax2*v.z()*v.z();
	B -= m_fs->Amax2*p.z()*v.z() + m_fs->ABmax*v.z();
	C -= m_fs->Amax2*p.z()*p.z() + 2.*m_fs->ABmax*p.z() + m_fs->Bmax2;

	G4double t1;
	const G4bool out = m_fs->Amax*p.z() + m_fs->Bmax <= p.perp();
	if(check_D(t1, A, B, C, out)){
		const G4double zz1 = p.z() + v.z() * t1;
		LWSDBG(8, std::cout << "fcu z = " << zz1 << ", lim: (" << m_Zsect.front() << ", " << m_Zmid << ")" << std::endl);
		if(zz1 < m_Zsect.front() || zz1 > m_Zmid) return false;
		const G4double xx1 = p.x() + v.x() * t1;
		LWSDBG(8, std::cout << "fcu x = " << xx1 << ", lim: (" << m_fs->xmin << ", " << m_fs->xmax << ")" << std::endl);
		if(xx1 < m_fs->xmin || xx1 > m_fs->xmax) return false;
		q.setX(xx1);
		q.setY(p.y() + v.y() * t1);
		q.setZ(zz1);
		return true;
	}
	LWSDBG(8, std::cout << "fcu no cone intersection" << std::endl);
	return false;
}

/* p must be not outside "FanBound" */
LArWheelSolid::FanBoundExit_t LArWheelSolid::find_exit_point(
	const G4ThreeVector &p, const G4ThreeVector &v,
	G4ThreeVector &q) const
{
	LWSDBG(6, std::cout << "in fep p" << MSG_VECTOR(p)<< ", v"<< MSG_VECTOR(v) << ", q" << MSG_VECTOR(q) << std::endl);

/* by construction, cannot have true from both upper and lower */
/* the only problem is the points on surface but "slightly outside" */
/* fs_cross_* account for (x, z) range */
// lower has to be checked first, since outer might find more distant
// intersection in the acceptable (x, z) range
	if(fs_cross_lower(p, v, q))	return ExitAtInner;
	LWSDBG(6, std::cout << "after fs_cross_lower q" << MSG_VECTOR(q) << std::endl);
	if(fs_cross_upper(p, v, q))	return ExitAtOuter;
	LWSDBG(6, std::cout << "after fs_cross_upper q" << MSG_VECTOR(q) << std::endl);

	FanBoundExit_t result = ExitAtSide;
	G4double d;
	if(v.x() > 0.) d = (m_fs->xmax - p.x()) / v.x();
	else if(v.x() < 0.) d = (m_fs->xmin - p.x()) / v.x();
	else d = kInfinity;

	G4double dz;
	FanBoundExit_t resultz = NoCross;
	if(v.z() > 0.){
		dz = (m_Zsect.back() - p.z()) / v.z();
		resultz = ExitAtBack;
	} else if(v.z() < 0.){
		dz = (m_Zsect.front() - p.z()) / v.z();
		resultz = ExitAtFront;
	} else {
		dz = kInfinity;
	}
	if(d > dz){
		d = dz;
		result = resultz;
	}
	q = p + v * d;
	LWSDBG(7, std::cout << "fep side " << d << " " << result << " q" << MSG_VECTOR(q) << std::endl);
	const G4double out_distlower = m_fs->Amin*q.z() + m_fs->Bmin - q.perp(); // > 0 - below lower cone
	LWSDBG(7, std::cout << "fep out_distlower(q)=" << out_distlower << " Tolerance=" << s_Tolerance << std::endl);
	if (out_distlower >= 0.0) {
		// side intersection point is below lower cone
		// initial point p was at exit boundary
		q = p;
		return NoCross;
	}

	if (m_IsOuter && q.z() >= m_Zmid && q.z() <= m_Zsect.back()+s_Tolerance && q.perp2() >= m_fs->Cflat2) {
		// outside of upper cylinder
		q = p;
		return NoCross;
	}
	const G4double out_distupper = m_fs->Amax*q.z() + m_fs->Bmax - q.perp(); // < 0 - above upper cone
	LWSDBG(7, std::cout << "fep out_distupper(q)=" << out_distupper << " Tolerance=" << s_Tolerance << std::endl);
	if (out_distupper <= 0.0) {
		// side intersection point is above upper cone
		// initial point p was at exit boundary
		q = p;
		return NoCross;
	}
	assert((q - p).mag() < kInfinity);
	return result;
}
