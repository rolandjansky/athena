/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrackCountsCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigTrackCountsCollection_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGTRACKCOUNTSCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigTrackCountsCollectionCnv_p1.h"   
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p2.h"
 
class TrigTrackCountsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTrackCountsCollectionCnv_p1, TrigTrackCountsCollection_tlp1 >
{
 
 public:
 
  TrigTrackCountsCollectionCnv_tlp1();
  virtual ~TrigTrackCountsCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigTrackCountsCollection_tlp1 *storage );
 
 protected: 
  TrigTrackCountsCollectionCnv_p1    m_trigTrackCountsCollectionCnv;
  TrigTrackCountsCnv_p1              m_trigTrackCountsCnv_p1;
  TrigTrackCountsCnv_p2              m_trigTrackCountsCnv_p2;
 
}; //end of class definitions
 


#endif
