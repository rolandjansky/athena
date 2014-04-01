/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauClusterDetails
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetails_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"

class TrigTauClusterDetailsCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigTauClusterDetailsCnv_p1, TrigTauClusterDetails_tlp1>
{

 public:

  TrigTauClusterDetailsCnv_tlp1();
  virtual ~TrigTauClusterDetailsCnv_tlp1() {}

  virtual void setPStorage( TrigTauClusterDetails_tlp1 *storage );

 protected:

  //tp converters for ALL types used in TrigTauClusterDetails
  TrigTauClusterDetailsCnv_p1            m_trigTauClusterCnv;

};

#endif
