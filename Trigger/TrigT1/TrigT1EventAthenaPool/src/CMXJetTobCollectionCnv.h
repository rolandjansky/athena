/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXJetTobCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXJetTobCollectionCnv_H
#define CMXJetTobCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXJetTobCollection.h"
#include "TrigT1EventTPCnv/CMXJetTobCollectionCnv_tlp1.h"

typedef CMXJetTobCollection_tlp1 CMXJetTobCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMXJetTobCollection, CMXJetTobCollection_PERS> CMXJetTobCollectionCnvBase;

class CMXJetTobCollectionCnv: public CMXJetTobCollectionCnvBase {
friend class CnvFactory<CMXJetTobCollectionCnv>;

protected:

  CMXJetTobCollectionCnv (ISvcLocator* svcloc) :
    CMXJetTobCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXJetTobCollectionCnv" )
     {}
  
  ~CMXJetTobCollectionCnv() {}
  
  virtual CMXJetTobCollection_PERS*   createPersistent (CMXJetTobCollection* transCont);
  virtual CMXJetTobCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXJetTobCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


