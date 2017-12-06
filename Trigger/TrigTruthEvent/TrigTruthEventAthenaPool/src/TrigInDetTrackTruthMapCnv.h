/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TrigInDetTrackTruthMapCnv
 *
 * @brief transient persistent converter for TrigInDetTrackTruthMapCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthMapCnv.h,v 1.2 2008-08-19 04:27:37 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTATHENAPOOL_TRIGINDETTRACKTRUTHMAPCNV_H
#define TRIGTRUTHEVENTATHENAPOOL_TRIGINDETTRACKTRUTHMAPCNV_H

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp2.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp2.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp3.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

//typedef to the latest top level persistent class
typedef TrigInDetTrackTruthMap_tlp2  TrigInDetTrackTruthMap_PERS;

typedef T_AthenaPoolCustomCnv<TrigInDetTrackTruthMap, TrigInDetTrackTruthMap_PERS > TrigInDetTrackTruthMapCnvBase; 

class MsgStream;

class TrigInDetTrackTruthMapCnv : public TrigInDetTrackTruthMapCnvBase
{

  friend class CnvFactory<TrigInDetTrackTruthMapCnv>;

  virtual StatusCode initialize();

 protected:

  TrigInDetTrackTruthMapCnv(ISvcLocator* svcloc);
  ~TrigInDetTrackTruthMapCnv();

  virtual TrigInDetTrackTruthMap_PERS*  createPersistent(TrigInDetTrackTruthMap* transObj);
  virtual TrigInDetTrackTruthMap*       createTransient ();

 private:

  TrigInDetTrackTruthMapCnv_tlp2 m_trigInDetTrackTruthMapCnv_tlp2;
  TrigInDetTrackTruthMapCnv_tlp3 m_trigInDetTrackTruthMapCnv_tlp3;


};


#endif
