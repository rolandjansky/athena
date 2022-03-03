/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_TRACKINGVOLUMEHELPER_H
#define TRKDETDESCRTOOLS_TRACKINGVOLUMEHELPER_H

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current,test) current = current < test ? current : test
#define takeBigger(current,test)  current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSmaller(cSmallest, test); takeBigger(cBiggest, test)
#endif
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
// STL
#include <algorithm>
#include <string>
#include <fstream>

#include "CxxUtils/checker_macros.h"
namespace Trk {

    class ILayerArrayCreator;
    class ITrackingVolumeArrayCreator;
    class TrackingVolume;
    class VolumeBounds;
    class CylinderVolumeBounds;
    class Material;
    class LayerMaterialProperties;

    /** @class TrackingVolumeHelper

      The TrackingVolumeHelper is a simple Tool that helps to construct
      volumes mainly for gap regions in the fully connective geometry.      

      @author Andreas.Salzburger@cern.ch   
     */

    class TrackingVolumeHelper : public AthAlgTool,
                                 public TrackingVolumeManipulator,
                         virtual public ITrackingVolumeHelper {

      public:
        /** Constructor */
        TrackingVolumeHelper(const std::string&,const std::string&,const IInterface*);
        /** Destructor */
        virtual ~TrackingVolumeHelper();

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() { return IID_ITrackingVolumeHelper; }

        /** AlgTool initialize method */
        StatusCode initialize();


        /** Method to glue two Volumes together
            input:
            - first TrackingVolume that keeps boundary surface
            - face of the BoundarySurface to be kept
            - second volume that gets glued to the first volume
            - face of the BoundarySurface to be shared

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        void glueTrackingVolumes
        ATLAS_NOT_THREAD_SAFE(const TrackingVolume& firstVol,
                              BoundarySurfaceFace firstFace,
                              const TrackingVolume& secondVol,
                              BoundarySurfaceFace secondFace,
                              bool buildBoundaryLayer = false) const;

        /** Method to glue two Volumes together
            input:
            - first TrackingVolume that keeps boundary surface
            - face of the BoundarySurface to be kept
            - second volume that gets glued to the first volume
            - face of the BoundarySurface to be shared

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        void glueTrackingVolumes ATLAS_NOT_THREAD_SAFE(
          const TrackingVolume& firstVol,
          BoundarySurfaceFace firstFace,
          const std::vector<const TrackingVolume*>& secondVolumes,
          BoundarySurfaceFace secondFace,
          bool buildBoundaryLayer = false,
          bool boundaryFaceExchange = false) const;

        /** Method to glue two VolumeArrays together (at navigation level) - without output

            - relies on information from glueDescriptor
            input:
            - first TrackingVolume array 
            - face of the BoundarySurface of the first volume
            - second TrackingVolume array
            - face of the BoundarySurface of the second volume
            
            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
        */
        void glueTrackingVolumes ATLAS_NOT_THREAD_SAFE(
          const std::vector<const TrackingVolume*>& firstVolumes,
          BoundarySurfaceFace firstFace,
          const std::vector<const TrackingVolume*>& secondVolumes,
          BoundarySurfaceFace secondFace,
          bool buildBoundaryLayer = false,
          bool boundaryFaceExchange = false) const;

        /** Method to glue two VolumeArrays together (at navigation level)
            - relies on information from glueDescriptor
            input:
            - first TrackingVolume
            - face of the BoundarySurface of the first volume
            - second volume that gets glued to the first volume
            - face of the BoundarySurface of the second volume
            output:
             - returns enveloping TrackingVolume with glueDescription set for consequent gluing

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        Trk::TrackingVolume* glueTrackingVolumeArrays
        ATLAS_NOT_THREAD_SAFE(const TrackingVolume& firstVol,
                              BoundarySurfaceFace firstFace,
                              const TrackingVolume& secondVol,
                              BoundarySurfaceFace secondFace,
                              std::string name) const;

        /** protected method to set inside Volume of a BoundarySurface:
            input:
            - the volume that holds the BoundarySurface
            - the face type of the boundary to be set
            - the volume to be set as inside volume

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        void setInsideTrackingVolume
        ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                              BoundarySurfaceFace face,
                              const TrackingVolume* insidevol) const;

        /** protected method to set inside VolumeArray of a BoundarySurface:
            input:
            - the volume that holds the BoundarySurface
            - the face type of the boundary to be set
            - the volume array to be set as inside volume array

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        void setInsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
          const TrackingVolume& tvol,
          BoundarySurfaceFace face,
          BinnedArray<const TrackingVolume>* insidevolarray) const;

        void setInsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
          const TrackingVolume& tvol,
          BoundarySurfaceFace face,
          SharedObject<BinnedArray<const TrackingVolume>> insidevolarray) const;

        /** protected method to set outside Volume of a BoundarySurface:
            input:
            - the volume that holds the BoundarySurface
            - the face type of the boundary to be set
            - the volume to be set as outside volume

            --- Necessary as friendship cannot be inherited: your father's friend isn't necessary yours ---
         */
        void setOutsideTrackingVolume
        ATLAS_NOT_THREAD_SAFE(const TrackingVolume& tvol,
                              BoundarySurfaceFace face,
                              const TrackingVolume* outsidevol) const;

        /** protected method to set outside VolumeArray of a BoundarySurface:
            input:
            - the volume that holds the BoundarySurface
            - the face type of the boundary to be set
            - the volume array to be set as outside volume array */
        void setOutsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
          const TrackingVolume& tvol,
          BoundarySurfaceFace face,
          BinnedArray<const TrackingVolume>* outsidevolarray) const;

