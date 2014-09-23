/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/sincos.h"
//#include <cmath>
#include <climits>
#include <cassert>
#include <iostream>

#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "./LArWheelCalculator_Impl/IDistanceCalculator.h"
#include "./LArWheelCalculator_Impl/IFanCalculator.h"

#ifdef HARDDEBUG
#include<stdio.h>
#endif

void LArWheelCalculator::parameterized_sin(const double r, double &sin_a, double &cos_a) const
{
	const double r2 = r*r;
	const double r3 = r2*r;
	const double r4 = r2*r2;
#if LARWC_SINCOS_POLY > 4
	const double r5 = r4*r;
#endif
	sin_a = m_sin_parametrization[0]
	      + m_sin_parametrization[1]*r
	      + m_sin_parametrization[2]*r2
	      + m_sin_parametrization[3]*r3
	      + m_sin_parametrization[4]*r4
#if LARWC_SINCOS_POLY > 4
	      + m_sin_parametrization[5]*r5
#endif
	;
	cos_a = sqrt(1. - sin_a*sin_a);
}

void LArWheelCalculator::parameterized_sincos(const double r, double &sin_a, double &cos_a) const
{
	const double r2 = r*r;
	const double r3 = r2*r;
	const double r4 = r2*r2;
#if LARWC_SINCOS_POLY > 4
	const double r5 = r4*r;
#endif
	sin_a = m_sin_parametrization[0]
	      + m_sin_parametrization[1]*r
	      + m_sin_parametrization[2]*r2
	      + m_sin_parametrization[3]*r3
	      + m_sin_parametrization[4]*r4
#if LARWC_SINCOS_POLY > 4
	      + m_sin_parametrization[5]*r5
#endif
	;
	cos_a = m_cos_parametrization[0]
	      + m_cos_parametrization[1]*r
	      + m_cos_parametrization[2]*r2
	      + m_cos_parametrization[3]*r3
	      + m_cos_parametrization[4]*r4
#if LARWC_SINCOS_POLY > 4
	      + m_cos_parametrization[5]*r5
#endif
	;
}

// calculates wave slant angle using parametrization for current wheel
// for given distance from calorimeter axis
#ifndef LARWHEELCALCULATOR_PSA_DEVELOPMENT
double LArWheelCalculator::parameterized_slant_angle(double r) const
{
	const double r2=r*r;
	const double r3=r2*r;
	const double r4=r2*r2;
	const double result =    m_slant_parametrization[0] +
				     r*m_slant_parametrization[1] +
				    r2*m_slant_parametrization[2] +
				    r3*m_slant_parametrization[3] +
				    r4*m_slant_parametrization[4];
    return result*deg;
}
#else // LARWHEELCALCULATOR_PSA_DEVELOPMENT
double LArWheelCalculator::parameterized_slant_angle(double r) const
{
  if(OptFoldA == "param"){ // EMECoption requires parametrized form
	const double r2=r*r;
	const double r3=r2*r;
	const double r4=r2*r2;
	const double result =    m_slant_parametrization[0] +
				     r*m_slant_parametrization[1] +
				    r2*m_slant_parametrization[2] +
				    r3*m_slant_parametrization[3] +
				    r4*m_slant_parametrization[4];
    return result*deg;
  }
  else if (OptFoldA == "table") {       // interpolation in engeneering table
    assert(r>=RminA && r<=RmaxA);
    double x = (r-RminA)/RstepA;
    int    i = int(x);
    double dr= (x-i);
    double alf= FoldA[i]*(1.-dr) + FoldA[i+1]*dr; //interpolated value
    return alf;
  }
  else if (OptFoldA == "iter") return (pi-AlphIter(m_HalfWaveLength,r))/2.; //exact
  else {
    std::cout<<" unknown option to compute slant angle"<<std::endl;
    exit(99);
  }
}
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT

#ifdef LARWHEELCALCULATOR_PSA_DEVELOPMENT
double LArWheelCalculator::AlphIter(double hwl, double r) const{
  double   epsi=1.e-10;
  double   zigl=(ZlOpt[0]*(ROpt[1]-r)+ZlOpt[1]*(r-ROpt[0]))/(ROpt[1]-ROpt[0]);
  double salf2n=hwl/zigl;
  double salf2 =-1.;
  double alf=0.,alf2,calf2,talf2;
  while (fabs(salf2n-salf2) > epsi ){
    salf2 = salf2n;
    alf2  = asin(salf2);
    alf   = 2.*alf2;
    calf2 = cos(alf2);
    talf2 = salf2/calf2;
    salf2n= hwl/(zigl+2.*m_FanFoldRadius/talf2-(pi-alf)*m_FanFoldRadius);
  }
  return alf;
}
#endif // LARWHEELCALCULATOR_PSA_DEVELOPMENT

