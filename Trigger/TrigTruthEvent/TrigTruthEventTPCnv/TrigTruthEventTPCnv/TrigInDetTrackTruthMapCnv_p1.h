/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMapCnv_p1
 *
 * @brief persistent version of TrigInDetTrackTruthMapCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthMapCnv_p1.h,v 1.3 2008-08-05 17:27:45 baines Exp $
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_CNV_P1_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_p1.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"

class MsgStream;

class TrigInDetTrackTruthMapCnv_p1  : public T_AthenaPoolTPCnvBase<TrigInDetTrackTruthMap, TrigInDetTrackTruthMap_p1>  {
  
 public:

  TrigInDetTrackTruthMapCnv_p1() : m_trigInDetTrackTruthCnv(0) {}
  
  virtual void persToTrans(const TrigInDetTrackTruthMap_p1* persObj,  TrigInDetTrackTruthMap* transObj,   MsgStream &log );
  virtual void transToPers(const TrigInDetTrackTruthMap* transObj, TrigInDetTrackTruthMap_p1* persObj, MsgStream &log );

 private:

  ITPConverterFor<TrigInDetTrackTruth>* m_trigInDetTrackTruthCnv;
  //TrigInDetTrackTruthCnv_p1* m_trigInDetTrackTruthCnv_p1;

};


#endif
