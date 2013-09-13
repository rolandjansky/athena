/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureCnv_tlp1
 *
 * @brief "top level" persistent partner for TrigMuonClusterFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington 
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureCnv_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECNV_TLP1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"


class TrigMuonClusterFeatureCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigMuonClusterFeatureCnv_p1, TrigMuonClusterFeature_tlp1>  {

 public:
  TrigMuonClusterFeatureCnv_tlp1();
  virtual ~TrigMuonClusterFeatureCnv_tlp1() {}

  virtual void setPStorage(TrigMuonClusterFeature_tlp1 *storage);

  
 protected:

};

#endif
