/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HomogeneousLayerMaterialCnv_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HOMOGENEOUSLAYERMATERIALCNV_P1_TRK_H
#define HOMOGENEOUSLAYERMATERIALCNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterial_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"


class MsgStream;

class HomogeneousLayerMaterialCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::LayerMaterialProperties, Trk::HomogeneousLayerMaterial, Trk::HomogeneousLayerMaterial_p1 >
{
  public:
    HomogeneousLayerMaterialCnv_p1() {}
 
    void persToTrans( const Trk::HomogeneousLayerMaterial_p1*, Trk::HomogeneousLayerMaterial*, MsgStream& );
    void transToPers( const Trk::HomogeneousLayerMaterial*, Trk::HomogeneousLayerMaterial_p1*, MsgStream& ); 
      
  
};

#endif