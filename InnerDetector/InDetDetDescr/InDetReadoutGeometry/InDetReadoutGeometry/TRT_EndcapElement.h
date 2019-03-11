/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_EndcapElement.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_EndcapElement_h
#define TRT_EndcapElement_h 1
#include "InDetReadoutGeometry/TRT_EndcapCode.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetReadoutGeometry/TRT_EndcapDescriptor.h"
#include <atomic>

class TRT_ID;

namespace Trk{
  class SurfaceBounds;
}

namespace InDetDD {

  class TRT_EndcapConditions;

  /** @class TRT_EndcapElement 
  
      Extended class of a TRT_BaseElement to describe a readout elment in the endcap.
      It secifies the strawTransfrom methods by getting the information from GeoModel
      
      The readout element in the EC is a wheel with straws radially placed on it.
      
      The number of straws described by this readout element can be 
      retrieved by nStraws() 
      
      @author: Grant Gorfine
      - modified & maintained: Nick Styles, Andreas Salzburger
      
      */
  
  
  class TRT_EndcapElement : public TRT_BaseElement   {
    
    public:
      /** Constructor */
      TRT_EndcapElement(const GeoVFullPhysVol *volume, 
      	                const TRT_EndcapDescriptor *descriptor, 
      	                bool isPositive, 
      	                unsigned int wheelIndex, 
      	                unsigned int strawLayIndex,
      	                unsigned int phiIndex,
      	                const TRT_ID * idHelper,
      		            const TRT_Conditions * conditions);
    
      /** Destructor: */
      virtual ~TRT_EndcapElement();
      
      /** Type information: */
      virtual TRT_BaseElement::Type type() const {return TRT_BaseElement::ENDCAP;} 
    
      /** Default Local -> global transform of the straw (ie before alignment corrections) */
      virtual HepGeom::Transform3D defStrawTransform(int straw) const;
    
      /** Number of straws in the element */
      virtual unsigned int nStraws() const;
    
      /** Active straw length */
      virtual const double & strawLength() const;
    
      /** StrawDirection. +1 if axis is in same direction as local coordinate, -1 otherwise. */
      virtual int strawDirection() const;
    
      /** Returns a pointer to conditions data.  This includes
         information on dead and noisy wires, as well as wire
         sags. */
      const TRT_EndcapConditions * getConditionsData() const;
      
      /** Returns a pointer to a descriptor, giving common
          information on module construction */
      const TRT_EndcapDescriptor * getDescriptor() const;
      
      /** Next in Z: */
      const TRT_EndcapElement * nextInZ() const;
      
      /** Prev in Z: */
      const TRT_EndcapElement * previousInZ() const;
      
      /** Set Next in Z */
      void setNextInZ(const TRT_EndcapElement *element);
      
      /** Set Previous in Z */
      void setPreviousInZ(const TRT_EndcapElement *element);
      
      /** Doomed (??) */
      const TRT_EndcapCode & getCode() const;
      
      // Surface bounds
      virtual const Trk::SurfaceBounds & strawBounds() const;
    
    
    protected:
      /** These transforms are effectively to the local coord
        system of a straw derived from GeoModel -> hence CLHEP */
      HepGeom::Transform3D calculateStrawTransform(int straw) const;
      HepGeom::Transform3D calculateLocalStrawTransform(int straw) const;
    
      /** return the surface of the element */ 
      const Trk::Surface & elementSurface() const;
      
      /** create the cache for the element */
      void createSurfaceCache() const;
      
      /** create the cache for the straw of identifier id */
      void createSurfaceCache(Identifier id) const;
    
    private:
      
      // Illegal Operations:
      TRT_EndcapElement(const TRT_EndcapElement &right);
      const TRT_EndcapElement & operator=(const TRT_EndcapElement &right);
      
      // Private Member Data:
      TRT_EndcapCode              m_code;
      const TRT_EndcapDescriptor *m_descriptor;
      const TRT_EndcapElement    *m_nextInZ;
      const TRT_EndcapElement    *m_previousInZ;
  };
  
}

#include "TRT_EndcapElement.icc"  
#endif