// Determines the nearest to the input point fan.
// Relays on the fact that each two fans have a fan of a different type between
// them.
// Returns distance to the nearest fan. Vector p is set to nearest fan coord.
// system.
// m_fan_number is set to nearest fan number
double LArWheelCalculator::DistanceToTheNearestFan(CLHEP::Hep3Vector &p) //const
{
//std::cout << "DistanceToTheNearestFan " << p << std::endl;
#ifdef USE_FANCALCULATOR_IMPL
	return m_fanCalcImpl->DistanceToTheNearestFan(p);
#else
	int fan_number = int((p.phi() - halfpi - m_ZeroFanPhi_ForDetNeaFan) / m_FanStepOnPhi);
	double angle = m_FanStepOnPhi * fan_number + m_ZeroFanPhi_ForDetNeaFan;
#ifdef HARDDEBUG
	printf("DistanceToTheNearestFan: initial FN %4d\n", fan_number);
#endif
	p.rotateZ(-angle);
// determine search direction
	set_m_fan_number(fan_number);
	double d0 = DistanceToTheNeutralFibre(p);
//	double d1, d2;
	double d1 = d0;
	int delta = 1;
	if(d0 < 0.) delta = -1; // search direction has been determined
	angle = - m_FanStepOnPhi * delta;
	do { // search:
//		d2 = d1;
		p.rotateZ(angle);
		fan_number += delta;
		set_m_fan_number(fan_number);
#ifdef HARDDEBUG
	printf("DistanceToTheNearestFan: step FN %4d %4d\n", fan_number, m_fan_number);
#endif
		d1 = DistanceToTheNeutralFibre(p);
	} while(d0 * d1 > 0.);
// if signs of d1 and d0 are different, the point is between current pair
	if(delta > 0) fan_number --;
	if(m_isModule){
		m_fan_number = fan_number;
		if(m_fan_number < m_FirstFan){
			p.rotateZ((m_fan_number - m_FirstFan) * m_FanStepOnPhi);
			m_fan_number = m_FirstFan;
		} else if(m_fan_number >= m_LastFan){
			p.rotateZ((m_fan_number - m_LastFan + 1) * m_FanStepOnPhi);
			m_fan_number = m_LastFan - 1;
		}
	} else {
		set_m_fan_number(fan_number);
	}
	p.rotateZ(-0.5 * angle);
#ifdef HARDDEBUG
	printf("DistanceToTheNearestFan: final MFN %4d\n", m_fan_number);
#endif
	const double result = DistanceToTheNeutralFibre(p);
std::cout << "NF end: " << p << std::endl;
	return result;
#endif
}

// Relays on the fact that each two fans have a fan of a different type between
// them.
// Affects m_fan_number.
std::pair<int, int> LArWheelCalculator::GetPhiGapAndSide(const CLHEP::Hep3Vector &p)
{
//std::cout << "GetPhiGapAndSide " << p << std::endl;
#ifdef USE_FANCALCULATOR_IMPL
	return m_fanCalcImpl->GetPhiGapAndSide(p);
#else
	static CLHEP::Hep3Vector p1;
	p1 = p;

	int fan_number = int((p.phi() - halfpi - m_ZeroFanPhi) / m_FanStepOnPhi);
	double angle = m_FanStepOnPhi * fan_number + m_ZeroFanPhi;
	p1.rotateZ(-angle);
	set_m_fan_number(fan_number);
	double d0 = DistanceToTheNeutralFibre(p1);
	double d1;
	d1 = d0;
	int delta = 1;
	if(d0 < 0.) delta = -1;
	angle = - m_FanStepOnPhi * delta;
	do {
		p1.rotateZ(angle);
		fan_number += delta;
		set_m_fan_number(fan_number);
		d1 = DistanceToTheNeutralFibre(p1);
	} while(d0 * d1 > 0.);
	if(delta > 0) fan_number --;
	if(!m_isElectrode) fan_number ++;

	if(m_isModule){
		m_fan_number = fan_number;
		if(m_fan_number < m_FirstFan) m_fan_number = m_FirstFan - 1;
		else if(m_fan_number > m_LastFan) m_fan_number = m_LastFan;
	} else {
		set_m_fan_number(fan_number);
	}
	p1.rotateZ(-0.5 * angle);
	double dd = DistanceToTheNeutralFibre(p1);
	int side = dd < 0.? -1: 1;
#ifdef HARDDEBUG
	printf("GetPhiGapAndSide: MFN %4d\n", m_fan_number);
#endif
	return std::pair<int, int>(m_fan_number, side);
#endif
}

