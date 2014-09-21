/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXCPTobCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXCPTobCollectionCnv_H
#define CMXCPTobCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXCPTobCollection.h"
#include "TrigT1EventTPCnv/CMXCPTobCollectionCnv_tlp1.h"

typedef CMXCPTobCollection_tlp1 CMXCPTobCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMXCPTobCollection, CMXCPTobCollection_PERS> CMXCPTobCollectionCnvBase;

class CMXCPTobCollectionCnv: public CMXCPTobCollectionCnvBase {
friend class CnvFactory<CMXCPTobCollectionCnv>;

protected:

  CMXCPTobCollectionCnv (ISvcLocator* svcloc) :
    CMXCPTobCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXCPTobCollectionCnv" )
     {}
  
  ~CMXCPTobCollectionCnv() {}
  
  virtual CMXCPTobCollection_PERS*   createPersistent (CMXCPTobCollection* transCont);
  virtual CMXCPTobCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXCPTobCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


