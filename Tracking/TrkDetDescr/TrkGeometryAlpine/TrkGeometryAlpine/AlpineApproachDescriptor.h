/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlpineApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ALPINEAPPROPACHDESCRIPTOR_H
#define TRKGEOMETRY_ALPINEAPPROPACHDESCRIPTOR_H

// Trk
#include "TrkGeometry/IApproachDescriptor.h"


namespace Trk {
    /**
     @class AlpineApproachDescriptor
     
     Class to decide and return which approaching surface to be taken,
     it either has a 
     
     @author Remi.Lafaye@cern.ch
    */

  class AlpineApproachDescriptor : public IApproachDescriptor {
  public: 
    // Default constructor
    AlpineApproachDescriptor(ApproachSurfaces* aSurfaces, bool rebuild=true) : 
    IApproachDescriptor(aSurfaces,rebuild) {}
    
    // Default constructor
    AlpineApproachDescriptor(BinnedArray<ApproachSurfaces>* aSurfaceArray, Surface* aSurfaceArraySurface = 0) : 
    IApproachDescriptor(aSurfaceArray,aSurfaceArraySurface) {}
        
    /** get the compatible surfaces 
	- return : a boolean indicating if an actual intersection had been tried
	- fill vector of intersections
	- primary bin surface : sf
	- position & direction : pos, dir
        */
    virtual const ApproachSurfaces* approachSurfaces(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;
  };
}

#endif // TRKGEOMETRY_ALPINEAPPROPACHDESCRIPTOR_H