// Represents aproximate, probably underestimate, distance to the
// neutral fibre of the vertical fan. Sign of return value means
// side of the fan; negative - lower phi.
//
// Uses m_fan_number to compute sagging.
double LArWheelCalculator::DistanceToTheNeutralFibre(const CLHEP::Hep3Vector& P) const
{
//	std::cout << "LArWheelCalculator::DistanceToTheNeutralFibre this=" << this << "\n";
//	std::cout << "LArWheelCalculator::DistanceToTheNeutralFibre m_distanceCalcImpl=" << m_distanceCalcImpl << std::endl;
#ifdef USE_DISTANCECALCULATOR_IMPL
	return m_distanceCalcImpl->DistanceToTheNeutralFibre(P);
#else
	double distance = 0.;
	double z = P.z() - m_StraightStartSection;
	double x = P.x();

	if(m_SaggingOn) x += get_sagging(P); //

	double alpha = parameterized_slant_angle(P.y());
//	double cos_a, sin_a;
//	::sincos(alpha, &sin_a, &cos_a);
	CxxUtils::sincos scalpha(alpha);
	const double &cos_a = scalpha.cs, &sin_a = scalpha.sn;

// determination of the nearest quarter-wave number
	int nqwave;
	if(z < 0.) nqwave = 0;
	else nqwave = int(z / m_QuarterWaveLength);
	bool begin_qw = false;
	if((nqwave % 2) != 0){
		nqwave ++;
		begin_qw = true;
	}

	nqwave /= 2;

// now nqwave is not the number of quarter-wave but the number of half-wave
// half-waves with last and zero numbers are not really half-waves but start
// and finish quarter-waves
// It means that half-wave number 1 begins after starting quarter-wave
	if(nqwave != 0 && nqwave != m_NumberOfHalfWaves){ // regular half-waves
		z -= nqwave * m_HalfWaveLength;
// there are some symmetries, so use them
		if((nqwave % 2) == 0) x = -x;
		if(begin_qw){
			z = -z;
			x = -x;
		}
// certain situation: rising slope of wave, z is positive
// rotate to prime-coordinate system (see description)
		double z_prime = z * cos_a + x * sin_a;
		double x_prime = x * cos_a - z * sin_a;
		double straight_part = (m_QuarterWaveLength - m_FanFoldRadius * sin_a) / cos_a;
		double dz = straight_part - z_prime;
		if(dz > 0) distance = x_prime; // straight part of the quarter-wave
		else { // fold region
			double dx = x_prime + m_FanFoldRadius;
			distance = sqrt(dz*dz + dx*dx) - m_FanFoldRadius;
		}
// set correct sign for result
		if(!begin_qw) distance = -distance;
		if((nqwave % 2) == 0) distance = -distance;
	} else { // start and finish quarter-waves
		if(nqwave == 0)	{ // start quarter-wave
			x = - x;
		} else { // finish quarter-wave
			z = m_ActiveLength - z;
		}

		double tan_beta = tan(alpha * 0.5); // sin_a/(1+cos_a)
		double local_straight_section = m_FanFoldRadius * tan_beta;
		if(z < - local_straight_section && (x < m_FanFoldRadius || x < - m_StraightStartSection * z / local_straight_section / tan_beta )  )
		{
			distance = - x;
		} else {
			double z_prime = z * cos_a + x * sin_a;
			double x_prime = x * cos_a - z * sin_a;
			if (z_prime < local_straight_section) { // start fold region
				double dz = local_straight_section - z_prime;
				double dx = x_prime - m_FanFoldRadius;
				distance = sqrt(dz*dz + dx*dx) - m_FanFoldRadius;
			} else {
				double straight_part = (m_QuarterWaveLength - m_FanFoldRadius * sin_a) / cos_a;
				if (z_prime <= straight_part) { // straight part of quarter-wave
					distance = - x_prime;
				} else { // regular fold region of the quarter-wave
					double dz = straight_part - z_prime;
					double dx = x_prime + m_FanFoldRadius;
					distance = m_FanFoldRadius - sqrt(dz*dz + dx*dx);
				}
			}
		}
// set correct sign
		if (nqwave == 0) distance = -distance;
	}
 	return distance;
#endif
}

