/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file RODHeaderCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef RODHeaderCollectionCnv_H
#define RODHeaderCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/RODHeaderCollection.h"
#include "TrigT1EventTPCnv/RODHeaderCollectionCnv_tlp1.h"

typedef RODHeaderCollection_tlp1 RODHeaderCollection_PERS;
typedef T_AthenaPoolCustomCnv<RODHeaderCollection, RODHeaderCollection_PERS> RODHeaderCollectionCnvBase;

class RODHeaderCollectionCnv: public RODHeaderCollectionCnvBase {
friend class CnvFactory<RODHeaderCollectionCnv>;

protected:

  RODHeaderCollectionCnv (ISvcLocator* svcloc) :
    RODHeaderCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "RODHeaderCollectionCnv" )
     {}
  
  ~RODHeaderCollectionCnv() {}
  
  virtual RODHeaderCollection_PERS*   createPersistent (RODHeaderCollection* transCont);
  virtual RODHeaderCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  RODHeaderCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


