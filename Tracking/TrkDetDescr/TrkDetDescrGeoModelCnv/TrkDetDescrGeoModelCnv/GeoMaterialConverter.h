/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoMaterialConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
#define TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
// Trk
#include "TrkGeometry/Material.h"
// STL
#include <iostream>

class GeoMaterial;

namespace Trk {

  /**
    @class GeoMaterialConverter
    
    A Simple Helper Class that convertes the GeoMaterial object
    used in GeoModel full detector description to an appropriate
    Trk::ExtendedMaterialProperties object.
        
    @author Andreas.Salzburger@cern.ch
    */
    
    class GeoMaterialConverter {
      
      public:
        /** Default constructor*/
        GeoMaterialConverter()
       {}

        /** Destructor*/
        virtual ~GeoMaterialConverter(){}
    
        /** Single conversion ,
        input type GeoMaterial - output type Trk::MaterialProperties */
        Material convert(const GeoMaterial* gm) const;

      private:
        static double s_densityCnvFactor; //!< the conversion factor from GeoUnits to Tracking
    };
 



} // end of namespace Trk

#endif

