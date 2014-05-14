/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryLayerPovider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ENTRYLAYERPROVIDER_H
#define TRKGEOMETRY_ENTRYLAYERPROVIDER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/Layer.h"

namespace Trk {

     /**
     @class EntryLayerPovider
     
     Base class to be overloaded for describing 
     the entry/exit layers of a volume.
     
     The entry layers are owned by the EntryLayerProvider

     @author Andreas.Salzburger@cern.ch
    */

    class EntryLayerProvider {

      public:
           
        /** Default constructor */
        EntryLayerProvider(){}        

        /**Virtual destructor*/
        virtual ~EntryLayerProvider(){}
        
        /** returns the actual bending correction */
        virtual const Layer* associatedLayer(const Amg::Vector3D&) const = 0;
    
        /** returns the actual bending correction */
        virtual const Layer* entryLayer(const Amg::Vector3D& pos, const Amg::Vector3D& neg) const = 0;

        /** returns the actual bending correction,  */
        virtual const Layer* exitLayer(const Amg::Vector3D& pos, const Amg::Vector3D& neg) const = 0;

        /** provide the layers to the outside world */
        virtual const std::vector<const Layer*>& layers() const = 0;

    };

}

#endif // TRKGEOMETRY_ENTRYLAYERPROVIDER_H


