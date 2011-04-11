/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CPMHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CPMHitsCollectionCnv_H
#define CPMHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "TrigT1EventTPCnv/CPMHitsCollectionCnv_tlp1.h"

typedef CPMHitsCollection_tlp1 CPMHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<CPMHitsCollection, CPMHitsCollection_PERS> CPMHitsCollectionCnvBase;

class CPMHitsCollectionCnv: public CPMHitsCollectionCnvBase {
friend class CnvFactory<CPMHitsCollectionCnv>;

protected:

  CPMHitsCollectionCnv (ISvcLocator* svcloc) :
    CPMHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CPMHitsCollectionCnv" )
     {}
  
  ~CPMHitsCollectionCnv() {}
  
  virtual CPMHitsCollection_PERS*   createPersistent (CPMHitsCollection* transCont);
  virtual CPMHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CPMHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


