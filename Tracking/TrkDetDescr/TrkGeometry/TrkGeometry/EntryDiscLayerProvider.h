/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryDiscLayerProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ENTRYDISCLAYERPROVIDER_H
#define sTRKGEOMETRY_ENTRYDISCLAYERPROVIDER_H


// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/DiscLayer.h"

namespace Trk {
    
     /**
     @class EntryDiscLayerProvider

     provides 2 disc-like entry/exit layers     

     @author Andreas.Salzburger@cern.ch
    */

    class EntryDiscLayerProvider : public EntryLayerProvider {

      public: 
        /** Default constructor */
        EntryDiscLayerProvider(DiscLayer* layerAtInnerR,
                               DiscLayer* layerAtOuterR,
                               double iTolerance = 0.02);       

        /**Virtual destructor*/
        ~EntryDiscLayerProvider();

        /** returns the associated Layer to the position */
        const DiscLayer* associatedLayer(const Amg::Vector3D&) const;
    
        /** returns the most probably entry Layer */
        const DiscLayer* entryLayer(const Amg::Vector3D&, const Amg::Vector3D&) const;

        /** returns the most probably exit Layer */
        const DiscLayer* exitLayer(const Amg::Vector3D&, const Amg::Vector3D&) const;
  
        /** the layers for the outside world - in base class */
        const std::vector<const Layer*>& layers() const;

    private:
        /** Copy constructor - forbidden */
        EntryDiscLayerProvider(const EntryDiscLayerProvider&) :
          EntryLayerProvider(){}


        double                  m_rMin;                //!< dimensions in r for the cylinder layers
        double                  m_rMax;                //!< dimensions in r for the cylinder layers
        double                  m_zMin;                //!< dimensions in r for the cylinder layers                
        double                  m_zMax;                //!< dimensions in r for the cylinder layers
        
        double                  m_tolerance;           //!< intersection tolerance

        DiscLayer*              m_layerAtZmin;  //!< the inner Z entry layer
        DiscLayer*              m_layerAtZmax;  //!< the outer Z entry layer

        std::vector<const Trk::Layer*> m_cachedBaseClassLayers;

    };

  inline const std::vector<const Trk::Layer*>& EntryDiscLayerProvider::layers() const { return m_cachedBaseClassLayers; }


}

#endif // TRKGEOMETRY_ENTRYLAYERPROVIDER_H


