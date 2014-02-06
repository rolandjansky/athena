/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
        CylinderVolumeBounds* convert(const GeoTubs* gtub) const;

        /** Convert a tube */
        CylinderVolumeBounds* convert(const GeoTube* gtub) const;

        /** Convert a Polygon into a CylinderVolume -> smooth it*/
        CylinderVolumeBounds* convert(const GeoPcon* gtub,  std::vector<double>& zbounds) const;

        /** Convert a Box */
        CuboidVolumeBounds* convert(const GeoBox* gbox) const;

        /** Convert an arbitrary GeoShape into Trk::Volume */
        Volume* translateGeoShape( const GeoShape*, Amg::Transform3D* ) const;

        /** Decode and dump arbitrary GeoShape for visual inspection */
        void decodeShape( const GeoShape* ) const;

      private:
    };
 

} // end of namespace Trk

#endif

