/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Fast (approximate) methods for solenoidal field properties
 ----------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExSolenoidalIntersector/SolenoidParametrization.h"

namespace {
    class RestoreIOSFlags 
    {
    public:
      RestoreIOSFlags (std::ostream &os) 
        : m_os(&os), 
          //m_flags(m_os->flags()),
          m_precision(m_os->precision())
      {}
      ~RestoreIOSFlags() {
        //m_os->flags(m_flags);
        m_os->precision(m_precision);
      }
    private:
      std::ostream *m_os;
      //std::ios_base::fmtflags m_flags;
      std::streamsize  m_precision;
    };
}


namespace Trk
{
   
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>

const double   	SolenoidParametrization::s_binInvSizeTheta	= 1./0.1;
const double   	SolenoidParametrization::s_binInvSizeZ		= 1./20.*Gaudi::Units::mm;
const double   	SolenoidParametrization::s_binZeroTheta		= 0.;
const double   	SolenoidParametrization::s_binZeroZ		= -160.*Gaudi::Units::mm;
const double   	SolenoidParametrization::s_lightSpeed		= -1.*299792458*Gaudi::Units::m/Gaudi::Units::s;
const int      	SolenoidParametrization::s_maxBinTheta 		= 72;
const int      	SolenoidParametrization::s_maxBinZ		= 17;
const double   	SolenoidParametrization::s_maximumImpactAtOrigin= 30.*Gaudi::Units::mm;
const double   	SolenoidParametrization::s_maximumZatOrigin	= 250.*Gaudi::Units::mm;
const int      	SolenoidParametrization::s_numberParameters	= 6;
const double   	SolenoidParametrization::s_rInner		= 570.*Gaudi::Units::mm;
const double   	SolenoidParametrization::s_rOuter		= 1050.*Gaudi::Units::mm;
const double   	SolenoidParametrization::s_zInner		= 2150.0*Gaudi::Units::mm;  // just after wheel #7
const double   	SolenoidParametrization::s_zOuter		= 2800.0*Gaudi::Units::mm;  // just after wheel #9


SolenoidParametrization::BinParameters::BinParameters (const double r,
                                                       const double z,
                                                       const double cotTheta)
{
  if (cotTheta > 0) {
    m_signTheta = 1;
    m_cotTheta  = cotTheta;
    m_zAtAxis  	= z - r*cotTheta;
  }
  else {
    m_signTheta = -1;
    m_cotTheta = -cotTheta;
    m_zAtAxis  =  r*cotTheta - z;
  }
}


SolenoidParametrization::Parameters::Parameters (const SolenoidParametrization& spar,
                                                 const double r,
                                                 const double z,
                                                 const double cotTheta)
  : BinParameters (r, z, cotTheta)
{
  int	key  		= fieldKey(*this);
  if (r > s_rInner || m_signTheta*z > s_zInner)
  {
    key	+= s_numberParameters/2;
  }
  spar.setTerms (key, *this);
}


//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

// note: both constructors are private
// SolenoidParametrization::SolenoidParametrization(void)
// {
//    // field component not defined without a m_magFieldSvc
//    // get central field value in units required for fast tracking
//    m_centralField    	= fieldComponent(0.,0.,0.);
//
//    // note field = B*c 
//    RestoreIOSFlags restore(std::cout);
//    std::cout << " SolenoidParametrization: centralField "
//              << std::setw(6) << std::setprecision(3) << m_centralField/s_lightSpeed /Gaudi::Units::tesla
//              << "T" << std::endl;
//}

SolenoidParametrization::SolenoidParametrization(MagField::IMagFieldSvc* magFieldSvc)
  : m_magFieldSvc	    (magFieldSvc),
    m_parameters            ()
{
    // allow 0.1% fluctuation in current.
    double current = magFieldSvc->solenoidCurrent();
    m_currentMax	 = 1.001*current;
    m_currentMin	 = 0.999*current;

  // get central field value in units required for fast tracking (i.e. field = B*c)
    if (!m_magFieldSvc)
       throw std::logic_error("fieldComponent not defined without magnetic field service.");
    m_centralField    	= fieldComponent(0.,0.,0.);
    RestoreIOSFlags restore(std::cout);
    std::cout << " SolenoidParametrization: centralField "
              << std::setw(6) << std::setprecision(3) << m_centralField/s_lightSpeed /Gaudi::Units::tesla
	      << "T";

    // now parametrise field - if requested
    //if (magFieldSvc)
    {
	std::cout << "  - please be patient while the solenoid is parametrised !! ";
	parametrizeSolenoid();
    }
    std::cout << std::endl;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
SolenoidParametrization::parametrizeSolenoid(void)
{
    // set parametrisation granularity (up to cotTheta = 7.)
    // get value of cubic term for approx: Bz = Bcentral*(1 - term * z^3)
    // 'fit' to average over cotTheta lines
    double	smallOffset	= 0.0000000000001; // avoid FPE
    double zAtAxis 	= s_binZeroZ;	// + smallOffset ? 
    
    for (int binZ = 0; binZ < s_maxBinZ; ++binZ)
    { 
        double cotTheta	= smallOffset; 
	for (int binTheta = 0; binTheta < s_maxBinTheta - 1; ++binTheta)
	{
	    double 	r      	= 0.;
	    double 	z      	= zAtAxis;
	    int 	n      	= 200;
	    double	dr;
	    if (cotTheta < s_zOuter/s_rOuter)
	    {
		dr = s_rOuter/double(n);
	    }
	    else
	    {
		dr = s_zOuter/(cotTheta*double(n));
	    }

	    Amg::VectorX difference(n);
	    Amg::MatrixX derivative	= Amg::MatrixX::Zero(n,s_numberParameters);
	    for (int k = 0; k < n; ++k)
	    {
		r 			+= dr;
		z 			+= dr*cotTheta;
		double	w		= (n - k)*(n - k);
		double	zLocal		= z - zAtAxis;
		if (r > s_rInner || z > s_zInner)
		{
		    // derivative(k,0)	= 0.;
		    // derivative(k,1)	= 0.;
		    // derivative(k,2)	= 0.;
		    derivative(k,3)	= w;
		    derivative(k,4)	= w*zLocal*zLocal;
		    derivative(k,5)	= w*zLocal*zLocal*zLocal;
		}
		else
		{
		    derivative(k,0)	= w;
		    derivative(k,1)	= w*zLocal*zLocal;
		    derivative(k,2)	= w*zLocal*zLocal*zLocal;
		    // derivative(k,3)	= 0.; 
		    // derivative(k,4)	= 0.;
		    // derivative(k,5)	= 0.;
		}
		difference(k)		= w*(fieldComponent(r,z,cotTheta) - m_centralField);
	    }

	    // solve for parametrization coefficients
	    Amg::VectorX solution	= derivative.colPivHouseholderQr().solve(difference);
            BinParameters parms (zAtAxis, cotTheta);
	    
	    int			key    		= fieldKey(parms);
	    assert (m_parameters[key] == 0.);
	    m_parameters[key++]		= m_centralField + solution(0);
	    m_parameters[key++]		= solution(1);
	    m_parameters[key++]		= solution(2);
	    m_parameters[key++]		= m_centralField + solution(3);
	    m_parameters[key++]		= solution(4);
	    m_parameters[key++]		= solution(5);

	    // duplicate last z-bin for contiguous neighbour lookup
	    if (binZ == s_maxBinZ - 1)
	    {
		assert (m_parameters[key] == 0.);
		m_parameters[key++]    	= m_centralField + solution(0);
		m_parameters[key++]    	= solution(1);
		m_parameters[key++]    	= solution(2);
		m_parameters[key++]    	= m_centralField + solution(3);
		m_parameters[key++]    	= solution(4);
		m_parameters[key++]    	= solution(5);
		key	-= s_numberParameters;
	    }

	    // duplicate next to previous z-bin for contiguous neighbour lookup
	    if (binZ > 0)
	    {
		key	-= 2*s_numberParameters*s_maxBinTheta;
		assert (m_parameters[key] == 0.);
		m_parameters[key++]    	= m_centralField + solution(0);
		m_parameters[key++]    	= solution(1);
		m_parameters[key++]    	= solution(2);
		m_parameters[key++]    	= m_centralField + solution(3);
		m_parameters[key++]    	= solution(4);
		m_parameters[key++]    	= solution(5);
	    }

	    // some debug print
//  	    key	 -= 6;
//  	    if (   key <  s_numberParameters*s_maxBinTheta
//  		|| key >= s_numberParameters*s_maxBinTheta*(s_maxBinZ - 2))
//  	    {
//  		double	z_max;
//  		if (cotTheta < s_zInner/s_rInner)
//  		{
//  		    z_max = s_rInner*cotTheta + zAtAxis;
//  		}
//  		else
//  		{
//  		    z_max = s_zInner;
//  		}
//  		cout << std::setiosflags(std::ios::fixed) << key
//  		     << "   cotTheta " 	<< std::setw(7)  << std::setprecision(2)  << cotTheta
//  		     << "   inner terms:  z0 "<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key]/m_centralField
//  		     << "   z^2 "<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key+1]*z_max*z_max/m_centralField
//  		     << "   z^3 "	<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key+2]*z_max*z_max*z_max/m_centralField
//  		     << "   outer terms:  z0 "<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key+3]/m_centralField
//  		     << "   z^2 "<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key+4]*z_max*z_max/m_centralField
//  		     << "   z^3 "	<< std::setw(6) << std::setprecision(3)
//  		     << m_parameters[key+5]*z_max*z_max*z_max/m_centralField
//  		     << std::resetiosflags(std::ios::fixed) << endl;
//  	    }
	    cotTheta	+= 1./s_binInvSizeTheta;
	}
	zAtAxis 	+= 1./s_binInvSizeZ;
    }

    // duplicate end theta-bins for contiguous neighbour lookup
    zAtAxis 	= s_binZeroZ;	// + smallOffset ?? 
    for (int binZ = 0; binZ < s_maxBinZ; ++binZ)
    { 
	double cotTheta	= double(s_maxBinTheta)/s_binInvSizeTheta;
        BinParameters parms (zAtAxis, cotTheta);
	int	key    	= fieldKey(parms);
	for (int k = 0; k < 2*s_numberParameters; ++k)
	{
	    assert (m_parameters[key+2*s_numberParameters] == 0.);
	    m_parameters[key+2*s_numberParameters] = m_parameters[key];
	    ++key;
	}
	zAtAxis 	+= 1./s_binInvSizeZ;
    }
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

void
SolenoidParametrization::printFieldIntegrals (void) const
{
    // integrate along lines of const eta from origin to r = 1m or |z| = 2.65m
    // direction normalised st transverse component = 1 (equiv to a fixed pt)
    std::cout << std::setiosflags(std::ios::fixed)
	      << "   eta    rEnd   mean(Bz) max(dBz/dR)     mean(Bt) max(dBt/dR)      "
	      << "min(Bt) max(Bt)  reverse-bend(z)  integrals: Bt.dR   Bl.dR"
	      << "     asymm: x        y        z"
	      << "      " << std::endl
	      << "             m          T         T/m            T         T/m      "
	      << "      T       T                m               T.m     T.m"
	      << "            T        T        T"
	      << std::endl;
    
    double maxR 	= 1000.*Gaudi::Units::mm;
    double maxZ		= 2650.*Gaudi::Units::mm;
    int numSteps	= 1000;

    // step through eta-range
    double eta	= 0.;
    for (int i = 0; i != 31; ++i)
    {
	double phi		=  0.;
	double cotTheta		=  sinh(eta);
	Amg::Vector3D direction(cos(phi),sin(phi),cotTheta);
	double rEnd		=  maxR;
	if (std::abs(cotTheta) > maxZ/maxR) rEnd =  maxZ/direction.z();
	double step		=  rEnd/static_cast<double>(numSteps);	// radial step in mm
	Amg::Vector3D position(0.,0.,0.);
	double meanBL		= 0.;
	double meanBT		= 0.;
	double meanBZ		= 0.;
	double maxBT		= 0.;
	double minBT		= 9999.;
	double maxGradBT	= 0.;
	double maxGradBZ	= 0.;
	double prevBT		= 0.;
	double prevBZ		= 0.;
	double reverseZ		= 0.;

	// look up field along eta-line
	for (int j = 0; j != numSteps; ++j)
	{
	    position			+= 0.5*step*direction;
	    Amg::Vector3D field;
	    m_magFieldSvc->getField(&position,&field);
	    Amg::Vector3D vCrossB	=  direction.cross(field);
	    position			+= 0.5*step*direction;
	    double BZ			=  field.z();
	    double BT			=  vCrossB.x()*direction.y() - vCrossB.y()*direction.x();
	    double BL			=  std::sqrt(vCrossB.mag2() - BT*BT);
	    meanBL			+= BL;
	    meanBT			+= BT;
	    meanBZ			+= BZ;
	    if (BT > maxBT) maxBT = BT;
	    if (BT < minBT) minBT = BT;
	    if (j > 0)
	    {
		if (BT*prevBT < 0.) reverseZ = position.z() - step*direction.z();
		double grad = std::abs(BT - prevBT);
		if (grad > maxGradBT) maxGradBT = grad;
		grad = std::abs(BZ - prevBZ);
		if (grad > maxGradBZ) maxGradBZ = grad;
	    }
	    prevBT		=  BT;
	    prevBZ		=  BZ;
	}

	// normalize
	maxGradBT		*= static_cast<double>(numSteps)/step;
	maxGradBZ		*= static_cast<double>(numSteps)/step;
	meanBL			/= static_cast<double>(numSteps);
	meanBT			/= static_cast<double>(numSteps);
	meanBZ			/= static_cast<double>(numSteps);
	double integralBL	= meanBL*rEnd; 
	double integralBT	= meanBT*rEnd;
	
	std::cout << std::setw(6)	<< std::setprecision(2) << eta
		  << std::setw(8)	<< std::setprecision(3) << rEnd /Gaudi::Units::meter
		  << std::setw(11)	<< std::setprecision(4) << meanBZ /Gaudi::Units::tesla
		  << std::setw(12)	<< std::setprecision(3) << maxGradBZ /Gaudi::Units::tesla
		  << std::setw(13)	<< std::setprecision(4) << meanBT /Gaudi::Units::tesla
		  << std::setw(12)	<< std::setprecision(3) << maxGradBT /Gaudi::Units::tesla
		  << std::setw(13)	<< std::setprecision(4) << minBT /Gaudi::Units::tesla
		  << std::setw(8)	<< std::setprecision(4) << maxBT /Gaudi::Units::tesla;
	if (reverseZ > 0.)
	{
	    std::cout << std::setw(17)	<< std::setprecision(2) << reverseZ /Gaudi::Units::meter
		      << std::setw(18)	<< std::setprecision(4) << integralBT /(Gaudi::Units::tesla*Gaudi::Units::meter)
		      << std::setw(8)	<< std::setprecision(4) << integralBL /(Gaudi::Units::tesla*Gaudi::Units::meter)
		      << "    ";
	}
	else
	{
	    std::cout << std::setw(35)	<< std::setprecision(4) << integralBT /(Gaudi::Units::tesla*Gaudi::Units::meter)
		      << std::setw(8)	<< std::setprecision(4) << integralBL /(Gaudi::Units::tesla*Gaudi::Units::meter)
		      << "    ";
	}

	// check symmetry (reflect in each axis)
	for (int k = 0; k != 3; ++k)
	{
	    if (k == 0) direction = Amg::Vector3D(-cos(phi),sin(phi),cotTheta);
	    if (k == 1) direction = Amg::Vector3D(cos(phi),-sin(phi),cotTheta);
	    if (k == 2) direction = Amg::Vector3D(cos(phi),sin(phi),-cotTheta);
	    position		= Amg::Vector3D(0.,0.,0.);
	    double asymm	= 0.;
	    // look up field along eta-line
	    for (int j = 0; j != numSteps; ++j)
	    {
		position		+= 0.5*step*direction;
		Amg::Vector3D field;
		m_magFieldSvc->getField(&position,&field);
		Amg::Vector3D vCrossB	=  direction.cross(field);
		position		+= 0.5*step*direction;
		double BT		=  vCrossB.x()*direction.y() - vCrossB.y()*direction.x();
		asymm			+= BT;
	    }
	    asymm	= asymm/static_cast<double>(numSteps) - meanBT;
	    std::cout << std::setw(9)	<< std::setprecision(4) << asymm /Gaudi::Units::tesla;
	}
	std::cout << std::endl;
	eta	+= 0.1;
    }
}

void
SolenoidParametrization::printParametersForEtaLine (double eta, double z_origin) const
{
    double cotTheta	= 1./std::tan(2.*std::atan(1./std::exp(eta)));
    BinParameters parms (z_origin, cotTheta);
    int		key    	= fieldKey(parms);
    double	z_max;
    if (cotTheta < s_zInner/s_rInner)
    {
	z_max = s_rInner*cotTheta;
    }
    else
    {
	z_max = s_zInner;
    }
    std::cout << std::setiosflags(std::ios::fixed)
	      << "SolenoidParametrization:  line with eta "  << std::setw(6) << std::setprecision(2) << eta
	      << "   from (r,z)  0.0,"         << std::setw(6) << std::setprecision(1) << z_origin
	      << "   inner terms:  z0 "<< std::setw(6) << std::setprecision(2)
	      << m_parameters[key]/m_centralField
	      << "   z^2 "<< std::setw(6) << std::setprecision(3)
	      << m_parameters[key+1]*z_max*z_max/m_centralField
	      << "   z^3 "	<< std::setw(6) << std::setprecision(3)
	      << m_parameters[key+2]*z_max*z_max*z_max/m_centralField
	      << "    outer terms:  z0 "<< std::setw(6) << std::setprecision(3)
	      << m_parameters[key+3]/m_centralField
	      << "   z^2 "<< std::setw(6) << std::setprecision(3)
	      << m_parameters[key+4]*z_max*z_max/m_centralField
	      << "   z^3 "	<< std::setw(6) << std::setprecision(3)
	      << m_parameters[key+5]*z_max*z_max*z_max/m_centralField
	      << std::resetiosflags(std::ios::fixed) << std::endl;
}
        
void
SolenoidParametrization::printResidualForEtaLine (double eta, double zOrigin) const
{
    double 	cotTheta 	= 1./std::tan(2.*std::atan(1./std::exp(std::abs(eta))));
    double 	z		= zOrigin;
    double 	r 		= 0.;
    int 	n      		= 200;
    double	dr;
    if (cotTheta < s_zOuter/s_rOuter)
    {
	dr = s_rOuter/double(n);
    }
    else
    {
	dr = s_zOuter/(cotTheta*double(n));
    }
    double 	chiSquareIn	= 0.;
    double 	chiSquareOut	= 0.;
    double 	nIn		= 0.;
    double 	nOut		= 0.;
    double 	worstBCalc   	= 0.;
    double 	worstBTrue   	= 0.;
    double 	worstDiff     	= -1.;
    double 	worstR		= 0.;
    double 	worstZ		= 0.;
    for (int k = 0; k < n; ++k)
    {
	double	b 	= fieldComponent(r,z,cotTheta);
        Parameters parms (*this, r, z, cotTheta);
	double	diff 	= (fieldComponent(z, parms) - b)/s_lightSpeed;
	
	if (r > s_rInner || z > s_zInner)
	{
	    chiSquareOut+= diff*diff;
	    nOut	+= 1.;
	}
	else
	{
	    chiSquareIn	+= diff*diff;
	    nIn		+= 1.;
	}
	
	if (std::abs(diff) > worstDiff)
	{
	    worstDiff  	= std::abs(diff);
	    worstBCalc	= fieldComponent(z, parms);
	    worstBTrue	= b;
	    worstR	= r;
	    worstZ	= z;
	}
	r	+= dr;
	z 	+= dr*cotTheta;
    } 

    std::cout << std::setiosflags(std::ios::fixed)
	      << "SolenoidParametrization:  line with eta "  	<< std::setw(6) << std::setprecision(2) << eta
	      << "   from (r,z)  0.0, "  	<< std::setw(6) << std::setprecision(1) << zOrigin
	      << "   rms diff inner/outer "	<< std::setw(6) << std::setprecision(3)
	      << std::sqrt(chiSquareIn/nIn) /Gaudi::Units::tesla << " " << std::setw(6) << std::setprecision(3)
	      << std::sqrt(chiSquareOut/nOut) /Gaudi::Units::tesla << std::endl
	      << "            worst residual at:  (r,z) "
	      << std::setw(6) << std::setprecision(1) << worstR
	      << ", " << std::setw(6) << std::setprecision(1) << worstZ
	      << "   with B true/calc " << std::setw(6) << std::setprecision(3)
	      << worstBTrue/s_lightSpeed /Gaudi::Units::tesla
	      << "  " << std::setw(6) << std::setprecision(3) << worstBCalc/s_lightSpeed /Gaudi::Units::tesla
	      << std::resetiosflags(std::ios::fixed) << std::endl;
}


bool SolenoidParametrization::currentMatches (double current) const
{
  // allow 0.1% fluctuation
  return current >= m_currentMin && current < m_currentMax;
}


} // end of namespace

    
