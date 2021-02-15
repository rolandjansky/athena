/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignableSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKALIGNABLESURFACES_ALGINABLESURFACE_H
#define TRKALIGNABLESURFACES_ALGINABLESURFACE_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

class Surface;

/**
 @class AlignableSurface

 Base class of all alignable surfaces.
 An AlignableSurface extends a surface of concrete type
(and has to point back to the one it is created from).
 Hence only public surfaces can be used to create AlignableSurfaces.

 Let's denote the nominal transform as @f$ T_n @f$ and the
 transform describing the local frame with respect to the global frame as @f$ T @f$,
 then the methods (1), (2) and (3) yield to :

   - <b>addAlignmentCorrection(@f$ T_c @f$)</b> : @f$ T = T_c \cot T @f$

   - <b>setAlignmentCorrection(@f$ T_c @f$)</b> : @f$ T = T_c \cot T_n @f$

   - <b>setAlignableTransform(@f$ T_a @f$)</b> : @f$ T = T_a @f$

 To avoid dependencies on GeoModel and/or InnerDetector pure Amg::Transform3D objects are used.

 @author Andreas.Salzburger@cern.ch
*/

class AlignableSurface
{

public:
  /** Default Constructor - needed for pool and inherited classes */
  AlignableSurface() = default;

  /** Destructor */
  virtual ~AlignableSurface() = default;

  /** Get the Surface representation */
  virtual const Surface& surfaceRepresentation() const = 0;

  /** Get the nominal surface */
  virtual const Surface& nominalSurface() const = 0;

  /** Get the nominal transformation */
  virtual const Amg::Transform3D& nominalTransform() const = 0;

  /** Add an alignment correction on top of the actual one */
  virtual void addAlignmentCorrection(Amg::Transform3D& corr) = 0;

  /** Set an alignment correction on top of the nominal one */
  virtual void setAlignmentCorrection(Amg::Transform3D& corr) = 0;

  /** Set an alignment correction on top of the nominal one */
  virtual void setAlignableTransform(Amg::Transform3D& trans) = 0;
};

} // end of namespace Trk

#endif // TRKALIGNABLESURFACES_ALGINABLESURFACE_H
