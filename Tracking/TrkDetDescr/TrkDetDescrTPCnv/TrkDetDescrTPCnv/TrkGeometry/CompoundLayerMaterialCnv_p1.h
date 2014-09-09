/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// CompoundLayerMaterialCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef COMPOUNDLAYERMATERIALCNV_P1_TRK_H
#define COMPOUNDLAYERMATERIALCNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/CompoundLayerMaterial.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"


class MsgStream;

class CompoundLayerMaterialCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::LayerMaterialProperties, Trk::CompoundLayerMaterial, Trk::CompoundLayerMaterial_p1 >
{
  public:
    CompoundLayerMaterialCnv_p1() {}
 
    void persToTrans( const Trk::CompoundLayerMaterial_p1*, Trk::CompoundLayerMaterial*, MsgStream& );
    void transToPers( const Trk::CompoundLayerMaterial*, Trk::CompoundLayerMaterial_p1*, MsgStream& ); 
  
  protected:
    BinUtilityCnv_p1     m_gBinUtilityCnv;
    
      
  
};

#endif
