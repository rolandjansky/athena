/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JEMHitsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JEMHitsCollectionCnv_H
#define JEMHitsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/JEMHitsCollection.h"
#include "TrigT1EventTPCnv/JEMHitsCollectionCnv_tlp1.h"

typedef JEMHitsCollection_tlp1 JEMHitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<JEMHitsCollection, JEMHitsCollection_PERS> JEMHitsCollectionCnvBase;

class JEMHitsCollectionCnv: public JEMHitsCollectionCnvBase {
friend class CnvFactory<JEMHitsCollectionCnv>;

protected:

  JEMHitsCollectionCnv (ISvcLocator* svcloc) :
    JEMHitsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "JEMHitsCollectionCnv" )
     {}
  
  ~JEMHitsCollectionCnv() {}
  
  virtual JEMHitsCollection_PERS*   createPersistent (JEMHitsCollection* transCont);
  virtual JEMHitsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  JEMHitsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


