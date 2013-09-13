/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureCnv_tlp1
 *
 * @brief "top level" persistent partner for CombinedMuonFeatureCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureCnv_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_TLP1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeature_tlp1.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"


class CombinedMuonFeatureCnv_tlp1 : public AthenaPoolTopLevelTPConverter<CombinedMuonFeatureCnv_p1, CombinedMuonFeature_tlp1>  {

 public:
  CombinedMuonFeatureCnv_tlp1();
  virtual ~CombinedMuonFeatureCnv_tlp1() {}

  virtual void setPStorage(CombinedMuonFeature_tlp1 *storage);

  
 protected:
  MuonFeatureCnv_p1              m_mfCnv;
  TrigInDetTrackCnv_p1           m_IDTkCnv;
  TrigInDetTrackFitParCnv_p1     m_IDTkFitParCnv;

};

#endif
