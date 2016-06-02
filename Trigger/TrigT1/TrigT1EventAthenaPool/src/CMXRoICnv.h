/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXRoICnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXRoICnv_H
#define CMXRoICnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1EventTPCnv/CMXRoI_p1.h"
#include "TrigT1EventTPCnv/CMXRoICnv_p1.h"

typedef CMXRoI_p1 CMXRoI_PERS;
typedef T_AthenaPoolCustomCnv<LVL1::CMXRoI, CMXRoI_PERS> CMXRoICnvBase;

class CMXRoICnv: public CMXRoICnvBase {
friend class CnvFactory<CMXRoICnv>;

protected:

  CMXRoICnv (ISvcLocator* svcloc) :
    CMXRoICnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXRoICnv" )
     {}
  
  ~CMXRoICnv() {}
  
  virtual CMXRoI_PERS*   createPersistent (LVL1::CMXRoI* transCont);
  virtual LVL1::CMXRoI*  createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXRoICnv_p1  m_TPConverter_p1;
};

#endif


