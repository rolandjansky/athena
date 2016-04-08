/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinedVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H
#define TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H
    
// Trk
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/Volume.h"
#include "TrkDetDescrUtils/EightObjectsAccessor.h"

//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

   class SurfaceBounds;
   class Volume;
   class Surface;
   
  /** 
   @class CombinedVolumeBounds
    
   Bounds for a generic combined volume, the decomposeToSurfaces method creates a
   vector of n surfaces (n1+n2-nshared):
      
   BoundarySurfaceFace [index]: [n1+n2-nshared] combined surfaces 

   designed to allow transcript of GeoShapeUnion and GeoShapeIntersection
   
   @author Sarka.Todorova@cern.ch 
  */
    
 class CombinedVolumeBounds : public VolumeBounds {
  
  public:
    /**Default Constructor*/
    CombinedVolumeBounds();
      
    /**Constructor - the box boundaries */
    CombinedVolumeBounds( Volume* first,Volume* second, bool intersection);
    
    /**Copy Constructor */
    CombinedVolumeBounds(const CombinedVolumeBounds& bobo);
    
    /**Destructor */
    virtual ~CombinedVolumeBounds();
    
    /**Assignment operator*/
    CombinedVolumeBounds& operator=(const CombinedVolumeBounds& bobo);
    
    /**Virtual constructor */
    virtual CombinedVolumeBounds* clone() const override;
    
    /**This method checks if position in the 3D volume frame is inside the volume*/     
    virtual bool inside(const Amg::Vector3D&, double tol=0.) const override;
         
    /** Method to decompose the Bounds into boundarySurfaces */
    virtual const std::vector<const Trk::Surface*>* decomposeToSurfaces(const Amg::Transform3D& transform) const override;
    
    /** Provide accessor for BoundarySurfaces */
    virtual
    const ObjectAccessor& boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                                  const Amg::Vector3D& dir,
                                                  bool forceInside=false) const override;
                                                
    /**This method returns the first VolumeBounds*/
    Volume* first() const;
    
    /**This method returns the second VolumeBounds*/
    Volume* second() const;    
    
    /**This method distinguishes between Union(0) and Intersection(1)*/
    bool intersection() const;
    
    /**This method returns bounds orientation*/
    const std::vector<bool> boundsOrientation() const;
    
    /** Output Method for MsgStream*/
    virtual MsgStream& dump(MsgStream& sl) const override;
    
    /** Output Method for std::ostream */
    virtual std::ostream& dump(std::ostream& sl) const override;

  private:

    Trk::Volume* createSubtractedVolume(const Amg::Transform3D& transf, Trk::Volume* subtrVol) const;
 
    Volume* m_first;
    Volume* m_second;
    bool m_intersection;
    EightObjectsAccessor m_objectAccessor;   
    mutable std::vector<bool> m_boundsOrientation;        
  
 };

 inline CombinedVolumeBounds* CombinedVolumeBounds::clone() const
 { return new CombinedVolumeBounds(*this); }

 inline bool CombinedVolumeBounds::inside(const Amg::Vector3D &pos, double tol) const
 { 
   if (m_intersection) return (m_first->inside(pos,tol) && m_second->inside(pos,tol) );
   return (m_first->inside(pos,tol) || m_second->inside(pos,tol) );
 }

 inline Volume* CombinedVolumeBounds::first() const { return m_first; }

 inline Volume* CombinedVolumeBounds::second() const { return m_second; }

 inline bool CombinedVolumeBounds::intersection() const { return m_intersection; }

 inline const ObjectAccessor& CombinedVolumeBounds::boundarySurfaceAccessor(const Amg::Vector3D&,
                                                                          const Amg::Vector3D&,
                                                                          bool) const
  { return(m_objectAccessor); }

 inline const std::vector<bool> CombinedVolumeBounds::boundsOrientation() const
  { return(m_boundsOrientation); }
                        
}

#endif // TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H



