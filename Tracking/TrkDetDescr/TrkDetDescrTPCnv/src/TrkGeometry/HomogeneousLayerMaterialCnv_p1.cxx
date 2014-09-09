/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HomogeneousLayerMaterialCnv_p1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#define private public
#define protected public
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#undef private
#undef protected
#include "TrkGeometry/MaterialProperties.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterialCnv_p1.h"


void HomogeneousLayerMaterialCnv_p1::persToTrans( const Trk::HomogeneousLayerMaterial_p1 *persObj,
                                             Trk::HomogeneousLayerMaterial    *transObj,
                                             MsgStream &mlog )
{


    
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    transObj->m_fullMaterial = createTransFromPStore( &materialCnv, persObj->material, mlog );
    
}

void HomogeneousLayerMaterialCnv_p1::transToPers( const Trk::HomogeneousLayerMaterial    *transObj,
                                             Trk::HomogeneousLayerMaterial_p1 *persObj,
                                             MsgStream  &mlog )
{
    // assign the split factor
     persObj->splitFactor = transObj->m_splitFactor;
    
    // create the persistent material properties
    // pointer to converter (will be auto-retrieved)
    ITPConverterFor<Trk::MaterialProperties> *materialCnv = 0;
    persObj->material =  toPersistent( &materialCnv, transObj->m_fullMaterial, mlog );
}
