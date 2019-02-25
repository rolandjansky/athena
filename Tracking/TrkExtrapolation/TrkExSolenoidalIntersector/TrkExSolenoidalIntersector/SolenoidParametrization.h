/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Fast (approximate) methods for solenoidal field properties
 ----------------------------------------------------------
 ***************************************************************************/

#ifndef TRKEXSOLENOIDALINTERSECTOR_SOLENOIDPARAMETRIZATION_H
#define TRKEXSOLENOIDALINTERSECTOR_SOLENOIDPARAMETRIZATION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <cmath>
// #include <iostream>
#include "GeoPrimitives/GeoPrimitives.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

//  class SolenoidParametrization
//
//  This is a singleton class so that any object can obtain access to the 
//  approximate field without repeating the expensive parametrisation method
//

namespace Trk
{
   
class SolenoidParametrization
{
    
public:
    class BinParameters
    {
    public:
      BinParameters (const double zAtAxis, const double cotTheta)
        : m_cotTheta (cotTheta), m_zAtAxis (zAtAxis) {}
      BinParameters (const double r, const double z, const double cotTheta);
      double m_signTheta = 1;
      double m_cotTheta = 0;
      double m_zAtAxis = 0;
      double m_interpolateZ = 0;
      double m_complementZ = 0;
      double m_interpolateTheta = 0;
      double m_complementTheta = 0;
    };
    class Parameters
      : public BinParameters
    {
    public:
      Parameters (const SolenoidParametrization& spar,
                  const double r, const double z, const double cotTheta);
      double m_fieldAtOrigin;
      double m_quadraticTerm;
      double m_cubicTerm;
    };

    SolenoidParametrization		(MagField::IMagFieldSvc* magFieldSvc);	// configuration from Intersector
    ~SolenoidParametrization() = default;
    
    // forbidden copy constructor
    // forbidden assignment operator

    double			centralField() const;
    double			fieldComponent (double z,
                                                const Parameters& parms) const;		// parametrized - perp to track in rz-plane
    double			fieldComponent (double r, double z, double cotTheta) const; // from MagFieldSvc
    void			fieldIntegrals (double& firstIntegral,
                                                double& secondIntegral,
                                                double zBegin,
                                                double zEnd,
                                                Parameters& parms) const;
    double			maximumR() const;		// param valid to maximumR
    double			maximumZ() const;		// param valid to maximumZ
    void			printFieldIntegrals() const;
    void			printParametersForEtaLine(double eta, double z_origin) const;
    void			printResidualForEtaLine (double eta, double zOrigin) const;
    bool			validOrigin(const Amg::Vector3D& origin) const; // param valid for this origin?

    // OK to use this parametrization for CURRENT?
    bool                        currentMatches (double current) const;

private:
    friend class Parameters;

    static int			fieldKey(BinParameters& parms);
    void			integrate(double& firstIntegral,
                                          double& secondIntegral,
                                          double zBegin,
                                          double zEnd,
                                          const Parameters& parms) const;
    double			interpolate(int key1, int key2,
                                            int key3, int key4,
                                            const Parameters& parms) const;
    void			parametrizeSolenoid();
    void			setTerms(int, Parameters& parms) const;

    static const double		s_binInvSizeTheta;
    static const double		s_binInvSizeZ;
    static const double		s_binZeroTheta;
    static const double		s_binZeroZ;
    static const double		s_lightSpeed;
    static const int	       	s_maxBinTheta;
    static const int	       	s_maxBinZ;
    static const double		s_maximumImpactAtOrigin;
    static const double		s_maximumZatOrigin;
    static const int	       	s_numberParameters;
    static const double		s_rInner;
    static const double		s_rOuter;
    static const double		s_zInner;
    static const double		s_zOuter;

    MagField::IMagFieldSvc*	m_magFieldSvc;
    double                      m_currentMin;
    double                      m_currentMax;
    double		        m_centralField;
    double		        m_parameters[14688];
    
