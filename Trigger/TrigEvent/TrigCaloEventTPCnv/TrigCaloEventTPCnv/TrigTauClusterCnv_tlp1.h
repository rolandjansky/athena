/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigTauCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"

class TrigTauClusterCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigTauClusterCnv_p1, TrigTauCluster_tlp1>
{

 public:

  TrigTauClusterCnv_tlp1();
  virtual ~TrigTauClusterCnv_tlp1() {}

  virtual void setPStorage( TrigTauCluster_tlp1 *storage );

 protected:

  //tp converters for ALL types used in TrigTauCluster
  TrigTauClusterCnv_p1            m_trigTauClusterCnv;
  TrigCaloClusterCnv_p1           m_trigCaloClusterCnv;

};

#endif
