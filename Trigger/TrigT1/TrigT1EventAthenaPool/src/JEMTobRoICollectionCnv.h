/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JEMTobRoICollectionCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JEMTobRoICollectionCnv_H
#define JEMTobRoICollectionCnv_H
 
class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigT1CaloEvent/JEMTobRoICollection.h"
#include "TrigT1EventTPCnv/JEMTobRoICollectionCnv_tlp1.h"

typedef JEMTobRoICollection_tlp1 JEMTobRoICollection_PERS;
typedef T_AthenaPoolCustomCnv<JEMTobRoICollection, JEMTobRoICollection_PERS> JEMTobRoICollectionCnvBase;

class JEMTobRoICollectionCnv: public JEMTobRoICollectionCnvBase {
friend class CnvFactory<JEMTobRoICollectionCnv>;

protected:

  JEMTobRoICollectionCnv (ISvcLocator* svcloc) :
    JEMTobRoICollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "JEMTobRoICollectionCnv" )
     {}
  
  ~JEMTobRoICollectionCnv() {}
  
  virtual JEMTobRoICollection_PERS*   createPersistent (JEMTobRoICollection* transCont);
  virtual JEMTobRoICollection*        createTransient ();

private:
  
  IMessageSvc                  *m_msgSvc;
  MsgStream                     m_log;
   
  JEMTobRoICollectionCnv_tlp1  m_TPConverter_tlp1;
};

#endif


