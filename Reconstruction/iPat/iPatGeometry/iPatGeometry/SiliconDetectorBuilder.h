/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 build one silicon detector element in athena (from InDetDD)
 ***************************************************************************/

#ifndef IPATGEOMETRY_SILICONDETECTORBUILDER_H
# define IPATGEOMETRY_SILICONDETECTORBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "iPatGeometry/DetectorShape.h"
#include "iPatGeometry/SiliconDetector.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace InDetDD
{
    class SiDetectorElement;
}

class SiliconDetectorBuilder
{

public:
    // constructor
    SiliconDetectorBuilder (void);
    // destructor
    ~SiliconDetectorBuilder (void);

    // create detector corresponding to SiDetectorElement
    SiliconDetector*			detector(const InDetDD::SiDetectorElement& element);
    // current shape
    DetectorShape*			shape(void) const;
    // vector of all known shapes
    const std::vector<DetectorShape*>&	shapes(void) const;

private:
    typedef	std::vector<DetectorShape*>				shape_vector;
    typedef	shape_vector::reverse_iterator				shape_riterator;
 
    shape_vector		m_shapes;
    shape_riterator		m_shapeRiterator;
    
    // copy, assignment: no semantics, no implementation
    SiliconDetectorBuilder (const SiliconDetectorBuilder&);
    SiliconDetectorBuilder &operator= (const SiliconDetectorBuilder&);
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline DetectorShape*
SiliconDetectorBuilder::shape(void) const
{ return *m_shapeRiterator; }

inline const std::vector<DetectorShape*>&
SiliconDetectorBuilder::shapes(void) const
{ return m_shapes; }
    
#endif // IPATGEOMETRY_SILICONDETECTORBUILDER_H

