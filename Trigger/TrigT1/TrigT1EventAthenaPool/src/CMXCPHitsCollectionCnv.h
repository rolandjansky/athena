/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXCPHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXCPHitsCollectionCnv_H
#define CMXCPHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXCPHitsCollection.h"
#include "TrigT1EventTPCnv/CMXCPHitsCollectionCnv_tlp1.h"

typedef CMXCPHitsCollection_tlp1 CMXCPHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMXCPHitsCollection, CMXCPHitsCollection_PERS> CMXCPHitsCollectionCnvBase;

class CMXCPHitsCollectionCnv: public CMXCPHitsCollectionCnvBase {
friend class CnvFactory<CMXCPHitsCollectionCnv>;

protected:

  CMXCPHitsCollectionCnv (ISvcLocator* svcloc) :
    CMXCPHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXCPHitsCollectionCnv" )
     {}
  
  ~CMXCPHitsCollectionCnv() {}
  
  virtual CMXCPHitsCollection_PERS*   createPersistent (CMXCPHitsCollection* transCont);
  virtual CMXCPHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXCPHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


