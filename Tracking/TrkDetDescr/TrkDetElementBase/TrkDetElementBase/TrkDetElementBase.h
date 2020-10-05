/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkDetElementBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETELEMENTBASE_TRKDETELEMENTBASE_H
#define TRKDETELEMENTBASE_TRKDETELEMENTBASE_H

// Math & Geometry library
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "GeoModelKernel/GeoVDetectorElement.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

namespace Trk 
{

  class Surface;
  class SurfaceBounds;

  /** @class TrkDetElementBase
  
    This is the base class for all tracking detector elements
    with read-out relevant information.
  
    It serves as the interface to GeoModel and hence incorporates the division between
    CLHEP (GeoModel) and Amg (Tracking) math libraries.
  
    It is extended in the Inner Detector and Muon System realm.
  
    @author Andreas.Salzburger@cern.ch
  
    */
  
   
  class TrkDetElementBase : public GeoVDetectorElement {
  
    public:
    
      /** Constructor from GeoVFullPhysVolume */
      TrkDetElementBase(const GeoVFullPhysVol* fullPhysVol); 
      
      /** virtual Destructor */
      virtual ~TrkDetElementBase() = default;

      /** Identifier */
      virtual Identifier identify() const = 0;
    
      /**Identifier hash*/
      virtual IdentifierHash identifyHash() const = 0;
    
      /**Return local to global transform*/
      virtual const Amg::Transform3D & transform() const = 0;
    
      /**Return local to global transform associated with this identifier*/
      virtual const Amg::Transform3D & transform(const Identifier& id) const = 0;
    
      /**Return surface associated with this detector element*/
      virtual const Surface& surface () const = 0;
      
      /**Return surface associated with this identifier, which should come from the
      PrepRawData object 
      (i.e. Surface s = PRD.detElement().surface( PRD.identify() ). 
      This is only really relevant for the TRT (where there are several 
      surfaces per detector element). For other detector elements it will just
      return surface() ... the Identifier is ignored.
      */
      virtual const Surface& surface (const Identifier& id) const = 0;
     
      /**Return the boundaries of the element*/
      virtual const SurfaceBounds & bounds() const = 0;
    
      /**Return the boundaries of the surface associated with this identifier.
      For TRT this will be a straw, while for silicon it returns the same as
      bounds()*/
      virtual const SurfaceBounds & bounds(const Identifier& id) const = 0;
    
      /**Return the center of the element*/
      virtual const Amg::Vector3D & center() const = 0;
    
      /**Return the center of the surface associated with this identifier
       In the case of silicon it returns the same as center()*/  
      virtual const Amg::Vector3D & center(const Identifier& id) const = 0;
    
      /**Return the normal of the element*/
      virtual const Amg::Vector3D & normal() const = 0;
    
      /**Return the normal of the surface associated with this identifier
       In the case of silicon it returns the same as normal()*/  
      virtual const Amg::Vector3D & normal(const Identifier& id) const = 0;
    
    };

}//end of ns

#endif

