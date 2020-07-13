/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_APPROPACHDESCRIPTOR_H
#define TRKGEOMETRY_APPROPACHDESCRIPTOR_H

// Trk
#include "TrkGeometry/IApproachDescriptor.h"

namespace Trk {
    /**
     @class ApproachDescriptor
     
     Class to decide and return which approaching surface to be taken,
     it either has a 
     
     @author Andreas.Salzburger@cern.ch
    */

  class ApproachDescriptor : public IApproachDescriptor {
      public: 
        // Default constructor
        ApproachDescriptor(std::unique_ptr<ApproachSurfaces> aSurfaces,
                           bool rebuild = true)
          : IApproachDescriptor(std::move(aSurfaces), rebuild)
        {}

        // Default constructor
        ApproachDescriptor(
          std::unique_ptr<BinnedArray<ApproachSurfaces>> aSurfaceArray,
          Surface* aSurfaceArraySurface = nullptr)
          : IApproachDescriptor(std::move(aSurfaceArray), aSurfaceArraySurface)
        {}

        /** get the compatible surfaces 
            - return : a boolean indicating if an actual intersection had been tried
            - fill vector of intersections
            - primary bin surface : sf
            - position & direction : pos, dir
        */
        virtual const ApproachSurfaces*
        approachSurfaces(const Amg::Vector3D& pos,
                         const Amg::Vector3D& dir) const override;

      private :
    };
}

#endif

