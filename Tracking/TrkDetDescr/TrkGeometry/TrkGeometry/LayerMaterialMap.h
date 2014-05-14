/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
	 
#ifndef TRKGEOMETRY_LAYERMATERIALMAP_H
#define TRKGEOMETRY_LAYERMATERIALMAP_H

#include <vector>
#include <map>
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {
    
  class LayerMaterialMap : public std::map< Trk::LayerIndex, const Trk::LayerMaterialProperties* > {
 
    public:
      // custom destructor
      ~LayerMaterialMap();
   };

  inline LayerMaterialMap::~LayerMaterialMap() {
    LayerMaterialMap::iterator it    = this->begin();
    LayerMaterialMap::iterator itend = this->end();
    for (;it != itend; it++)
      delete ((*it).second);
  }
  
} // end of namespace

CLASS_DEF( Trk::LayerMaterialMap, 142190734, 1 )

// 142190734 Trk::LayerMaterialMap None

#endif