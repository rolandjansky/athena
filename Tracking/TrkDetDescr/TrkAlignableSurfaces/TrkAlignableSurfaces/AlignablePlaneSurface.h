/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignablePlaneSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKALIGNABLESURFACES_ALGINABLEPLANESURFACE_H
#define TRKALIGNABLESURFACES_ALGINABLEPLANESURFACE_H

// Trk
#include "TrkAlignableSurfaces/AlignableSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Identifier
#include "Identifier/Identifier.h"

namespace Trk {

class Surface;
class Layer;
class TrkDetElementBase;

/**
 @class AlignablePlaneSurface

 see base classes for more desription

 Let's denote the nominal transform as @f$ T_n @f$ and the
 transform describing the local frame with respect to the global frame as @f$ T @f$,
 then the methods (1), (2) and (3) yield to :

   - <b>addAlignmentCorrection(@f$ T_c @f$)</b> : @f$ T = T_c \cot T @f$

   - <b>setAlignmentCorrection(@f$ T_c @f$)</b> : @f$ T = T_c \cot T_n @f$

   - <b>setAlignableTransform(@f$ T_a @f$)</b> : @f$ T = T_a @f$

 To avoid dependencies on GeoModel and/or InnerDetector pure Amg::Transform3D objects are used.

 @author Andreas.Salzburger@cern.ch
*/

class AlignablePlaneSurface final
  : public PlaneSurface
  , public AlignableSurface
{

public:
  /** Default Constructor - needed for pool and inherited classes */
  AlignablePlaneSurface();

  /** Constructor with already different transform from the nominal one */
  AlignablePlaneSurface(const PlaneSurface& psf, Amg::Transform3D* alignedTransf = nullptr);

  /** Copy Constructor  */
  AlignablePlaneSurface(const AlignablePlaneSurface& psf);

  /** Destructor */
  virtual ~AlignablePlaneSurface();

  /** -------------------------- interface from Surface ------------------ */

  /**Assignment operator */
  AlignablePlaneSurface& operator=(const AlignablePlaneSurface& sf);
  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override final;
  /**Implicit constructor - uses the copy constructor */
  virtual AlignablePlaneSurface* clone() const override final;

  /** return associated Detector Element - forwarded from nominal Surface */
  const TrkDetElementBase* associatedDetectorElement() const ;

  /** return Identifier of the associated Detector Element - forwarded from nominal Surface */
  Identifier associatedDetectorElementIdentifier() const;

  /** return the associated Layer - forwarded from nominal Surface */
  const Layer* associatedLayer() const;

  /** -------------------------- interface from AlignableSurface ------------------ */
  /** Get the Surface representation */
  virtual const PlaneSurface& surfaceRepresentation() const override final;

  /** Get the nominal surface */
  virtual const PlaneSurface& nominalSurface() const override final; 

  /** Get the nominal transformation */
  virtual const Amg::Transform3D& nominalTransform() const override final;

  /** Add an alignment correction on top of the actual one */
  virtual void addAlignmentCorrection(Amg::Transform3D& corr) override final;

  /** Set an alignment correction on top of the nominal one */
  virtual void setAlignmentCorrection(Amg::Transform3D& corr) override final;

  /** Set an alignment correction on top of the nominal one */
  virtual void setAlignableTransform(Amg::Transform3D& trans) override final ;

protected:
  /** The pointer ro the nominal Surface */
  const PlaneSurface* m_nominalSurface;
};

} // end of namespace Trk

#include "TrkAlignableSurfaces/AlignablePlaneSurface.icc"
#endif // TRKALIGNABLESURFACES_ALGINABLESURFACE_H
