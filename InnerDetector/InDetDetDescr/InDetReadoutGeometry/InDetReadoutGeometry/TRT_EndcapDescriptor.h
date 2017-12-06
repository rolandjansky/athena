/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_EndcapDescriptor_h
#define TRT_EndcapDescriptor_h 1
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoXF.h"

namespace Trk{
  class CylinderBounds;
  class SurfaceBounds;
}

namespace InDetDD {

  /** class TRT_EndcapDescriptor
      
       Helper class to access GeoModel infromation and calculation for straws 
       */

  class TRT_EndcapDescriptor : public RCBase 
    
    {
      
    public:
      
      /** Constructor */
      TRT_EndcapDescriptor();
      
      /** Sets the transform field for straws and offset.  We do not own the function: */
      void setStrawTransformField(const GeoXF::Function *xf, size_t offsetInto);

      /** The number of straws in a module: */
      unsigned int &nStraws();
      inline const unsigned int &nStraws() const;
      
      /** The straw pitch (angular!!) */
      double &strawPitch();
      inline const double &strawPitch() const;
      
      /** The starting phi (angular!!) */
      double &startPhi();
      inline const double &startPhi() const;
      
      //** The straw length: */
      double &strawLength();
      inline const double &strawLength() const;
      
      /** The inner radius: */
      double &innerRadius();
      inline const double &innerRadius() const;
      
      /** Get inner tube radius of the straw */
      inline double innerTubeRadius() const;
      
      /**  Get the tranformation field, which we do not own: */
      inline const GeoXF::Function *getStrawTransform() const;
      
      /** Get the offset into the transformation field: */
      inline size_t getStrawTransformOffset() const;
      
      /** Get Bounds */
      const Trk::SurfaceBounds & strawBounds() const;

    protected:
      
      
      virtual ~TRT_EndcapDescriptor();
      
      
    private:
      
      // Illegal to copy:
      TRT_EndcapDescriptor(const TRT_EndcapDescriptor &right);
      
      
      // Illegal to assign:
      const TRT_EndcapDescriptor & operator=(const TRT_EndcapDescriptor &right);
      
      
      // Number of straws in the module.
      unsigned int m_nStraws;
      
      // (Angular) straw pitch
      double m_strawPitch;
      
      // Starting phi:
      double m_startPhi;
      
      // Straw length:
      double m_strawLength;
      
      // Inner radius of straw:
      double m_innerRadius;

      // Inner tube radius of straw.
      double m_innerTubeRadius;
      
      // The straw transformation field:
      const GeoXF::Function *m_f;

      // And offset:
      size_t                 m_o;
      
      // Bounds
      mutable Trk::CylinderBounds * m_bounds;


    };
}


#include "TRT_EndcapDescriptor.icc"



#endif
