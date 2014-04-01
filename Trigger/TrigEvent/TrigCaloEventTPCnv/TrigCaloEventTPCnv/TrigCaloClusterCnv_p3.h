/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P3_H

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigCaloClusterCnv_p3 : public T_AthenaPoolTPCnvBase<TrigCaloCluster, TrigCaloCluster_p3>
{
 public:
  
  TrigCaloClusterCnv_p3() {}
  
  virtual void persToTrans( const TrigCaloCluster_p3 *persObj,
			    TrigCaloCluster    *transObj,
			    MsgStream            &log );
  virtual void transToPers( const TrigCaloCluster    *transObj,
			    TrigCaloCluster_p3 *persObj,
			    MsgStream            &log );
  
};


#endif
