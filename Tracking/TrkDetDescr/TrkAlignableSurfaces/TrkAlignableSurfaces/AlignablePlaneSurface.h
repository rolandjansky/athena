/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
 class AlignablePlaneSurface : public PlaneSurface, 
                               public AlignableSurface {

    public:
     /** Default Constructor - needed for pool and inherited classes */
     AlignablePlaneSurface();

     /** Constructor with already different transform from the nominal one */
     AlignablePlaneSurface(const PlaneSurface& psf, Amg::Transform3D* alignedTransf=0);

     /** Copy Constructor  */
     AlignablePlaneSurface(const AlignablePlaneSurface& psf);

     /** Destructor */
     virtual ~AlignablePlaneSurface();

     /** -------------------------- interface from Surface ------------------ */
     
     /**Assignment operator */
     AlignablePlaneSurface& operator=(const AlignablePlaneSurface& sf);
     /**Equality operator*/
     bool operator==(const Surface& sf) const; 
     /**Implicit constructor - uses the copy constructor */
     AlignablePlaneSurface* clone() const;

     /** return associated Detector Element - forwarded from nominal Surface */
     const TrkDetElementBase* associatedDetectorElement() const;
    
     /** return Identifier of the associated Detector Element - forwarded from nominal Surface */
     const Identifier associatedDetectorElementIdentifier() const;
    
     /** return the associated Layer - forwarded from nominal Surface */
     const Layer* associatedLayer() const;

    /** -------------------------- interface from AlignableSurface ------------------ */
     
    /** Get the Surface representation */
    const PlaneSurface& surfaceRepresentation() const;

    /** Get the nominal surface */
    const PlaneSurface& nominalSurface() const;

    /** Get the nominal transformation */
    const Amg::Transform3D& nominalTransform() const;

    /** Add an alignment correction on top of the actual one */
    void addAlignmentCorrection(Amg::Transform3D& corr) const;

    /** Set an alignment correction on top of the nominal one */
    void setAlignmentCorrection(Amg::Transform3D& corr) const;

    /** Set an alignment correction on top of the nominal one */
    void setAlignableTransform(Amg::Transform3D& trans) const;

   protected:
    /** The pointer ro the nominal Surface */
    mutable const PlaneSurface*        m_nominalSurface;
                         
  };
  
  inline AlignablePlaneSurface* AlignablePlaneSurface::clone() const
  { return new AlignablePlaneSurface(*this); }

  inline const TrkDetElementBase* AlignablePlaneSurface::associatedDetectorElement() const
  { return m_nominalSurface->associatedDetectorElement(); }
    
  inline const Identifier AlignablePlaneSurface::associatedDetectorElementIdentifier() const
  { return m_nominalSurface->associatedDetectorElementIdentifier(); }
    
  inline const Trk::Layer* AlignablePlaneSurface::associatedLayer() const
  { return m_nominalSurface->associatedLayer(); }

  inline const PlaneSurface& AlignablePlaneSurface::surfaceRepresentation() const
  { return(*this); }

  inline const PlaneSurface& AlignablePlaneSurface::nominalSurface() const
  { return(*m_nominalSurface); }

  inline const Amg::Transform3D& AlignablePlaneSurface::nominalTransform() const
  { return m_nominalSurface->transform(); }

} // end of namespace Trk

#endif // TRKALIGNABLESURFACES_ALGINABLESURFACE_H

