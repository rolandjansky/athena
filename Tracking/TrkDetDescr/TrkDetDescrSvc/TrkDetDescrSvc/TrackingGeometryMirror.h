#ifndef _Trk_TrackingGeometryMirror_h_
#define _Trk_TrackingGeometryMirror_h_
namespace Trk {
   class TrackingGeometryMirror;
}
#include "TrkGeometry/TrackingGeometry.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TrkGeometry/Layer.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include <iostream>

namespace Trk {

   /// Helper class to provide the same objects as an existing TrackingGeometry without claiming ownership over these objects.
   /// The class is solely meant to allow using the TrackingGeometry conditions data and the TrackingGeometrySvc at the same time.
   class TrackingGeometryMirror : public Trk::TrackingGeometry {
   public:
      TrackingGeometryMirror(TrackingGeometry *src) : TrackingGeometry(nullptr,src->m_navigationLevel) {
         update(src);
      }

      ~TrackingGeometryMirror() {
         cleanup();
      }

      void update(TrackingGeometry *src) {
         m_world = src->m_world;
         m_beam = src->m_beam;
         /** The unique boundary Layers */
         m_boundaryLayers = src->m_boundaryLayers;
         /** The Volumes in a map for later finding */
         m_trackingVolumes = src->m_trackingVolumes;
      }

      void cleanup() {
         m_world=nullptr;
         m_beam=nullptr;
         m_boundaryLayers.clear();
         m_trackingVolumes.clear();
      }

      template <class T_Ostream>
      void dump(T_Ostream &out, const std::string &head) const {
         for(const std::pair<const Layer*,int> &bound_layers : m_boundaryLayers) {
            out << head << " [" << bound_layers.second  << "] ";
            dumpLayer(out, "",bound_layers.first);
         }
         int counter=0;
         for (const std::pair<const std::string, const TrackingVolume*> &volume : m_trackingVolumes) {
            out << head << " [" << counter++  << "] " << volume.first << " volumeBound=" << volume.second->volumeBounds() << std::endl;
            if (volume.second->confinedArbitraryLayers()) {
               int j=0;
               for(const Layer* confined_layer :  *volume.second->confinedArbitraryLayers()) {
                  out << head << " [" << counter++  << "] " << volume.first << " confinedArbitrary layer " << j++ << " ";
                  dumpLayer(out, "",confined_layer);
               }
            }
            if (volume.second->confinedLayers()) {
               int j=0;
               for(const Layer* confined_layer :  volume.second->confinedLayers()->arrayObjects()) {
                  out << head << " [" << counter++  << "] " << volume.first << " confined layer" << j++ << " ";
                  dumpLayer(out,"",confined_layer);
               }
            }
         }
      }

   protected:
      template <class T_Ostream>
      static void dumpLayer(T_Ostream &out, const std::string &head, const Layer *layer) {
         if (layer) {
            out << head
                << layer->layerIndex().value() << " [t=" << layer->layerType() << "] d=" << layer->thickness();
            if (layer->representingVolume()) {
               out << " vol=" << layer->representingVolume()->volumeBounds();
            }
            out << layer->surfaceRepresentation();
         }
         out << std::endl;

      }
   };
}

#endif
