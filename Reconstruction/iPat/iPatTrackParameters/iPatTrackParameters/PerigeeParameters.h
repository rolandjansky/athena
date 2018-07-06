/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 PerigeeParameters describes 5 track parameters with their covariance matrix
 in the perigree representation (at closest tranverse approach to vertex)
 ------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKPARAMETERS_PERIGEE_PARAMETERS_H
# define IPATTRACKPARAMETERS_PERIGEE_PARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventPrimitives/EventPrimitives.h"
#include "iPatTrackParameters/TrackParameters.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PerigeeParameters:public TrackParameters
{
public:
    PerigeeParameters (void);
    PerigeeParameters (double  			cosPhi,
		       double  			cotTheta,
		       double  			qOverPt,
		       double  			sinPhi,
		       double  			transverseImpact,
		       const Amg::Vector3D&	vertex,
		       double  			z);
    PerigeeParameters (const PerigeeParameters&);	// copy constructor
    ~PerigeeParameters (void);				// destructor
 
    // forbidden assignment operator
   
    // note: set methods provided for the 5 perigee parameters, individually or 
    // together (fillPerigee), vertex and covariance matrix.

    bool			hasCovariance (void) const;
    AmgSymMatrix(5)*		getCovariance (void);
    const AmgSymMatrix(5)&	covariance (void) const;
    void			covariance (AmgSymMatrix(5)*);
    void			fillPerigee(double    	cosPhi,	
					    double    	cotTheta,	
					    double    	qOverPt,	
					    double    	sinPhi,
					    double    	transverseImpact,
					    double    	z);
    void			fillPerigee (const PerigeeParameters&);
    Amg::Vector3D		origin (void) const;    // transverse approach to vertex (on track)
    void			origin (double transverseImpact, double z);
    void			print (void) const;
    double			transverseImpact (void) const;
    void			transverseImpact (double);
    Amg::Vector3D		vertex (void) const;
    void			vertex (const Amg::Vector3D&);
    double			z (void) const;
    void			z (double);
    
private:
    AmgSymMatrix(5)*	m_covariance;
    double		m_transverseImpact;
    Amg::Vector3D	m_vertex;

    // assignment operator: undefined, no semantics
    PerigeeParameters &operator= (const PerigeeParameters&);
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
PerigeeParameters::PerigeeParameters (void)
    :	TrackParameters		(),
	m_covariance		(0),
	m_transverseImpact	(0.),
	m_vertex		(Amg::Vector3D(0.,0.,0.))
{}

inline	
PerigeeParameters::PerigeeParameters (double			cosPhi,
				      double			cotTheta,
				      double			qOverPt,
				      double			sinPhi,
				      double			transverseImpact,
				      const Amg::Vector3D&	vertex,
				      double			z)
    :	TrackParameters		(Amg::Vector3D(vertex.x() - transverseImpact*sinPhi,
					       vertex.y() + transverseImpact*cosPhi,
					       z),
				 cosPhi,
				 sinPhi,
				 cotTheta,
				 qOverPt),
    	m_covariance		(0),
    	m_transverseImpact    	(transverseImpact),		 
    	m_vertex	       	(vertex)
{}

inline
PerigeeParameters::PerigeeParameters (const PerigeeParameters& perigee)
    :	TrackParameters		(perigee.m_position,
				 perigee.m_cosPhi,
				 perigee.m_sinPhi,
				 perigee.m_cotTheta,
				 perigee.m_qOverPt),
    	m_covariance		(0),
    	m_transverseImpact	(perigee.m_transverseImpact),
        m_vertex		(perigee.m_vertex)
{
    if (perigee.m_covariance != 0) m_covariance = new AmgSymMatrix(5)(*perigee.m_covariance);
}

inline
PerigeeParameters::~PerigeeParameters (void)
{ delete m_covariance; }

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline bool
PerigeeParameters::hasCovariance (void) const
{
    if (m_covariance != 0) return true;
    return false;
}

inline AmgSymMatrix(5)*
PerigeeParameters::getCovariance (void)
{ return m_covariance; }

inline const AmgSymMatrix(5)&
PerigeeParameters::covariance (void) const
{ return *m_covariance; }

inline void
PerigeeParameters::covariance (AmgSymMatrix(5)* value)
{
    delete m_covariance;
    m_covariance = value;
}

inline void
PerigeeParameters::fillPerigee(double	cosPhi,
				double	cotTheta,
				double	qOverPt,
				double	sinPhi,
				double	transverseImpact,
				double	z)
{
    m_position		=  Amg::Vector3D(m_vertex.x() - transverseImpact*sinPhi,
					 m_vertex.y() + transverseImpact*cosPhi,
					 z);    
    m_cosPhi 		= cosPhi;
    m_sinPhi 		= sinPhi;
    m_cotTheta 		= cotTheta;
    m_qOverPt 	= qOverPt;
    m_transverseImpact	= transverseImpact;
}

inline void
PerigeeParameters::fillPerigee(const PerigeeParameters& perigee)
{
    m_position		= perigee.m_position;
    m_cosPhi 		= perigee.m_cosPhi;
    m_sinPhi 		= perigee.m_sinPhi;
    m_cotTheta		= perigee.m_cotTheta;
    m_qOverPt	 	= perigee.m_qOverPt;
    m_transverseImpact	= perigee.m_transverseImpact;
    m_vertex		= perigee.m_vertex;
}

inline Amg::Vector3D
PerigeeParameters::origin (void) const
{ return m_position; } 

inline void
PerigeeParameters::origin (double transverseImpact, double z)
{
    m_transverseImpact	= transverseImpact;
    m_position 		= Amg::Vector3D(m_vertex.x() - transverseImpact*m_sinPhi,
					m_vertex.y() + transverseImpact*m_cosPhi,
					z);
}

inline double
PerigeeParameters::transverseImpact (void) const
{ return m_transverseImpact; }

inline void
PerigeeParameters::transverseImpact (double value)
{
    m_transverseImpact	= value;
    m_position		= Amg::Vector3D(m_vertex.x() - m_transverseImpact*m_sinPhi,
					m_vertex.y() + m_transverseImpact*m_cosPhi,
					m_position.z());
}

inline Amg::Vector3D
PerigeeParameters::vertex (void) const
{ return m_vertex; }

inline void
PerigeeParameters::vertex (const Amg::Vector3D& value)
{ m_vertex = value; }

inline double
PerigeeParameters::z (void) const
{ return m_position.z(); }

inline void
PerigeeParameters::z (double value)
{ m_position = Amg::Vector3D(m_position.x(),m_position.y(),value); }

#endif // IPATTRACKPARAMETERS_PERIGEE_PARAMETERS_H
