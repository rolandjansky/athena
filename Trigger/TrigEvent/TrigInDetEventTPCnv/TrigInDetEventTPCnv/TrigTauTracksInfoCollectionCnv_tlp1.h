/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigTauTracksInfoCollection_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCOLLECTION_CNV_TLP1_H
#define  TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_tlp1.h"
 
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_p1.h"   

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

 
class TrigTauTracksInfoCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTauTracksInfoCollectionCnv_p1, TrigTauTracksInfoCollection_tlp1 >
{
 
 public:
 
  TrigTauTracksInfoCollectionCnv_tlp1();
  virtual ~TrigTauTracksInfoCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigTauTracksInfoCollection_tlp1 *storage );
 
 protected: 
  TrigTauTracksInfoCollectionCnv_p1    m_trigTauTracksInfoCollectionCnv;
  TrigTauTracksInfoCnv_p1              m_trigTauTracksInfoCnv;
  P4PtEtaPhiMCnv_p1                    m_p4PtEtaPhiMCnv;

}; //end of class definitions
 



#endif
