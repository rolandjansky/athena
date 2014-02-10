/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollectionCnv_tlp1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALCOLLECTION_CNV_TLP1_TRK_H
#define LAYERMATERIALCOLLECTION_CNV_TLP1_TRK_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrkDetDescrTPCnv/LayerMaterialCollection_tlp1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialCollectionCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"

class LayerMaterialCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< LayerMaterialCollectionCnv_p1, Trk::LayerMaterialCollection_tlp1 > {
  
  public:

    LayerMaterialCollectionCnv_tlp1();
    virtual ~LayerMaterialCollectionCnv_tlp1();

    virtual void  setPStorage( Trk::LayerMaterialCollection_tlp1 *storage );

    // return list of tokens to extending persistent objects
    // it is stored in m_tokenList member of the Track persistent rep
    virtual TPCnvTokenList_p1*    getTokenListVar() { return &(getTLPersObject()->tokenList); }

  protected:
    MaterialPropertiesCnv_p1          m_materialPropertiesCnv;

    BinnedLayerMaterialCnv_p1         m_blMaterialCnv;
    CompressedLayerMaterialCnv_p1     m_clMaterialCnv;
    HomogeneousLayerMaterialCnv_p1    m_hlMaterialCnv;

};

#endif // LAYERMATERIALCOLLECTION_CNV_TLP1_TRK_H