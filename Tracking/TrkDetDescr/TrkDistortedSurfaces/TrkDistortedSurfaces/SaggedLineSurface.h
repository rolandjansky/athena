/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SaggedLineSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTORTEDSURFACES_SAGGEDLINESURFACE_H
#define TRKDISTORTEDSURFACES_SAGGEDLINESURFACE_H

// Geo & Maths
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDistortedSurfaces/DistortedSurface.h"
#include "TrkDistortedSurfaces/LineSaggingDescriptor.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "CxxUtils/CachedValue.h"
namespace Trk {

  class TrkDetElementBase;

  /** 
   @class SaggedLineSurface

   The base class of all distorted surfaces
   in the Tracking realm.

   The distorted surface itself is the nominal surface
   + a distortion description.

   @author Andreas.Salzburger@cern.ch 
  */
  
  class SaggedLineSurface : public StraightLineSurface, virtual public DistortedSurface {

    public:
     /** Default Constructor - needed for pool and inherited classes */
     SaggedLineSurface();

     /** Constructor for private surface */
     SaggedLineSurface(const Amg::Transform3D& htrans, 
                       double radius,
                       double halez,
                       LineSaggingDescriptor* lsd);

     SaggedLineSurface(const Amg::Transform3D& htrans);

     /** Constructor for public surface */
     SaggedLineSurface(const TrkDetElementBase& detelement, const Identifier& id);

     /** Constructor for public surface */
     SaggedLineSurface(
       const TrkDetElementBase& detelement,
       const Identifier& id,
       double wireLength,
       double wireTension,
       double linearDensity);

     /** Copy Constructor */
     SaggedLineSurface(const SaggedLineSurface& sls);

     /** Destructor */
     ~SaggedLineSurface();

     Trk::SaggedLineSurface& operator=(const Trk::SaggedLineSurface& sls);

     /** Get the Surface representation */
     const StraightLineSurface& surfaceRepresentation() const;

     /** Get the Distortion descriptor  */
     const LineSaggingDescriptor& distortionDescriptor() const;
   
     /** Get the NEW corrected surface - this is a factory (the user has to delete the surface) */
     StraightLineSurface* correctedSurface(const Amg::Vector2D& lp) const;


   protected:
     LineSaggingDescriptor*                         m_saggingDescriptor; //!< the distortion descriptor
     CxxUtils::CachedValue<Amg::Vector3D>           m_saggedLineDirection;     //!< nominal end position 
  };
 
/** provide the Surface interface */
inline const StraightLineSurface& SaggedLineSurface::surfaceRepresentation() const
{ return (*this); }

/** provide the distortion descriptor for the outside world */
inline const LineSaggingDescriptor& SaggedLineSurface::distortionDescriptor() const
{ return (*m_saggingDescriptor); }
                          
} // end of namespace Trk

#endif // TRKDISTORTEDSURFACES_SAGGEDLINESURFACE_H

