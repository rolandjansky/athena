/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************
 Vertex region data-class

*****************************************************************************/

#ifndef IPATUTILITY_VERTEXREGION_H
#define IPATUTILITY_VERTEXREGION_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GeoPrimitives/GeoPrimitives.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class VertexRegion
{
public:
    VertexRegion (void);			// empty constructor
    VertexRegion (const Amg::Vector3D& centre,
		  const Amg::Vector3D& width);
    VertexRegion (const VertexRegion&);		// copy constructor
    ~VertexRegion (void);	       		// destructor

    const Amg::Vector3D&	centre (void) const;
    void			centre (Amg::Vector3D);
    const Amg::Vector3D&	width (void) const;
    void			width (Amg::Vector3D);

private:    
    Amg::Vector3D		m_centre;
    Amg::Vector3D		m_width;
    
    // assignment operator: no implementation
    VertexRegion &operator= (const VertexRegion&);
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const Amg::Vector3D&
VertexRegion::centre (void) const
{ return m_centre; }

inline void
VertexRegion::centre (Amg::Vector3D value)
{ m_centre  = value; }

inline const Amg::Vector3D&
VertexRegion::width (void) const
{ return m_width; }

inline void
VertexRegion::width (Amg::Vector3D value)
{ m_width  = value; }

#endif // IPATUTILITY_VERTEXREGION_H
