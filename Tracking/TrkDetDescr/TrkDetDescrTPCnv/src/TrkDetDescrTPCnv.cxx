/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCnv_p1.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"


DECLARE_TPCNV_FACTORY(BinUtilityCnv_p1,
                      Trk::BinUtility,
                      Trk::BinUtility_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(BinnedLayerMaterialCnv_p1,
                      Trk::BinnedLayerMaterial,
                      Trk::BinnedLayerMaterial_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CompoundLayerMaterialCnv_p1,
                      Trk::CompoundLayerMaterial,
                      Trk::CompoundLayerMaterial_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(CompressedLayerMaterialCnv_p1,
                      Trk::CompressedLayerMaterial,
                      Trk::CompressedLayerMaterial_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(ElementTableCnv_p1,
                      Trk::ElementTable,
                      Trk::ElementTable_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(HomogeneousLayerMaterialCnv_p1,
                      Trk::HomogeneousLayerMaterial,
                      Trk::HomogeneousLayerMaterial_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MaterialCnv_p1,
                      Trk::Material,
                      Trk::Material_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MaterialPropertiesCnv_p1,
                      Trk::MaterialProperties,
                      Trk::MaterialProperties_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MaterialStepCnv_p1,
                      Trk::MaterialStep,
                      Trk::MaterialStep_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(LayerMaterialMapCnv_tlp1,
                      Trk::LayerMaterialMap,
                      Trk::LayerMaterialMap_tlp1,
                      Athena::TPCnvVers::Current)
