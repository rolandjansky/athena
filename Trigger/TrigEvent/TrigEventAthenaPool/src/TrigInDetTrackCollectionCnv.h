/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TrigInDetTrackCollectionCnv
 *
 * @brief transient persistent converter for TrigInDetTrackCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCollectionCnv.h,v 1.8 demelian Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGINDETTRACKCOLLECTIONCNV_H
#define TRIGEVENTATHENAPOOL_TRIGINDETTRACKCOLLECTIONCNV_H

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class TrigInDetTrackCollection_tlp4;
class TrigInDetTrackCollectionCnv_tlp4;
typedef TrigInDetTrackCollection_tlp4  TrigInDetTrackCollection_PERS;
typedef TrigInDetTrackCollectionCnv_tlp4  TrigInDetTrackCollectionCnv_PERS;

typedef T_AthenaPoolCustomCnv<TrigInDetTrackCollection, TrigInDetTrackCollection_PERS > TrigInDetTrackCollectionCnvBase; 

class MsgStream;

class TrigInDetTrackCollectionCnv : public TrigInDetTrackCollectionCnvBase{

  friend class CnvFactory<TrigInDetTrackCollectionCnv>;

public:
  TrigInDetTrackCollectionCnv(ISvcLocator* svcloc);
  ~TrigInDetTrackCollectionCnv();

protected:
  virtual TrigInDetTrackCollection_PERS*  createPersistent(TrigInDetTrackCollection* transObj);
  virtual TrigInDetTrackCollection*       createTransient ();

 private:

  TrigInDetTrackCollectionCnv_PERS* m_tlp_Converter;


};


#endif
