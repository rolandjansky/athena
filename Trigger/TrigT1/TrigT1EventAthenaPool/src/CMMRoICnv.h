/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMMRoICnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMMRoICnv_H
#define CMMRoICnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1EventTPCnv/CMMRoI_p1.h"
#include "TrigT1EventTPCnv/CMMRoICnv_p1.h"

typedef CMMRoI_p1 CMMRoI_PERS;
typedef T_AthenaPoolCustomCnv<LVL1::CMMRoI, CMMRoI_PERS> CMMRoICnvBase;

class CMMRoICnv: public CMMRoICnvBase {
friend class CnvFactory<CMMRoICnv>;

protected:

  CMMRoICnv (ISvcLocator* svcloc) :
    CMMRoICnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMMRoICnv" )
     {}
  
  ~CMMRoICnv() {}
  
  virtual CMMRoI_PERS*   createPersistent (LVL1::CMMRoI* transCont);
  virtual LVL1::CMMRoI*  createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMMRoICnv_p1  m_TPConverter_p1;
};

#endif


