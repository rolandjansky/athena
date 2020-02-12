/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoObject.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_GEOOBJECT_H
#define TRKDETDESCRGEOMODELCNV_GEOOBJECT_H

// GeoModelKernel
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"
//Amg
#include "GeoPrimitives/GeoPrimitives.h"
//Trk
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkVolumes/Volume.h"
// STL
#include <iostream>
// messaging
#include "AthenaKernel/getMessageSvc.h"

namespace Trk {

  class Volume;

  /**
    @class GeoObject
    
    A Simple Helper Class for decoding of the GM tree content, material-wise.
        
    @author sarka.todorova@cern.ch
    */
  
    class GeoObject{
      
    public:
      std::string                 name;
      std::string                 motherName;
      const GeoShape*     geoshape;
      std::vector<int>      geotree;
      std::vector<Amg::Transform3D>  transform;
      float                          volumeSize;
      Trk::Volume*            trVolume;
      int                             embedLevel; 
      const GeoMaterial*  material;
      
     /** Constructor */ 
      GeoObject(std::string nameObj, const GeoShape* shape, std::vector<int> tree, int index,  Amg::Transform3D tr, std::string mname )
      : name(nameObj)
	,motherName(mname)
	,geoshape(shape)
	,geotree(tree)
	,volumeSize(0.)
	,trVolume(nullptr)
	,embedLevel(-2)
	,material(0) 
	{
	  MsgStream msg(Athena::getMessageSvc(), "GeoObject");
	  //
	  geotree.push_back(index);
	  transform.push_back(tr);
	  Trk::GeoShapeConverter geoShapeConverter; 
	  if (nameObj=="BeamPipeFwd" || nameObj=="BeamPipeCentral") {
	    volumeSize=0.;
	    embedLevel=-1.; 
	  } else if (geoshape) {
	    trVolume = geoShapeConverter.translateGeoShape(geoshape,&tr); 
	    volumeSize = trVolume ? geoShapeConverter.calculateVolume(trVolume) : 0.;
	  } else { 
             msg << MSG::WARNING << " GeoShape not defined for GeoObject:"<< name<<":"<<mname<< endmsg;  
	  }
	} ; 
      
      /** Destructor*/
virtual ~GeoObject() { delete trVolume; };
 
      void setEmbedLevel(int elevel) ; 
      void setMaterial(const GeoMaterial* mat); 
      void addClone(Amg::Transform3D transf); 
      
     };

} // end of namespace Trk

#endif

