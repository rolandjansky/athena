/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIG_MONEVENT_CNV_H
#define TRIGEVENTATHENAPOOL_TRIG_MONEVENT_CNV_H

#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;
class TrigMonEvent_p1;
class TrigMonEventCnv_p1;

// typedef to the latest persistent version
typedef TrigMonEvent_p1  TrigMonEvent_PERS;

class TrigMonEventCnv : public T_AthenaPoolCustomCnv<TrigMonEvent, TrigMonEvent_PERS> 
{
  friend class CnvFactory<TrigMonEventCnv>;

 protected:
  
  TrigMonEventCnv(ISvcLocator* svcloc);
  virtual ~TrigMonEventCnv();

  TrigMonEvent_PERS*  createPersistent(TrigMonEvent* transObj);  
  TrigMonEvent*       createTransient ();

 private:

  MsgStream          *m_log;
  TrigMonEventCnv_p1 *m_TPConverter;
};


#endif
