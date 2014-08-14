/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIG_MONEVENT_COLLECTIONCNV_H
#define TRIGEVENTATHENAPOOL_TRIG_MONEVENT_COLLECTIONCNV_H

#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;
class TrigMonEventCollection_tlp1;
class TrigMonEventCollectionCnv_impl;

// typedef to the latest persistent version
typedef TrigMonEventCollection_tlp1  TrigMonEventCollection_PERS;

class TrigMonEventCollectionCnv 
   : public T_AthenaPoolCustomCnv<TrigMonEventCollection, TrigMonEventCollection_PERS> 
{

  friend class CnvFactory<TrigMonEventCollectionCnv>;

 protected:
  
  TrigMonEventCollectionCnv(ISvcLocator* svcloc);
  virtual ~TrigMonEventCollectionCnv();

  TrigMonEventCollection_PERS*  createPersistent(TrigMonEventCollection* transObj);
  TrigMonEventCollection*       createTransient ();

 private:

  MsgStream                      *m_log;
  TrigMonEventCollectionCnv_impl *m_impl;
};


#endif
