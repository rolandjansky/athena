/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_SUBTRACTEDVOLUMEBOUNDS_H
#define TRKVOLUMES_SUBTRACTEDVOLUMEBOUNDS_H
    
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
   @class SubtractedVolumeBounds
    
   Bounds for a generic subtracted volume, the decomposeToSurfaces method creates a
   vector of n surfaces (n1+n2-n_subtracted):
      
    BoundarySurfaceFace [index]: [n1] surfaces from 'outer' volume
                                 [n1+n2-n_subtr] remaining surfaces (after subtraction) from 'inner' volume
                
    @author Sarka.Todorova@cern.ch 
    */
    
 class SubtractedVolumeBounds : public VolumeBounds {
  
  public:
    /**Default Constructor*/
    SubtractedVolumeBounds(); 
     
    /**Constructor - the box boundaries */
    SubtractedVolumeBounds(Volume* outerVol, Volume* innerVol);
    
    /**Copy Constructor */
    SubtractedVolumeBounds(const SubtractedVolumeBounds& bobo);
    
    /**Destructor */
    virtual ~SubtractedVolumeBounds();
    
    /**Assignment operator*/
    SubtractedVolumeBounds& operator=(const SubtractedVolumeBounds& bobo);
    
    /**Virtual constructor */
    SubtractedVolumeBounds* clone() const override;
    
    /**This method checks if position in the 3D volume frame is inside the cylinder*/     
    bool inside(const Amg::Vector3D& , double tol=0.) const override;
         
    /** Method to decompose the Bounds into boundarySurfaces */
    const std::vector<const Trk::Surface*>* decomposeToSurfaces(const Amg::Transform3D& transform) const override;
    
    /** Provide accessor for BoundarySurfaces */
    const ObjectAccessor& boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                                  const Amg::Vector3D& dir,
                                                  bool forceInside=false) const override;
                                                
    /**This method returns the outer Volume*/
    Volume* outer() const;
    
    /**This method returns the inner Volume*/
    Volume* inner() const;    
    
    /**This method returns bounds orientation*/
    const std::vector<bool> boundsOrientation() const;
    
    /** Output Method for MsgStream*/
    MsgStream& dump(MsgStream& sl) const override;
    
    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:

    Trk::Volume* createSubtractedVolume(const Amg::Transform3D& transf, Trk::Volume* subtrVol) const;
 
    Volume* m_outer;
    Volume* m_inner;
    
    /** There's only one single object Acessor for the moment
       has to be implemented if Subtracteds are used more widely */
    EightObjectsAccessor m_objectAccessor;

    mutable std::vector<bool> m_boundsOrientation;        
    
 };

 inline SubtractedVolumeBounds* SubtractedVolumeBounds::clone() const
 { return new SubtractedVolumeBounds(*this); }

 inline bool SubtractedVolumeBounds::inside(const Amg::Vector3D &pos, double tol) const
 { 
   return (m_outer->inside(pos,tol) && !m_inner->inside(pos,-tol) );
 }

 inline Volume* SubtractedVolumeBounds::outer() const { return m_outer; }
 
 inline Volume* SubtractedVolumeBounds::inner() const { return m_inner; }

 inline const ObjectAccessor& SubtractedVolumeBounds::boundarySurfaceAccessor(const Amg::Vector3D&,
                                                                          const Amg::Vector3D&,
                                                                          bool) const
  { return(m_objectAccessor); }
                        
 inline const std::vector<bool> SubtractedVolumeBounds::boundsOrientation() const
  { return(m_boundsOrientation); }

}

#endif // TRKVOLUMES_SUBTRACTEDVOLUMEBOUNDS_H



