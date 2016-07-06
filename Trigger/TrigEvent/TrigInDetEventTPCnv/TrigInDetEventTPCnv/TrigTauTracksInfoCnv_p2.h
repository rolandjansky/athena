/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCnv_p2
 *
 * @brief transient-persistent converter for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Olga Igonkina                               - Nikhef
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_P2_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "TrigInDetEvent/TrigTauTracksInfo.h"

class MsgStream;

class TrigTauTracksInfoCnv_p2 : public T_AthenaPoolTPCnvBase<TrigTauTracksInfo, TrigTauTracksInfo_p2> 
{

 public:

//  TrigTauTracksInfoCnv_p2() : m_3fastestP4PtEtaPhiMCnv(0) {}
  TrigTauTracksInfoCnv_p2() {}

  virtual void persToTrans(const TrigTauTracksInfo_p2* persObj, TrigTauTracksInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigTauTracksInfo* transObj, TrigTauTracksInfo_p2* persObj, MsgStream &log);
  
 protected:
  P4PtEtaPhiMCnv_p1      m_3fastestP4PtEtaPhiMCnv ;
  P4PtEtaPhiMCnv_p1      m_P4PtEtaPhiMCnv ;
};


#endif
