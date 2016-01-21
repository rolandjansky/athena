/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigOperationalInfo
 *
 * @brief transient persistent converter for TrigOperationalInfo
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGOPERATIONALINFOCNV_H
#define TRIGEVENTATHENAPOOL_TRIGOPERATIONALINFOCNV_H


#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigOperationalInfo_p1;

// typedef to the latest persistent version
typedef TrigOperationalInfo_p1  TrigOperationalInfo_PERS;

class MsgStream;

class TrigOperationalInfoCnv_p1;
class TrigOperationalInfoCnv : public T_AthenaPoolCustomCnv<TrigOperationalInfo, TrigOperationalInfo_PERS > 
{

  friend class CnvFactory<TrigOperationalInfoCnv>;

 protected:
  
  TrigOperationalInfoCnv(ISvcLocator* svcloc);
  ~TrigOperationalInfoCnv();

  TrigOperationalInfo_PERS*  createPersistent(TrigOperationalInfo* transObj);

  TrigOperationalInfo*       createTransient ();

 private:

  TrigOperationalInfoCnv_p1* m_TPConverter;

};


#endif
