/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_TLP2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_TLP2_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigTauCluster_tlp2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"

class TrigTauClusterCnv_tlp2 : public AthenaPoolTopLevelTPConverter<TrigTauClusterCnv_p2, TrigTauCluster_tlp2>
{

 public:

  TrigTauClusterCnv_tlp2();
  virtual ~TrigTauClusterCnv_tlp2() {}

  virtual void setPStorage( TrigTauCluster_tlp2 *storage );

 protected:

  //tp converters for ALL types used in TrigTauCluster
  TrigTauClusterCnv_p2            m_trigTauClusterCnv;
  TrigCaloClusterCnv_p1           m_trigCaloClusterCnv;

};

#endif
