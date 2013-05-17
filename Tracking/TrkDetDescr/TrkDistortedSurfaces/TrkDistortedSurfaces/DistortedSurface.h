/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistortedSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTORTEDSURFACES_DISTORTEDSURFACE_H
#define TRKDISTORTEDSURFACES_DISTORTEDSURFACE_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
 
  class Surface;
  class DistortionDescriptor;

  /** 
   @class DistortedSurface

   The base class of all distorted surfaces
   in the Tracking realm.

   The distorted surface itself is the nominal surface
   + a distortion description.

   @author Andreas.Salzburger@cern.ch 
  */
  
 class DistortedSurface {

    public:
     /** Default Constructor - needed for pool and inherited classes */
     DistortedSurface(){}

     /** Destructor */
     virtual ~DistortedSurface(){}

     /** Get the Surface representation */
     virtual const Surface& surfaceRepresentation() const = 0;

     /** Get the Distortion descriptor  */
     virtual const DistortionDescriptor& distortionDescriptor() const = 0;
   
     /** Get the corrected surface - this is a factory (the user has to delete the surface) */
     virtual Surface* correctedSurface(const Amg::Vector2D& lp) const = 0;

  };
                           
} // end of namespace Trk

#endif // TRKALIGNABLESURFACES_DISTORTEDSURFACE_H

