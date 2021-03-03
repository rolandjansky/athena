/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetachedTrackingVolume.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_DETACHEDTRACKINGVOLUME_H
#define TRKGEOMETRY_DETACHEDTRACKINGVOLUME_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include "CxxUtils/checker_macros.h"
namespace Trk {
    
  class TrackingVolume;
  class Surface;
  class MaterialProperties;
  class MagneticFieldProperties;
  
  /**
   @class DetachedTrackingVolume
  
   Base Class for a navigation object (active/passive) in the Tracking realm.

   @author Sarka.Todorova@cern.ch
   
   */

  class DetachedTrackingVolume {
   
      /**Declare the IDetachedTrackingVolumeBuilder as a friend, to be able to change the volumelink */
      friend class TrackingVolume;
      friend class DetachedTrackingVolumeBuilder;
      friend class IDetachedTrackingVolumeBuilder;
      
      public:                
        /**Default Constructor*/
        DetachedTrackingVolume();
        
        /**Constructor with name */
        DetachedTrackingVolume(std::string name,
                         const TrackingVolume* vol );

        /**Constructor with name & layer representation*/
        DetachedTrackingVolume(std::string name,
                         const TrackingVolume* vol,
                         const Layer* layer,
                         const std::vector<const Layer*>* multilayer=nullptr );
     
        /**Destructor*/
        ~DetachedTrackingVolume();
        
        /** returns the TrackingVolume */
        const TrackingVolume* trackingVolume() const;

        /** returns the Name */
        std::string  name() const;
                     
        /** moving object around */
        void move ATLAS_NOT_THREAD_SAFE( Amg::Transform3D& shift) const;

        /** clone with transform*/
        const DetachedTrackingVolume* clone ATLAS_NOT_THREAD_SAFE( const std::string& name, Amg::Transform3D& shift) const;

        /** returns layer representation */
        const Layer* layerRepresentation() const;

        /** returns (multi)layer representation */
        const std::vector<const Layer*>* multilayerRepresentation() const;

        /** sign the volume - the geometry builder has to do that */
        void sign ATLAS_NOT_THREAD_SAFE(GeometrySignature signat, GeometryType geotype) const;
        
        /** return the Signature */
        GeometrySignature geometrySignature() const;

        /** return the Type */
        GeometryType geometryType() const;

        /** set the simplified calculable components */
        void saveConstituents(const std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> >* );
        void saveConstituents ATLAS_NOT_THREAD_SAFE (const std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> >* ) const;
        /** get the simplified calculable components */
        const std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> >* constituents() const;

	    /** alignment methods: set base transform / default argument to current transform */
	
	    void setBaseTransform( Amg::Transform3D* transf=nullptr );
	    void setBaseTransform ATLAS_NOT_THREAD_SAFE ( Amg::Transform3D* transf=nullptr ) const;
	    /** alignment methods: realign  / default argument to base transform */
	    void realign ATLAS_NOT_THREAD_SAFE ( Amg::Transform3D* transf=nullptr ) const;

    private:
        /** Compactify -- set TG as owner to surfaces */
        void compactify ATLAS_NOT_THREAD_SAFE (size_t& cSurfaces, size_t& tSurfaces) const;
         
        const TrackingVolume*                                                        m_trkVolume;
        const std::string                                                            m_name;
        const Layer*                                                                 m_layerRepresentation;
        const std::vector<const Layer*>*                                             m_multilayerRepresentation;
	      Amg::Transform3D*                                                            m_baseTransform;         // optional use (for alignment purpose) 
        const std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> >*     m_constituents;
  };

inline const TrackingVolume* DetachedTrackingVolume::trackingVolume() const { return (m_trkVolume); } 

inline std::string DetachedTrackingVolume::name() const { return (m_name); }

inline const Layer* DetachedTrackingVolume::layerRepresentation() const { return (m_layerRepresentation); }

inline const std::vector<const Layer*>* DetachedTrackingVolume::multilayerRepresentation() const { return (m_multilayerRepresentation); }

inline void DetachedTrackingVolume::saveConstituents(const std::vector<std::pair<std::unique_ptr<const Trk::Volume>, float>> *constituents) {
  m_constituents = constituents;
}
inline void DetachedTrackingVolume::saveConstituents ATLAS_NOT_THREAD_SAFE(
  const std::vector<std::pair<std::unique_ptr<const Trk::Volume>, float>> *constituents) const {
 
  const_cast<DetachedTrackingVolume *>(this)->saveConstituents(constituents);
}

inline const std::vector<std::pair<std::unique_ptr<const Trk::Volume>, float>> *DetachedTrackingVolume::constituents() const {
  return m_constituents;
}

inline void DetachedTrackingVolume::setBaseTransform ATLAS_NOT_THREAD_SAFE(Amg::Transform3D *transf) const {
  const_cast<DetachedTrackingVolume *>(this)->setBaseTransform(transf);
}

} // end of namespace

#endif // TRKGEOMETRY_DETACHEDTRACKINGVOLUME_H



