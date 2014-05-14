/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignableTrackingVolume.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ALIGNABLETRACKINGVOLUME_H
#define TRKGEOMETRY_ALIGNABLETRACKINGVOLUME_H

class MsgStream;

#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/BinnedMaterial.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    
  class Surface;
  class MaterialProperties;
  
  /**
   @class AlignableTrackingVolume
  
   Base Class for a navigation object (active) in the Calo realm.   
   Takes BinnedMaterial as an argument ( can be dummy )

   @author Sarka.Todorova@cern.ch
   
   */

  class AlignableTrackingVolume : public TrackingVolume {
         
      public:                
        /**Default Constructor*/
        AlignableTrackingVolume();

        /**Constructor*/
        AlignableTrackingVolume(Amg::Transform3D* htrans,
				Amg::Transform3D* align,
				VolumeBounds*   volbounds,
				const BinnedMaterial* matprop,
                                int sampleID,
                                const std::string& volumeName="undefined");
             
        /**Destructor*/
        ~AlignableTrackingVolume();
        
        /** returns the alignedTrackingVolume */
        const TrackingVolume* alignedTrackingVolume() const;

        /** returns the id */
        int identify() const;

        /** access to binned material */
        const BinnedMaterial* binnedMaterial() const;

    private:
         
        mutable const TrackingVolume*                                      m_alignedTV;
        int                                                                m_sampleID;
	mutable Amg::Transform3D*                                          m_alignment;  
        const BinnedMaterial*                                              m_binnedMaterial;        
  };


inline int AlignableTrackingVolume::identify() const { return (m_sampleID); }

inline const BinnedMaterial* AlignableTrackingVolume::binnedMaterial() const { return m_binnedMaterial; }
 
} // end of namespace

#endif // TRKGEOMETRY_ALIGNABLETRACKINGVOLUME_H