    // copy, assignment: no semantics, no implementation
    SolenoidParametrization (const SolenoidParametrization&) = delete;
    SolenoidParametrization &operator= (const SolenoidParametrization&) = delete;
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline int
SolenoidParametrization::fieldKey(BinParameters& parms)
{
    double	z	= parms.m_zAtAxis - s_binZeroZ;
    int		zBin  	= static_cast<int>(s_binInvSizeZ*z);
    parms.m_interpolateZ = z*s_binInvSizeZ - double(zBin);
    if (zBin < 0)
    {
	parms.m_interpolateZ	= 0.;
	zBin 		= 0;
    }
    else if (zBin > s_maxBinZ - 2)
    {
	parms.m_interpolateZ	= 0.;
	zBin 		= s_maxBinZ - 1;
    }
    parms.m_complementZ	= 1. - parms.m_interpolateZ;
    
    int	thetaBin 	= static_cast<int>(s_binInvSizeTheta*parms.m_cotTheta);
    parms.m_interpolateTheta = parms.m_cotTheta*s_binInvSizeTheta - double(thetaBin);
    if (thetaBin > s_maxBinTheta - 3)
    {
	parms.m_interpolateTheta	= 0.;
	thetaBin 		= s_maxBinTheta - 2;
    }
    parms.m_complementTheta	= 1. - parms.m_interpolateTheta;
    // std::cout << " zAtAxis " << zAtAxis
    // 	      << "   cotTheta " << cotTheta
    // 	      << "   zBin " << zBin
    // 	      << "   interpolateZ " << parms.m_interpolateZ
    // 	      << "   thetaBin " << thetaBin
    // 	      << "   interpolateTheta " << parms.m_interpolateTheta << std::endl;
    
    return 2*s_numberParameters*(s_maxBinTheta*zBin + thetaBin);
}

inline void
SolenoidParametrization::integrate(double& 	firstIntegral,
				   double& 	secondIntegral,
				   double 	zBegin,
				   double 	zEnd,
                                   const Parameters& parms) const
{ 
    double	zDiff	=  zEnd - zBegin;
    double 	zBeg2	=  zBegin*zBegin;
    double 	zBeg3	=  zBeg2*zBegin;
    double 	zEnd2	=  zEnd*zEnd;
    double 	zEnd3	=  zEnd2*zEnd;
    double	zDiff4	=  0.25*(zEnd2 + zBeg2)*(zEnd2 - zBeg2);
    
    firstIntegral	+= parms.m_fieldAtOrigin*zDiff +
			   parms.m_quadraticTerm*(zEnd3 - zBeg3)*0.333333333333 +
			   parms.m_cubicTerm*zDiff4;
    double   zDiffInv	=  1./zDiff;
    secondIntegral	+= parms.m_fieldAtOrigin*zDiff +
 			   parms.m_quadraticTerm*(zDiffInv*zDiff4 - zBeg3)*0.666666666667 +
			   parms.m_cubicTerm*(0.1*zDiffInv*(zEnd2*zEnd3 - zBeg2*zBeg3) - 0.5*zBeg2*zBeg2);
}

inline double
SolenoidParametrization::interpolate(int key1,
                                     int key2,
                                     int key3,
                                     int key4,
                                     const Parameters& parms) const
{
    return ((m_parameters[key1]*parms.m_complementZ +
	     m_parameters[key2]*parms.m_interpolateZ)*parms.m_complementTheta +
	    (m_parameters[key3]*parms.m_complementZ +
	     m_parameters[key4]*parms.m_interpolateZ)*parms.m_interpolateTheta);
}

inline void
SolenoidParametrization::setTerms(int key1, Parameters& parms) const
{
    int		key2   	= key1 + s_numberParameters;
    int		key3   	= key2 + s_numberParameters;
    int		key4   	= key3 + s_numberParameters;

    assert (key1 >= 0);
    assert (key4 < 14688);
    assert (m_parameters[key1] != 0.);
    assert (m_parameters[key3] != 0.);
    if (parms.m_cotTheta < 7.)
    {
	assert (m_parameters[key2] != 0.);
	assert (m_parameters[key4] != 0.);
    }
    
    
    parms.m_fieldAtOrigin	= interpolate(key1++,key2++,key3++,key4++,parms);
    parms.m_quadraticTerm 	= interpolate(key1++,key2++,key3++,key4++,parms);
    parms.m_cubicTerm       	= interpolate(key1,key2,key3,key4,parms);
}

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline double
SolenoidParametrization::centralField (void) const
{ return m_centralField; }

inline double
SolenoidParametrization::fieldComponent (double z, const Parameters& parms) const
{
    double	z_local		= parms.m_signTheta*z - parms.m_zAtAxis;
    double	z_squared	= z_local*z_local;
    double    	value		= parms.m_fieldAtOrigin +
				  parms.m_quadraticTerm*z_squared +
				  parms.m_cubicTerm*z_squared*z_local;
    return value;
}

inline double
SolenoidParametrization::fieldComponent (double r, double z, double cotTheta) const
{
    Amg::Vector3D field;
    Amg::Vector3D position(r, 0., z);
    m_magFieldSvc->getField(&position,&field);
    return	s_lightSpeed*(field.z() - field.perp()*cotTheta);
}

inline void
SolenoidParametrization::fieldIntegrals(double&	firstIntegral,
					double&	secondIntegral,
					double 	zBegin,
					double 	zEnd,
                                        Parameters& parms) const
{
    zBegin	= parms.m_signTheta*zBegin;
    zEnd	= parms.m_signTheta*zEnd;
    if (zEnd < s_zInner || zBegin > s_zInner)
    {
	integrate(firstIntegral,secondIntegral,zBegin-parms.m_zAtAxis,zEnd-parms.m_zAtAxis, parms);
    }
    else
    {
	integrate(firstIntegral,secondIntegral,zBegin-parms.m_zAtAxis,s_zInner, parms);
	int	key	= fieldKey(parms) + s_numberParameters/2;
	setTerms(key, parms);
	integrate(firstIntegral,secondIntegral,s_zInner,zEnd-parms.m_zAtAxis,parms);
    }
	
    // std::cout << " zBegin < 0. " << zBegin
    // 	      << "   zEnd " << zEnd
    // 	      << "   m_signTheta " << parms.m_signTheta
    // 	      << "   m_zAtAxis " << parms.m_zAtAxis << std::endl;
}

inline double
SolenoidParametrization::maximumR (void) const
{ return s_rInner; }

inline double
SolenoidParametrization::maximumZ (void) const
{ return s_zInner; }

inline bool
SolenoidParametrization::validOrigin(const Amg::Vector3D& origin) const
{ return (origin.perp()			< s_maximumImpactAtOrigin
	  && std::abs(origin.z())	< s_maximumZatOrigin); }

} // end of namespace

#endif // TRKEXSOLENOIDALINTERSECTOR_SOLENOIDPARAMETRIZATION_H
