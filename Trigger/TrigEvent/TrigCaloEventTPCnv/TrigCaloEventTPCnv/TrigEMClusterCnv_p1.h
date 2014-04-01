/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterCnv_p1
 *
 * @brief transient persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"

#include "TrigCaloEventTPCnv/TrigEMClusterConverterBase.h"

class MsgStream;

template <>
class TrigEMClusterConverterBase<TrigEMCluster_p1> : public T_AthenaPoolTPCnvBase<TrigEMCluster, TrigEMCluster_p1>  {
 public:
    TrigEMClusterConverterBase() : m_trigCaloClusterCnv(0) {}

    void persToTrans(const TrigEMCluster_p1* persObj,  TrigEMCluster* transObj,   MsgStream& log);
    void transToPers(const TrigEMCluster* transObj, TrigEMCluster_p1* persObj, MsgStream& log);

 protected:

    ITPConverterFor<TrigCaloCluster>*   m_trigCaloClusterCnv;
};

typedef TrigEMClusterConverterBase<TrigEMCluster_p1> TrigEMClusterCnv_p1;

#endif
