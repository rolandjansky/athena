/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JetElementCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JetElementCollectionCnv_H
#define JetElementCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1EventTPCnv/JetElementCollectionCnv_tlp1.h"

typedef JetElementCollection_tlp1 JetElementCollection_PERS;
typedef T_AthenaPoolCustomCnv<JetElementCollection, JetElementCollection_PERS> JetElementCollectionCnvBase;

class JetElementCollectionCnv: public JetElementCollectionCnvBase {
friend class CnvFactory<JetElementCollectionCnv>;

protected:

  JetElementCollectionCnv (ISvcLocator* svcloc) :
    JetElementCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "JetElementCollectionCnv" )
     {}
  
  ~JetElementCollectionCnv() {}
  
  virtual JetElementCollection_PERS*   createPersistent (JetElementCollection* transCont);
  virtual JetElementCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  JetElementCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


