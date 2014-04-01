/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigEMCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"

class TrigEMClusterCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigEMClusterCnv_p1, TrigEMCluster_tlp1>
{

 public:

  TrigEMClusterCnv_tlp1();
  virtual ~TrigEMClusterCnv_tlp1() {}

  virtual void setPStorage( TrigEMCluster_tlp1 *storage );

 protected:

  //tp converters for ALL types used in TrigEMCluster
  TrigEMClusterCnv_p1            m_trigEMClusterCnv;
  TrigCaloClusterCnv_p1          m_trigCaloClusterCnv;

};

#endif
