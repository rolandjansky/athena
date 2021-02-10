/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_OVERLAPDESCRIPTOR_H
#define TRKGEOMETRY_OVERLAPDESCRIPTOR_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkSurfaces/Surface.h"

//#include "TrkParameters/TrackParameters.h"

// STL
#include <utility>

namespace Trk {


     typedef ObjectIntersection<Surface> SurfaceIntersection;
    
     /**
     @class OverlapDescriptor
     
     BaseClass to be overloaded for describing overlaps 
     and next-by elements for the sub-detector implementations.
     
     It takes the TrackParameters (on the Surface),
     and the Surface pointer to an eventual former search for multiple calls

     the return value is a pair of Surface, PropDirection
     
     @author Andreas.Salzburger@cern.ch
    */

    class OverlapDescriptor {
      public: 
        /**Default constructor*/
        OverlapDescriptor(){}
        
        /**Virtual destructor*/
        virtual ~OverlapDescriptor(){}
        
        /**Pseudo-constructor*/
        virtual OverlapDescriptor* clone() const  = 0;
    
        /** get the compatible surfaces 
            - return : a boolean indicating if an actual intersection had been tried
            - fill vector of intersections
            - primary bin surface : sf
            - position & direction : pos, dir
        */
        virtual bool reachableSurfaces(std::vector<SurfaceIntersection>& cSurfaces, 
                                       const Trk::Surface& sf,
                                       const Amg::Vector3D& pos,
                                       const Amg::Vector3D& dir) const = 0;
                                       
        virtual bool reachableSurfaces(std::vector<SurfaceIntersection>& cSurfaces, 
                                       const Amg::Vector3D& pos,
                                       const Amg::Vector3D& dir) const = 0;
       
    };

}

#endif


