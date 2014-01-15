/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGangedClusterAmbiguitiesCnv_p1_H
#define PixelGangedClusterAmbiguitiesCnv_p1_H

#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"

class StoreGateSvc;

class PixelGangedClusterAmbiguitiesCnv_p1 : public T_AthenaPoolTPCnvBase<InDet::PixelGangedClusterAmbiguities, InDet::PixelGangedClusterAmbiguities_p1>
{
 public:
  PixelGangedClusterAmbiguitiesCnv_p1() : m_storeGate(0), m_isInitialized(0) {};
  
  virtual void	persToTrans(const InDet::PixelGangedClusterAmbiguities_p1* persCont,
			    InDet::PixelGangedClusterAmbiguities* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDet::PixelGangedClusterAmbiguities* transCont,
			    InDet::PixelGangedClusterAmbiguities_p1* persCont,
			    MsgStream &log) ;

 private:
   StoreGateSvc *m_storeGate;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);
};

#endif


