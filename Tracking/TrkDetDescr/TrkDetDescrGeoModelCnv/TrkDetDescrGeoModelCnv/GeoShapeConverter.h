/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoShapeConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_GEOSHAPRECONVERTER_H
#define TRKDETDESCRGEOMODELCNV_GEOSHAPRECONVERTER_H
// Trk
#include "TrkGeometry/MaterialProperties.h"
//Eigen
#include "GeoPrimitives/GeoPrimitives.h"
// STL
#include <iostream>

class GeoTubs;
class GeoTube;
class GeoPcon;
class GeoBox;
class GeoShape;


namespace Trk {

  class CylinderVolumeBounds;
  class CuboidVolumeBounds;
  class Volume;

  /**
    @class GeoShapeConverter
    
    A Simple Helper Class that convertes the GeoShape object
    used in GeoModel full detector description to an appropriate
    Trk::VolumeBounds (factory type) object.
        
    @author Andreas.Salzburger@cern.ch
    generalization by Sarka.Todorova@cern.ch
    */
    
    class GeoShapeConverter {
      
      public:
        /** Default constructor*/
        GeoShapeConverter()
       {}

        /** Destructor*/
        virtual ~GeoShapeConverter(){}
    
        /** Convert a tubs */
        static CylinderVolumeBounds* convert(const GeoTubs* gtub) ;

        /** Convert a tube */
        static CylinderVolumeBounds* convert(const GeoTube* gtub) ;

        /** Convert a Polygon into a CylinderVolume -> smooth it*/
        static CylinderVolumeBounds* convert(const GeoPcon* gtub,  std::vector<double>& zbounds) ;

        /** Convert a Box */
        static CuboidVolumeBounds* convert(const GeoBox* gbox) ;

        /** Convert an arbitrary GeoShape into Trk::Volume */
        Volume* translateGeoShape( const GeoShape*, Amg::Transform3D* ) const;

        /** Decode and dump arbitrary GeoShape for visual inspection */
        void decodeShape( const GeoShape* ) const;

      private:
    };
 

} // end of namespace Trk

#endif

