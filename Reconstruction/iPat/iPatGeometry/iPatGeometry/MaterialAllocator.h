/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 hard-wired material properties for silicon layers, inert structure and Trt
 (radiation thickness, energy loss coefficients)
 --------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATGEOMETRY_MATERIALALLOCATOR_H
# define IPATGEOMETRY_MATERIALALLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <string>
#include <utility>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class SiliconLayer;

class MaterialAllocator
{

public:
    // constructor
    MaterialAllocator (std::string versionName);

    // implicit destructor
    // forbidden copy constructor
    // forbidden assignment operator

    // energy loss coefficient and radiation thickness
    std::pair<double,double>	layerMaterial (const SiliconLayer& layer);
					 
private:
    
    bool		m_useTDR_Material;
    std::string		m_versionName;
    
    // copy, assignment: no semantics, no implementation
    MaterialAllocator (const MaterialAllocator&);
    MaterialAllocator &operator= (const MaterialAllocator&);
};

#endif // IPATGEOMETRY_MATERIALALLOCATOR_H
