/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    // private constructors (as singleton)
    ~SolenoidParametrization			(void);
    
    // forbidden copy constructor
    // forbidden assignment operator

    static SolenoidParametrization*	instance (MagField::IMagFieldSvc* magFieldSvc);	// initialize singleton
    static void                 clearInstance(void);	// clear (to be used before reinitialization)
    
    double			centralField (void) const;
    double			fieldComponent (double) const;		// parametrized - perp to track in rz-plane
    double			fieldComponent (double r, double z, double cotTheta) const; // from MagFieldSvc
    void			fieldIntegrals (double&, double&, double, double);
    bool			hasParametrization (void) const;
    double			maximumR (void) const;		// param valid to maximumR
    double			maximumZ (void) const;		// param valid to maximumZ
    void			printFieldIntegrals (void) const;
    void			printParametersForEtaLine (double, double);
    void			printResidualForEtaLine (double, double);
    void			setParameters (double, double, double); // set line in rz-plane
    bool			validOrigin(Amg::Vector3D origin) const; // param valid for this origin?

private:
    //  implementation does not work without magnetic field service
    //  SolenoidParametrization		(void);				// singleton constructor
    SolenoidParametrization		(MagField::IMagFieldSvc* magFieldSvc);	// configuration from Intersector
    int				fieldKey(void);
    void			integrate(double&, double&, double, double) const;
    double			interpolate(int, int, int, int) const;
    void			parametrizeSolenoid (void);
    void			setTerms(int);

    static SolenoidParametrization* 		s_instance;
    static const double		s_binInvSizeTheta;
    static const double		s_binInvSizeZ;
    static const double		s_binZeroTheta;
    static const double		s_binZeroZ;
    static double		s_centralField;
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
    static double		s_parameters[14688];

    double 			m_complementTheta;
    double 			m_complementZ;
    double 			m_cotTheta;
    double			m_cubicTerm;
    double			m_fieldAtOrigin;
    bool			m_hasParametrization;
    double 			m_interpolateTheta;
    double 			m_interpolateZ;
    MagField::IMagFieldSvc*	m_magFieldSvc;
    double			m_quadraticTerm;
    double			m_signTheta;
    double			m_zAtAxis;
    
    // copy, assignment: no semantics, no implementation
    SolenoidParametrization (const SolenoidParametrization&);
    SolenoidParametrization &operator= (const SolenoidParametrization&);
};

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline int
SolenoidParametrization::fieldKey(void) 
{
    double	z	= m_zAtAxis - s_binZeroZ;
    int		zBin  	= static_cast<int>(s_binInvSizeZ*z);
    m_interpolateZ    	= z*s_binInvSizeZ - double(zBin);
    if (zBin < 0)
    {
	m_interpolateZ	= 0.;
	zBin 		= 0;
    }
    else if (zBin > s_maxBinZ - 2)
    {
	m_interpolateZ	= 0.;
	zBin 		= s_maxBinZ - 1;
    }
    m_complementZ	= 1. - m_interpolateZ;
    
    int	thetaBin 	= static_cast<int>(s_binInvSizeTheta*m_cotTheta);
    m_interpolateTheta = m_cotTheta*s_binInvSizeTheta - double(thetaBin);
    if (thetaBin > s_maxBinTheta - 3)
    {
	m_interpolateTheta	= 0.;
	thetaBin 		= s_maxBinTheta - 2;
    }
    m_complementTheta	= 1. - m_interpolateTheta;
    // std::cout << " m_zAtAxis " << m_zAtAxis
    // 	      << "   m_cotTheta " << m_cotTheta
    // 	      << "   zBin " << zBin
    // 	      << "   interpolateZ " << m_interpolateZ
    // 	      << "   thetaBin " << thetaBin
    // 	      << "   interpolateTheta " << m_interpolateTheta << std::endl;
    
    return 2*s_numberParameters*(s_maxBinTheta*zBin + thetaBin);
}

inline void
SolenoidParametrization::integrate(double& 	firstIntegral,
				   double& 	secondIntegral,
				   double 	zBegin,
				   double 	zEnd) const
{ 
    double	zDiff	=  zEnd - zBegin;
    double 	zBeg2	=  zBegin*zBegin;
    double 	zBeg3	=  zBeg2*zBegin;
    double 	zEnd2	=  zEnd*zEnd;
    double 	zEnd3	=  zEnd2*zEnd;
    double	zDiff4	=  0.25*(zEnd2 + zBeg2)*(zEnd2 - zBeg2);
    
    firstIntegral	+= m_fieldAtOrigin*zDiff +
			   m_quadraticTerm*(zEnd3 - zBeg3)*0.333333333333 +
			   m_cubicTerm*zDiff4;
    double   zDiffInv	=  1./zDiff;
    secondIntegral	+= m_fieldAtOrigin*zDiff +
 			   m_quadraticTerm*(zDiffInv*zDiff4 - zBeg3)*0.666666666667 +
			   m_cubicTerm*(0.1*zDiffInv*(zEnd2*zEnd3 - zBeg2*zBeg3) - 0.5*zBeg2*zBeg2);
}

