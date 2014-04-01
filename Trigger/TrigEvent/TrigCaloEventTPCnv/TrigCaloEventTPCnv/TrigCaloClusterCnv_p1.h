/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigCaloClusterCnv_p1
 *
 * @brief transient persistent converter for TrigCaloCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. of Geneva, Switzerland 
 *
 * File and Version Information:
 * $Id: TrigCaloClusterCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCNV_P1_H

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigCaloClusterCnv_p1 : public T_AthenaPoolTPCnvBase<TrigCaloCluster, TrigCaloCluster_p1>
{
 public:
  
  TrigCaloClusterCnv_p1() {}
  
  virtual void persToTrans( const TrigCaloCluster_p1 *persObj,
			    TrigCaloCluster    *transObj,
			    MsgStream            &log );
  virtual void transToPers( const TrigCaloCluster    *transObj,
			    TrigCaloCluster_p1 *persObj,
			    MsgStream            &log );
  
};


#endif
