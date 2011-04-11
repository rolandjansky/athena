/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file CPMTowerCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef CPMTowerCollectionCnv_H
#define CPMTowerCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "TrigT1EventTPCnv/CPMTowerCollectionCnv_tlp1.h"

typedef CPMTowerCollection_tlp1 CPMTowerCollection_PERS;
typedef T_AthenaPoolCustomCnv<CPMTowerCollection, CPMTowerCollection_PERS> CPMTowerCollectionCnvBase;

class CPMTowerCollectionCnv: public CPMTowerCollectionCnvBase {
friend class CnvFactory<CPMTowerCollectionCnv>;

protected:

  CPMTowerCollectionCnv (ISvcLocator* svcloc) :
    CPMTowerCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CPMTowerCollectionCnv" )
     {}
  
  ~CPMTowerCollectionCnv() {}
  
  virtual CPMTowerCollection_PERS*   createPersistent (CPMTowerCollection* transCont);
  virtual CPMTowerCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  CPMTowerCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


