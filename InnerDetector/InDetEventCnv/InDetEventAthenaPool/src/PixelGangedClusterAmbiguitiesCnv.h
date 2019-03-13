/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file PixelGangedClusterAmbiguitiesCnv.h
 * @brief AthenaPool converter for PixelGangedClusterAmbiguitiesCnv
 * @author Andreas.Wildauer@cern.ch
 */

#ifndef PixelGangedClusterAmbiguitiesCnv_H
#define PixelGangedClusterAmbiguitiesCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguitiesCnv_p1.h"

// the latest persistent representation type of PixelGangedClusterAmbiguities
typedef  InDet::PixelGangedClusterAmbiguities_p1  PixelGangedClusterAmbiguities_PERS;
typedef  T_AthenaPoolCustomCnv<InDet::PixelGangedClusterAmbiguities, PixelGangedClusterAmbiguities_PERS >   PixelGangedClusterAmbiguitiesCnvBase;

class PixelGangedClusterAmbiguitiesCnv : public PixelGangedClusterAmbiguitiesCnvBase {
friend class CnvFactory<PixelGangedClusterAmbiguitiesCnv >;
protected:
public:
  PixelGangedClusterAmbiguitiesCnv (ISvcLocator* svcloc) : PixelGangedClusterAmbiguitiesCnvBase(svcloc, "PixelGangedClusterAmbiguitiesConverter") {}
protected:
  virtual PixelGangedClusterAmbiguities_PERS*   createPersistent (InDet::PixelGangedClusterAmbiguities* transObj);
  virtual InDet::PixelGangedClusterAmbiguities*        createTransient ();
private:
  PixelGangedClusterAmbiguitiesCnv_p1 m_TPconverter;
};

#endif
