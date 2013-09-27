/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCnv_p5
 *
 * @brief transient-persistent converter for TrigInDetTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCnv_p5.h,v 1.4 demelian Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCNV_p5_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCNV_p5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrigInDetEventTPCnv/TrigInDetTrack_p5.h"

//this include is needed by CombinedMuonFeatureCnv 
//an incorrect dependency which should be fixed
// #include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h" // not needed at all ?!

class MsgStream;

class TrigInDetTrackCnv_p5: public T_AthenaPoolTPCnvBase<TrigInDetTrack, TrigInDetTrack_p5>
{
public:

  TrigInDetTrackCnv_p5() : m_fpCnv(0) {}

  virtual void persToTrans( const TrigInDetTrack_p5 *, TrigInDetTrack *, MsgStream& );
  virtual void transToPers( const TrigInDetTrack *, TrigInDetTrack_p5 *, MsgStream& );
  
protected:
  
  ITPConverterFor<TrigInDetTrackFitPar>        	*m_fpCnv;
  //  TrigInDetTrackFitParCnv_p1   *m_fpCnv;

};

#endif 
