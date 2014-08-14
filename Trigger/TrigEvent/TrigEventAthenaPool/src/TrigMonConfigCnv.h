/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIG_MONCONFIG_CNV_H
#define TRIGEVENTATHENAPOOL_TRIG_MONCONFIG_CNV_H

#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;
class TrigMonConfig_p1;
class TrigMonConfigCnv_p1;

// typedef to the latest persistent version
typedef TrigMonConfig_p1  TrigMonConfig_PERS;

class TrigMonConfigCnv : public T_AthenaPoolCustomCnv<TrigMonConfig, TrigMonConfig_PERS> 
{
  friend class CnvFactory<TrigMonConfigCnv>;

 protected:
  
  TrigMonConfigCnv(ISvcLocator* svcloc);
  virtual ~TrigMonConfigCnv();

  TrigMonConfig_PERS*  createPersistent(TrigMonConfig* transObj);  
  TrigMonConfig*       createTransient ();

 private:

  MsgStream           *m_log;
  TrigMonConfigCnv_p1 *m_TPConverter;
};


#endif
