/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
                         const std::vector<const Layer*>* multilayer=0 );
     
        /**Destructor*/
        ~DetachedTrackingVolume();
        
        /** returns the TrackingVolume */
        const TrackingVolume* trackingVolume() const;

        /** returns the Name */
        const std::string  name() const;
                     
        /** moving object around */
        void move( Amg::Transform3D& shift) const;

        /** clone with transform*/
        const DetachedTrackingVolume* clone( std::string name, Amg::Transform3D& shift) const;

        /** returns layer representation */
        const Layer* layerRepresentation() const;

        /** returns (multi)layer representation */
        const std::vector<const Layer*>* multilayerRepresentation() const;

        /** sign the volume - the geometry builder has to do that */
        void sign(GeometrySignature signat, GeometryType geotype) const;
        
        /** return the Signature */
        GeometrySignature geometrySignature() const;

        /** return the Type */
        GeometryType geometryType() const;

        /** set the simplified calculable components */
        void saveConstituents(std::vector<std::pair<const Trk::Volume*,float> >* ) const;

        /** get the simplified calculable components */
        std::vector<std::pair<const Trk::Volume*,float> >* constituents() const;

	    /** alignment methods: set base transform / default argument to current transform */
	    void setBaseTransform( Amg::Transform3D* transf=0 ) const;
	
	    /** alignment methods: realign  / default argument to base transform */
	    void realign( Amg::Transform3D* transf=0 ) const;

    private:
        /** Compactify -- set TG as owner to surfaces */
        void compactify(size_t& cSurfaces, size_t& tSurfaces) const;
         
        const TrackingVolume*                                        m_trkVolume;
        const std::string                                            m_name;
        const Layer*                                                 m_layerRepresentation;
        const std::vector<const Layer*>*                             m_multilayerRepresentation;
	    mutable Amg::Transform3D*                                    m_baseTransform;         // optional use (for alignment purpose) 
        mutable std::vector<std::pair<const Trk::Volume*,float> >*   m_constituents;  
        
                
  };

inline const TrackingVolume* DetachedTrackingVolume::trackingVolume() const { return (m_trkVolume); } 

inline const std::string DetachedTrackingVolume::name() const { return (m_name); }

inline const Layer* DetachedTrackingVolume::layerRepresentation() const { return (m_layerRepresentation); }

inline const std::vector<const Layer*>* DetachedTrackingVolume::multilayerRepresentation() const { return (m_multilayerRepresentation); }
 
inline void DetachedTrackingVolume::saveConstituents(std::vector<std::pair<const Trk::Volume*,float> >* constituents ) const { m_constituents = constituents; } 

inline std::vector<std::pair<const Trk::Volume*,float> >* DetachedTrackingVolume::constituents() const
   { return m_constituents;}

} // end of namespace

#endif // TRKGEOMETRY_DETACHEDTRACKINGVOLUME_H



