/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollection_tlp1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALCOLLECTION_TLP1_TRK_H
#define LAYERMATERIALCOLLECTION_TLP1_TRK_H

#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialCollection_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialProperties_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterial_p1.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
#include <vector>

namespace Trk
{
   class LayerMaterialCollection_tlp1 
   {
     public:
       LayerMaterialCollection_tlp1() {}

       TPCnvTokenList_p1                              tokenList;

       std::vector< Trk::LayerMaterialCollection_p1 > layerMaterialCollections; //!< in case you write multiple collections

       std::vector<Trk::MaterialProperties_p1>        materialStore;            //!< the common store to all material properties

       std::vector<Trk::BinnedLayerMaterial_p1>       binnedLayerMaterial;
       std::vector<Trk::CompressedLayerMaterial_p1>   compressedLayerMaterial;
       std::vector<Trk::HomogeneousLayerMaterial_p1>  homogeneousLayerMaterial;
       
       

   };
}

#endif // LAYERMATERIALCOLLECTION_TLP1_TRK_H