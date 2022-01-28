/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"


void CompressedLayerMaterialCnv_p1::persToTrans( const Trk::CompressedLayerMaterial_p1 *persObj,
                                             Trk::CompressedLayerMaterial    *transObj,
                                             MsgStream &mlog )
{
  // create the transient BinUtility
  auto binUtility = std::make_unique<Trk::BinUtility>();
  m_gBinUtilityCnv.persToTrans(&persObj->binUtility, binUtility.get(), mlog);

  // create the persistent material properties
  // pointer to converter (will be auto-retrieved)
  ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
  Trk::MaterialPropertiesVector mvec;
  mvec.reserve (persObj->materialVector.size());
  for (const TPObjRef& ref : persObj->materialVector)
    mvec.push_back (createTransFromPStore( &materialCnv, ref, mlog ));

  Trk::CompressedLayerMaterial mat (std::move(binUtility),
                                    std::move(mvec),
                                    persObj->materialBins,
                                    persObj->splitFactor);
  *transObj = mat;
}

void CompressedLayerMaterialCnv_p1::transToPers( const Trk::CompressedLayerMaterial    *transObj,
                                             Trk::CompressedLayerMaterial_p1 *persObj,
                                             MsgStream  &mlog )
{
    
    // create the persistent BinUtility
    m_gBinUtilityCnv.transToPers(transObj->binUtility(), &persObj->binUtility, mlog);
    
    // assign the material bins
    persObj->materialBins = transObj->materialBins();
    
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    const Trk::MaterialPropertiesVector& fullMaterial = transObj->fullMaterial();
    size_t sz = fullMaterial.size();
    persObj->materialVector = std::vector< TPObjRef >(sz, TPObjRef());
    for (size_t im = 0; im < sz; im++) {
        persObj->materialVector[im] = toPersistent( &materialCnv, fullMaterial[im], mlog );
    }
    persObj->splitFactor = transObj->alongPostFactor();
    

}
