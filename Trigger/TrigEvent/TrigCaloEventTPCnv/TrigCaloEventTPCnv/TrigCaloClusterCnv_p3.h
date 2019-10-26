/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P3_H

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigCaloClusterCnv_p3 : public T_AthenaPoolTPCnvConstBase<TrigCaloCluster, TrigCaloCluster_p3>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigCaloClusterCnv_p3() {}
  
  virtual void persToTrans( const TrigCaloCluster_p3 *persObj,
			    TrigCaloCluster    *transObj,
			    MsgStream            &log ) const override;
  virtual void transToPers( const TrigCaloCluster    *transObj,
			    TrigCaloCluster_p3 *persObj,
			    MsgStream            &log ) const override;
  
};


#endif
