/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMap_tlp1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALMAP_TLP1_TRK_H
#define LAYERMATERIALMAP_TLP1_TRK_H

#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMap_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialProperties_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/Material_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterial_p1.h"

#include "TrkDetDescrTPCnv/TrkGeometry/ElementTable_p1.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
#include <vector>

namespace Trk
{
   class LayerMaterialMap_tlp1 
   {
     public:
       LayerMaterialMap_tlp1() {}

       TPCnvTokenList_p1                              tokenList;

       std::vector< Trk::LayerMaterialMap_p1 >        layerMaterialMaps;            //!< in case you write multiple collections

       std::vector<Trk::MaterialProperties_p1>        materialPropertiesStore;     //!< the common store to all material properties
       std::vector<Trk::ElementTable_p1>              elementTableStore;            //!< this is where the elements are stored

       std::vector<Trk::BinnedLayerMaterial_p1>       binnedLayerMaterialStore;
       std::vector<Trk::CompressedLayerMaterial_p1>   compressedLayerMaterialStore;
       std::vector<Trk::CompoundLayerMaterial_p1>     compoundLayerMaterialStore;
       std::vector<Trk::HomogeneousLayerMaterial_p1>  homogeneousLayerMaterialStore;
       
   };
}

#endif // LAYERMATERIALCOLLECTION_TLP1_TRK_H