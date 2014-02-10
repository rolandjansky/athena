/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinnedLayerMaterialCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BINNEDLAYERMATERIALCNV_P1_TRK_H
#define BINNEDLAYERMATERIALCNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"

class MsgStream;

class BinnedLayerMaterialCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::LayerMaterialProperties, Trk::BinnedLayerMaterial, Trk::BinnedLayerMaterial_p1 >
{
  public:
    BinnedLayerMaterialCnv_p1() {}
 
    void persToTrans( const Trk::BinnedLayerMaterial_p1*, Trk::BinnedLayerMaterial*, MsgStream& );
    void transToPers( const Trk::BinnedLayerMaterial*, Trk::BinnedLayerMaterial_p1*, MsgStream& ); 
  
  protected:
    BinUtilityCnv_p1     m_gBinUtilityCnv;
      
  
};

#endif
