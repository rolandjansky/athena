/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIG_MONCONFIG_COLLECTIONCNV_H
#define TRIGEVENTATHENAPOOL_TRIG_MONCONFIG_COLLECTIONCNV_H

#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;
class TrigMonConfigCollection_tlp1;
class TrigMonConfigCollectionCnv_impl;

// typedef to the latest persistent version
typedef TrigMonConfigCollection_tlp1  TrigMonConfigCollection_PERS;

class TrigMonConfigCollectionCnv 
   : public T_AthenaPoolCustomCnv<TrigMonConfigCollection, TrigMonConfigCollection_PERS> 
{

  friend class CnvFactory<TrigMonConfigCollectionCnv>;

 protected:
  
  TrigMonConfigCollectionCnv(ISvcLocator* svcloc);
  virtual ~TrigMonConfigCollectionCnv();

  TrigMonConfigCollection_PERS*  createPersistent(TrigMonConfigCollection* transObj);
  TrigMonConfigCollection*       createTransient ();

 private:

  MsgStream                       *m_log;
  TrigMonConfigCollectionCnv_impl* m_impl;
};


#endif
