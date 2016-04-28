/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinnedLayerMaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"
#include "CxxUtils/make_unique.h"

void BinnedLayerMaterialCnv_p1::persToTrans( const Trk::BinnedLayerMaterial_p1 *persObj,
                                             Trk::BinnedLayerMaterial    *transObj,
                                             MsgStream &mlog )
{

    // create the transient BinUtility
    auto binUtility = CxxUtils::make_unique<Trk::BinUtility>();
    m_gBinUtilityCnv.persToTrans(&persObj->binUtility, binUtility.get(), mlog);
    transObj->updateBinning (binUtility.release());
    
    // assign the split factor
    transObj->m_splitFactor =  persObj->splitFactor;
    
    // create the transient material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    size_t matrixOuterDim = transObj->m_binUtility->max(1)+1;
    size_t matrixInnerDim = transObj->m_binUtility->max(0)+1;
    transObj->m_fullMaterial = std::vector< std::vector<const Trk::MaterialProperties*> >(matrixOuterDim, std::vector< const Trk::MaterialProperties* >(matrixInnerDim, 0) );
    for ( size_t iod = 0; iod < matrixOuterDim; ++iod )
        for ( size_t iid = 0; iid < matrixInnerDim; ++iid) {
            // create the material properties
            const Trk::MaterialProperties* mprop = createTransFromPStore( &materialCnv, persObj->materialMatrix[iid+matrixInnerDim*iod], mlog );
            // check if the entry carries material or not
            if (mprop && mprop->thicknessInX0() > 10e-5 && mprop->averageZ() > 1 )
                (transObj->m_fullMaterial)[iod][iid] = mprop; 
            else {
                (transObj->m_fullMaterial)[iod][iid] = 0; 
                delete mprop; // clear this instance and set to 0
                
            }
        }
}

void BinnedLayerMaterialCnv_p1::transToPers( const Trk::BinnedLayerMaterial    *transObj,
                                             Trk::BinnedLayerMaterial_p1 *persObj,
                                             MsgStream  &mlog )
{
    
    // create the persistent BinUtility
    m_gBinUtilityCnv.transToPers(transObj->binUtility(), &persObj->binUtility, mlog);

    // assign the split factor
    persObj->splitFactor = transObj->m_splitFactor;
    
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    // prepare the matrix
    size_t matrixOuterDim =transObj->m_binUtility->max(1)+1;
    size_t matrixInnerDim =transObj->m_binUtility->max(0)+1;
    persObj->materialMatrix = std::vector<TPObjRef> ( matrixInnerDim*matrixOuterDim, TPObjRef() ) ;
    for ( size_t iod = 0; iod < matrixOuterDim; ++iod )
        for ( size_t iid = 0; iid < matrixInnerDim; ++iid)
            (persObj->materialMatrix)[iod*matrixInnerDim+iid] = toPersistent( &materialCnv, transObj->m_fullMaterial[iod][iid], mlog );
}

