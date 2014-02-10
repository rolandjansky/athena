/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#define private public
#define protected public
#include "TrkGeometry/CompressedLayerMaterial.h"
#undef private
#undef protected
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"


void CompressedLayerMaterialCnv_p1::persToTrans( const Trk::CompressedLayerMaterial_p1 *persObj,
                                             Trk::CompressedLayerMaterial    *transObj,
                                             MsgStream &mlog )
{

    // create the transient BinUtility
    transObj->m_binUtility  = new Trk::BinUtility;
    m_gBinUtilityCnv.persToTrans(&persObj->binUtility, transObj->m_binUtility, mlog);
    
    // assign the split factor
    transObj->m_splitFactor =  persObj->splitFactor;
    
    // assign the material bins
    transObj->m_materialBins = persObj->materialBins;
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    transObj->m_fullMaterial = std::vector< const Trk::MaterialProperties* >(persObj->materialVector.size(),0);
    std::vector< TPObjRef >::const_iterator mIter  = persObj->materialVector.begin();
    std::vector< TPObjRef >::const_iterator mIterE = persObj->materialVector.end();
    for (size_t im = 0 ; mIter != mIterE; ++mIter, ++im){
        transObj->m_fullMaterial[im] = createTransFromPStore( &materialCnv, persObj->materialVector[im], mlog );
    }
    
    
    
}

void CompressedLayerMaterialCnv_p1::transToPers( const Trk::CompressedLayerMaterial    *transObj,
                                             Trk::CompressedLayerMaterial_p1 *persObj,
                                             MsgStream  &mlog )
{
    
    // create the persistent BinUtility
    m_gBinUtilityCnv.transToPers(transObj->m_binUtility, &persObj->binUtility, mlog);
    
    // assign the material bins
    persObj->materialBins = transObj->m_materialBins;
    
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    persObj->materialVector = std::vector< TPObjRef >(transObj->m_fullMaterial.size(), TPObjRef());
    std::vector< const Trk::MaterialProperties*>::const_iterator mIter  = transObj->m_fullMaterial.begin();
    std::vector< const Trk::MaterialProperties*>::const_iterator mIterE = transObj->m_fullMaterial.end();
    for (size_t im = 0 ; mIter != mIterE; ++mIter, ++im){
        persObj->materialVector[im] = toPersistent( &materialCnv, transObj->m_fullMaterial[im], mlog );
    }
    

}