inline double
SolenoidParametrization::interpolate(int key1,
		       int key2,
		       int key3,
		       int key4) const
{
    return ((s_parameters[key1]*m_complementZ +
	     s_parameters[key2]*m_interpolateZ)*m_complementTheta +
	    (s_parameters[key3]*m_complementZ +
	     s_parameters[key4]*m_interpolateZ)*m_interpolateTheta);
}

inline void
SolenoidParametrization::setTerms(int key1)
{
    int		key2   	= key1 + s_numberParameters;
    int		key3   	= key2 + s_numberParameters;
    int		key4   	= key3 + s_numberParameters;

    assert (key1 >= 0);
    assert (key4 < 14688);
    assert (s_parameters[key1] != 0.);
    assert (s_parameters[key3] != 0.);
    if (m_cotTheta < 7.)
    {
	assert (s_parameters[key2] != 0.);
	assert (s_parameters[key4] != 0.);
    }
    
    
    m_fieldAtOrigin	= interpolate(key1++,key2++,key3++,key4++);
    m_quadraticTerm 	= interpolate(key1++,key2++,key3++,key4++);
    m_cubicTerm       	= interpolate(key1,key2,key3,key4);
}

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

// class does not work without magnetic field service
//inline SolenoidParametrization*
//SolenoidParametrization::instance()
//{
//    if (s_instance == 0) s_instance = new SolenoidParametrization();
//    return s_instance;
//}

inline SolenoidParametrization*
SolenoidParametrization::instance(MagField::IMagFieldSvc* magFieldSvc)
{
    // this method is provided to configure the initialization.
    // It should not be called more than once without an intervening clearInstance().
    assert (s_instance == 0);
    s_instance = new SolenoidParametrization(magFieldSvc);
    return s_instance;
}

inline double
SolenoidParametrization::centralField (void) const
{ return s_centralField; }

inline double
SolenoidParametrization::fieldComponent (double z) const
{
    double	z_local		= m_signTheta*z - m_zAtAxis;
    double	z_squared	= z_local*z_local;
    double    	value		= m_fieldAtOrigin +
				  m_quadraticTerm*z_squared +
				  m_cubicTerm*z_squared*z_local;
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
					double 	zEnd) 
{
    zBegin	= m_signTheta*zBegin;
    zEnd	= m_signTheta*zEnd;
    if (zEnd < s_zInner || zBegin > s_zInner)
    {
	integrate(firstIntegral,secondIntegral,zBegin-m_zAtAxis,zEnd-m_zAtAxis);
    }
    else
    {
	integrate(firstIntegral,secondIntegral,zBegin-m_zAtAxis,s_zInner);
	int	key	= fieldKey() + s_numberParameters/2;
	setTerms(key);
	integrate(firstIntegral,secondIntegral,s_zInner,zEnd-m_zAtAxis);
    }
	
    // std::cout << " zBegin < 0. " << zBegin
    // 	      << "   zEnd " << zEnd
    // 	      << "   m_signTheta " << m_signTheta
    // 	      << "   m_zAtAxis " << m_zAtAxis << std::endl;
}

inline bool
SolenoidParametrization::hasParametrization (void) const
{ return m_hasParametrization; }
   
inline double
SolenoidParametrization::maximumR (void) const
{ return s_rInner; }

inline double
SolenoidParametrization::maximumZ (void) const
{ return s_zInner; }

inline void
SolenoidParametrization::setParameters (double r, double z, double cotTheta) 
{
    if (cotTheta > 0.)
    {
	m_signTheta	= 1.;
	m_cotTheta	= cotTheta;
	m_zAtAxis    	= z - r*cotTheta;
    }
    else
    {
	m_signTheta	= -1.;
	m_cotTheta	= -cotTheta;
	m_zAtAxis    	=  r*cotTheta - z;
    }
    int	key  		= fieldKey();
    if (r > s_rInner || m_signTheta*z > s_zInner)
    {
	key	+= s_numberParameters/2;
    }
//      cout << "r " << r << "   key " << key << endl;
    setTerms(key);
}
 
inline bool
SolenoidParametrization::validOrigin(Amg::Vector3D origin) const
{ return (origin.perp()			< s_maximumImpactAtOrigin
	  && std::abs(origin.z())	< s_maximumZatOrigin); }

} // end of namespace

#endif // TRKEXSOLENOIDALINTERSECTOR_SOLENOIDPARAMETRIZATION_H
