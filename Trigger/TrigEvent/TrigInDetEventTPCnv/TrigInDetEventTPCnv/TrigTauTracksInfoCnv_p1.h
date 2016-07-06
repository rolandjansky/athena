/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCnv_p1
 *
 * @brief transient-persistent converter for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Olga Igonkina                               - Nikhef
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "TrigInDetEvent/TrigTauTracksInfo.h"

class MsgStream;


class TrigTauTracksInfoCnv_p1 : public T_AthenaPoolTPCnvBase<TrigTauTracksInfo, TrigTauTracksInfo_p1> 
{

 public:

  TrigTauTracksInfoCnv_p1() : m_3fastestP4PtEtaPhiMCnv(0),
			      m_P4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigTauTracksInfo_p1* persObj, TrigTauTracksInfo* transObj, MsgStream &log);
  virtual void transToPers(const TrigTauTracksInfo* transObj, TrigTauTracksInfo_p1* persObj, MsgStream &log);
  
 protected:
  P4PtEtaPhiMCnv_p1      *m_3fastestP4PtEtaPhiMCnv ;
  P4PtEtaPhiMCnv_p1      *m_P4PtEtaPhiMCnv ;
};


#endif
