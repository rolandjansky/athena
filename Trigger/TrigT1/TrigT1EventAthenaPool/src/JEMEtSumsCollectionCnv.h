/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JEMEtSumsCollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JEMEtSumsCollectionCnv_H
#define JEMEtSumsCollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "TrigT1EventTPCnv/JEMEtSumsCollectionCnv_tlp1.h"

typedef JEMEtSumsCollection_tlp1 JEMEtSumsCollection_PERS;
typedef T_AthenaPoolCustomCnv<JEMEtSumsCollection, JEMEtSumsCollection_PERS> JEMEtSumsCollectionCnvBase;

class JEMEtSumsCollectionCnv: public JEMEtSumsCollectionCnvBase {
friend class CnvFactory<JEMEtSumsCollectionCnv>;

protected:

  JEMEtSumsCollectionCnv (ISvcLocator* svcloc) :
    JEMEtSumsCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "JEMEtSumsCollectionCnv" )
     {}
  
  ~JEMEtSumsCollectionCnv() {}
  
  virtual JEMEtSumsCollection_PERS*   createPersistent (JEMEtSumsCollection* transCont);
  virtual JEMEtSumsCollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  JEMEtSumsCollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