        void setOutsideTrackingVolumeArray ATLAS_NOT_THREAD_SAFE(
          const TrackingVolume& tvol,
          BoundarySurfaceFace face,
          SharedObject<BinnedArray<const TrackingVolume>> outsidevolarray) const;

      private:

        Trk::LayerMaterialProperties* layerMaterialProperties(const Trk::Surface& sf) const;  //!< helper method to construct barrel material


        /** Private method - it takes the full vector of given volumes to create the supervolume,
                           - it compares the volumes with the ones scheduled to build the face (could probably be done at once)
                           - it checks whether there is a sub-structure (i.e. GlueVolumeDescriptor exists)
                             and adds either the volume itself or the associated subvolume
                           - volumes on glueVols level are all on navigation level*/
        static void fillGlueVolumes ATLAS_NOT_THREAD_SAFE(
          const std::vector<const TrackingVolume*>& topLevelVolumes,
          const std::vector<const TrackingVolume*>& envelopeFaceVolumes,
          BoundarySurfaceFace glueFace,
          std::vector<const Trk::TrackingVolume*>& glueVols) ;

        /** Execute the glueing  - the input volumes are all on navigation level */
        void glueTrackingVolumes ATLAS_NOT_THREAD_SAFE(
          const std::vector<const Trk::TrackingVolume*>& glueVols,
          BoundarySurfaceFace glueFace,
          BoundarySurfaceFace secondFace) const;

        ToolHandle<ILayerArrayCreator>          m_layerArrayCreator;            //!< A Tool for coherent LayerArray creation
        ToolHandle<ITrackingVolumeArrayCreator> m_trackingVolumeArrayCreator;   //!< Helper Tool to create TrackingVolume Arrays
        

        int                                     m_barrelLayerBinsZ;               //!< material bins in Z
        int                                     m_barrelLayerBinsPhi;             //!< material bins in Phi
        int                                     m_endcapLayerBinsR;               //!< material bins in R
        int                                     m_endcapLayerBinsPhi;             //!< material bins in Phi

        static constexpr double                s_layerThickness = 1.*Gaudi::Units::mm;  //!< standard layer thickness

    };

}

#endif

