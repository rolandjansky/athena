/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCnv_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigTauTracksInfoCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigTauTracksInfoCnv_p1, TrigTauTracksInfo_tlp1>
{

 public:

  TrigTauTracksInfoCnv_tlp1();
  virtual ~TrigTauTracksInfoCnv_tlp1() {}

  virtual void setPStorage( TrigTauTracksInfo_tlp1 *storage );

 protected:

  //tp converters for ALL types used in TrigTauTracksInfo
  TrigTauTracksInfoCnv_p1      m_trigTauTracksInfoCnv;
  P4PtEtaPhiMCnv_p1            m_p4PtEtaPhiMCnv;

};


#endif
