/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigCaloClusterCnv_p2
 *
 * @brief transient persistent converter for TrigCaloCluster
 *
 * @author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * File and Version Information:
 * $Id: TrigCaloClusterCnv_p2.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P2_H

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigCaloClusterCnv_p2 : public T_AthenaPoolTPCnvConstBase<TrigCaloCluster, TrigCaloCluster_p2>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;

  
  TrigCaloClusterCnv_p2() {}
  
  virtual void persToTrans( const TrigCaloCluster_p2 *persObj,
			    TrigCaloCluster    *transObj,
			    MsgStream            &log ) const override;
  virtual void transToPers( const TrigCaloCluster    *transObj,
			    TrigCaloCluster_p2 *persObj,
			    MsgStream            &log ) const override;
  
};


#endif