#ifndef USE_DISTANCECALCULATOR_IMPL
// the function uses m_fan_number for phi-dependent sagging computation
double LArWheelCalculator::get_sagging(const CLHEP::Hep3Vector &P) const
{
#ifdef HARDDEBUG
	std::cout << "get_sagging: MFN = " << m_fan_number << std::endl;
#endif
	double dx = P.z() / m_HalfWheelThickness - 1.;
	dx *= dx;
	dx = 1. - dx;
//	dx *= SaggingAmplitude * sin(FanStepOnPhi * m_fan_number + ZeroFanPhi);
/*	int n = m_fan_number;
	if(n < 0) n += m_NumberOfFans;
	n += m_ZeroGapNumber;
	if(n >= m_NumberOfFans) n -= m_NumberOfFans;
	const std::vector<double>& sp = m_sagging_parameter[n]; */
	const std::vector<double>& sp = m_sagging_parameter[m_fan_number];
	double R = P.r() / mm;
	double r = R;
	double result = sp[0];
	result += R * sp[1];
	R *= r;
	result += R * sp[2];
	R *= r;
	result += R * sp[3];
	R *= r;
	result += R * sp[4];

#ifdef HARDDEBUG
/* printf("get_sagging: (%6.3f, %6.3f, %6.3f) %6.3f; MFN %4d;"
       "n %3d; sp %6.4f %6.4f; dx %6.3f; %.6f\n", P.x()/mm, P.y()/mm, P.z()/mm,
       r, m_fan_number, n, sp[0], sp[1], dx, result*dx); */
printf("get_sagging: (%6.3f, %6.3f, %6.3f) %6.3f;"
       " sp %6.4f %6.4f; dx %6.3f; %.6f\n", P.x()/mm, P.y()/mm, P.z()/mm,
       r, sp[0], sp[1], dx, result*dx);
#endif //HARDDEBUG

	return result * dx;
}
#endif


CLHEP::Hep3Vector LArWheelCalculator::NearestPointOnNeutralFibre(const CLHEP::Hep3Vector &P) const
{
#ifdef USE_DISTANCECALCULATOR_IMPL
	return m_distanceCalcImpl->NearestPointOnNeutralFibre(P);
#else
	static CLHEP::Hep3Vector result;
	double z = P.z() - m_StraightStartSection;
	double x = P.x();
	double y = P.y();

	//if(m_SaggingOn) x += get_sagging(P);  // !

	double alpha = parameterized_slant_angle(y);
	CxxUtils::sincos scalpha(alpha);
	const double &cos_a = scalpha.cs, &sin_a = scalpha.sn;

	int nqwave;
	if(z < 0.) nqwave = 0;
	else nqwave = int(z / m_QuarterWaveLength);
	bool begin_qw = false;
	if((nqwave % 2) != 0){
		nqwave ++;
		begin_qw = true;
	}
	nqwave /= 2;
	if(nqwave != 0 && nqwave != m_NumberOfHalfWaves){
		z -= nqwave * m_HalfWaveLength;
		if((nqwave % 2) == 0) x = -x;
		if(begin_qw){
			z = -z;
			x = -x;
		}
		double z_prime = z * cos_a + x * sin_a;
		double x_prime = x * cos_a - z * sin_a;
		double straight_part = (m_QuarterWaveLength - m_FanFoldRadius * sin_a) / cos_a;
		double dz = straight_part - z_prime;
		if (dz > 0) result.set(0., y, z_prime);
		else {
			double a = atan(fabs(dz / (x_prime + m_FanFoldRadius)));
			result.set(m_FanFoldRadius * (cos(a) - 1), y, straight_part + m_FanFoldRadius * sin(a));
		}
		result.rotateY(alpha);
		if(begin_qw){
			result.setX(-result.x());
			result.setZ(-result.z());
		}
		if((nqwave % 2) == 0) result.setX(-result.x());
		result.setZ(result.z() + nqwave * m_HalfWaveLength);
	} else {
		if(nqwave == 0) x = -x;
		else z = m_ActiveLength - z;
		double tan_beta = tan(alpha * 0.5); // sin_a/(1+cos_a)
		double local_straight_section = m_FanFoldRadius * tan_beta;
		if(z < - local_straight_section && (x < m_FanFoldRadius || x < - m_StraightStartSection * z / local_straight_section / tan_beta)){
			result.set(0., y, z);
		} else {
			double z_prime = z * cos_a + x * sin_a;
			double x_prime = x * cos_a - z * sin_a;
			if(z_prime < local_straight_section) {
				double a = fabs(atan((local_straight_section - z_prime) / (x_prime - m_FanFoldRadius)));

				result.set(m_FanFoldRadius * (1 - cos(a)), y, local_straight_section - m_FanFoldRadius * sin(a));
			} else {
				double straight_part = (m_QuarterWaveLength - m_FanFoldRadius * sin_a) / cos_a;
				if(z_prime <= straight_part) {
					result.set(0., y, z_prime);
				} else {
					double a = fabs(atan((straight_part - z_prime) /  (x_prime + m_FanFoldRadius)) );
					result.set(m_FanFoldRadius * (cos(a) - 1), y, straight_part + m_FanFoldRadius * sin(a));
		  		}
			}
			result.rotateY(alpha);
		}
		if(nqwave != 0){
			result.setZ(m_ActiveLength - result.z());
		} else {
			result.setX(-result.x());
		}
	}
	result.setZ(result.z() + m_StraightStartSection);
	return result;
#endif
}

