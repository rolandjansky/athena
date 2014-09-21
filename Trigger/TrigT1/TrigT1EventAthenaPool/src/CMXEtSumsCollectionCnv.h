/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMXEtSumsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMXEtSumsCollectionCnv_H
#define CMXEtSumsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMXEtSumsCollection.h"
#include "TrigT1EventTPCnv/CMXEtSumsCollectionCnv_tlp1.h"

typedef CMXEtSumsCollection_tlp1 CMXEtSumsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMXEtSumsCollection, CMXEtSumsCollection_PERS> CMXEtSumsCollectionCnvBase;

class CMXEtSumsCollectionCnv: public CMXEtSumsCollectionCnvBase {
friend class CnvFactory<CMXEtSumsCollectionCnv>;

protected:

  CMXEtSumsCollectionCnv (ISvcLocator* svcloc) :
    CMXEtSumsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMXEtSumsCollectionCnv" )
     {}
  
  ~CMXEtSumsCollectionCnv() {}
  
  virtual CMXEtSumsCollection_PERS*   createPersistent (CMXEtSumsCollection* transCont);
  virtual CMXEtSumsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMXEtSumsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


