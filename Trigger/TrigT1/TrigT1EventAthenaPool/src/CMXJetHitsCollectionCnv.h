/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXJetHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXJetHitsCollectionCnv_H
#define CMXJetHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXJetHitsCollection.h"
#include "TrigT1EventTPCnv/CMXJetHitsCollectionCnv_tlp1.h"

typedef CMXJetHitsCollection_tlp1 CMXJetHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMXJetHitsCollection, CMXJetHitsCollection_PERS> CMXJetHitsCollectionCnvBase;

class CMXJetHitsCollectionCnv: public CMXJetHitsCollectionCnvBase {
friend class CnvFactory<CMXJetHitsCollectionCnv>;

protected:

  CMXJetHitsCollectionCnv (ISvcLocator* svcloc) :
    CMXJetHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXJetHitsCollectionCnv" )
     {}
  
  ~CMXJetHitsCollectionCnv() {}
  
  virtual CMXJetHitsCollection_PERS*   createPersistent (CMXJetHitsCollection* transCont);
  virtual CMXJetHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXJetHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


