/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCnv_p4
 *
 * @brief transient-persistent converter for TrigInDetTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCnv_p4.h,v 1.4 demelian Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCNV_P4_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrigInDetEventTPCnv/TrigInDetTrack_p4.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

//this include is needed by CombinedMuonFeatureCnv 
//an incorrect dependency which should be fixed
// #include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h" // not needed at all ?!

class MsgStream;

class TrigInDetTrackCnv_p4: public T_AthenaPoolTPCnvBase<TrigInDetTrack, TrigInDetTrack_p4>
{
public:

 TrigInDetTrackCnv_p4() : m_fpCnv(0), m_isInitialized(0) {}

  virtual void persToTrans( const TrigInDetTrack_p4 *, TrigInDetTrack *, MsgStream& );
  virtual void transToPers( const TrigInDetTrack *, TrigInDetTrack_p4 *, MsgStream& );

  void setPixelID (const PixelID* pixId);
  
protected:
  
  ITPConverterFor<TrigInDetTrackFitPar>        	*m_fpCnv;
  //  TrigInDetTrackFitParCnv_p1   *m_fpCnv;
  const PixelID *m_pixId;
  StoreGateSvc *m_storeGate;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);

};

#endif 
