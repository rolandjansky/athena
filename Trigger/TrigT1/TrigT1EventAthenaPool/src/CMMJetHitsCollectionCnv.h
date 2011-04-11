/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMMJetHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMMJetHitsCollectionCnv_H
#define CMMJetHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "TrigT1EventTPCnv/CMMJetHitsCollectionCnv_tlp1.h"

typedef CMMJetHitsCollection_tlp1 CMMJetHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMMJetHitsCollection, CMMJetHitsCollection_PERS> CMMJetHitsCollectionCnvBase;

class CMMJetHitsCollectionCnv: public CMMJetHitsCollectionCnvBase {
friend class CnvFactory<CMMJetHitsCollectionCnv>;

protected:

  CMMJetHitsCollectionCnv (ISvcLocator* svcloc) :
    CMMJetHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMMJetHitsCollectionCnv" )
     {}
  
  ~CMMJetHitsCollectionCnv() {}
  
  virtual CMMJetHitsCollection_PERS*   createPersistent (CMMJetHitsCollection* transCont);
  virtual CMMJetHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMMJetHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


