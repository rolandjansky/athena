/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CPMRoICollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CPMRoICollectionCnv_H
#define CPMRoICollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CPMRoICollection.h"
#include "TrigT1EventTPCnv/CPMRoICollectionCnv_tlp1.h"

typedef CPMRoICollection_tlp1 CPMRoICollection_PERS;
typedef T_AthenaPoolCustomCnv<CPMRoICollection, CPMRoICollection_PERS> CPMRoICollectionCnvBase;

class CPMRoICollectionCnv: public CPMRoICollectionCnvBase {
friend class CnvFactory<CPMRoICollectionCnv>;

protected:

  CPMRoICollectionCnv (ISvcLocator* svcloc) :
    CPMRoICollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CPMRoICollectionCnv" )
     {}
  
  ~CPMRoICollectionCnv() {}
  
  virtual CPMRoICollection_PERS*   createPersistent (CPMRoICollection* transCont);
  virtual CPMRoICollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CPMRoICollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


