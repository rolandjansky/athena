/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JEMRoICollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JEMRoICollectionCnv_H
#define JEMRoICollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/JEMRoICollection.h"
#include "TrigT1EventTPCnv/JEMRoICollectionCnv_tlp1.h"

typedef JEMRoICollection_tlp1 JEMRoICollection_PERS;
typedef T_AthenaPoolCustomCnv<JEMRoICollection, JEMRoICollection_PERS> JEMRoICollectionCnvBase;

class JEMRoICollectionCnv: public JEMRoICollectionCnvBase {
friend class CnvFactory<JEMRoICollectionCnv>;

protected:

  JEMRoICollectionCnv (ISvcLocator* svcloc) :
    JEMRoICollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "JEMRoICollectionCnv" )
     {}
  
  ~JEMRoICollectionCnv() {}
  
  virtual JEMRoICollection_PERS*   createPersistent (JEMRoICollection* transCont);
  virtual JEMRoICollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  JEMRoICollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


