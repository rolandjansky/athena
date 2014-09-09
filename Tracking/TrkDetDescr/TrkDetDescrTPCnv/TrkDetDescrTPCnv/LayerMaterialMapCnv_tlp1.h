/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMapCnv_tlp1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LAYERMATERIALMAP_CNV_TLP1_TRK_H
#define LAYERMATERIALMAP_CNV_TLP1_TRK_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrkDetDescrTPCnv/LayerMaterialMap_tlp1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/LayerMaterialMapCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/CompressedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialPropertiesCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"

class LayerMaterialMapCnv_tlp1 : public AthenaPoolTopLevelTPConverter< LayerMaterialMapCnv_p1, Trk::LayerMaterialMap_tlp1 > {
  
  public:

    LayerMaterialMapCnv_tlp1();
    virtual ~LayerMaterialMapCnv_tlp1();

    virtual void  setPStorage( Trk::LayerMaterialMap_tlp1 *storage );

    // return list of tokens to extending persistent objects
    // it is stored in m_tokenList member of the Track persistent rep
    virtual TPCnvTokenList_p1*    getTokenListVar() { return &(getTLPersObject()->tokenList); }

  protected:
    MaterialPropertiesCnv_p1          m_materialPropertiesCnv;
    ElementTableCnv_p1                m_elementTableCnv;
    
    BinnedLayerMaterialCnv_p1         m_blMaterialCnv;
    CompressedLayerMaterialCnv_p1     m_klMaterialCnv;
    CompoundLayerMaterialCnv_p1       m_clMaterialCnv;
    HomogeneousLayerMaterialCnv_p1    m_hlMaterialCnv;

};

#endif // LAYERMATERIALCOLLECTION_CNV_TLP1_TRK_H
