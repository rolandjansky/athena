/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Track parameters base class
 note: all methods are inline to avoid library dependencies
 ----------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKPARAMETERS_TRACKPARAMETERS_H
# define IPATTRACKPARAMETERS_TRACKPARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TrackParameters
{
public:
    TrackParameters (void);
    TrackParameters (const Amg::Vector3D&	position,	// point where parameters defined,
		     double	       		cosPhi,		// transverse direction cosines,
		     double    			sinPhi,
		     double			cotTheta,
		     double			qOverPt); 	// signed inverse transverse momentum
    TrackParameters (const Amg::Vector3D&	position,	// point on track,
		     const Amg::Vector3D&	direction,	// direction cosines at point,
		     double			qOverP);	// signed inverse momentum at point
    TrackParameters (const TrackParameters&); 			// copy constructor
    virtual ~TrackParameters (void);	       			// destructor

    double		abs_pt (void) const;    		// unsigned pt (defined 0 if 1/pt=0)
    double		charge (void) const;			// defined 0 if 1/pt=0
    double		cosPhi (void) const;
    double		cosTheta (void) const;
    double		cotTheta (void) const;
    void		cotTheta (double);
    Amg::Vector3D	direction (void) const; 		// direction cosines
    double		eta (void) const;
    void		fill (const Amg::Vector3D&	position, // fill as similar constructor
			      double			cosPhi,	
			      double			sinPhi,
			      double			cotTheta,
			      double			qOverPt);
    void		fill(const Amg::Vector3D& 	position,
			     const Amg::Vector3D&	direction,
			     double			qOverP);
    void		fill (const TrackParameters&);
    double		qOverPt (void) const;
    void		qOverPt (double);
    bool		isDefined (void) const;
    double		phi (void) const;
    void		phi (double, double);
    Amg::Vector3D	position (void) const;
    void		position (const Amg::Vector3D&);
    double		pt (void) const;			// signed pt (0 if 1/pt=0 or charge=0)
    double		qOverP (void) const;
    double		sinPhi (void) const;
    double		sinTheta (void) const;
    double		theta (void) const;
    Amg::Vector3D	transverseDirection (void) const;	// transverse direction cosines
    void		transverseDirection (double cosPhi, double sinPhi, double cotTheta);

protected:
    Amg::Vector3D	m_position;
    double		m_cosPhi;
    double		m_sinPhi;
    double		m_cotTheta;
    double		m_qOverPt;

private:
    // assignment operator: no implementation
    TrackParameters &operator= (const TrackParameters&);
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
TrackParameters::TrackParameters (void)
    :	m_position	(0.,0.,0.),
	m_cosPhi	(0.),
	m_sinPhi	(0.),
	m_cotTheta	(0.),
	m_qOverPt	(0.)
{}

inline
TrackParameters::TrackParameters (const Amg::Vector3D&	position,
				  double		cosPhi,
				  double		sinPhi,
				  double		cotTheta,
				  double		qOverPt)
    :	m_position		(position),
	m_cosPhi		(cosPhi),
	m_sinPhi		(sinPhi),
	m_cotTheta		(cotTheta),
	m_qOverPt		(qOverPt)
{}

inline
TrackParameters::TrackParameters (const Amg::Vector3D&	position,
				  const Amg::Vector3D&	direction,
				  double		qOverP)
    :	m_position		(position)
{
    double inverseSinTheta	= 1./direction.perp();
    m_cosPhi			= direction.x()*inverseSinTheta;
    m_sinPhi			= direction.y()*inverseSinTheta;
    m_cotTheta			= direction.z()*inverseSinTheta;
    m_qOverPt			= qOverP*inverseSinTheta;
}

// copy constructor
inline
TrackParameters::TrackParameters (const TrackParameters& p)
    :	m_position		(p.m_position),
	m_cosPhi		(p.m_cosPhi),
	m_sinPhi		(p.m_sinPhi),
	m_cotTheta		(p.m_cotTheta),
	m_qOverPt		(p.m_qOverPt)
{}

// destructor
inline
TrackParameters::~TrackParameters (void)
{}

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
TrackParameters::abs_pt (void) const
{
    if (m_qOverPt != 0.)
    {
	return std::abs(1./m_qOverPt);
    }
    else
    {
	return 0.;
    }
}

inline double
TrackParameters::charge (void) const
{
    if (m_qOverPt > 0.)
    {
	return 1.;
    }
    else if (m_qOverPt < 0.)
    {
	return -1.;
    }
    else
    {
	return 0.;
    }
}
		
inline double
TrackParameters::cosPhi (void) const
{ return m_cosPhi; }

inline double
TrackParameters::cosTheta (void) const
{ return m_cotTheta/sqrt(1. + m_cotTheta*m_cotTheta); }

inline double
TrackParameters::cotTheta (void) const
{ return m_cotTheta; }

inline void
TrackParameters::cotTheta (double value)
{ m_cotTheta = value; }

inline Amg::Vector3D
TrackParameters::direction (void) const
{
    Amg::Vector3D	direction(m_cosPhi,m_sinPhi,m_cotTheta);
    direction 		*= 1./sqrt(1. + m_cotTheta*m_cotTheta);
    return direction;
}

inline double
TrackParameters::eta (void) const
{ return asinh(m_cotTheta); }

inline void
TrackParameters::fill (const Amg::Vector3D& 	position,
		       double			cosPhi,
		       double       		sinPhi,
		       double       		cotTheta,
		       double      		qOverPt)
{
    m_position	= position;
    m_cosPhi	= cosPhi;
    m_sinPhi	= sinPhi;
    m_cotTheta	= cotTheta;
    m_qOverPt	= qOverPt;
}

inline void
TrackParameters::fill (const Amg::Vector3D&	position,
		       const Amg::Vector3D&	direction,
		       double			qOverP)
{
    m_position  	= position;
    double projection	= 1./direction.perp();
    m_cosPhi 		= projection*direction.x();
    m_sinPhi 	       	= projection*direction.y();
    m_cotTheta		= projection*direction.z();
    m_qOverPt		= projection*qOverP;
}

inline void
TrackParameters::fill (const TrackParameters& p)
{
    m_position	= p.m_position;
    m_cosPhi	= p.m_cosPhi;
    m_sinPhi	= p.m_sinPhi;
    m_cotTheta	= p.m_cotTheta;
    m_qOverPt	= p.m_qOverPt;
}

inline double
TrackParameters::qOverPt (void) const
{ return m_qOverPt; }

inline void
TrackParameters::qOverPt (double value)
{ m_qOverPt = value; }

inline bool
TrackParameters::isDefined (void) const
{
    if (m_qOverPt != 0.
	|| m_cosPhi != 0.
	|| m_sinPhi != 0.) return true;
    return false;
}

inline double
TrackParameters::phi (void) const
{ return atan2(m_sinPhi,m_cosPhi); }

inline void
TrackParameters::phi (double cosPhi, double sinPhi)
{
    m_cosPhi	= cosPhi;
    m_sinPhi 	= sinPhi;
}

inline Amg::Vector3D
TrackParameters::position (void) const
{ return m_position; }

inline void
TrackParameters::position (const Amg::Vector3D& value)
{ m_position  = value; }

inline double
TrackParameters::pt (void) const
{
    if (m_qOverPt != 0.)
    {
	return 1./m_qOverPt;
    }
    else
    {
	return 0.;
    }
}

inline double
TrackParameters::qOverP (void) const
{ return m_qOverPt/sqrt(1. + m_cotTheta*m_cotTheta); }

inline double
TrackParameters::sinPhi (void) const
{ return m_sinPhi; }

inline double
TrackParameters::sinTheta (void) const
{ return 1./sqrt(1. + m_cotTheta*m_cotTheta); }

inline double
TrackParameters::theta (void) const
{ return acos(m_cotTheta/sqrt(1. + m_cotTheta*m_cotTheta)); }

inline Amg::Vector3D
TrackParameters::transverseDirection (void) const
{ return Amg::Vector3D(m_cosPhi,m_sinPhi,m_cotTheta); }

inline void
TrackParameters::transverseDirection (double cosPhi, double sinPhi, double cotTheta)
{
    m_cosPhi 	= cosPhi;
    m_sinPhi 	= sinPhi;
    m_cotTheta	= cotTheta;
}

#endif // IPATTRACKPARAMETERS_TRACKPARAMETERS_H