/*
input is in local fan's coordinate system
side: < 0 - lower phi
      > 0 - greater phi
	  = 0 - neutral fibre
*/
double LArWheelCalculator::AmplitudeOfSurface(const CLHEP::Hep3Vector& P, int side) const
{
#ifdef USE_DISTANCECALCULATOR_IMPL
	return m_distanceCalcImpl->AmplitudeOfSurface(P, side);
#else
	double result = 0.;
	double rho = m_FanFoldRadius;
	double z = P.z() - m_StraightStartSection;

	double alpha = parameterized_slant_angle(P.y());
	CxxUtils::sincos scalpha(alpha);
	const double &cos_a = scalpha.cs, &sin_a = scalpha.sn;

// determination of the nearest quarter-wave number
	int nqwave;
	if(z < 0.) nqwave = 0;
	else nqwave = int(z / m_QuarterWaveLength);
	bool begin_qw = false;
	if((nqwave % 2) != 0){
		nqwave ++;
		begin_qw = true;
	}
	nqwave /= 2;
// now nqwave is not the number of quarter-wave but the number of half-wave
// half-waves with last and zero numbers are not really half-waves but start
// and finish quarter-waves
// It means that half-wave number 1 begins after starting quarter-wave
	if(nqwave != 0 && nqwave != m_NumberOfHalfWaves){ // regular half-waves
		z -= nqwave * m_HalfWaveLength;
		if(begin_qw) z = -z;
		double dz = m_QuarterWaveLength - z;

		int local_side = 1;
		if((nqwave % 2) == 0){
			if(begin_qw) local_side = -1;
		} else {
			if(!begin_qw) local_side = -1;
		}

		rho += m_FanHalfThickness * local_side * side;

		if(dz >= rho * sin_a){
			result = z * sin_a / cos_a; // straight part of the quarter-wave
		} else { // fold region
			result = (m_QuarterWaveLength * sin_a - rho) / cos_a
			       + sqrt(rho * rho - dz * dz);
		}
		result *= -local_side;
		if(side < 0) result += m_FanHalfThickness / cos_a;
		else if(side > 0) result -= m_FanHalfThickness / cos_a;
	} else { // start and finish quarter-waves
		int local_side = 1;
		if(nqwave == 0)	{ // start quarter-wave
			local_side = -1;
		} else { // finish quarter-wave
			z = m_ActiveLength - z;
		}

		double rho1 = m_FanFoldRadius;
		double tan_beta = tan(alpha * 0.5); // sin_a/(1+cos_a)
		double min_local_fold_region = rho1 * tan_beta;

		if(z <= - min_local_fold_region){
			result = - side * m_FanHalfThickness;
		} else {
			rho1 += m_FanHalfThickness * side * local_side;

			double max_local_fold_region = rho1 * tan_beta * (1. + cos_a) - min_local_fold_region;
			if(z < max_local_fold_region){ // start fold region
				z += min_local_fold_region;
				result = rho1 - sqrt(rho1 * rho1 - z * z);
				if(nqwave == 0) result = -result;
				if(side < 0) result += m_FanHalfThickness;
				else if(side > 0) result -= m_FanHalfThickness;
			} else {
				rho -= m_FanHalfThickness * local_side * side;
				double dz = m_QuarterWaveLength - z;
				if(dz >= rho * sin_a){
					result = z * sin_a / cos_a; // straight part of the quarter-wave
				} else { // regular fold region
					result = (m_QuarterWaveLength * sin_a - rho) / cos_a
					       + sqrt(rho * rho - dz * dz);
				}
				if(nqwave == 0) result = -result;
				if(side < 0) result += m_FanHalfThickness / cos_a;
				else if(side > 0) result -= m_FanHalfThickness / cos_a;
			}
		}
	}

	// if(m_SaggingOn) result -= get_sagging(P); // !

 	return result;
 #endif
}

