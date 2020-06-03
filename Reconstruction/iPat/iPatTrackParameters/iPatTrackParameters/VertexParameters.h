/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Vertex parameters data-class
 ----------------------------
 ***************************************************************************/

#ifndef IPATTRACKPARAMETERS_VERTEXPARAMETERS_H
# define IPATTRACKPARAMETERS_VERTEXPARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "EventPrimitives/EventPrimitives.h"

class VertexParameters
{
public:
    VertexParameters (void);				// empty constructor
    VertexParameters (Amg::Vector3D position, AmgSymMatrix(5)* covariance);
    VertexParameters (const VertexParameters&);		// copy constructor
    ~VertexParameters (void);	       			// destructor

    bool			has_covariance (void) const;
    const AmgSymMatrix(5)&	covariance (void) const;
    void			covariance (AmgSymMatrix(5)*);
    Amg::Vector3D		position (void) const;
    void			position (Amg::Vector3D);

private:    
    AmgSymMatrix(5)*	m_covariance;
    Amg::Vector3D	m_position;

    // assignment operator: undefined, no semantics
    VertexParameters &operator= (const VertexParameters &);     
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
VertexParameters::VertexParameters (void)
    :	m_covariance		(0),
	m_position		(Amg::Vector3D(0.,0.,0.))
{}

inline
VertexParameters::VertexParameters (Amg::Vector3D position, AmgSymMatrix(5)* covariance)
    :	m_covariance		(covariance),
	m_position		(position)
{}


inline
VertexParameters::VertexParameters (const VertexParameters& vertex)
    :	m_covariance		(0),
	m_position		(vertex.m_position)
{
    if (vertex.m_covariance != 0) m_covariance = new AmgSymMatrix(5)(*vertex.m_covariance);
}

inline
VertexParameters::~VertexParameters (void)
{
    delete m_covariance;
}

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline bool
VertexParameters::has_covariance (void) const
{
    if (m_covariance != 0) return true;
    return false;
}

inline const AmgSymMatrix(5)&
VertexParameters::covariance (void) const
{ return *m_covariance; }

inline void
VertexParameters::covariance (AmgSymMatrix(5)* value)
{
    delete m_covariance;
    m_covariance  = value;
}

inline Amg::Vector3D
VertexParameters::position (void) const
{ return m_position; }

inline void
VertexParameters::position (Amg::Vector3D value)
{ m_position  = value; }

#endif // IPATTRACKPARAMETERS_VERTEXPARAMETERS_H
