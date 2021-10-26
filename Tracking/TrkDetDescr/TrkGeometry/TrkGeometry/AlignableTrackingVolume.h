/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
                                VolumeBounds* volbounds,
                                const BinnedMaterial* matprop,
                                int sampleID,
                                const std::string& volumeName = "undefined")
          ATLAS_CTORDTOR_NOT_THREAD_SAFE;
        // unsafe cloneTV

        /**Destructor*/
        virtual ~AlignableTrackingVolume() override;
        
        /** returns the alignedTrackingVolume */
        const TrackingVolume* alignedTrackingVolume() const;

        /** returns the id */
        int identify() const;

        /** access to binned material */
        const BinnedMaterial* binnedMaterial() const;

        virtual bool isAlignable () const override final;
    private:
         
        const TrackingVolume*                                              m_alignedTV;
        int                                                                m_sampleID;
	      Amg::Transform3D*                                                  m_alignment;  
        const BinnedMaterial*                                              m_binnedMaterial;        
  };


inline int AlignableTrackingVolume::identify() const { return (m_sampleID); }

inline const BinnedMaterial* AlignableTrackingVolume::binnedMaterial() const { return m_binnedMaterial; }
 
inline bool AlignableTrackingVolume::isAlignable () const{return true;}
} // end of namespace

#endif // TRKGEOMETRY_ALIGNABLETRACKINGVOLUME_H



