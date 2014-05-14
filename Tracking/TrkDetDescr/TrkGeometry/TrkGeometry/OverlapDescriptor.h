/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_OVERLAPDESCRIPTOR_H
#define TRKGEOMETRY_OVERLAPDESCRIPTOR_H

// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
// STL
#include <utility>

namespace Trk {

    class Surface;

    typedef std::pair<const Surface*, PropDirection> OverlapCell;
    typedef std::pair<OverlapCell,int>               OverlapCellCondition;
    
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
    
        /**Interface method for most probable OverlapCell*/
        virtual const OverlapCell overlapSurface(const TrackParameters& tp,
                                                 const Surface* sfreq,
                                                 PropDirection dir) const = 0;
                                                 
                                                 
        /** Interface method for RISC logic - start/end surface are needed for on-layer search*/
        virtual const std::vector<OverlapCellCondition>&  overlapCells(const TrackParameters& tp,
                                                                       PropDirection dir,
                                                                       const Trk::Surface* startSf = 0,
                                                                       const Trk::Surface* endSf = 0) const = 0;
                                                 
       
    };

}

#endif


