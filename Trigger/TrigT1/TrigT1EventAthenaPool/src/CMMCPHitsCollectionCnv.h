/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CMMCPHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CMMCPHitsCollectionCnv_H
#define CMMCPHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CMMCPHitsCollection.h"
#include "TrigT1EventTPCnv/CMMCPHitsCollectionCnv_tlp1.h"

typedef CMMCPHitsCollection_tlp1 CMMCPHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CMMCPHitsCollection, CMMCPHitsCollection_PERS> CMMCPHitsCollectionCnvBase;

class CMMCPHitsCollectionCnv: public CMMCPHitsCollectionCnvBase {
friend class CnvFactory<CMMCPHitsCollectionCnv>;

protected:

  CMMCPHitsCollectionCnv (ISvcLocator* svcloc) :
    CMMCPHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CMMCPHitsCollectionCnv" )
     {}
  
  ~CMMCPHitsCollectionCnv() {}
  
  virtual CMMCPHitsCollection_PERS*   createPersistent (CMMCPHitsCollection* transCont);
  virtual CMMCPHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CMMCPHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


