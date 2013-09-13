/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureCnv_tlp1
 *
 * @brief "top level" persistent partner for IsoMuonFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureCnv_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_TLP1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/IsoMuonFeature_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p1.h"


class IsoMuonFeatureCnv_tlp1 : public AthenaPoolTopLevelTPConverter<IsoMuonFeatureCnv_p1, IsoMuonFeature_tlp1>  {

 public:
  IsoMuonFeatureCnv_tlp1();
  virtual ~IsoMuonFeatureCnv_tlp1() {}

  virtual void setPStorage(IsoMuonFeature_tlp1 *storage);

  
 protected:

};

#endif
