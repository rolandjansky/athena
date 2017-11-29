/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_BarrelDescriptor.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_BarrelDescriptor_h
#define TRT_BarrelDescriptor_h 1
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoXF.h"
#include <vector>

namespace Trk{
  class CylinderBounds;
  class SurfaceBounds;
}

namespace InDetDD {

      /** @class TRT_BarrelDescriptor 
       
         Local Straw Positions (from the center of the module.)  
                                                               
         The center of the module is defined like this:          
                                                               
                              .--------.                       
                             / \      /                       
                            /   \    /                         
                           /  o  \  /                          
                          /_______\/                                        
                                                               
       */
  
  class TRT_BarrelDescriptor : public RCBase  
    {
      
    public:
      
      /** Constructor: */
      TRT_BarrelDescriptor();
      
      /** Add a new straw at position x, y: */
      void addStraw(double xPos, double yPos);
      
      /**  Sets the transform field for straws and offset. Ownership is not passed. */
      void setStrawTransformField(const GeoXF::Function *xf, size_t offsetInto);

      /** Get X Position: */
      inline double & strawXPos       (unsigned int i);
      inline const double & strawXPos (unsigned int i) const;
      
      /** Get Y Position: */
      inline double & strawYPos       (unsigned int i);
      inline const double & strawYPos (unsigned int i) const;
      
      /** Get the number of straws: */
      inline unsigned int nStraws() const;
      
      /** Get the length of the straws: */
      inline const double & strawLength() const;
      inline double & strawLength();
      inline void strawLength(double newLength);

      /** Get inner tube radius of the straw */
      inline double innerTubeRadius() const;
    
      /** Dead part */
      inline const double & strawZDead() const;
      inline double & strawZDead();
      inline void strawZDead(double zDead);      

      /** Position of center of active region. */
      inline const double & strawZPos() const;
      inline double & strawZPos();
      inline void strawZPos(double Pos);      

      /** Get the tranformation field, which we do not own:
          Does not include the z shift which is different for pos and
          negative barrel: */
      inline const GeoXF::Function *getStrawTransform() const;
      
      /** Get the offset into the transformation field: */
      inline size_t getStrawTransformOffset() const;

      /** Get Bounds */
      const Trk::SurfaceBounds & strawBounds() const;

    protected:
      
      virtual ~TRT_BarrelDescriptor();
      
    private:
      
      TRT_BarrelDescriptor(const TRT_BarrelDescriptor &right);
      const TRT_BarrelDescriptor & operator=(const TRT_BarrelDescriptor &right);
      
      std::vector<double> m_x;
      std::vector<double> m_y;
      double              m_length;
      double              m_zDead;
      double              m_zPos;
      double              m_innerTubeRadius; 
      const GeoXF::Function *m_f;
      size_t                 m_o;

      mutable Trk::CylinderBounds * m_bounds;

    };
}
#include "TRT_BarrelDescriptor.icc"
#endif


