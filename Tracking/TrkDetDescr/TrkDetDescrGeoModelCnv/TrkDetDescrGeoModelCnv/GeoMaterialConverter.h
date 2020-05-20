/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoMaterialConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
#define TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
// Trk
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoObject.h"
#include "TrkDetDescrGeoModelCnv/MaterialElement.h"
#include "TrkGeometry/Material.h"
//Amg
#include "GeoPrimitives/GeoPrimitives.h"
// GeoModelKernel
#include "GeoModelKernel/GeoMaterial.h"

class GeoVPhysVol;
class GeoPVConstLink;

namespace Trk {

  class Volume;

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
    
        /** Single conversion , input type GeoMaterial - output type Trk::Material */
        Material convert(const GeoMaterial* gm) const;

	void printInfo(const GeoPVConstLink pv) const;

	std::vector<GeoObject*> decodeGMtree(const GeoPVConstLink pv) const;

	void decodeMaterialContent(const GeoPVConstLink pv, GeoObject* mother, int index, Amg::Transform3D& transf, double& vol,
				   std::vector<GeoObject*>&, std::vector<GeoObject*>&) const;

	void convertGeoObject(GeoObject* geo, std::vector<MaterialElement>& material, bool fixedVolume) const;

      private:
        static double s_densityCnvFactor; //!< the conversion factor from GeoUnits to Tracking
	MaterialElement envelope2element(Trk::Volume* envelope, const GeoMaterial* material, std::string name, double geoVolSize = 0.) const;
  };
 
} // end of namespace Trk

#endif

