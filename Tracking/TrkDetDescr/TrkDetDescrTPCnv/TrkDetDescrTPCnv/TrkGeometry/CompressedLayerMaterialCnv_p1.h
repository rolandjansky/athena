/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPRESSEDLAYERMATERIALCNV_P1_TRK_H
#define COMPRESSEDLAYERMATERIALCNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"


class MsgStream;

class CompressedLayerMaterialCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::LayerMaterialProperties, Trk::CompressedLayerMaterial, Trk::CompressedLayerMaterial_p1 >
{
  public:
    CompressedLayerMaterialCnv_p1() {}
 
    void persToTrans( const Trk::CompressedLayerMaterial_p1*, Trk::CompressedLayerMaterial*, MsgStream& );
    void transToPers( const Trk::CompressedLayerMaterial*, Trk::CompressedLayerMaterial_p1*, MsgStream& ); 
  
  protected:
    BinUtilityCnv_p1     m_gBinUtilityCnv;
    
      
  
};

#endif